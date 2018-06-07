#ifdef IMPLEMENTATION

typedef struct {
	uint32_t magicNumber; // 0x7F followed by 'ELF'
	uint8_t bits; // 1 = 32 bit, 2 = 64 bit
	uint8_t endianness; // 1 = LE, 2 = BE
	uint8_t version1;
	uint8_t abi; // 0 = System V
	uint8_t _unused0[8];
	uint16_t type; // 1 = relocatable, 2 = executable, 3 = shared
	uint16_t instructionSet; // 0x03 = x86, 0x28 = ARM, 0x3E = x86-64, 0xB7 = AArch64
	uint32_t version2;

#ifdef ARCH_32
	uint32_t entry;
	uint32_t programHeaderTable;
	uint32_t sectionHeaderTable;
	uint32_t flags;
	uint16_t headerSize;
	uint16_t programHeaderEntrySize;
	uint16_t programHeaderEntries;
	uint16_t sectionHeaderEntrySize;
	uint16_t sectionHeaderEntries;
	uint16_t sectionNameIndex;
#else
	uint64_t entry;
	uint64_t programHeaderTable;
	uint64_t sectionHeaderTable;
	uint32_t flags;
	uint16_t headerSize;
	uint16_t programHeaderEntrySize;
	uint16_t programHeaderEntries;
	uint16_t sectionHeaderEntrySize;
	uint16_t sectionHeaderEntries;
	uint16_t sectionNameIndex;
#endif
} ElfHeader;

#ifdef ARCH_32
typedef struct {
	uint32_t type; // 0 = unused, 1 = load, 2 = dynamic, 3 = interp, 4 = note
	uint32_t fileOffset;
	uint32_t virtualAddress;
	uint32_t _unused0;
	uint32_t dataInFile;
	uint32_t segmentSize;
	uint32_t flags; // 1 = executable, 2 = writable, 4 = readable
	uint32_t alignment;
} ElfProgramHeader;
#else
typedef struct {
	uint32_t type; // 0 = unused, 1 = load, 2 = dynamic, 3 = interp, 4 = note
	uint32_t flags; // 1 = executable, 2 = writable, 4 = readable
	uint64_t fileOffset;
	uint64_t virtualAddress;
	uint64_t _unused0;
	uint64_t dataInFile;
	uint64_t segmentSize;
	uint64_t alignment;
} ElfProgramHeader;
#endif

uintptr_t LoadELF(char *imageName, size_t imageNameLength) {
	Process *thisProcess = GetCurrentThread()->process;

	uint64_t fileFlags = OS_OPEN_NODE_READ_ACCESS
		| OS_OPEN_NODE_WRITE_BLOCK
		| OS_OPEN_NODE_RESIZE_BLOCK
		| OS_OPEN_NODE_FAIL_IF_NOT_FOUND;

	OSNodeInformation node;
	OSError error = OSOpenNode(imageName, imageNameLength, fileFlags, &node);

	if (error != OS_SUCCESS) {
		// We couldn't open the executable.
		// `error` should contain some more information, but it doesn't really matter.
		return 0;
	}

	Defer(OSCloseHandle(node.handle));

	ElfHeader header;
	size_t bytesRead;
	bytesRead = OSReadFileSync(node.handle, 0, sizeof(ElfHeader), (uint8_t *) &header);
	if (bytesRead != sizeof(ElfHeader)) return 0;

	size_t programHeaderEntrySize = header.programHeaderEntrySize;

	if (header.magicNumber != 0x464C457F) return 0;
	if (header.bits != 2) return 0;
	if (header.endianness != 1) return 0;
	if (header.abi != 0) return 0;
	if (header.type != 2) return 0;
	if (header.instructionSet != 0x3E) return 0;

	ElfProgramHeader *programHeaders = (ElfProgramHeader *) OSHeapAllocate(programHeaderEntrySize * header.programHeaderEntries, false);
	Defer(OSHeapFree(programHeaders));

	bytesRead = OSReadFileSync(node.handle, header.programHeaderTable, programHeaderEntrySize * header.programHeaderEntries, (uint8_t *) programHeaders);
	if (bytesRead != programHeaderEntrySize * header.programHeaderEntries) return 0;

	KernelObjectType type = KERNEL_OBJECT_NODE;
	void *object = kernelProcess->handleTable.ResolveHandle(node.handle, type, RESOLVE_HANDLE_TO_USE);
	Defer(kernelProcess->handleTable.CompleteHandle(object, node.handle));

	for (uintptr_t i = 0; i < header.programHeaderEntries; i++) {
		ElfProgramHeader *header = (ElfProgramHeader *) ((uint8_t *) programHeaders + programHeaderEntrySize * i);
		if (header->type != 1) continue;

		void *segment = (void *) header->virtualAddress;

		if (segment > (void *) 0x8000000000000000 || header->segmentSize > 0x1000000000000) {
			return 0;
		}

		OSPrint("FileOffset %x    VirtualAddress %x    SegmentSize %x    DataInFile %x\n",
				header->fileOffset, header->virtualAddress, header->segmentSize, header->dataInFile);

		segment = (void *) (((uintptr_t) segment) & ~(PAGE_SIZE - 1));

#if 0
		thisProcess->vmm->lock.Acquire();
		bool success = thisProcess->vmm->AddRegion((uintptr_t) segment, 
				(header->segmentSize / PAGE_SIZE) + 1, 0, VMM_REGION_STANDARD, VMM_MAP_ALL, true, nullptr);
		thisProcess->vmm->lock.Release();
#else
		header->dataInFile = (header->dataInFile + 0xFFF) & ~0xFFF;
		header->segmentSize = (header->segmentSize + 0xFFF) & ~0xFFF;

		void *success = thisProcess->vmm->Allocate("Executable", header->dataInFile, VMM_MAP_CHUNKS, VMM_REGION_SHARED,
				header->fileOffset, VMM_REGION_FLAG_CACHABLE | VMM_REGION_FLAG_READ_ONLY, &((Node *) object)->region, (uintptr_t) segment);

		if (success && header->segmentSize - header->dataInFile) {
			success = thisProcess->vmm->Allocate("Executable", header->segmentSize - header->dataInFile, VMM_MAP_LAZY, VMM_REGION_STANDARD,
				0, VMM_REGION_FLAG_CACHABLE, nullptr, (uintptr_t) segment + header->dataInFile);
		}
#endif

		if (!success) {
			return 0;
		}

#if 0
		bytesRead = OSReadFileSync(node.handle, header->fileOffset, header->dataInFile, (uint8_t *) segment + (header->virtualAddress - (uintptr_t) segment));
		if (bytesRead != header->dataInFile) return 0;
#endif
	}

	return header.entry;
}

#endif
