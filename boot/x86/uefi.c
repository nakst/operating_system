#include <efi.h>
#include <efilib.h>

// TODO This doesn't work with AHCI? It tries to register interrupt 255.

#define LOW_MEMORY_MAP_START (0xFFFFFF0000000000)
#define PAGE_TABLE_L4 ((volatile uint64_t *) 0xFFFFFFFFFFFFF000)
#define PAGE_TABLE_L3 ((volatile uint64_t *) 0xFFFFFFFFFFE00000)
#define PAGE_TABLE_L2 ((volatile uint64_t *) 0xFFFFFFFFC0000000)
#define PAGE_TABLE_L1 ((volatile uint64_t *) 0xFFFFFF8000000000)
#define ENTRIES_PER_PAGE_TABLE (512)
#define ENTRIES_PER_PAGE_TABLE_BITS (9)
#define PAGE_SIZE (4096)
#define PAGE_BITS (12)

typedef struct __attribute__((packed)) GDTData {
	uint16_t length;
	uint64_t address;
} GDTData;

typedef struct MemoryRegion {
	uintptr_t base, pages;
} MemoryRegion;

#define MAX_MEMORY_REGIONS (1024)
MemoryRegion memoryRegions[1024];
#define KERNEL_BUFFER_SIZE (1048576)
#define kernelBuffer ((char *) 0x200000)
#define IID_BUFFER_SIZE (64)
char iidBuffer[IID_BUFFER_SIZE];
#define MEMORY_MAP_BUFFER_SIZE (16384)
char memoryMapBuffer[MEMORY_MAP_BUFFER_SIZE];

#define VESA_VM_INFO_ONLY
#define ARCH_64
#include "../../kernel/graphics.cpp"
#include "../../kernel/elf.cpp"

void ZeroMemory(void *pointer, uint64_t size) {
	char *d = (char *) pointer;

	for (uintptr_t i = 0; i < size; i++) {
		d[i] = 0;
	}
}

