#ifndef IMPLEMENTATION

#define AHCI_TIMEOUT (1000)
#define AHCI_SECTOR_SIZE (512)

enum AHCIPacketType {
	AHCI_PACKET_TYPE_HOST_TO_DEVICE = 0x27,
	AHCI_PACKET_TYPE_DEVICE_TO_HOST = 0x34,
	AHCI_PACKET_TYPE_DMA_ACTIVATE = 0x39,
	AHCI_PACKET_TYPE_DMA_SETUP = 0x41,
	AHCI_PACKET_TYPE_DATA = 0x46,
	AHCI_PACKET_TYPE_BIST = 0x58,
	AHCI_PACKET_TYPE_PIO_SETUP = 0x5F,
	AHCI_PACKET_TYPE_SET_DEVICE_BITS = 0xA1,
};

struct AHCIPacketHostToDevice {
	volatile uint8_t packetType;
	volatile uint8_t portMultiplier:4;
	volatile uint8_t _reserved0:3;
	volatile uint8_t controlOrCommand:1;
	volatile uint8_t command;
	volatile uint8_t featureLow;
	
	volatile uint8_t lba0;	
	volatile uint8_t lba1;	
	volatile uint8_t lba2;	
	volatile uint8_t device;
	
	volatile uint8_t lba3;	
	volatile uint8_t lba4;	
	volatile uint8_t lba5;	
	volatile uint8_t featureHigh;
	
	volatile uint8_t countLow;
	volatile uint8_t countHigh;
	volatile uint8_t icc;
	volatile uint8_t control;
	
	volatile uint8_t _reserved1[4];
};

struct AHCIPacketDeviceToHost {
	volatile uint8_t packetType;
	volatile uint8_t portMultiplier:4;
	volatile uint8_t _reserved0:2;
	volatile uint8_t interrupt:1;
	volatile uint8_t _reserved1:1;
	volatile uint8_t status;
	volatile uint8_t error;
	
	volatile uint8_t lba0;	
	volatile uint8_t lba1;	
	volatile uint8_t lba2;	
	volatile uint8_t device;
	
	volatile uint8_t lba3;	
	volatile uint8_t lba4;	
	volatile uint8_t lba5;	
	volatile uint8_t _reserved2;
	
	volatile uint8_t countLow;
	volatile uint8_t countHigh;
	volatile uint8_t _reserved3[2];
	
	volatile uint8_t _reserved4[4];
};

struct AHCIPacketData {
	volatile uint8_t packetType;
	volatile uint8_t portMultiplier:4;
	volatile uint8_t _reserved0:4;
	volatile uint8_t _reserved1[2];

	// Data follows.
};

struct AHCIPacketPIOSetup {
	volatile uint8_t packetType;
	volatile uint8_t portMultiplier:4;
	volatile uint8_t _reserved0:1;
	volatile uint8_t transferDirection:1;
	volatile uint8_t interrupt:1;
	volatile uint8_t _reserved1:1;
	volatile uint8_t status;
	volatile uint8_t error;
	
	volatile uint8_t lba0;	
	volatile uint8_t lba1;	
	volatile uint8_t lba2;	
	volatile uint8_t device;
	
	volatile uint8_t lba3;	
	volatile uint8_t lba4;	
	volatile uint8_t lba5;	
	volatile uint8_t _reserved2;
	
	volatile uint8_t countLow;
	volatile uint8_t countHigh;
	volatile uint8_t _reserved3;
	volatile uint8_t newStatus;
	
	volatile uint16_t transferCount;
	volatile uint8_t _reserved4[2];
};

struct AHCIPacketDMASetup {
	volatile uint8_t packetType;
	volatile uint8_t portMultiplier:4;
	volatile uint8_t _reserved0:1;
	volatile uint8_t transferDirection:1;
	volatile uint8_t interrupt:1;
	volatile uint8_t autoActivate:1;
	volatile uint8_t _reserved1[2];
	
	volatile uint32_t dmaBufferIDLow;
	volatile uint32_t dmaBufferIDHigh;
	volatile uint32_t _reserved2;
	volatile uint32_t dmaBufferOffset;
	volatile uint32_t transferCount;
	volatile uint32_t _reserved3;
};

