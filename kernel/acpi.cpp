#ifdef IMPLEMENTATION

#define USE_ACPICA
// #define USE_SMP

#define SIGNATURE_RSDP 0x2052545020445352

#define SIGNATURE_RSDT 0x54445352
#define SIGNATURE_XSDT 0x54445358
#define SIGNATURE_MADT 0x43495041

struct RootSystemDescriptorPointer {
	uint64_t signature;
	uint8_t checksum;
	char OEMID[6];
	uint8_t revision;
	uint32_t rsdtAddress;
	uint32_t length;
	uint64_t xsdtAddress;
	uint8_t extendedChecksum;
	uint8_t reserved[3];
};

struct ACPIDescriptorTable {
#define ACPI_DESCRIPTOR_TABLE_HEADER_LENGTH 36
	uint32_t signature;
	uint32_t length;
	uint64_t id;
	uint64_t tableID;
	uint32_t oemRevision;
	uint32_t creatorID;
	uint32_t creatorRevision;
};

struct MultipleAPICDescriptionTable {
	uint32_t lapicAddress; 
	uint32_t flags;
};

struct ACPIProcessor {
	uint8_t processorID, kernelProcessorID;
	uint8_t apicID;
	bool bootstrapProcessor;
	bool started;
	void **kernelStack;
};

struct ACPIIoApic {
	uint32_t ReadRegister(uint32_t reg);
	void WriteRegister(uint32_t reg, uint32_t value);

	uint8_t id;
	uint32_t address;
	uint32_t gsiBase;
};

uint32_t ACPIIoApic::ReadRegister(uint32_t reg) {
	uint32_t volatile *addr = (uint32_t volatile *) (LOW_MEMORY_MAP_START + address);
	addr[0] = reg; return addr[4];
}

void ACPIIoApic::WriteRegister(uint32_t reg, uint32_t value) {
	uint32_t volatile *addr = (uint32_t volatile *) (LOW_MEMORY_MAP_START + address);
	addr[0] = reg; addr[4] = value;
}

struct ACPIInterruptOverride {
	uint8_t sourceIRQ;
	uint32_t gsiNumber;
	bool activeLow, levelTriggered;
};

struct ACPILapicNMI {
	uint8_t processor; // 0xFF for all processors
	uint8_t lintIndex;
	bool activeLow, levelTriggered;
};

struct ACPILapic {
	uint32_t ReadRegister(uint32_t reg);
	void EndOfInterrupt();
	void WriteRegister(uint32_t reg, uint32_t value);
	void NextTimer(size_t ms);

	volatile uint32_t *address;
	size_t ticksPerMs;
};

void ACPILapic::NextTimer(size_t ms) {
	WriteRegister(0x320 >> 2, TIMER_INTERRUPT | (1 << 17)); 
	WriteRegister(0x380 >> 2, ticksPerMs * ms); 
}

void ACPILapic::EndOfInterrupt() {
	WriteRegister(0xB0 >> 2, 0);
}

uint32_t ACPILapic::ReadRegister(uint32_t reg) {
	return address[reg];
}

void ACPILapic::WriteRegister(uint32_t reg, uint32_t value) {
	address[reg] = value;
}

struct ACPI {
	void Initialise();
	void Initialise2();

	void FindRootSystemDescriptorPointer();
	void *FindTable(uint32_t tableSignature, ACPIDescriptorTable **header = nullptr);

	void PrintMADTContents();

	void ShutdownComputer();

	size_t processorCount;
	size_t ioapicCount;
	size_t interruptOverrideCount;
	size_t lapicNMICount;

	ACPIProcessor processors[256];
	ACPIProcessor *bootstrapProcessor;
	ACPIIoApic ioApics[16];
	ACPIInterruptOverride interruptOverrides[256];
	ACPILapicNMI lapicNMIs[32];
	ACPILapic lapic;

	uint64_t timestampTicksPerMs;

	private:

	RootSystemDescriptorPointer *rsdp;
	ACPIDescriptorTable *sdt; bool isXSDT;

#define ACPI_MAX_TABLE_LENGTH (16384)
#define ACPI_MAX_TABLES (512)
	ACPIDescriptorTable *tables[ACPI_MAX_TABLES];
	size_t tablesCount;
};

ACPI acpi;

#ifdef ARCH_X86_64
void ACPI::FindRootSystemDescriptorPointer() {
	PhysicalMemoryRegion searchRegions[2];

	searchRegions[0].baseAddress = (uintptr_t) (((uint16_t *) LOW_MEMORY_MAP_START)[0x40E] << 4) + LOW_MEMORY_MAP_START;
	searchRegions[0].pageCount = 0x400;
	searchRegions[1].baseAddress = (uintptr_t) 0xE0000 + LOW_MEMORY_MAP_START;
	searchRegions[1].pageCount = 0x20000;

	for (uintptr_t i = 0; i < 2; i++) {
		for (uintptr_t address = searchRegions[i].baseAddress;
				address < searchRegions[i].baseAddress + searchRegions[i].pageCount;
				address += 16) {
			rsdp = (RootSystemDescriptorPointer *) address;

			if (rsdp->signature != SIGNATURE_RSDP) {
				continue;
			}

			if (rsdp->revision == 0) {
				if (SumBytes((uint8_t *) rsdp, 20)) {
					continue;
				}

				return;
			} else if (rsdp->revision == 2) {
				if (SumBytes((uint8_t *) rsdp, sizeof(RootSystemDescriptorPointer))) {
					continue;
				}

				return;
			}
		}
	}

	// We didn't find the RSDP.
	rsdp = nullptr;
}
#endif

