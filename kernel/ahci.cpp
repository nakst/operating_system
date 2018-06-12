#ifndef IMPLEMENTATION

// #define AHCI_VERBOSE		
#define AHCI_DRIVE_IDENTIFY	(10)
#define AHCI_TIMEOUT		(1000)

#define AHCI_HBA_CAP            (0x00)		// Host capabilities.
#define AHCI_HBA_GHC            (0x04)		// Global host control.
#define AHCI_HBA_IS             (0x08)		// Interrupt status.
#define AHCI_HBA_PI             (0x0C)		// Ports implemented.
#define AHCI_HBA_VS             (0x10)		// Version.
#define AHCI_HBA_CCC_CTL        (0x14)		// Command completion coalescing control.
#define AHCI_HBA_CCC_PORTS      (0x18)		// Command completion coalescing ports.
#define AHCI_HBA_EM_LOC         (0x1C)		// Enclosure management location.
#define AHCI_HBA_EM_CTL         (0x20)		// Enclosure management control.
#define AHCI_HBA_CAP2           (0x24)		// Host capabilities extended.
#define AHCI_HBA_BOHC           (0x28)		// BIOS/OS handoff control and status.

#define AHCI_PORT_BASE 		(0x100)
#define AHCI_PORT_EACH		(0x80)
#define AHCI_PORT(x)		(AHCI_PORT_BASE + AHCI_PORT_EACH * (x))

#define AHCI_PORT_CLB		(0x00)		// Command list base address.
#define AHCI_PORT_CLBU		(0x04)		// Command list base address upper 32-bits.
#define AHCI_PORT_FB		(0x08)		// FIS base address.
#define AHCI_PORT_FBU		(0x0C)		// FIS base address upper 32-bits.
#define AHCI_PORT_IS		(0x10)		// Interrupt status.
#define AHCI_PORT_IE		(0x14)		// Interrupt enable.
#define AHCI_PORT_CMD		(0x18)		// Command and status.
#define AHCI_PORT_TFD		(0x20)		// Task file data.
#define AHCI_PORT_SIG		(0x24)		// Signature.
#define AHCI_PORT_SSTS		(0x28)		// Serial ATA status.
#define AHCI_PORT_SCTL		(0x2C)		// Serial ATA controler.
#define AHCI_PORT_SERR		(0x30)		// Serial ATA error.
#define AHCI_PORT_SACT		(0x34)		// Serial ATA active.
#define AHCI_PORT_CI		(0x38)		// Command issue.
#define AHCI_PORT_SNTF		(0x3C)		// Serial ATA notification.
#define AHCI_PORT_FBS		(0x40)		// FIS-based switching control.
#define AHCI_PORT_DEVSLP	(0x44)		// Device sleep.

#define AHCI_CAP_S64A           (1 << 31)	// Supports 64-bit addressing.
#define AHCI_CAP_SNCQ	        (1 << 30)	// Supports native command queuing.
#define AHCI_CAP_SSNTF          (1 << 29)	// Supports SNotification register.
#define AHCI_CAP_SMPS           (1 << 28)	// Supports mechanical presence switch.
#define AHCI_CAP_SSS            (1 << 27)	// Supports staggered spin-up.
#define AHCI_CAP_SALP           (1 << 26)	// Supports aggressive link power management.
#define AHCI_CAP_SAL            (1 << 25)	// Supports activity LED.
#define AHCI_CAP_SCLO           (1 << 24)	// Supports command list override.
#define AHCI_CAP_ISS            (15 << 20)	// Interrupt speed support.
#define AHCI_CAP_SAM            (1 << 18)	// Supports AHCI only mode.
#define AHCI_CAP_SPM            (1 << 17)	// Supports port multiplier.
#define AHCI_CAP_FBSS           (1 << 16)	// FIS-based switching supported.
#define AHCI_CAP_PMD            (1 << 15)	// PIO multiple DRQ block.
#define AHCI_CAP_SSC            (1 << 14)	// Slumber slate capable.
#define AHCI_CAP_PSC            (1 << 13)	// Partial state capable.
#define AHCI_CAP_NCS            (15 << 8)	// Number of command slots.
#define AHCI_CAP_CCCS           (1 << 7)	// Command completion coalescing supported.
#define AHCI_CAP_EMS            (1 << 6)	// Enclosure management supported.
#define AHCI_CAP_SXS            (1 << 5)	// Supports external SATA.
#define AHCI_CAP_NP             (15 << 0)	// Number of ports.