struct AHCIPort {
	volatile uint32_t commandListBaseAddressLow;
	volatile uint32_t commandListBaseAddressHigh;
	volatile uint32_t fisBaseAddressLow;
	volatile uint32_t fisBaseAddressHigh;
	volatile uint32_t interruptStatus;
	volatile uint32_t interruptEnable;

	union {
		volatile uint32_t command;
		volatile uint32_t status;
	};

	volatile uint32_t _reserved0;
	volatile uint32_t taskFileData;
	volatile uint32_t signature;
	volatile uint32_t sataStatus;
	volatile uint32_t sataControl;
	volatile uint32_t sataError;
	volatile uint32_t sataActive;
	volatile uint32_t commandIssue;
	volatile uint32_t sataNotification;
	volatile uint32_t fisSwitchControl;
	volatile uint32_t _reserved1[11];
	volatile uint32_t _reserved2[4];
};

struct AHCIReceivedPacket {
	volatile AHCIPacketDMASetup dmaSetup;
	volatile uint8_t _pad0[4];
	
	volatile AHCIPacketPIOSetup pioSetup;
	volatile uint8_t _pad1[12];
	
	volatile AHCIPacketDeviceToHost deviceToHost;
	volatile uint8_t _pad2[4];
	
	volatile uint8_t sdbfis[8];
	volatile uint8_t ufis[64];
	volatile uint8_t _reserved[0x100 - 0xA0];
};

struct AHCICommandHeader {
	volatile uint8_t commandLength:5;
	volatile uint8_t atapi:1;
	volatile uint8_t write:1;
	volatile uint8_t prefetchable:1;
	volatile uint8_t reset:1;
	volatile uint8_t bist:1;
	volatile uint8_t clearBusy:1;
	volatile uint8_t _reserved0:1;
	volatile uint8_t portMultiplier:4;
	volatile uint16_t prdEntryCount;
	
	volatile uint32_t prdByteCount;
	
	volatile uint32_t commandTableDescriptorLow;
	volatile uint32_t commandTableDescriptorHigh;
	
	volatile uint32_t _reserved1[4];
};

struct AHCIPRDTEntry {
	volatile uint32_t targetAddressLow;
	volatile uint32_t targetAddressHigh;
	volatile uint32_t _reserved0;
	volatile uint32_t byteCount:22;
	volatile uint32_t _reserved1:9;
	volatile uint32_t interruptOnCompletion:1;
};

struct AHCICommandTable {
	volatile uint8_t commandPacket[64];
	volatile uint8_t atapiCommand[16];
	volatile uint8_t _reserved0[48];
	volatile AHCIPRDTEntry prdtEntries[8];
};

struct AHCIHBA {
	volatile uint32_t capabilities;
	volatile uint32_t globalHostControl;
	volatile uint32_t interruptStatus;
	volatile uint32_t portImplemented;
	volatile uint32_t version;
	volatile uint32_t commandCompletionCoalescingControl;
	volatile uint32_t commandCompletionCoalescingPorts;
	volatile uint32_t enclosureManagementLocation;
	volatile uint32_t enclosureManagementControl;
	volatile uint32_t capabilitiesExtended;
	volatile uint32_t biosHandoffStatus;
	
	volatile uint8_t _reserved0[0xA0 - 0x2C];
	volatile uint8_t _reserved1[0x100 - 0xA0];
	
	volatile AHCIPort ports[32];
};

struct AHCIOperation {
	struct IOPacket *ioPacket;
	struct AHCIPresentDrive *drive;
	uintptr_t commandIndex;
	int operation;
	void *buffer, *_buffer;
	uintptr_t offsetIntoSector;
	size_t countBytes;
};

struct AHCIPresentDrive {
	// In theory we could use 32, but we only use 16.
	// To change this, some uint16_t would need changing to uint32_ts.
#define AHCI_COMMAND_COUNT (16)

	uintptr_t port;
	bool atapi;
	AHCICommandHeader *commandList;
	AHCIReceivedPacket *receivedPacket;
	AHCICommandTable *commandTable;
	volatile uint16_t commandsInUse; // Bitset.
	volatile uint16_t commandsReady; // Bitset.
	Mutex mutex;
	struct Device *device;
	uint64_t sectorCount;
	Event completeCommands[AHCI_COMMAND_COUNT];
	Timer timeout;

	uintptr_t physicalBuffers[AHCI_COMMAND_COUNT];
	void *buffers[AHCI_COMMAND_COUNT];

	AHCIOperation operations[AHCI_COMMAND_COUNT];