void ACPI::PrintMADTContents() {
	ACPIDescriptorTable *header;
	MultipleAPICDescriptionTable *madt = (MultipleAPICDescriptionTable *) FindTable(SIGNATURE_MADT, &header);

	if (!madt) {
		KernelPanic("ACPI::Initialise - Could not find the MADT table.\nThis is required to use the APIC.\n");
	}

	uintptr_t length = header->length - ACPI_DESCRIPTOR_TABLE_HEADER_LENGTH - sizeof(MultipleAPICDescriptionTable);
	uintptr_t startLength = length;
	uint8_t *data = (uint8_t *) (madt + 1);

	StartDebugOutput();
	Print("LAPIC address: %x\n", lapic.address);
	Print("Flags: %x\n", madt->flags);

	while (length && length <= startLength) {
		uint8_t entryType = data[0];
		uint8_t entryLength = data[1];

		switch (entryType) {
			case 0: {
				Print("Processor: id %d, apic ID %d\n", (int) data[2], (int) data[3]);
				for (uintptr_t i = 0; i < processorCount; i++) {
					if (processors[i].processorID == data[2]) {
						Print("(started = %d\n)", processors[i].started);
					}
				}
			} break;

			case 1: {
				Print("IOAPIC: id %d, address %x, base GSI %d\n", (int) data[2], ((uint32_t *) data)[1], ((uint32_t *) data)[2]);
			} break;

			case 2: {
				Print("Interrupt source override: from %d to %d, flags = %X\n", (int) data[3], ((uint32_t *) data)[1], (int) data[8]);
			} break;

			case 4: {
				Print("NMI: processor %d, lint %d, flags %X\n", (int) data[2], (int) data[5], (int) data[3]);
			} break;

			default: {
				KernelLog(LOG_WARNING, "ACPI::Initialise - Found unknown entry of type %d in MADT\n", entryType);
			} break;
		}

		length -= entryLength;
		data += entryLength;
	}

	while (1);
}

