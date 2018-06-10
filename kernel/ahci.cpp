#ifndef IMPLEMENTATION

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
	uintptr_t commandListBase, receivedFISBase;
};

struct AHCIController {
	uint32_t capabilities;
	uint32_t version;
	int abar;

#define AHCI_MAX_PORTS (32)
	AHCIPort ports[AHCI_MAX_PORTS];
};

#define AHCI_MAX_CONTROLLERS (4)
AHCIController ahciControllers[4];
size_t ahciControllerCount;

#else

bool AHCIIRQHandler(uintptr_t interruptIndex) {
	// TODO.
	(void) interruptIndex;
	return false;
}

void AHCIRegisterController(PCIDevice *device) {
	bool error = false;
	(void) error;

	if (ahciControllerCount == AHCI_MAX_CONTROLLERS) {
		KernelLog(LOG_WARNING, "AHCIRegisterController - Maximum AHCI controller limit (%d) exceeded.\n", AHCI_MAX_CONTROLLERS);
		error = true;
		return;
	}

	AHCIController *controller = ahciControllers + ahciControllerCount++;
	
	Timer timeout = {};
	timeout.Set(100, false); // 100ms to initialise the controller.
	Defer(timeout.Remove());

	controller->abar = 5;

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

	// Get 2KB * 32 ports of physical memory for the command lists and received FISes.

	uintptr_t physical64KB = pmm.AllocateContiguous64KB();
	// Defer(if (error) pmm.FreeContiguous(physical64KB, 65536));

#ifdef ARCH_64
	if (physical64KB > 0x100000000 && !(controller->capabilities & AHCI_CAP_S64A)) {
		KernelLog(LOG_WARNING, "AHCIRegisterController - Controller does not support 64-bit addressing. (No contiguous low memory was available.)\n");
		error = true;
		return;
	}
#endif

	// Enumerate the ports.

	uintptr_t commandListBase = physical64KB;
	uintptr_t receivedFISBase = physical64KB + 0x400;
	
	for (uintptr_t i = 1, j = 0; i < 0x100000000; 
			i <<= 1, commandListBase += 0x800, receivedFISBase += 0x800, j++) {
		if (!(availablePorts & i)) {
			continue;
		}

		AHCIPort *port = controller->ports + j;
		
		// Disable interrupts and stop operations.

		device->WriteBAR32(controller->abar, AHCI_PORT(j) + AHCI_PORT_CMD, 
				device->ReadBAR32(controller->abar, AHCI_PORT(j) + AHCI_PORT_CMD) & ~(AHCI_CMD_ST));
		device->WriteBAR32(controller->abar, AHCI_PORT(j) + AHCI_PORT_IE, 0);

		// Set the command list base and received FIS base.

		port->commandListBase = commandListBase;
		port->receivedFISBase = receivedFISBase;

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

		KernelLog(LOG_VERBOSE, "AHCIRegisterController - Found SATA drive on port %d.\n", j);

		// Enable interrupts.

		device->WriteBAR32(controller->abar, AHCI_PORT(j) + AHCI_PORT_IE,
				AHCI_INT_TFE | AHCI_INT_HBF | AHCI_INT_HBD
				| AHCI_INT_IF | AHCI_INT_OF | AHCI_INT_DMP
				| AHCI_INT_PRC | AHCI_INT_PC | AHCI_INT_DP
				| AHCI_INT_UF | AHCI_INT_SDB | AHCI_INT_DS
				| AHCI_INT_PSS | AHCI_INT_DHR);
	}

	// Register the IRQ.

	if (!RegisterIRQHandler(device->interruptLine, AHCIIRQHandler)) {
		KernelLog(LOG_WARNING, "AHCIRegisterController - Could not register IRQ %d.\n", device->interruptLine);
		error = true;
		return;
	}

	KernelLog(LOG_VERBOSE, "AHCIRegisterController - Registered AHCI controller!\n"
			"\t[version %x, %d ports, %z Gbps]\n", 
			controller->version, (controller->capabilities & AHCI_CAP_NP) + 1, 
			(controller->capabilities & AHCI_CAP_ISS) == AHCI_ISS_GEN_1 ? "1.5" 
			: (controller->capabilities & AHCI_CAP_ISS) == AHCI_ISS_GEN_2 ? "3" 
			: (controller->capabilities & AHCI_CAP_ISS) == AHCI_ISS_GEN_3 ? "6" : "??");
}

#endif