EFI_STATUS EFIAPI efi_main(EFI_HANDLE imageHandle, EFI_SYSTEM_TABLE *systemTable) {
	UINTN mapKey;
	uint32_t *framebuffer, horizontalResolution, verticalResolution;
	InitializeLib(imageHandle, systemTable);
	ElfHeader *header;
	Print(L"Loading OS...\n");

	// Make sure 0x100000 -> 0x300000 is identity mapped.
	{
		EFI_PHYSICAL_ADDRESS address = 0x100000;

		if (EFI_SUCCESS != uefi_call_wrapper(ST->BootServices->AllocatePages, 4, AllocateAddress, EfiLoaderData, 0x200, &address)) {
			Print(L"Error: Could not map 0x100000 -> 0x180000.\n");
			while (1);
		}
	}

	// Get the graphics mode information.
	{
		EFI_GRAPHICS_OUTPUT_PROTOCOL *graphicsOutputProtocol;
		EFI_GUID graphicsOutputProtocolGUID = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;

		if (EFI_SUCCESS != uefi_call_wrapper(ST->BootServices->LocateProtocol, 3, &graphicsOutputProtocolGUID, NULL, &graphicsOutputProtocol)) {
			Print(L"Error: Could not open protocol 3.\n");
			while (1);
		}

		framebuffer = (uint32_t *) graphicsOutputProtocol->Mode->FrameBufferBase;
		horizontalResolution = graphicsOutputProtocol->Mode->Info->HorizontalResolution;
		verticalResolution = graphicsOutputProtocol->Mode->Info->VerticalResolution;
	}

	// Read the kernel, IID and loader files.
	{
		EFI_GUID loadedImageProtocolGUID = LOADED_IMAGE_PROTOCOL;
		EFI_GUID simpleFilesystemProtocolGUID = SIMPLE_FILE_SYSTEM_PROTOCOL;

		EFI_LOADED_IMAGE_PROTOCOL *loadedImageProtocol;
		EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *simpleFilesystemProtocol;

		EFI_FILE *filesystemRoot, *kernelFile, *iidFile, *loaderFile;

		UINTN size;

		if (EFI_SUCCESS != uefi_call_wrapper(ST->BootServices->OpenProtocol, 6, imageHandle, &loadedImageProtocolGUID, &loadedImageProtocol, imageHandle, NULL, EFI_OPEN_PROTOCOL_GET_PROTOCOL)) {
			Print(L"Error: Could not open protocol 1.\n");
			while (1);
		}

		EFI_HANDLE deviceHandle = loadedImageProtocol->DeviceHandle; 

		if (EFI_SUCCESS != uefi_call_wrapper(ST->BootServices->OpenProtocol, 6, deviceHandle, &simpleFilesystemProtocolGUID, &simpleFilesystemProtocol, imageHandle, NULL, EFI_OPEN_PROTOCOL_GET_PROTOCOL)) {
			Print(L"Error: Could not open procotol 2.\n");
			while (1);
		}

		if (EFI_SUCCESS != uefi_call_wrapper(simpleFilesystemProtocol->OpenVolume, 2, simpleFilesystemProtocol, &filesystemRoot)) {
			Print(L"Error: Could not open ESP volume.\n");
			while (1);
		}

		if (EFI_SUCCESS != uefi_call_wrapper(filesystemRoot->Open, 5, filesystemRoot, &kernelFile, L"EssenceKernel.esx", EFI_FILE_MODE_READ, 0)) {
			Print(L"Error: Could not open EssenceKernel.esx.\n");
			while (1);
		}

		size = KERNEL_BUFFER_SIZE;

		if (EFI_SUCCESS != uefi_call_wrapper(kernelFile->Read, 3, kernelFile, &size, kernelBuffer)) {
			Print(L"Error: Could not load EssenceKernel.esx.\n");
			while (1);
		}

		Print(L"Kernel size: %d bytes\n", size);

		if (size == KERNEL_BUFFER_SIZE) {
			Print(L"Kernel too large to fit into buffer.\n");
			while (1);
		}

		if (EFI_SUCCESS != uefi_call_wrapper(filesystemRoot->Open, 5, filesystemRoot, &iidFile, L"EssenceIID.dat", EFI_FILE_MODE_READ, 0)) {
			Print(L"Error: Could not open EssenceIID.dat.\n");
			while (1);
		}

		size = IID_BUFFER_SIZE;

		if (EFI_SUCCESS != uefi_call_wrapper(iidFile->Read, 3, iidFile, &size, iidBuffer)) {
			Print(L"Error: Could not load EssenceIID.dat.\n");
			while (1);
		}

		if (EFI_SUCCESS != uefi_call_wrapper(filesystemRoot->Open, 5, filesystemRoot, &loaderFile, L"EssenceLoader.bin", EFI_FILE_MODE_READ, 0)) {
			Print(L"Error: Could not open EssenceLoader.bin.\n");
			while (1);
		}

		size = 0x80000;

		if (EFI_SUCCESS != uefi_call_wrapper(loaderFile->Read, 3, loaderFile, &size, (char *) 0x180000)) {
			Print(L"Error: Could not load EssenceLoader.bin.\n");
			while (1);
		}
	}

#if 0
	// Print the memory map.
	{
		UINTN descriptorSize, descriptorVersion, size = MEMORY_MAP_BUFFER_SIZE;

		if (EFI_SUCCESS != uefi_call_wrapper(ST->BootServices->GetMemoryMap, 5, &size, (EFI_MEMORY_DESCRIPTOR *) memoryMapBuffer, &mapKey, &descriptorSize, &descriptorVersion)) {
			Print(L"Error: Could not get memory map.\n");
			while (1);
		}

		WCHAR *memoryTypes[] = {
			L"EfiReservedMemoryType",
			L"EfiLoaderCode",
			L"EfiLoaderData",
			L"EfiBootServicesCode",
			L"EfiBootServicesData",
			L"EfiRuntimeServicesCode",
			L"EfiRuntimeServicesData",
			L"EfiConventionalMemory",
			L"EfiUnusableMemory",
			L"EfiACPIReclaimMemory",
			L"EfiACPIMemoryNVS",
			L"EfiMemoryMappedIO",
			L"EfiMemoryMappedIOPortSpace",
			L"EfiPalCode",
			L"EfiMaxMemoryType",
		};

		for (uintptr_t i = 0; i < size / descriptorSize; i++) {
			EFI_MEMORY_DESCRIPTOR *descriptor = (EFI_MEMORY_DESCRIPTOR *) (memoryMapBuffer + i * descriptorSize);
			Print(L"memory %s: %llx -> %llx\n", memoryTypes[descriptor->Type], descriptor->PhysicalStart, descriptor->PhysicalStart + descriptor->NumberOfPages * 0x1000 - 1);
		}
	}
#endif

	// Get the memory map.
	{
		UINTN descriptorSize, descriptorVersion, size = MEMORY_MAP_BUFFER_SIZE;

		if (EFI_SUCCESS != uefi_call_wrapper(ST->BootServices->GetMemoryMap, 5, &size, (EFI_MEMORY_DESCRIPTOR *) memoryMapBuffer, &mapKey, &descriptorSize, &descriptorVersion)) {
			Print(L"Error: Could not get memory map.\n");
			while (1);
		}

		uintptr_t memoryRegionCount = 0;

		for (uintptr_t i = 0; i < size / descriptorSize && memoryRegionCount != MAX_MEMORY_REGIONS - 1; i++) {
			EFI_MEMORY_DESCRIPTOR *descriptor = (EFI_MEMORY_DESCRIPTOR *) (memoryMapBuffer + i * descriptorSize);

			if (descriptor->Type == EfiConventionalMemory && descriptor->PhysicalStart >= 0x300000) {
				memoryRegions[memoryRegionCount].base = descriptor->PhysicalStart;
				memoryRegions[memoryRegionCount].pages = descriptor->NumberOfPages;
				memoryRegionCount++;
			}
		}

		memoryRegions[memoryRegionCount].base = 0;
	}

	// Exit boot services.
	{
		if (EFI_SUCCESS != uefi_call_wrapper(ST->BootServices->ExitBootServices, 2, imageHandle, mapKey)) {
			Print(L"Error: Could not exit boot services.\n");
			while (1);
		}
	}

	// Identity map the first 3MB for the loader.
	{
		uint64_t *paging = (uint64_t *) 0x140000;
		ZeroMemory(paging, 0x5000);

		paging[0x1FF] = 0x140003;
		paging[0x000] = 0x141003;
		paging[0x200] = 0x142003;
		paging[0x400] = 0x143003;
		paging[0x401] = 0x144003;

		for (uintptr_t i = 0; i < 0x400; i++) {
			paging[0x600 + i] = (i * 0x1000) | 3;
		}
	}

	// Copy the installation ID across.
	{
		uint8_t *destination = (uint8_t *) (0x107FF0);

		for (uintptr_t i = 0; i < 16; i++) {
			char c1 = iidBuffer[i * 3 + 0];
			char c2 = iidBuffer[i * 3 + 1];
			if (c1 >= '0' && c1 <= '9') c1 -= '0'; else c1 -= 'A' - 10;
			if (c2 >= '0' && c2 <= '9') c2 -= '0'; else c2 -= 'A' - 10;
			destination[i] = (c2) | ((c1) << 4);
		}
	}

	// Copy the graphics information across.
	{
		struct VESAVideoModeInformation *destination = (struct VESAVideoModeInformation *) (0x107000);
		destination->widthPixels = horizontalResolution;
		destination->heightPixels = verticalResolution;
		destination->bufferPhysical = (uintptr_t) framebuffer; // TODO 64-bit framebuffers.
		destination->bytesPerScanlineLinear = horizontalResolution * 4;
		destination->bitsPerPixel = 32;
	}

	// Allocate and map memory for the kernel.
	{
		uint64_t nextPageTable = 0x1C0000;

		header = (ElfHeader *) kernelBuffer;
		ElfProgramHeader *programHeaders = (ElfProgramHeader *) (kernelBuffer + header->programHeaderTable);
		uintptr_t programHeaderEntrySize = header->programHeaderEntrySize;

		for (uintptr_t i = 0; i < header->programHeaderEntries; i++) {
			ElfProgramHeader *header = (ElfProgramHeader *) ((uint8_t *) programHeaders + programHeaderEntrySize * i);
			if (header->type != 1) continue;

			uintptr_t pagesToAllocate = header->segmentSize >> 12;
			if (header->segmentSize & 0xFFF) pagesToAllocate++;
			uintptr_t physicalAddress = 0;

			for (uintptr_t j = 0; j < MAX_MEMORY_REGIONS; j++) {
				MemoryRegion *region = memoryRegions + j;
				if (!region->base) break;
				if (region->pages < pagesToAllocate) continue;
				physicalAddress = region->base;
				region->pages -= pagesToAllocate;
				region->base += pagesToAllocate << 12;
				break;
			}

			if (!physicalAddress) {
				// TODO Error handling.
				*((uint32_t *) framebuffer + 3) = 0xFFFF00FF;
				while (1);
			}

			for (uintptr_t j = 0; j < pagesToAllocate; j++, physicalAddress += 0x1000) {
				uintptr_t virtualAddress = header->virtualAddress + j * PAGE_SIZE;
				physicalAddress &= 0xFFFFFFFFFFFFF000;
				virtualAddress  &= 0x0000FFFFFFFFF000;

				uintptr_t indexL4 = (virtualAddress >> (PAGE_BITS + ENTRIES_PER_PAGE_TABLE_BITS * 3)) & (ENTRIES_PER_PAGE_TABLE - 1);
				uintptr_t indexL3 = (virtualAddress >> (PAGE_BITS + ENTRIES_PER_PAGE_TABLE_BITS * 2)) & (ENTRIES_PER_PAGE_TABLE - 1);
				uintptr_t indexL2 = (virtualAddress >> (PAGE_BITS + ENTRIES_PER_PAGE_TABLE_BITS * 1)) & (ENTRIES_PER_PAGE_TABLE - 1);
				uintptr_t indexL1 = (virtualAddress >> (PAGE_BITS + ENTRIES_PER_PAGE_TABLE_BITS * 0)) & (ENTRIES_PER_PAGE_TABLE - 1);

				uint64_t *tableL4 = (uint64_t *) 0x140000;

				if (!(tableL4[indexL4] & 1)) {
					tableL4[indexL4] = nextPageTable | 7;
					ZeroMemory((void *) nextPageTable, PAGE_SIZE);
					nextPageTable += PAGE_SIZE;
				}

				uint64_t *tableL3 = (uint64_t *) (tableL4[indexL4] & ~(PAGE_SIZE - 1));

				if (!(tableL3[indexL3] & 1)) {
					tableL3[indexL3] = nextPageTable | 7;
					ZeroMemory((void *) nextPageTable, PAGE_SIZE);
					nextPageTable += PAGE_SIZE;
				}

				uint64_t *tableL2 = (uint64_t *) (tableL3[indexL3] & ~(PAGE_SIZE - 1));

				if (!(tableL2[indexL2] & 1)) {
					tableL2[indexL2] = nextPageTable | 7;
					ZeroMemory((void *) nextPageTable, PAGE_SIZE);
					nextPageTable += PAGE_SIZE;
				}

				uint64_t *tableL1 = (uint64_t *) (tableL2[indexL2] & ~(PAGE_SIZE - 1));
				uintptr_t value = physicalAddress | 3;
				tableL1[indexL1] = value;
			}
		}
	}

	// Copy the memory regions information across.
	{
		MemoryRegion *destination = (MemoryRegion *) 0x160000;

		for (uintptr_t i = 0; i < MAX_MEMORY_REGIONS; i++) {
			destination[i] = memoryRegions[i];
		}
	}

	*framebuffer = 0xFFFF;

	// Start the loader.
	{
		((void (*)()) 0x180000)();
	}

	while (1);
	return EFI_SUCCESS;
}