void ACPI::Initialise() {
	ZeroMemory(this, sizeof(ACPI));

	uint64_t uefiRSDP = *((uint64_t *) (LOW_MEMORY_MAP_START + bootloaderInformationOffset + 0x7FE8));

	if (!uefiRSDP) {
		FindRootSystemDescriptorPointer();
	} else {
		rsdp = (RootSystemDescriptorPointer *) kernelVMM.Allocate("ACPI", ACPI_MAX_TABLE_LENGTH, VMM_MAP_ALL, VMM_REGION_PHYSICAL, (uintptr_t) uefiRSDP);
	}

	if (rsdp) {
		if (rsdp->revision == 2 && rsdp->xsdtAddress) {
			isXSDT = true;
			sdt = (ACPIDescriptorTable *) rsdp->xsdtAddress;
		} else {
			isXSDT = false;
			sdt = (ACPIDescriptorTable *) (uintptr_t) rsdp->rsdtAddress;
		}

		sdt = (ACPIDescriptorTable *) kernelVMM.Allocate("ACPI", ACPI_MAX_TABLE_LENGTH, VMM_MAP_ALL, VMM_REGION_PHYSICAL, (uintptr_t) sdt);
	} else {
		KernelPanic("ACPI::Initialise - Could not find supported root system descriptor pointer.\nACPI support is required.\n");
	}

	if (((sdt->signature == SIGNATURE_XSDT && isXSDT) || (sdt->signature == SIGNATURE_RSDT && !isXSDT)) 
			&& sdt->length < ACPI_MAX_TABLE_LENGTH && !SumBytes((uint8_t *) sdt, sdt->length)) {
		tablesCount = (sdt->length - sizeof(ACPIDescriptorTable)) >> (isXSDT ? 3 : 2);

		if (tablesCount >= ACPI_MAX_TABLES || tablesCount < 1) {
			KernelPanic("ACPI::Initialise - The system descriptor table contains an unsupported number of tables (%d).\n", tablesCount);
		} 

		uintptr_t tableListAddress = (uintptr_t) sdt + ACPI_DESCRIPTOR_TABLE_HEADER_LENGTH;

		// KernelLog(LOG_VERBOSE, "ACPI::Initialise - Found %d tables.\n", tablesCount);

		for (uintptr_t i = 0; i < tablesCount; i++) {
			uintptr_t address;

			if (isXSDT) {
				address = ((uint64_t *) tableListAddress)[i];
			} else {
				address = ((uint32_t *) tableListAddress)[i];
			}

			tables[i] = (ACPIDescriptorTable *) kernelVMM.Allocate("ACPI", ACPI_MAX_TABLE_LENGTH, VMM_MAP_ALL, VMM_REGION_PHYSICAL, address);

			// KernelLog(LOG_VERBOSE, "ACPI::Initialise - Found ACPI table '%s'.\n", 4, &tables[i]->signature);

			if (tables[i]->length > ACPI_MAX_TABLE_LENGTH || SumBytes((uint8_t *) tables[i], tables[i]->length)) {
				KernelPanic("ACPI::Initialise - ACPI table %d with signature %s was invalid or unsupported.\n", i, 4, &tables[i]->signature);
			}
		}
	} else {
		KernelPanic("ACPI::Initialise - Could not find a valid or supported system descriptor table.\nACPI support is required.\n");
	}

#ifdef ARCH_X86_64
	{ 
		// Set up the APIC.
		
		ACPIDescriptorTable *header;
		MultipleAPICDescriptionTable *madt = (MultipleAPICDescriptionTable *) FindTable(SIGNATURE_MADT, &header);

		if (!madt) {
			KernelPanic("ACPI::Initialise - Could not find the MADT table.\nThis is required to use the APIC.\n");
		}

		uintptr_t length = header->length - ACPI_DESCRIPTOR_TABLE_HEADER_LENGTH - sizeof(MultipleAPICDescriptionTable);
		uintptr_t startLength = length;
		uint8_t *data = (uint8_t *) (madt + 1);

		lapic.address = (uint32_t volatile *) (LOW_MEMORY_MAP_START + madt->lapicAddress);

		while (length && length <= startLength) {
			uint8_t entryType = data[0];
			uint8_t entryLength = data[1];

			switch (entryType) {
				case 0: {
					// A processor and its LAPIC.
					if ((data[4] & 1) == 0) goto nextEntry;
					ACPIProcessor *processor = processors + processorCount;
					processor->processorID = data[2];
					processor->apicID = data[3];
					processorCount++;
				} break;

				case 1: {
					// An I/O APIC.
					ioApics[ioapicCount].id = data[2];
					ioApics[ioapicCount].address = ((uint32_t *) data)[1];
					ioApics[ioapicCount].ReadRegister(0); // Make sure it's mapped.
					ioApics[ioapicCount].gsiBase = ((uint32_t *) data)[2];
					ioapicCount++;
				} break;

				case 2: {
					// An interrupt source override structure.
					interruptOverrides[interruptOverrideCount].sourceIRQ = data[3];
					interruptOverrides[interruptOverrideCount].gsiNumber = ((uint32_t *) data)[1];
					interruptOverrides[interruptOverrideCount].activeLow = (data[8] & 2) ? true : false;
					interruptOverrides[interruptOverrideCount].levelTriggered = (data[8] & 8) ? true : false;
					interruptOverrideCount++;
				} break;

				case 4: {
					// A non-maskable interrupt.
					lapicNMIs[lapicNMICount].processor = data[2];
					lapicNMIs[lapicNMICount].lintIndex = data[5];
					lapicNMIs[lapicNMICount].activeLow = (data[3] & 2) ? true : false;
					lapicNMIs[lapicNMICount].levelTriggered = (data[3] & 8) ? true : false;
					lapicNMICount++;
				} break;

				default: {
					KernelLog(LOG_WARNING, "ACPI::Initialise - Found unknown entry of type %d in MADT\n", entryType);
				} break;
			}

			nextEntry:
			length -= entryLength;
			data += entryLength;
		}

		if (processorCount > 256 || ioapicCount > 16 || interruptOverrideCount > 256 || lapicNMICount > 32) {
			KernelPanic("ACPI::KernelPanic - Invalid number of processors (%d/%d), \n"
				    "                    I/O APICs (%d/%d), interrupt overrides (%d/%d)\n"
				    "                    and LAPIC NMIs (%d/%d)\n", 
				    processorCount, 256, ioapicCount, 16, interruptOverrideCount, 256, lapicNMICount, 32);
		}

		uint8_t bootstrapLapicID = (lapic.ReadRegister(0x20 >> 2) >> 24); 

		for (uintptr_t i = 0; i < processorCount; i++) {
			if (processors[i].apicID == bootstrapLapicID) {
				// That's us!
				bootstrapProcessor = processors + i;
				bootstrapProcessor->started 
					= bootstrapProcessor->bootstrapProcessor 
					= true;
			}
		}

		if (!bootstrapProcessor) {
			KernelPanic("ACPI::Initialise - Could not find the bootstrap processor\n");
		}

		// TODO Use bootloaderInformationOffset.
#define AP_TRAMPOLINE 0x10000

#ifdef USE_SMP
		// Put the trampoline code in memory.
		CopyMemory((void *) (LOW_MEMORY_MAP_START + AP_TRAMPOLINE),
				(void *) ProcessorAPStartup,
				0x1000); // Assume that the AP trampoline code is 4KB.

		// Put the paging table location at AP_TRAMPOLINE + 0xFF0.
		uint64_t cr3 = ProcessorReadCR3();
		CopyMemory((void *) (LOW_MEMORY_MAP_START + AP_TRAMPOLINE + 0xFF0),
				&cr3, sizeof(cr3));

		// Put the 64-bit GDTR at AP_TRAMPOLINE + 0xFE0.
		CopyMemory((void *) (LOW_MEMORY_MAP_START + AP_TRAMPOLINE + 0xFE0),
				(void *) processorGDTR, 0x10);

		// Put the GDT at AP_TRAMPOLINE + 0x1000.
		CopyMemory((void *) (LOW_MEMORY_MAP_START + AP_TRAMPOLINE + 0x1000),
				(void *) gdt_data, 0x1000);

		// Put the startup flag at AP_TRAMPOLINE + 0xFC0
		uint8_t volatile *startupFlag = (uint8_t *) (LOW_MEMORY_MAP_START + AP_TRAMPOLINE + 0xFC0);
#endif

		// Temporarily identity map a 2 pages in at 0x10000.
		{
			kernelVMM.virtualAddressSpace->lock.Acquire();
			kernelVMM.virtualAddressSpace->Map(0x10000, 0x10000, false);
			kernelVMM.virtualAddressSpace->Map(0x11000, 0x11000, false);
			kernelVMM.virtualAddressSpace->lock.Release();
		} 

		scheduler.processors = 1;

#ifdef USE_SMP
		for (uintptr_t i = 0; i < processorCount; i++) {
			ACPIProcessor *processor = processors + i;

			if (processor->bootstrapProcessor == false) {
				// Clear the startup flag.
				*startupFlag = 0;

				// Put the stack at AP_TRAMPOLINE + 0xFD0.
				void *stack = (void *) ((uintptr_t) kernelVMM.Allocate("StartupStack", 0x4000) + 0x4000);
				CopyMemory((void *) (LOW_MEMORY_MAP_START + AP_TRAMPOLINE + 0xFD0),
						&stack, sizeof(void *));
				// KernelLog(LOG_VERBOSE, "Trampoline stack: %x->%x\n", stack, (uintptr_t) stack + 0x4000);

				// Send an INIT IPI.
				lapic.WriteRegister(0x310 >> 2, processor->apicID << 24);
				lapic.WriteRegister(0x300 >> 2, 0x4500);
				for (uintptr_t i = 0; i < 10; i++) Delay1MS();

				// Send a startup IPI.
				lapic.WriteRegister(0x310 >> 2, processor->apicID << 24);
				lapic.WriteRegister(0x300 >> 2, 0x4600 | (AP_TRAMPOLINE >> PAGE_BITS));
				for (uintptr_t i = 0; i < 100 && *startupFlag == 0; i++) Delay1MS();

				if (*startupFlag) {
					// The processor started correctly.
					processor->started = true;
				} else {
					// Send a startup IPI, again.
					lapic.WriteRegister(0x310 >> 2, processor->apicID << 24);
					lapic.WriteRegister(0x300 >> 2, 0x4600 | (AP_TRAMPOLINE >> PAGE_BITS));
					for (uintptr_t i = 0; i < 1000 && *startupFlag == 0; i++) Delay1MS(); // Wait longer this time.

					if (*startupFlag) {
						// The processor started correctly.
						processor->started = true;
					} else {
						// The processor could not be started.
						KernelLog(LOG_WARNING, "ACPI::Initialise - Could not start processor %d\n", processor->processorID);
						continue;
					}
				}

				for (uintptr_t i = 0; i < 10000 && *startupFlag != 2; i++) Delay1MS();

				if (*startupFlag == 2) {
					// The processor started!
					scheduler.processors++;
				} else {
					// The processor did not report it completed initilisation, worringly.
					// Don't let it continue.

					processor->started = false;
					KernelLog(LOG_WARNING, "ACPI::Initialise - Could not initialise processor %d\n", processor->processorID);

					// TODO Send IPI to stop the processor.
				}
			}
		}
#endif

		SetupProcessor2();
		
		// Remove the identity pages needed for the trampoline code.
		{
			kernelVMM.virtualAddressSpace->lock.Acquire();
			kernelVMM.virtualAddressSpace->Remove(0x10000, 2);
			kernelVMM.virtualAddressSpace->lock.Release();
		}

		// Calibrate the LAPIC's timer and processor's timestamp counter.
		ProcessorDisableInterrupts();
		uint64_t start = ProcessorReadTimeStamp();
		acpi.lapic.WriteRegister(0x380 >> 2, (uint32_t) -1); 
		for (int i = 0; i < 8; i++) Delay1MS(); // Average over 8ms
		acpi.lapic.ticksPerMs = ((uint32_t) -1 - acpi.lapic.ReadRegister(0x390 >> 2)) >> 4;
		osRandomByteSeed ^= acpi.lapic.ReadRegister(0x390 >> 2);
		uint64_t end = ProcessorReadTimeStamp();
		timestampTicksPerMs = (end - start) >> 3;
		ProcessorEnableInterrupts();
	}
#endif
}