#define AHCI_ISS_GEN_1          (1 << 20)	// 1.5 Gbps
#define AHCI_ISS_GEN_2          (2 << 20)	// 3 Gbps
#define AHCI_ISS_GEN_3          (3 << 20)	// 6 Gbps

#define AHCI_GHC_AE		(1 << 31)	// AHCI enable.
#define AHCI_GHC_MRSM		(1 << 2)	// MSI revert to single message.
#define AHCI_GHC_IE		(1 << 1)	// Interrupt enable.
#define AHCI_GHC_HR		(1 << 0)	// HBA reset.

#define AHCI_CAP2_DESO		(1 << 5)	// DevSleep entrance from slumber only.
#define AHCI_CAP2_SADM		(1 << 4)	// Supports aggressive device sleep management.
#define AHCI_CAP2_SDS		(1 << 3)	// Supports device sleep.
#define AHCI_CAP2_APST		(1 << 2)	// Automatic partial to slumber transitions.
#define AHCI_CAP2_NVMP		(1 << 1)	// NVMHCI present.
#define AHCI_CAP2_BOH		(1 << 0)	// BIOS/OS handoff.

#define AHCI_INT_CPD		(1 << 31)	// Cold port detect.
#define AHCI_INT_TFE		(1 << 30)	// Task file error.
#define AHCI_INT_HBF		(1 << 29)	// Host bus fatal error.
#define AHCI_INT_HBD		(1 << 28)	// Host bus data error.
#define AHCI_INT_IF		(1 << 27)	// Interface fatal error.
#define AHCI_INT_INF		(1 << 26)	// Interface non-fatal error.
#define AHCI_INT_OF		(1 << 24)	// Overflow.
#define AHCI_INT_IPM		(1 << 23)	// Incorrect port muliplier status.
#define AHCI_INT_PRC		(1 << 22)	// PhyRdy change status.
#define AHCI_INT_DMP		(1 << 7)	// Device mechanical presence status.
#define AHCI_INT_PC		(1 << 6)	// Port connect change status.
#define AHCI_INT_DP		(1 << 5)	// Descriptor processed.
#define AHCI_INT_UF		(1 << 4)	// Unknown FIS.
#define AHCI_INT_SDB		(1 << 3)	// Set device bits.
#define AHCI_INT_DS		(1 << 2)	// DMA setup FIS.
#define AHCI_INT_PSS		(1 << 1)	// PIO setup FIS.
#define AHCI_INT_DHR		(1 << 0)	// Device to host register FIS.

#define AHCI_CMD_ICC		(15 << 28)	// Interface communication control.
#define AHCI_CMD_ICC_ACTIVE	(1 << 28)	//	Active.
#define AHCI_CMD_ASP		(1 << 27)	// Aggressive slumber/partial.
#define AHCI_CMD_ALPE		(1 << 26)	// Aggressive link power management enable.
#define AHCI_CMD_DLAE		(1 << 25)	// Drive LED on ATAPI enable.
#define AHCI_CMD_ATAPI		(1 << 24)	// Device is ATAPI.
#define AHCI_CMD_APSTE		(1 << 23)	// Automatic partial to slumber transitions enabled.
#define AHCI_CMD_FBSCP		(1 << 22)	// FIS-based switching capable port.
#define AHCI_CMD_ESP		(1 << 21)	// External SATA port.
#define AHCI_CMD_CPD		(1 << 20)	// Cold presence detection.
#define AHCI_CMD_MPSP		(1 << 19)	// Mechanical presence switch attached to port.
#define AHCI_CMD_HPCP		(1 << 18)	// Hot plug capable port.
#define AHCI_CMD_PMA		(1 << 17)	// Port multiplier attached.
#define AHCI_CMD_CPS		(1 << 16)	// Cold presence state.
#define AHCI_CMD_CR		(1 << 15)	// Command lsit running.
#define AHCI_CMD_FR		(1 << 14)	// FIS receive running.
#define AHCI_CMD_MPSS		(1 << 13)	// Mechanical presence switch state.
#define AHCI_CMD_CCS		(15 << 8)	// Current command slot.
#define AHCI_CMD_FRE		(1 << 4)	// FIS receive enable.
#define AHCI_CMD_CLO		(1 << 3)	// Command list override.
#define AHCI_CMD_POD		(1 << 2)	// Power on device.
#define AHCI_CMD_SUD		(1 << 1)	// Spin-up device.
#define AHCI_CMD_ST		(1 << 0)	// Start.