	LinkedList<struct AHCIBlockedOperation> blockedPackets;
	Semaphore available; // The number of available commands.
};

struct AHCIBlockedOperation {
	struct IOPacket *packet;
	uintptr_t drive;
	uint64_t offset;
	size_t countBytes;
	int operation;
	uint8_t *_buffer;

	LinkedItem<AHCIBlockedOperation> item;
};

struct AHCIDriver {
	void Initialise();
	bool Access(struct IOPacket *packet, uintptr_t drive, uint64_t sector, size_t count, int operation, uint8_t *buffer); // Returns true on success.
	void Unblock(AHCIPresentDrive *drive);

	bool foundController;

	struct PCIDevice *device;

	AHCIPresentDrive drives[32];
	size_t driveCount;

	AHCIHBA *r;
	Mutex blockedPacketsMutex;
};

AHCIDriver ahci;

#else

#define AHCI_DRIVER_IDENTFIY (2)

bool AHCIFinishOperation(void *argument) {
	AHCIOperation *operation = (AHCIOperation *) argument;
	AHCIPresentDrive *drive = operation->drive;

	IOPacket *packet = operation->ioPacket;
	bool cancelled = false;

	bool result = false;

	if (packet) {
		packet->request->mutex.Acquire();
		cancelled = packet->cancelled;
	}

	volatile AHCIPort *port = ahci.r->ports + drive->port;
	Semaphore &semaphore = drive->available;

	uint8_t ataStatus = drive->receivedPacket[operation->commandIndex].deviceToHost.status;

	if (port->commandIssue & (1 << operation->commandIndex)) {
		KernelLog(LOG_WARNING, "AHCIDriver::Access - Could not read from drive (1).\n");
		goto finish;
	}

	if (port->interruptStatus & (1 << 30)) {
		KernelLog(LOG_WARNING, "AHCIDriver::Access - Could not read from drive (3).\n");
		goto finish;
	}

	if (ataStatus & 1) {
		KernelLog(LOG_WARNING, "AHCIDriver::Access - Could not read from drive (2, %x).\n", drive->receivedPacket[operation->commandIndex].deviceToHost.error);
		goto finish;
	}

	// Copy to the output buffer.
	if (operation->operation != DRIVE_ACCESS_WRITE && !cancelled) {
		CopyMemory(operation->_buffer, (uint8_t *) operation->buffer + operation->offsetIntoSector, operation->countBytes);
	}

	result = true;
	finish:;

	if (packet) {
		IORequest *request = packet->request;

		ahci.Unblock(drive);
		packet->driverRunning = false;

		if (!result) {
			request->Cancel(OS_ERROR_UNKNOWN_OPERATION_FAILURE);
			// `packet` has been deallocated.
		} else {
			packet->Complete(result ? OS_SUCCESS : OS_ERROR_UNKNOWN_OPERATION_FAILURE);
		}

		request->mutex.Release();
	}

	// KernelLog(LOG_VERBOSE, "AHCIDriver::Access - Command %d complete with result %d.\n", operation->commandIndex, result);

	ahci.blockedPacketsMutex.Acquire();
	drive->commandsInUse &= ~(1 << operation->commandIndex);
	semaphore.Return(1);
	ahci.blockedPacketsMutex.Release();

	return result;
}

void AHCIDriver::Unblock(AHCIPresentDrive *drive) {
	AHCIBlockedOperation *operation = nullptr;

	blockedPacketsMutex.Acquire();

	if (drive->blockedPackets.firstItem) {
		operation = drive->blockedPackets.firstItem->thisItem;
		drive->blockedPackets.Remove(drive->blockedPackets.firstItem);
		operation->packet->driverTemp = nullptr;
	}

	blockedPacketsMutex.Release();

	if (operation && !Access(operation->packet, operation->drive, operation->offset, operation->countBytes, operation->operation, operation->_buffer)) {
		operation->packet->request->Cancel(OS_ERROR_UNKNOWN_OPERATION_FAILURE);
	}

	OSHeapFree(operation);
}