void *ACPI::FindTable(uint32_t tableSignature, ACPIDescriptorTable **header) {
	for (uintptr_t i = 0; i < tablesCount; i++) {
		if (tables[i]->signature == tableSignature) {
			if (header) {
				*header = tables[i];
			}

			return (uint8_t *) tables[i] + ACPI_DESCRIPTOR_TABLE_HEADER_LENGTH;
		}
	}

	// The system does not have the ACPI table.
	return nullptr;
}

#ifndef USE_ACPICA
void ACPI::ShutdownComputer() {
	KernelPanic("ACPI::ShutdownComputer - Attempt to shutdown computer without ACPI.\n");
}
#endif

#ifdef USE_ACPICA

// TODO Warning: Not all of the OSL has been tested.

extern "C"  {
#pragma GCC diagnostic ignored "-Wunused-parameter" push
#include "../ports/acpica/include/acpi.h"
#pragma GCC diagnostic pop
}

#define STB_SPRINTF_IMPLEMENTATION
#define STB_SPRINTF_STATIC
#include "../api/stb_sprintf.h"

bool acpiOSLayerActive = false;

OS_EXTERN_C ACPI_STATUS AcpiOsInitialize() {
	if (acpiOSLayerActive) KernelPanic("AcpiOsInitialize - ACPI has already been initialised.\n");
	acpiOSLayerActive = true;
	KernelLog(LOG_VERBOSE, "AcpiOsInitialize - Initialising ACPICA OS layer...\n");
	return AE_OK;
}