#define AHCI_TFD_ERC		(15 << 8)	// Error.
#define AHCI_TFD_BSY		(1 << 7)	// Indicates the interface is busy.
#define AHCI_TFD_CS1		(7 << 4)	// Command specific.
#define AHCI_TFD_DRQ		(1 << 3)	// Indicates a data transfer is requested.
#define AHCI_TFD_CS2		(3 << 1)	// Command specific.
#define AHCI_TFD_ERR		(1 << 0)	// Indicates an error during the transfer.

struct AHCIPort {
	bool present;

	uint64_t sectors;
	uint32_t runningCommands, usedCommands;
	Event commandsFinished[32];

	uintptr_t commandList, receivedFIS, 
		  commandTable, commandTableBase;
};

struct AHCIOperation {
	void *_buffer;
	size_t _countBytes;
	uintptr_t offsetIntoSector;
	uintptr_t commandIndex, bufferIndex, portIndex;
	struct AHCIController *controller;
	struct IOPacket *packet;
	Timer timeout;
	bool error;
};

struct AHCIController {
	uint32_t capabilities;
	uint32_t version;
	int abar;
	struct PCIDevice *device;

	uintptr_t driveBase;

#define AHCI_BUFFER_SIZE (65536)
#define AHCI_MAX_BUFFER_COUNT (16)
	uint8_t *buffers[AHCI_MAX_BUFFER_COUNT];
	uintptr_t buffersBase[AHCI_MAX_BUFFER_COUNT];
	size_t bufferCount;
	uint32_t usedBuffers;
	AHCIOperation operations[AHCI_MAX_BUFFER_COUNT];

	Spinlock spinlock;

#define AHCI_MAX_PORTS (32)
	AHCIPort ports[AHCI_MAX_PORTS];
};

struct AHCIBlockedOperation {
	struct IOPacket *packet;
	uintptr_t drive;
	uint64_t offset;
	size_t _countBytes;
	int operation;
	uint8_t *_buffer;
	LinkedItem<AHCIBlockedOperation> item;
};

#define AHCI_MAX_CONTROLLERS (4)
AHCIController ahciControllers[4];
size_t ahciControllerCount;

Mutex ahciBlockedPacketsMutex;
LinkedList<AHCIBlockedOperation> ahciBlockedOperations;

void AHCITimeout(AHCIOperation *op);
bool AHCIAccess(struct IOPacket *packet, uintptr_t _drive, uint64_t offset, size_t _countBytes, int operation, uint8_t *_buffer);
bool AHCIIRQHandler(uintptr_t interruptIndex);
void AHCIFinish(AHCIOperation *op);
void AHCIUnblock(void *argument);
void AHCIRegisterController(struct PCIDevice *device);

#else

void AHCITimeout(AHCIOperation *op) {
	AHCIController *controller = op->controller;
	AHCIPort *port = op->portIndex + controller->ports;

	controller->spinlock.Acquire();

	if (port->runningCommands & (1 << op->commandIndex)) {
		port->runningCommands &= ~(1 << op->commandIndex);
		op->error = true;
		AHCIFinish(op);
	}

	controller->spinlock.Release();
}