bool AHCIDriver::Access(IOPacket *ioPacket, uintptr_t _drive, uint64_t offset, size_t countBytes, int operation, uint8_t *_buffer) {
	uint64_t sector = offset / 512;
	uint64_t offsetIntoSector = offset % 512;
	uint64_t sectorsNeededToLoad = (countBytes + offsetIntoSector + 511) / 512;

	AHCIPresentDrive *drive = drives + _drive;
	volatile AHCIPort *port = r->ports + drive->port;
	Semaphore &semaphore = drive->available;

	// Find a command we can use.
	uintptr_t commandIndex;

	if (ioPacket) {
		ioPacket->driverRunning = true; 
		blockedPacketsMutex.Acquire();

		if (semaphore.units == 0) {
			AHCIBlockedOperation *op = (AHCIBlockedOperation *) OSHeapAllocate(sizeof(AHCIBlockedOperation), true);
			ioPacket->driverTemp = op;
			op->packet = ioPacket;
			op->drive = _drive;
			op->offset = offset;
			op->countBytes = countBytes;
			op->operation = operation;
			op->_buffer = _buffer;
			op->item.thisItem = op;
			drive->blockedPackets.InsertEnd(&op->item);
			blockedPacketsMutex.Release();
			return true;
		}
	} else {
		while (true) {
			semaphore.available.Wait(OS_WAIT_NO_TIMEOUT);
			blockedPacketsMutex.Acquire();

			if (semaphore.units) {
				break;
			}

			blockedPacketsMutex.Release();
		}
	}

	semaphore.Take(1);

	for (commandIndex = 0; commandIndex < AHCI_COMMAND_COUNT; commandIndex++) {
		if (!(drive->commandsInUse & (1 << commandIndex))) {
			drive->commandsReady &= ~(1 << commandIndex);
			drive->commandsInUse |= 1 << commandIndex;
			break;
		}
	}

	blockedPacketsMutex.Release();

	drive->mutex.Acquire();

	AHCICommandHeader *header = drive->commandList + commandIndex;
	AHCICommandTable *table = drive->commandTable + commandIndex;

	size_t prdtEntries = ((countBytes - 1) >> 22) + 1; 

	header->commandLength = sizeof(AHCIPacketDeviceToHost) / sizeof(uint32_t);
	header->write = operation == DRIVE_ACCESS_WRITE;
	header->prdEntryCount = prdtEntries;

	uintptr_t physicalBuffer = drive->physicalBuffers[commandIndex];
	void *buffer = drive->buffers[commandIndex];

	{
		AHCIOperation *op = drive->operations + commandIndex;
		op->ioPacket = ioPacket;
		op->drive = drive;
		op->commandIndex = commandIndex;
		op->operation = operation;
		op->buffer = buffer;
		op->_buffer = _buffer;
		op->offsetIntoSector = offsetIntoSector;
		op->countBytes = countBytes;
	}

	// Copy to the input buffer.
	if (operation == DRIVE_ACCESS_WRITE) {
		CopyMemory((uint8_t *) buffer + offsetIntoSector, _buffer, countBytes);
	}

	// Configure the PRDT.

	{
		uintptr_t i;
		uintptr_t countRemaining = countBytes + offsetIntoSector;

		for (i = 0; i < (size_t) (header->prdEntryCount - 1); i++) {
			table->prdtEntries[i].targetAddressLow = (uint32_t) ((physicalBuffer + i * 4194304) >> 0);
			table->prdtEntries[i].targetAddressHigh = (uint32_t) ((physicalBuffer + i * 4194304) >> 32);
			table->prdtEntries[i].byteCount = 4194304 - 1;
			table->prdtEntries[i].interruptOnCompletion = false; // Only interrupt on the last descriptor.
			countRemaining -= 4194304;
		}

		table->prdtEntries[i].targetAddressLow = (uint32_t) ((physicalBuffer + i * 4194304) >> 0);
		table->prdtEntries[i].targetAddressHigh = (uint32_t) ((physicalBuffer + i * 4194304) >> 32);
		table->prdtEntries[i].byteCount = ((countRemaining + 511) & ~0x1FF) - 1;
		table->prdtEntries[i].interruptOnCompletion = true;
	}

	AHCIPacketHostToDevice *packet = (AHCIPacketHostToDevice *) table->commandPacket;
	packet->packetType = AHCI_PACKET_TYPE_HOST_TO_DEVICE;
	packet->controlOrCommand = 1;
	packet->device = 1 << 6;

	switch (operation) {
		case AHCI_DRIVER_IDENTFIY: {
			packet->command = ATA_IDENTIFY;
		} break;

		case DRIVE_ACCESS_READ: {
			packet->command = ATA_READ_DMA_48;
		} break;

		case DRIVE_ACCESS_WRITE: {
			packet->command = ATA_WRITE_DMA_48;
		} break;
	}

	packet->countLow = (uint8_t) (sectorsNeededToLoad >> 0);
	packet->countHigh = (uint8_t) (sectorsNeededToLoad >> 8);

	packet->lba0 = (uint8_t) (sector >> 0);
	packet->lba1 = (uint8_t) (sector >> 8);
	packet->lba2 = (uint8_t) (sector >> 16);
	packet->lba3 = (uint8_t) (sector >> 24);
	packet->lba4 = (uint8_t) (sector >> 32);
	packet->lba5 = (uint8_t) (sector >> 40);

	{
		Timer *timeout = &drive->timeout;
		timeout->Set(AHCI_TIMEOUT, false);
		Defer(timeout->Remove());

		while (port->taskFileData & ((1 << 7) | (1 << 3)) && !timeout->event.Poll());
		if (timeout->event.Poll()) return false;
	}

	// static int requestIndex = 0;
	// KernelLog(LOG_VERBOSE, "AHCIDriver::Access - Issuing command %d (%x/%x). requestIndex = %d.\n", commandIndex, sector, countBytes, requestIndex++);

	// Clear the received packet information.
	ZeroMemory(drive->receivedPacket + commandIndex, sizeof(AHCIReceivedPacket));

	// Issue the command.
	drive->completeCommands[commandIndex].Reset();
	port->commandIssue = 1 << commandIndex; 
	drive->commandsReady |= 1 << commandIndex;
	drive->mutex.Release();

	if (ioPacket) {
		// The command has been successfully queued to the packet.
		return true;
	}

	// Wait for the command to complete.
	bool timeout = drive->completeCommands[commandIndex].Wait(AHCI_TIMEOUT);

	if (port->commandIssue & (1 << commandIndex)) {
		KernelLog(LOG_WARNING, "AHCIDriver::Access - Could not read from drive %d (1/%d).\n", _drive, timeout);
		return false;
	}

	// KernelLog(LOG_VERBOSE, "AHCIDriver::Access - Command %d complete.\n", commandIndex);

	AHCIFinishOperation(drive->operations + commandIndex);
	Unblock(drive);

	return true;
}