OS_EXTERN_C ACPI_STATUS AcpiOsTerminate() {
	if (!acpiOSLayerActive) KernelPanic("AcpiOsTerminate - ACPI has not been initialised.\n");
	acpiOSLayerActive = false;
	KernelLog(LOG_VERBOSE, "AcpiOsTerminate - Terminating ACPICA OS layer...\n");
	return AE_OK;
}

OS_EXTERN_C ACPI_PHYSICAL_ADDRESS AcpiOsGetRootPointer() {
	ACPI_PHYSICAL_ADDRESS address = 0;

	uint64_t uefiRSDP = *((uint64_t *) (LOW_MEMORY_MAP_START + bootloaderInformationOffset + 0x7FE8));

	if (uefiRSDP) {
		return uefiRSDP;
	}

	AcpiFindRootPointer(&address);
	return address;
}

OS_EXTERN_C ACPI_STATUS AcpiOsPredefinedOverride(const ACPI_PREDEFINED_NAMES *predefinedObject, ACPI_STRING *newValue) {
	(void) predefinedObject;
	*newValue = nullptr;
	return AE_OK;
}

OS_EXTERN_C ACPI_STATUS AcpiOsTableOverride(ACPI_TABLE_HEADER *existingTable, ACPI_TABLE_HEADER **newTable) {
	(void) existingTable;
	*newTable = nullptr;
	return AE_OK;
}

OS_EXTERN_C ACPI_STATUS AcpiOsPhysicalTableOverride(ACPI_TABLE_HEADER *existingTable, ACPI_PHYSICAL_ADDRESS *newAddress, uint32_t *newTableLength) {
	(void) existingTable;
	*newAddress = 0;
	*newTableLength = 0;
	return AE_OK;
}

OS_EXTERN_C void *AcpiOsMapMemory(ACPI_PHYSICAL_ADDRESS physicalAddress, ACPI_SIZE length) {
#ifdef ARCH_X86_64
	if ((uintptr_t) physicalAddress + (uintptr_t) length < (uintptr_t) 0x100000000) {
		return (void *) (LOW_MEMORY_MAP_START + physicalAddress);
	}
#endif

	void *address = kernelVMM.Allocate("acpica", length, 
			VMM_MAP_ALL, VMM_REGION_PHYSICAL, 
			physicalAddress, VMM_REGION_FLAG_NOT_CACHABLE);
	return address;
}

OS_EXTERN_C void AcpiOsUnmapMemory(void *address, ACPI_SIZE length) {
#ifdef ARCH_X86_64
	if ((uintptr_t) address - (uintptr_t) LOW_MEMORY_MAP_START < (uintptr_t) 0x100000000) {
		return;
	}
#endif

	(void) length;
	kernelVMM.Free(address);
}

OS_EXTERN_C ACPI_STATUS AcpiOsGetPhysicalAddress(void *virtualAddress, ACPI_PHYSICAL_ADDRESS *physicalAddress) {
	if (!virtualAddress || !physicalAddress) {
		return AE_BAD_PARAMETER;
	}

	kernelVMM.virtualAddressSpace->lock.Acquire();
	*physicalAddress = kernelVMM.virtualAddressSpace->Get((uintptr_t) virtualAddress);
	kernelVMM.virtualAddressSpace->lock.Release();
	
	return AE_OK;
}

OS_EXTERN_C void *AcpiOsAllocate(ACPI_SIZE size) {
	return OSHeapAllocate(size, false);
}

OS_EXTERN_C void AcpiOsFree(void *memory) {
	OSHeapFree(memory);
}