bool AHCIAccess(IOPacket *packet, uintptr_t _drive, uint64_t offset, size_t _countBytes, int operation, uint8_t *_buffer) {
	uint64_t sector = offset / ATA_SECTOR_SIZE;
	uint64_t offsetIntoSector = offset % ATA_SECTOR_SIZE;
	size_t countBytes = (_countBytes + offsetIntoSector + ATA_SECTOR_SIZE - 1) & ~(ATA_SECTOR_SIZE - 1);
	uintptr_t drive = _drive;

	for (uintptr_t i = ahciControllerCount; i > 0; i--) {
		if (drive > ahciControllers[i - 1].driveBase) {
			i--;
			drive -= ahciControllers[i - 1].driveBase;
			break;
		}
	}

	AHCIController *controller = ahciControllers + 0;
	AHCIPort *port = controller->ports + drive;

	if (!port->present) return false;
	if (sector > port->sectors) return false;

	uintptr_t commandIndex = 32;
	uintptr_t bufferIndex = AHCI_MAX_BUFFER_COUNT;

	controller->spinlock.Acquire();

	for (uintptr_t i = 0; i < 32; i++) {
		if (port->usedCommands & (1 << i)) {
			continue;
		}

		commandIndex = i;
		break;
	}

	for (uintptr_t i = 0; i < AHCI_MAX_BUFFER_COUNT; i++) {
		if (controller->usedBuffers & (1 << i)) {
			continue;
		}

		if (!controller->buffers[i]) {
			continue;
		}

		bufferIndex = i;
		break;
	}

	if (commandIndex != 32 && bufferIndex != AHCI_MAX_BUFFER_COUNT) {
		port->usedCommands |= (1 << commandIndex);
		controller->usedBuffers |= (1 << bufferIndex);
	}

	controller->spinlock.Release();

	if (commandIndex == 32 || bufferIndex == AHCI_MAX_BUFFER_COUNT) {
		ahciBlockedPacketsMutex.Acquire();

		AHCIBlockedOperation *op = (AHCIBlockedOperation *) OSHeapAllocate(sizeof(AHCIBlockedOperation), true);

		op->packet = packet;
		op->drive = _drive;
		op->offset = offset;
		op->_countBytes = _countBytes;
		op->operation = operation;
		op->_buffer = _buffer;
		op->item.thisItem = op;

		ahciBlockedOperations.InsertEnd(&op->item);

		ahciBlockedPacketsMutex.Release();
		return true;
	}

	if (packet) {
		packet->driverState = IO_PACKET_DRIVER_ISSUED;
	}

	// Setup the FIS.

	uint8_t *fis = (uint8_t *) port->commandTable + 0x100 * commandIndex;
	ZeroMemory(fis, 20);
	fis[0] = 0x27; // Host to device.
	fis[1] = 0x80; // Command.
	fis[2] = operation == DRIVE_ACCESS_READ ? ATA_READ_DMA_48 : operation == DRIVE_ACCESS_WRITE ? ATA_WRITE_DMA_48 : ATA_IDENTIFY;
	fis[4] = (sector >> 0) & 0xFF;
	fis[5] = (sector >> 8) & 0xFF;
	fis[6] = (sector >> 16) & 0xFF;
	fis[7] = 1 << 6;
	fis[8] = (sector >> 24) & 0xFF;
	fis[9] = (sector >> 32) & 0xFF;
	fis[10] = (sector >> 40) & 0xFF;
	fis[12] = (countBytes / ATA_SECTOR_SIZE) & 0xFF;
	fis[13] = ((countBytes / ATA_SECTOR_SIZE) >> 8) & 0xFF;
	fis[15] = 8;

	// Setup the command list.

	uint32_t *commandList = (uint32_t *) port->commandList + 8 * commandIndex;
	commandList[0] = 5 | (operation == DRIVE_ACCESS_WRITE ? (1 << 6) : 0) | (1 << 16);
	commandList[1] = 0;
	commandList[2] = (port->commandTableBase + 0x100 * commandIndex) & 0xFFFFFFFF;
	commandList[3] = ((port->commandTableBase + 0x100 * commandIndex) >> 32) & 0xFFFFFFFF;
	commandList[4] = commandList[5] = commandList[6] = commandList[7] = 0;

	// Setup the PRDT.
	
	uint32_t *prdt = (uint32_t *) port->commandTable + 0x20 + 0x40 * commandIndex;
	prdt[0] = controller->buffersBase[bufferIndex] & 0xFFFFFFFF;
	prdt[1] = (controller->buffersBase[bufferIndex] >> 32) & 0xFFFFFFFF;
	prdt[2] = 0;
	prdt[3] = countBytes - 1;

	// Copy the data into the input buffer.

	if (operation == DRIVE_ACCESS_WRITE) {
		CopyMemory(controller->buffers[bufferIndex], _buffer, _countBytes);
	}

	// Setup the operation.

	AHCIOperation *op = controller->operations + bufferIndex;

	op->_buffer = operation == DRIVE_ACCESS_READ || operation == AHCI_DRIVE_IDENTIFY ? _buffer : nullptr;
	op->_countBytes = _countBytes;
	op->offsetIntoSector = offsetIntoSector;
	op->commandIndex = commandIndex;
	op->bufferIndex = bufferIndex;
	op->portIndex = drive;
	op->controller = controller;
	op->packet = packet;
	op->error = false;

	// Start the command.

#ifdef AHCI_VERBOSE
	OSPrint("start %d, %d\n", bufferIndex, commandIndex);
#endif

	port->commandsFinished[commandIndex].Reset();

	controller->spinlock.Acquire();
	port->runningCommands |= (1 << commandIndex);
	controller->spinlock.Release();

	WaitMicroseconds(10);
	controller->device->WriteBAR32(controller->abar, AHCI_PORT(drive) + AHCI_PORT_CI, 1 << commandIndex);

	if (!packet) {
		// Wait for the command to complete.

		port->commandsFinished[0].Wait(AHCI_TIMEOUT);

		// Finish the operation.

		controller->spinlock.Acquire();
		port->usedCommands &= ~(1 << op->commandIndex);
		controller->usedBuffers &= ~(1 << op->bufferIndex);
		controller->spinlock.Release();
	} else {
		op->timeout.Remove();
		op->timeout.Set(AHCI_TIMEOUT, false, (AsyncTaskCallback) AHCITimeout, op);
	}

	return true;
}

