struct OSHeapRegion {
	union {
		uint16_t next;
		uint16_t size;
	};

	uint16_t previous;
	uint16_t offset;
	uint16_t used;

	OSHeapRegion *regionListNext;
	OSHeapRegion **regionListReference;
};

static uintptr_t OSHeapCalculateIndex(uintptr_t size) {
	int x = __builtin_clz(size);
	uintptr_t msb = sizeof(unsigned int) * 8 - x - 1;
	return msb - 4;
}

static OSHeapRegion *heapRegions[12];

#ifdef KERNEL
Mutex heapMutex;
#define OS_HEAP_ACQUIRE_MUTEX() heapMutex.Acquire()
#define OS_HEAP_RELEASE_MUTEX() heapMutex.Release()
#define OS_HEAP_PANIC() KernelPanic("Heap panic.\n")
#define OS_HEAP_ALLOCATE_CALL(x) kernelVMM.Allocate(x)
#define OS_HEAP_FREE_CALL(x) kernelVMM.Free(x)
#else
static OSHandle heapMutex;
#define OS_HEAP_ACQUIRE_MUTEX() OSAcquireMutex(heapMutex)
#define OS_HEAP_RELEASE_MUTEX() OSReleaseMutex(heapMutex)
#define OS_HEAP_PANIC() Panic()
#define OS_HEAP_ALLOCATE_CALL(x) OSAllocate(x)
#define OS_HEAP_FREE_CALL(x) OSFree(x)
#endif

#define OS_HEAP_REGION_HEADER(region) ((OSHeapRegion *) ((uint8_t *) region - 0x10))
#define OS_HEAP_REGION_DATA(region) ((uint8_t *) region + 0x10)
#define OS_HEAP_REGION_NEXT(region) ((OSHeapRegion *) ((uint8_t *) region + region->next))
#define OS_HEAP_REGION_PREVIOUS(region) (region->previous ? ((OSHeapRegion *) ((uint8_t *) region - region->previous)) : nullptr)

#ifndef KERNEL
static void OSHeapInitialise() {
	heapMutex = OSCreateMutex();
}
#endif

static void OSHeapRemoveFreeRegion(OSHeapRegion *region) {
	if (!region->regionListReference || region->used) {
		OS_HEAP_PANIC();
	}

	*region->regionListReference = region->regionListNext;

	if (region->regionListNext) {
		region->regionListNext->regionListReference = region->regionListReference;
	}
}

static void OSHeapAddFreeRegion(OSHeapRegion *region) {
	if (region->used || region->size < 32) {
		OS_HEAP_PANIC();
	}

	int index = OSHeapCalculateIndex(region->size);
	region->regionListNext = heapRegions[index];
	if (region->regionListNext) region->regionListNext->regionListReference = &region->regionListNext;
	heapRegions[index] = region;
	region->regionListReference = heapRegions + index;
}

void *OSHeapAllocate(size_t size) {
	if (!size) return nullptr;

	size += 0x10; // Region metadata.
	size = (size + 0x1F) & ~0x1F; // Allocation granularity: 32 bytes.

	if (size >= 32768) {
		// This is a very large allocation, so allocate it by itself.
		OSHeapRegion *region = (OSHeapRegion *) OS_HEAP_ALLOCATE_CALL(size);
		region->used = 0xABCD;
		if (!region) return nullptr; else return OS_HEAP_REGION_DATA(region);
	}

	OS_HEAP_ACQUIRE_MUTEX();

	OSHeapRegion *region = nullptr;

	for (int i = OSHeapCalculateIndex(size); i < 12; i++) {
		if (heapRegions[i] == nullptr || heapRegions[i]->size < size) {
			continue;
		}

		region = heapRegions[i];
		OSHeapRemoveFreeRegion(region);
		goto foundRegion;
	}

	region = (OSHeapRegion *) OS_HEAP_ALLOCATE_CALL(65536);
	if (!region) {
		OS_HEAP_RELEASE_MUTEX();
		return nullptr; 
	}
	region->size = 65536 - 32;

	// Prevent OSHeapFree trying to merge off the end of the block.
	{
		OSHeapRegion *endRegion = OS_HEAP_REGION_NEXT(region);
		endRegion->used = 0xABCD;
	}

	foundRegion:

	if (region->used || region->size < size) {
		OS_HEAP_PANIC();
	}

	if (region->size == size) {
		// If the size of this region is equal to the size of the region we're trying to allocate,
		// return this region immediately.
		region->used = 0xABCD;
		OS_HEAP_RELEASE_MUTEX();
		return OS_HEAP_REGION_DATA(region);
	}

	// Split the region into 2 parts.
	
	OSHeapRegion *allocatedRegion = region;
	size_t oldSize = allocatedRegion->size;
	allocatedRegion->size = size;
	allocatedRegion->used = 0xABCD;

	OSHeapRegion *freeRegion = OS_HEAP_REGION_NEXT(allocatedRegion);
	freeRegion->size = oldSize - size;
	freeRegion->previous = size;
	freeRegion->offset = allocatedRegion->offset + size;
	freeRegion->used = false;
	OSHeapAddFreeRegion(freeRegion);

	OSHeapRegion *nextRegion = OS_HEAP_REGION_NEXT(freeRegion);
	nextRegion->previous = freeRegion->size;

	OS_HEAP_RELEASE_MUTEX();
	return OS_HEAP_REGION_DATA(allocatedRegion);
}

void OSHeapFree(void *address) {
	if (!address) return;

	OSHeapRegion *region = OS_HEAP_REGION_HEADER(address);
	if (region->used != 0xABCD) OS_HEAP_PANIC();
	region->used = false;

	if (!region->size) {
		// The region was allocated by itself.
		OS_HEAP_FREE_CALL(region);
		return;
	}

	OS_HEAP_ACQUIRE_MUTEX();

	// Attempt to merge with the next region.

	OSHeapRegion *nextRegion = OS_HEAP_REGION_NEXT(region);

	if (nextRegion && !nextRegion->used) {
		OSHeapRemoveFreeRegion(nextRegion);

		// Merge the regions.
		region->size += nextRegion->size;
		OS_HEAP_REGION_NEXT(nextRegion)->previous = region->size;
	}

	// Attempt to merge with the previous region.

	OSHeapRegion *previousRegion = OS_HEAP_REGION_PREVIOUS(region);

	if (previousRegion && !previousRegion->used) {
		OSHeapRemoveFreeRegion(previousRegion);

		// Merge the regions.
		previousRegion->size += region->size;
		OS_HEAP_REGION_NEXT(region)->previous = previousRegion->size;
		region = previousRegion;
	}

	if (region->size == 65536 - 32) {
		if (region->offset) OS_HEAP_PANIC();

		// The memory block is empty.
		OS_HEAP_FREE_CALL(region);
		OS_HEAP_RELEASE_MUTEX();
		return;
	}

	// Put the free region in the region list.
	OSHeapAddFreeRegion(region);
	OS_HEAP_RELEASE_MUTEX();
}