OS_EXTERN_C BOOLEAN AcpiOsReadable(void *memory, ACPI_SIZE length) {
	(void) memory;
	(void) length;
	// This is only used by the debugger, which we don't use...
	return TRUE;
}

OS_EXTERN_C BOOLEAN AcpiOsWritable(void *memory, ACPI_SIZE length) {
	(void) memory;
	(void) length;
	// This is only used by the debugger, which we don't use...
	return TRUE;
}

OS_EXTERN_C ACPI_THREAD_ID AcpiOsGetThreadId() {
	return GetCurrentThread()->id;
}

Thread *acpiEvents[256];
size_t acpiEventCount;

struct ACPICAEvent {
	ACPI_OSD_EXEC_CALLBACK function;
	void *context;
};

void RunACPICAEvent(void *e) {
	ACPICAEvent *event = (ACPICAEvent *) e;
	event->function(event->context);
	OSHeapFree(event);
	scheduler.TerminateThread(GetCurrentThread());
}

OS_EXTERN_C ACPI_STATUS AcpiOsExecute(ACPI_EXECUTE_TYPE type, ACPI_OSD_EXEC_CALLBACK function, void *context) {
	(void) type;

	if (!function) return AE_BAD_PARAMETER;

	ACPICAEvent *event = (ACPICAEvent *) OSHeapAllocate(sizeof(ACPICAEvent), true);
	event->function = function;
	event->context = context;

	Thread *thread = scheduler.SpawnThread((uintptr_t) RunACPICAEvent, (uintptr_t) event, kernelProcess, false);

	if (acpiEventCount == 256) {
		KernelPanic("AcpiOsExecute - Exceeded maximum event count, 256.\n");
	}

	acpiEvents[acpiEventCount++] = thread;
	return AE_OK;
}

OS_EXTERN_C void AcpiOsSleep(UINT64 ms) {
	Timer timer = {};
	timer.Set(ms, true);
	timer.event.Wait(ms);
	timer.Remove();
}

OS_EXTERN_C void AcpiOsStall(UINT32 mcs) {
	(void) mcs;
	uint64_t start = ProcessorReadTimeStamp();
	uint64_t end = start + mcs * (acpi.timestampTicksPerMs / 1000);
	while (ProcessorReadTimeStamp() < end);
}

OS_EXTERN_C void AcpiOsWaitEventsComplete() {
	for (uintptr_t i = 0; i < acpiEventCount; i++) {
		Thread *thread = acpiEvents[i];
		thread->killedEvent.Wait(OS_WAIT_NO_TIMEOUT);
		CloseHandleToObject(thread, KERNEL_OBJECT_THREAD);
	}

	acpiEventCount = 0;
}

OS_EXTERN_C ACPI_STATUS AcpiOsCreateSemaphore(UINT32 maxUnits, UINT32 initialUnits, ACPI_SEMAPHORE *handle) {
	if (!handle) return AE_BAD_PARAMETER;

	Semaphore *semaphore = (Semaphore *) OSHeapAllocate(sizeof(Semaphore), true);
	semaphore->Return(initialUnits);
	semaphore->_custom = maxUnits;
	*handle = semaphore;
	return AE_OK;
}

OS_EXTERN_C ACPI_STATUS AcpiOsDeleteSemaphore(ACPI_SEMAPHORE handle) {
	if (!handle) return AE_BAD_PARAMETER;
	OSHeapFree(handle, sizeof(Semaphore));
	return AE_OK;
}

OS_EXTERN_C ACPI_STATUS AcpiOsWaitSemaphore(ACPI_SEMAPHORE handle, UINT32 units, UINT16 timeout) {
	(void) timeout;
	if (!handle) return AE_BAD_PARAMETER;
	Semaphore *semaphore = (Semaphore *) handle;

	if (semaphore->Take(units, timeout == (UINT16) -1 ? OS_WAIT_NO_TIMEOUT : timeout)) {
		return AE_OK;
	} else {
		return AE_TIME;
	}
}

OS_EXTERN_C ACPI_STATUS AcpiOsSignalSemaphore(ACPI_SEMAPHORE handle, UINT32 units) {
	if (!handle) return AE_BAD_PARAMETER;
	Semaphore *semaphore = (Semaphore *) handle;
	if (semaphore->units + units > semaphore->_custom) return AE_LIMIT;
	semaphore->Return(units);
	return AE_OK;
}

OS_EXTERN_C ACPI_STATUS AcpiOsCreateLock(ACPI_SPINLOCK *handle) {
	if (!handle) return AE_BAD_PARAMETER;
	Spinlock *spinlock = (Spinlock *) OSHeapAllocate(sizeof(Spinlock), true);
	*handle = spinlock;
	return AE_OK;
}

OS_EXTERN_C void AcpiOsDeleteLock(ACPI_HANDLE handle) {
	OSHeapFree(handle, sizeof(Spinlock));
}

OS_EXTERN_C ACPI_CPU_FLAGS AcpiOsAcquireLock(ACPI_SPINLOCK handle) {
	Spinlock *spinlock = (Spinlock *) handle;
	spinlock->Acquire();
	return 0;
}