void AHCIUnblock(void *argument) {
	(void) argument;

	ahciBlockedPacketsMutex.Acquire();

	LinkedItem<AHCIBlockedOperation> *item = ahciBlockedOperations.firstItem;

	if (item) {
		ahciBlockedOperations.Remove(item);
	}

	ahciBlockedPacketsMutex.Release();

	if (item) {
		AHCIBlockedOperation *op = item->thisItem;

		if (!AHCIAccess(op->packet, op->drive, op->offset, op->_countBytes, op->operation, op->_buffer)) {
			op->packet->request->Cancel(OS_ERROR_UNKNOWN_OPERATION_FAILURE);
		}

		OSHeapFree(op);
	}
}

void AHCIFinish(AHCIOperation *op) {
#ifdef AHCI_VERBOSE
	OSPrint("finish %d, %d\n", op->bufferIndex, op->commandIndex);
#endif

	AHCIController *controller = op->controller;
	AHCIPort *port = controller->ports + op->portIndex;

	op->timeout.Remove();

	bool cancelled = false;

	if (op->packet) {
		op->packet->request->mutex.Acquire();
		cancelled = op->packet->request->cancelled;
	}

	// Copy the data into the output buffer.

	if (op->_buffer && !op->error && !cancelled) {
		CopyMemory(op->_buffer, controller->buffers[op->bufferIndex] + op->offsetIntoSector, op->_countBytes);

#ifdef AHCI_VERBOSE
		bool allZero = true;

		for (uintptr_t i = 0; i < op->_countBytes; i++) {
			if (controller->buffers[op->bufferIndex][i]) {
				allZero = false;
			}
		}

		if (allZero) {
			OSPrint("allZero! %d, %d\n", op->bufferIndex, op->commandIndex);
		}
#endif
	}

	// We're done!

	controller->spinlock.Acquire();
	port->commandsFinished[op->commandIndex].Set();
	controller->spinlock.Release();

	if (op->packet) {
		controller->spinlock.Acquire();
		port->usedCommands &= ~(1 << op->commandIndex);
		controller->usedBuffers &= ~(1 << op->bufferIndex);
		controller->spinlock.Release();

		// Tell the packet it's done.

		IOPacket *packet = op->packet;
		IORequest *request = packet->request;

		packet->driverState = IO_PACKET_DRIVER_COMPLETE;
		if (op->error) packet->request->Cancel(OS_ERROR_UNKNOWN_OPERATION_FAILURE); 
		else packet->Complete(OS_SUCCESS);
		request->mutex.Release();
	}

	// Unblock a packet, if one exists.
	// This must be done on the AsyncTask thread.

	if (!op->packet) {
		scheduler.lock.Acquire();
		RegisterAsyncTask((AsyncTaskCallback) AHCIUnblock, nullptr, nullptr, true);
		scheduler.lock.Release();
	} else {
		AHCIUnblock(nullptr);
	}
}