bool AHCIIRQHandler(uintptr_t interruptIndex) {
	(void) interruptIndex;

	uint32_t pendingInterrupts = ahci.r->interruptStatus;
	Defer(ahci.r->interruptStatus = pendingInterrupts);

	bool switchThread = false;

	if (pendingInterrupts == 0) {
		KernelLog(LOG_WARNING, "AHCIIRQHandler - Received more interrupts than expected (1).\n"); 
		return false;
	}

	for (uint32_t i = 0; i < 32; i++) {
		if (pendingInterrupts & (1 << i)) {
			uint32_t portIndex = i;
			volatile AHCIPort *port = ahci.r->ports + i;

			uint32_t interruptStatus = port->interruptStatus;
			Defer(port->interruptStatus = interruptStatus);

			// Print("AHCI IRQ - port %d, status %x\n", i, interruptStatus);

			AHCIPresentDrive *drive = nullptr;

			// Find the drive this interrupt is for.
			for (uintptr_t i = 0; i < ahci.driveCount; i++) {
				if (ahci.drives[i].port == portIndex) {
					drive = ahci.drives + i;
				}
			}

			if (!drive) {
				KernelLog(LOG_WARNING, "AHCIIRQHandler - Received interrupt from uninitialised/non-present drive.\n"); 
				continue;
			}

			uint16_t commandsFinished = (~port->commandIssue) & (drive->commandsInUse);

			if (!commandsFinished) {
				KernelLog(LOG_WARNING, "AHCIIRQHandler - Received more interrupts than expected (3).\n"); 
			}

			for (uintptr_t i = 0; i < AHCI_COMMAND_COUNT; i++) {
				if (!(drive->commandsInUse & (1 << i)) || !(drive->commandsReady & (1 << i))) {
					continue;
				}

				// Is the command finished?
				if (port->commandIssue & (1 << i)) {
					continue;
				}

				uint8_t ataStatus = drive->receivedPacket[i].deviceToHost.status;
				// Print("ataStatus for com %d = %x\n", i, ataStatus);

				// Either the BSY or DRQ bits are set.
				if (ataStatus & 0x88) {
					continue;
				}

				if (drive->completeCommands[i].state) {
					// Hmmm... 
					// I think this means we finished multiple commands in a previous interrupt,
					// even though multiple interrupts were sent.
					// So I don't think this matters?
				} else {
					drive->completeCommands[i].Set();

					if (drive->operations[i].ioPacket) {
						// This operation is asynchronous, so make an asynchronous task to complete it.
						scheduler.lock.Acquire();
						RegisterAsyncTask((AsyncTaskCallback) AHCIFinishOperation, drive->operations + i, nullptr, true);
						scheduler.lock.Release();
						switchThread = true;
					}

					if (drive->completeCommands[i].blockedThreads.count) {
						switchThread = true;
					}
				}
			}
		}
	}

	if (switchThread) {
		GetLocalStorage()->irqSwitchThread = true; 
	}

	return true;
}