OS_EXTERN_C void AcpiOsReleaseLock(ACPI_SPINLOCK handle, ACPI_CPU_FLAGS flags) {
	(void) flags;
	Spinlock *spinlock = (Spinlock *) handle;
	spinlock->Release();
}

ACPI_OSD_HANDLER acpiInterruptHandlers[256];
void *acpiInterruptContexts[256];

bool ACPIInterrupt(uintptr_t interruptIndex) {
	if (acpiInterruptHandlers[interruptIndex]) {
		return ACPI_INTERRUPT_HANDLED == acpiInterruptHandlers[interruptIndex](acpiInterruptContexts[interruptIndex]);
	} else {
		return false;
	}
}

OS_EXTERN_C ACPI_STATUS AcpiOsInstallInterruptHandler(UINT32 interruptLevel, ACPI_OSD_HANDLER handler, void *context) {
	if (interruptLevel > 256 || !handler) return AE_BAD_PARAMETER;
	
	if (acpiInterruptHandlers[interruptLevel]) {
		return AE_ALREADY_EXISTS;
	}

	acpiInterruptHandlers[interruptLevel] = handler;
	acpiInterruptContexts[interruptLevel] = context;

	return RegisterIRQHandler(interruptLevel, ACPIInterrupt) ? AE_OK : AE_ERROR;
}

OS_EXTERN_C ACPI_STATUS AcpiOsRemoveInterruptHandler(UINT32 interruptNumber, ACPI_OSD_HANDLER handler) {
	if (interruptNumber > 256 || !handler) return AE_BAD_PARAMETER;

	if (!acpiInterruptHandlers[interruptNumber]) {
		return AE_NOT_EXIST;
	}

	if (handler != acpiInterruptHandlers[interruptNumber]) {
		return AE_BAD_PARAMETER;
	}

	acpiInterruptHandlers[interruptNumber] = nullptr;

	return AE_OK;
}

uint8_t acpicaPageBuffer[PAGE_SIZE];
Mutex acpicaPageBufferMutex;

OS_EXTERN_C ACPI_STATUS AcpiOsReadMemory(ACPI_PHYSICAL_ADDRESS address, UINT64 *value, UINT32 width) {
	acpicaPageBufferMutex.Acquire();
	Defer(acpicaPageBufferMutex.Release());

	uintptr_t page = (uintptr_t) address & ~(PAGE_SIZE - 1);
	uintptr_t offset = (uintptr_t) address & (PAGE_SIZE - 1);

	ReadPhysicalMemory(page, acpicaPageBuffer, 1);
	
	if (width == 64) {
		*value = *((uint64_t *) (acpicaPageBuffer + offset));
	} else if (width == 32) {
		*value = *((uint32_t *) (acpicaPageBuffer + offset));
	} else if (width == 16) {
		*value = *((uint16_t *) (acpicaPageBuffer + offset));
	} else {
		*value = acpicaPageBuffer[offset];
	}

	return AE_OK;
}

OS_EXTERN_C ACPI_STATUS AcpiOsWriteMemory(ACPI_PHYSICAL_ADDRESS address, UINT64 value, UINT32 width) {
	acpicaPageBufferMutex.Acquire();
	Defer(acpicaPageBufferMutex.Release());

	uintptr_t page = (uintptr_t) address & ~(PAGE_SIZE - 1);
	uintptr_t offset = (uintptr_t) address & (PAGE_SIZE - 1);

	ReadPhysicalMemory(page, acpicaPageBuffer, 1);
	
	if (width == 64) {
		*((uint64_t *) (acpicaPageBuffer + offset)) = value;
	} else if (width == 32) {
		*((uint32_t *) (acpicaPageBuffer + offset)) = value;
	} else if (width == 16) {
		*((uint16_t *) (acpicaPageBuffer + offset)) = value;
	} else {
		*((uint8_t *) (acpicaPageBuffer + offset)) = value;
	}

	CopyIntoPhysicalMemory(page, acpicaPageBuffer, 1);

	return AE_OK;
}

OS_EXTERN_C ACPI_STATUS AcpiOsReadPort(ACPI_IO_ADDRESS address, UINT32 *value, UINT32 width) {
	if (width == 8) {
		*value = ProcessorIn8(address);
	} else if (width == 16) {
		*value = ProcessorIn16(address);
	} else if (width == 32) {
		*value = ProcessorIn32(address);
	} else {
		return AE_ERROR;
	}

	return AE_OK;
}

OS_EXTERN_C ACPI_STATUS AcpiOsWritePort(ACPI_IO_ADDRESS address, UINT32 value, UINT32 width) {
	if (width == 8) {
		ProcessorOut8(address, (uint8_t) value);
	} else if (width == 16) {
		ProcessorOut16(address, (uint16_t) value);
	} else if (width == 32) {
		ProcessorOut32(address, (uint32_t) value);
	} else {
		return AE_ERROR;
	}

	return AE_OK;
}