bool AHCIIRQHandler(uintptr_t interruptIndex) {
	bool found = false;

	for (uintptr_t i = 0; i < ahciControllerCount; i++) {
		if (ahciControllers[i].device->interruptLine == interruptIndex) {
			AHCIController *controller = ahciControllers + i;
			PCIDevice *device = controller->device;

			uint32_t interruptStatus = device->ReadBAR32(controller->abar, AHCI_HBA_IS);

			if (!interruptStatus) {
				continue;
			}

			bool error = interruptStatus & (AHCI_INT_OF | AHCI_INT_IF | AHCI_INT_HBD | AHCI_INT_HBF | AHCI_INT_TFE);
			// TODO Reset the controller?
			
			found = true;

			for (uintptr_t j = 0; j < AHCI_MAX_PORTS; j++) {
				AHCIPort *port = controller->ports + j;

				if (!port->present) continue;

				uint32_t interruptStatus = device->ReadBAR32(controller->abar, AHCI_PORT(j) + AHCI_PORT_IS);
				device->WriteBAR32(controller->abar, AHCI_PORT(j) + AHCI_PORT_IS, interruptStatus);

				uint32_t runningCommands = device->ReadBAR32(controller->abar, AHCI_PORT(j) + AHCI_PORT_SACT)
					| device->ReadBAR32(controller->abar, AHCI_PORT(j) + AHCI_PORT_CI);

				uint32_t finishedCommands = port->runningCommands & ~runningCommands;

				controller->spinlock.Acquire();
				port->runningCommands &= ~finishedCommands;
				controller->spinlock.Release();

				for (uintptr_t k = 0; k < 32; k++) {
					if (!(finishedCommands & (1 << k))) continue;

					for (uintptr_t l = 0; l < AHCI_MAX_BUFFER_COUNT; l++) {
						if (controller->operations[l].commandIndex != k 
								|| controller->operations[l].portIndex != j) continue;

						if (error) controller->operations[l].error = true;

						if (controller->operations[l].packet) {
							scheduler.lock.Acquire();
							RegisterAsyncTask((AsyncTaskCallback) AHCIFinish, controller->operations + l, nullptr, true);
							scheduler.lock.Release();
						} else {
							AHCIFinish(controller->operations + l);
						}

						break;
					}
				}
			}

			device->WriteBAR32(controller->abar, AHCI_HBA_IS, interruptStatus);
		}
	}

	return found;
}