void AHCIDriver::Initialise() {
	Device *controller;

	{
		Device device = {};
		device.type = DEVICE_TYPE_AHCI_CONTROLLER;
		device.parent = DEVICE_PARENT_ROOT;
		controller = deviceManager.Register(&device);
	}

	uint32_t abar = device->baseAddresses[5];
	r = (AHCIHBA *) kernelVMM.Allocate("AHCI", sizeof(AHCIHBA), VMM_MAP_LAZY, VMM_REGION_PHYSICAL, abar, VMM_REGION_FLAG_NOT_CACHABLE, nullptr);

	if (!r) {
		KernelLog(LOG_ERROR, "AHCIDriver::Initialise - Could not allocate memory for ABAR.\n");
		return;
	}

	// Enable interrupts.
	r->globalHostControl |= 1 << 1;
	
	uint32_t connectedDrives = r->portImplemented;

	for (uintptr_t i = 0; i < 32; i++) {
		if (connectedDrives & (1 << i)) {
			volatile AHCIPort *port = r->ports + i;
			uint32_t status = port->sataStatus;

			// Check if the device is present.
			if ((status & 0xF) != 3) {
				continue;
			}

			switch (port->signature) {
				case 0x00000101: {
					drives[driveCount].port = i;
					drives[driveCount].atapi = false;
					driveCount++;
				} break;

				case 0xEB140101: {
					drives[driveCount].port = i;
					drives[driveCount].atapi = true;
					driveCount++;
				} break;
			}
		}
	}

	if (!driveCount) {
		// We didn't find any drives.
		return;
	}

	r->interruptStatus = r->interruptStatus; // Clear any remaining interrupts.

	RegisterIRQHandler(device->interruptLine, AHCIIRQHandler);

	uintptr_t commandListPage = 0;
	uintptr_t receivedPacketPage = 0;
	uint8_t *commandListPageVirtual = nullptr;
	uint8_t *receivedPacketPageVirtual = nullptr;

	for (uintptr_t i = 0; i < driveCount; i++) {
		if ((i % (PAGE_SIZE / 1024)) == 0) {
			commandListPage = pmm.AllocatePage(true);
			commandListPageVirtual = (uint8_t *) kernelVMM.Allocate("AHCI", sizeof(PAGE_SIZE), VMM_MAP_LAZY, VMM_REGION_PHYSICAL, commandListPage, VMM_REGION_FLAG_NOT_CACHABLE, nullptr);
		}

		if ((i % (PAGE_SIZE / 256)) == 0) {
			receivedPacketPage = pmm.AllocatePage(true);
			receivedPacketPageVirtual = (uint8_t *) kernelVMM.Allocate("AHCI", sizeof(PAGE_SIZE), VMM_MAP_LAZY, VMM_REGION_PHYSICAL, receivedPacketPage, VMM_REGION_FLAG_NOT_CACHABLE, nullptr);
		}

		AHCIPresentDrive *drive = drives + i;

		drive->available.Return(AHCI_COMMAND_COUNT);

		{
			// Start a timeout.
			Timer *timeout = &drive->timeout;
			timeout->Set(AHCI_TIMEOUT, false);
			Defer(timeout->Remove());

			uintptr_t commandTablePage = pmm.AllocatePage(true);
			drive->commandTable = (AHCICommandTable *) kernelVMM.Allocate("AHCI", sizeof(PAGE_SIZE), VMM_MAP_LAZY, VMM_REGION_PHYSICAL, commandTablePage, VMM_REGION_FLAG_NOT_CACHABLE, nullptr);

			volatile AHCIPort *port = r->ports + drives[i].port;
			port->command &= ~0x11; // Stop the command engine.
			while (port->status & 0xC000 && !timeout->event.Poll()); // Wait for it to stop...
			if (timeout->event.Poll()) continue;

			port->commandListBaseAddressLow = (uint32_t) (commandListPage >> 0);
			port->commandListBaseAddressHigh = (uint32_t) (commandListPage >> 32);
			drives[i].commandList = (AHCICommandHeader *) commandListPageVirtual;

			port->fisBaseAddressLow = (uint32_t) (receivedPacketPage >> 0);
			port->fisBaseAddressHigh = (uint32_t) (receivedPacketPage >> 32);
			drives[i].receivedPacket = (AHCIReceivedPacket *) receivedPacketPageVirtual;

			for (uintptr_t i = 0; i < AHCI_COMMAND_COUNT; i++) {
				AHCICommandHeader *command = drive->commandList + i;
				command->commandTableDescriptorLow = (uint32_t) ((commandTablePage + 256 * i) >> 0); 
				command->commandTableDescriptorHigh = (uint32_t) ((commandTablePage + 256 * i) >> 32);
				command->prdEntryCount = 8; // (256 - sizeof(AHCICommandTable)) / sizeof(AHCIPRDTEntry)
			}

			commandListPage += 1024;
			receivedPacketPage += 256;
			commandListPageVirtual += 1024;
			receivedPacketPageVirtual += 256;

			// Restart the command engine.
			while (port->status & 0x8000 && !timeout->event.Poll());
			if (timeout->event.Poll()) continue;
			port->command |= 0x11;

			port->interruptStatus = port->interruptStatus; // Clear any remaining interrupts.
			port->interruptEnable |= 0xFD800001; // Set the interrupts we want (all errors, and transfer processed).

			// drive->commandAvailable.Set();
		}

		for (uintptr_t i = 0; i < AHCI_COMMAND_COUNT; i++) {
			uintptr_t physicalBuffer = pmm.AllocateContiguous64KB();
			void *buffer = kernelVMM.Allocate("AHCI", 65536, VMM_MAP_ALL, VMM_REGION_PHYSICAL, physicalBuffer, VMM_REGION_FLAG_NOT_CACHABLE, nullptr);

			drive->physicalBuffers[i] = physicalBuffer;
			drive->buffers[i] = buffer;
		}

		// Get the identify data!
		uint16_t identifyData[256];
		bool success = Access(nullptr, i, 0, 512, AHCI_DRIVER_IDENTFIY, (uint8_t *) identifyData);

		if (!success) {
			continue;
		}

		// Work out the number of sectors in the drive.
		uint32_t lba28Sectors = ((uint32_t) identifyData[60] << 0) + ((uint32_t) identifyData[61] << 16);
		uint64_t lba48Sectors = ((uint64_t) identifyData[100] << 0) + ((uint64_t) identifyData[101] << 16) +
			((uint64_t) identifyData[102] << 32) + ((uint64_t) identifyData[103] << 48);
		bool supportsLBA48 = lba48Sectors && (identifyData[83] & 0x40);
		uint64_t sectors = supportsLBA48 ? lba48Sectors : lba28Sectors;
		drive->sectorCount = sectors;

		if (!sectors) {
			continue;
		}

		KernelLog(LOG_INFO, "AHCIDriver::Initialise - Found drive %d (%z).\n", drives[i].port, drives[i].atapi ? "SATAPI" : "SATA");

		{
			Device device = {};
			device.parent = controller;
			device.type = DEVICE_TYPE_BLOCK;
			device.block.driveID = i;
			device.block.sectorSize = AHCI_SECTOR_SIZE;
			device.block.sectorCount = drive->sectorCount;
			device.block.driver = BLOCK_DEVICE_DRIVER_AHCI;
			device.block.maxAccessSectorCount = 128; // 64KB buffer / 512
			drive->device = deviceManager.Register(&device);
		}
	}
}

void AHCIRegisterController(PCIDevice *device) {
	if (ahci.foundController) {
		KernelLog(LOG_WARNING, "AHCIRegisterController - Attempt to register multiple AHCI controllers; ignored.\n");
	} else {
		ahci.foundController = true;
		ahci.device = device;
		ahci.Initialise();
	}
}

#endif