OS_EXTERN_C ACPI_STATUS AcpiOsReadPciConfiguration(ACPI_PCI_ID *address, UINT32 reg, UINT64 *value, UINT32 width) {
	if (width == 64) {
		uint64_t x = (uint64_t) pci.ReadConfig(address->Bus, address->Device, address->Function, reg)
			| ((uint64_t) pci.ReadConfig(address->Bus, address->Device, address->Function, reg + 4) << 32);
		*value = x;
	} else {
		uint32_t x = pci.ReadConfig(address->Bus, address->Device, address->Function, reg & ~3);
		x >>= (reg & 3) * 8;

		if (width == 8) x &= 0xFF;
		if (width == 16) x &= 0xFFFF;

		*value = x;
	}

	return AE_OK;
}

OS_EXTERN_C ACPI_STATUS AcpiOsWritePciConfiguration(ACPI_PCI_ID *address, UINT32 reg, UINT64 value, UINT32 width) {
	if (width == 64) {
		pci.WriteConfig(address->Bus, address->Device, address->Function, reg, value);
		pci.WriteConfig(address->Bus, address->Device, address->Function, reg + 4, value >> 32);
	} else if (width == 32) {
		pci.WriteConfig(address->Bus, address->Device, address->Function, reg, value);
	} else {
		uint32_t x = pci.ReadConfig(address->Bus, address->Device, address->Function, reg & ~3);
		uint32_t o = reg & 3;

		if (width == 16) {
			if (o == 2) {
				x = (x & ~0xFFFF0000) | (value << 16);
			} else {
				x = (x & ~0x0000FFFF) | (value << 0);
			}
		} else if (width == 8) {
			if (o == 3) {
				x = (x & ~0xFF000000) | (value << 24);
			} else if (o == 2) {
				x = (x & ~0x00FF0000) | (value << 16);
			} else if (o == 1) {
				x = (x & ~0x0000FF00) | (value << 8);
			} else {
				x = (x & ~0x000000FF) | (value << 0);
			}
		}

		pci.WriteConfig(address->Bus, address->Device, address->Function, reg & ~3, x);
	}

	return AE_OK;
}

char acpiPrintf[65536];

#if 1
#define ENABLE_ACPICA_OUTPUT
#endif

OS_EXTERN_C void AcpiOsPrintf(const char *format, ...) {
	va_list arguments;
	va_start(arguments, format);
	int x = stbsp_vsnprintf(acpiPrintf, 65536, format, arguments);
#ifdef ENABLE_ACPICA_OUTPUT
	Print("%s", x, acpiPrintf);
#else 
	(void) x;
#endif
	va_end(arguments);
}

OS_EXTERN_C void AcpiOsVprintf(const char *format, va_list arguments) {
	int x = stbsp_vsnprintf(acpiPrintf, 65536, format, arguments);
#ifdef ENABLE_ACPICA_OUTPUT
	Print("%s", x, acpiPrintf);
#else 
	(void) x;
#endif
}

OS_EXTERN_C UINT64 AcpiOsGetTimer() {
	uint64_t tick = ProcessorReadTimeStamp();
	uint64_t ticksPerMs = acpi.timestampTicksPerMs;
	uint64_t ticksPer100Ns = ticksPerMs / 1000 / 10;
	if (ticksPer100Ns == 0) return tick;
	return tick / ticksPer100Ns;
}

OS_EXTERN_C ACPI_STATUS AcpiOsSignal(UINT32 function, void *information) {
	(void) function;
	(void) information;
	KernelPanic("AcpiOsSignal - ACPI requested kernel panic.\n");
	return AE_OK;
}

OS_EXTERN_C ACPI_STATUS AcpiOsEnterSleep(UINT8 sleepState, UINT32 registerAValue, UINT32 registerBValue) {
	(void) sleepState;
	(void) registerAValue;
	(void) registerBValue;
	return AE_OK;
}

void ACPI::ShutdownComputer() {
	AcpiEnterSleepStatePrep(5);
	ProcessorDisableInterrupts();
	AcpiEnterSleepState(5);
}

void ACPIPowerButtonPressed2(void *context) {
	(void) context;
	OSMessage message = {};
	message.type = OS_MESSAGE_POWER_BUTTON_PRESSED;
	desktopProcess->messageQueue.SendMessage(message);
}

UINT32 ACPIPowerButtonPressed(void *context) {
	(void) context;
	RegisterAsyncTask2(ACPIPowerButtonPressed2, nullptr);
	return 0;
}

#endif

void ACPI::Initialise2() {
#ifdef USE_ACPICA
	AcpiInitializeSubsystem();
	AcpiInitializeTables(nullptr, 256, true);
	AcpiLoadTables();
	AcpiEnableSubsystem(ACPI_FULL_INITIALIZATION);
	AcpiInitializeObjects(ACPI_FULL_INITIALIZATION);

	if (AE_OK == AcpiEnableEvent(ACPI_EVENT_POWER_BUTTON, 0)) {
		AcpiInstallFixedEventHandler(ACPI_EVENT_POWER_BUTTON, ACPIPowerButtonPressed, nullptr);
	}
#endif
}

#endif