void AHCIRegisterController(PCIDevice *device) {
	bool error = false;
	(void) error;

	if (ahciControllerCount == AHCI_MAX_CONTROLLERS) {
		KernelLog(LOG_WARNING, "AHCIRegisterController - Maximum AHCI controller limit (%d) exceeded.\n", AHCI_MAX_CONTROLLERS);
		error = true;
		return;
	}

	Device *controllerDevice;

	{
		Device device = {};
		device.type = DEVICE_TYPE_AHCI_CONTROLLER;
		device.parent = DEVICE_PARENT_ROOT;
		controllerDevice = deviceManager.Register(&device);
	}

	AHCIController *controller = ahciControllers + ahciControllerCount++;
	
	Timer timeout = {};
	timeout.Set(100, false); // 100ms to initialise the controller.
	Defer(timeout.Remove());

	controller->abar = 5;
	controller->device = device;
	controller->driveBase = (ahciControllerCount - 1) * AHCI_MAX_PORTS;

	// Check we're not using a really buggy controller.

	if (device->deviceID == 0xA01C177D 
			|| device->deviceID == 0x00311C36 
			|| (device->deviceID & 0xFFFF) == 0x11AB) {
		KernelLog(LOG_WARNING, "AHCIRegisterController - Rejected buggy controller.\n");
		error = true;
		return;
	}

	// Get information from the device.

	controller->capabilities = device->ReadBAR32(controller->abar, AHCI_HBA_CAP);
	controller->version = device->ReadBAR32(controller->abar, AHCI_HBA_VS);

	// Reset the controller.

	device->WriteBAR32(controller->abar, AHCI_HBA_GHC, AHCI_GHC_AE);
	device->WriteBAR32(controller->abar, AHCI_HBA_GHC, AHCI_GHC_AE | AHCI_GHC_HR);

	while (!timeout.event.state) {
		WaitMicroseconds(100);
		if (!(device->ReadBAR32(controller->abar, AHCI_HBA_GHC) & AHCI_GHC_HR)) goto deviceReset;
	}

	KernelLog(LOG_WARNING, "AHCIRegisterController - Timeout during controller reset.\n");
	error = true;
	return;

	deviceReset:;
	device->WriteBAR32(controller->abar, AHCI_HBA_GHC, AHCI_GHC_AE);

	// Find the available ports.

	uint32_t availablePorts = device->ReadBAR32(controller->abar, AHCI_HBA_PI);

	if (!availablePorts) {
		KernelLog(LOG_WARNING, "AHCIRegisterController - No ports implemented.\n");
		error = true;
		return;
	}

	// Enable interrupts.

	device->WriteBAR32(controller->abar, AHCI_HBA_IS, device->ReadBAR32(controller->abar, AHCI_HBA_IS));
	device->WriteBAR32(controller->abar, AHCI_HBA_GHC, device->ReadBAR32(controller->abar, AHCI_HBA_GHC) | AHCI_GHC_IE);

	// Get 4KB * 32 ports of physical memory for the command lists, received FISes and command tables.

	uintptr_t physical128KB = pmm.AllocateContiguous128KB();

	if (!physical128KB) {
		KernelLog(LOG_WARNING, "AHCIRegisterController - Could not allocate memory (approx. 128KB needed).\n");
		error = true;
		return;
	}

	// Defer(if (error) pmm.FreeContiguous(physical128KB, 65536));

#ifdef ARCH_64
	if (physical128KB > 0x100000000 && !(controller->capabilities & AHCI_CAP_S64A)) {
		KernelLog(LOG_WARNING, "AHCIRegisterController - Controller does not support 64-bit addressing. (No contiguous low memory was available.)\n");
		error = true;
		return;
	}
#endif

	uintptr_t virtual128KB = (uintptr_t) kernelVMM.Allocate("AHCIPhys", 65536, VMM_MAP_ALL, VMM_REGION_PHYSICAL,
			physical128KB, VMM_REGION_FLAG_NOT_CACHABLE);
	Defer(if (error) kernelVMM.Free((void *) virtual128KB));

	// Get 16 * 64KB buffers for data transfers.

	for (uintptr_t i = 0; i < AHCI_MAX_BUFFER_COUNT; i++, controller->bufferCount++) {
		uintptr_t physical64KB = pmm.AllocateContiguous64KB();

		if (!physical64KB) {
			if (!controller->bufferCount) {
				KernelLog(LOG_WARNING, "AHCIRegisterController - Could not allocate memory (approx. 128KB needed).\n");
				error = true;
				return;
			} else {
				break;
			}
		}

#ifdef ARCH_64
		if (physical64KB > 0x100000000 && !(controller->capabilities & AHCI_CAP_S64A)) {
			if (!controller->bufferCount) {
				KernelLog(LOG_WARNING, "AHCIRegisterController - Controller does not support 64-bit addressing. (No contiguous low memory was available.)\n");
				error = true;
				return;
			} else {
				break;
			}
		}
#endif

		uintptr_t virtual64KB = (uintptr_t) kernelVMM.Allocate("AHCIPhys2", 65536, VMM_MAP_ALL, VMM_REGION_PHYSICAL,
				physical64KB, VMM_REGION_FLAG_NOT_CACHABLE);
		Defer(if (error) kernelVMM.Free((void *) virtual64KB));

		controller->buffers[i] = (uint8_t *) virtual64KB;
		controller->buffersBase[i] = physical64KB;
	}

	// Register the IRQ.

	if (!RegisterIRQHandler(device->interruptLine, AHCIIRQHandler)) {
		KernelLog(LOG_WARNING, "AHCIRegisterController - Could not register IRQ %d.\n", device->interruptLine);
		error = true;
		return;
	}

	// Enumerate the ports.

	uintptr_t commandListBase = physical128KB;
	uintptr_t receivedFISBase = physical128KB + 0x400;
	uintptr_t commandTableBase = physical128KB + 0x800;
	uintptr_t commandList = virtual128KB;
	uintptr_t receivedFIS = virtual128KB + 0x400;
	uintptr_t commandTable = virtual128KB + 0x800;
	
	for (uintptr_t i = 1, j = 0; i < 0x100000000; 
			i <<= 1, j++,
			commandListBase += 0x1000, receivedFISBase += 0x1000, commandTableBase += 0x1000,
			commandList += 0x1000, receivedFIS += 0x1000, commandTable += 0x1000) {
		if (!(availablePorts & i)) {
			continue;
		}

		AHCIPort *port = controller->ports + j;
		
		// Disable interrupts and stop operations.

		device->WriteBAR32(controller->abar, AHCI_PORT(j) + AHCI_PORT_CMD, 
				device->ReadBAR32(controller->abar, AHCI_PORT(j) + AHCI_PORT_CMD) & ~(AHCI_CMD_ST));
		device->WriteBAR32(controller->abar, AHCI_PORT(j) + AHCI_PORT_IE, 0);

		// Set the command list base and received FIS base.

		port->commandList = commandList;
		port->receivedFIS = receivedFIS;
		port->commandTable = commandTable;
		port->commandTableBase = commandTableBase;

		device->WriteBAR32(controller->abar, AHCI_PORT(j) + AHCI_PORT_CLB, (uint32_t) (commandListBase >> 0));
		device->WriteBAR32(controller->abar, AHCI_PORT(j) + AHCI_PORT_CLBU, (uint32_t) (commandListBase >> 32));
		device->WriteBAR32(controller->abar, AHCI_PORT(j) + AHCI_PORT_FB, (uint32_t) (receivedFISBase >> 0));
		device->WriteBAR32(controller->abar, AHCI_PORT(j) + AHCI_PORT_FBU, (uint32_t) (receivedFISBase >> 32));

		// Start the device.

		device->WriteBAR32(controller->abar, AHCI_PORT(j) + AHCI_PORT_CMD,
				AHCI_CMD_ICC_ACTIVE | AHCI_CMD_POD | AHCI_CMD_SUD | AHCI_CMD_FRE);

		// Clear errors and pending interrupts.

		device->WriteBAR32(controller->abar, AHCI_PORT(j) + AHCI_PORT_SERR, 0xFFFFFFFF);
		device->WriteBAR32(controller->abar, AHCI_PORT(j) + AHCI_PORT_IS, 0xFFFFFFFF);

		// Start operations.
		
		device->WriteBAR32(controller->abar, AHCI_PORT(j) + AHCI_PORT_CMD, 
				(device->ReadBAR32(controller->abar, AHCI_PORT(j) + AHCI_PORT_CMD) 
				& ~(AHCI_CMD_PMA)) | AHCI_CMD_ST);

		// Get the device type.

		uint32_t status = device->ReadBAR32(controller->abar, AHCI_PORT(j) + AHCI_PORT_SSTS);

		if ((status & 15) != 3) continue;
		if (((status >> 8) & 15) != 1) continue;

		uint32_t signature = device->ReadBAR32(controller->abar, AHCI_PORT(j) + AHCI_PORT_SIG);

		if (signature != 0x101) continue;

		// Enable interrupts.

		device->WriteBAR32(controller->abar, AHCI_PORT(j) + AHCI_PORT_IE,
				AHCI_INT_TFE | AHCI_INT_HBF | AHCI_INT_HBD
				| AHCI_INT_IF | AHCI_INT_OF | AHCI_INT_DMP
				| AHCI_INT_PRC | AHCI_INT_PC | AHCI_INT_DP
				| AHCI_INT_UF | AHCI_INT_SDB | AHCI_INT_DS
				| AHCI_INT_PSS | AHCI_INT_DHR);

		// Wait a 500 microseconds before trying to send an IDENTIFY command.

		WaitMicroseconds(500);

		// Send IDENTIFY.

		uint16_t identifyData[256];
		port->present = true;
		bool success = AHCIAccess(nullptr, j, 0, 512, AHCI_DRIVE_IDENTIFY, (uint8_t *) identifyData);
		port->present = false;
		if (!success) continue;

		// Parse the IDENTIFY data.

		if (!(identifyData[49] & 0x200)) continue;
		if (!(identifyData[49] & 0x100)) continue;

		uint32_t lba28Sectors = ((uint32_t) identifyData[60] << 0) + ((uint32_t) identifyData[61] << 16);
		uint64_t lba48Sectors = ((uint64_t) identifyData[100] << 0) + ((uint64_t) identifyData[101] << 16) +
			((uint64_t) identifyData[102] << 32) + ((uint64_t) identifyData[103] << 48);
		bool supportsLBA48 = lba48Sectors && (identifyData[83] & 0x40);
		uint64_t sectors = supportsLBA48 ? lba48Sectors : lba28Sectors;

		port->sectors = sectors;
		port->present = true;

		KernelLog(LOG_INFO, "AHCIRegisterController - Found SATA drive on port %d with %x sectors.\n", j, sectors);
	}

	for (uintptr_t i = 0; i < AHCI_MAX_PORTS; i++) {
		AHCIPort *port = controller->ports + i;

		if (!port->present) continue;

		// Register the drive.

		Device device = {};
		device.parent = controllerDevice;
		device.type = DEVICE_TYPE_BLOCK;
		device.block.driveID = i + controller->driveBase;
		device.block.sectorSize = ATA_SECTOR_SIZE;
		device.block.sectorCount = port->sectors;
		device.block.driver = BLOCK_DEVICE_DRIVER_AHCI;
		device.block.maxAccessSectorCount = 64;

		if (!deviceManager.Register(&device)) {
			port->present = false;
		}
	}

	KernelLog(LOG_VERBOSE, "AHCIRegisterController - Registered AHCI controller!\n"
			"\t[version %x, %d ports, %z Gbps]\n", 
			controller->version, (controller->capabilities & AHCI_CAP_NP) + 1, 
			(controller->capabilities & AHCI_CAP_ISS) == AHCI_ISS_GEN_1 ? "1.5" 
			: (controller->capabilities & AHCI_CAP_ISS) == AHCI_ISS_GEN_2 ? "3" 
			: (controller->capabilities & AHCI_CAP_ISS) == AHCI_ISS_GEN_3 ? "6" : "??");
}

#endif
