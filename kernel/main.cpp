// TODO Prevent Meltdown/Spectre exploits.
// TODO Security.

#include "kernel.h"
#define IMPLEMENTATION
#include "kernel.h"

void KernelInitialisation() {
	pmm.Initialise2();
	InitialiseObjectManager();
	acpi.Initialise2();
	graphics.Initialise(); 
	vfs.Initialise();
	deviceManager.Initialise();
	windowManager.Initialise();

	char *desktop = (char *) "/OS/Desktop.esx";
	desktopProcess = scheduler.SpawnProcess(desktop, CStringLength(desktop));

	scheduler.TerminateThread(GetCurrentThread());
}

extern "C" void KernelAPMain() {
	while (!scheduler.started);
	scheduler.InitialiseAP();

	NextTimer(20);

	// The InitialiseAP call makes this an idle thread.
	// Therefore there are always enough idle threads for all processors.
	ProcessorIdle();
}

extern "C" void KernelMain() {
	vesaMode = (VESAVideoModeInformation *) (LOW_MEMORY_MAP_START + 0x7000 + bootloaderInformationOffset);
	*((uint32_t *) (LOW_MEMORY_MAP_START + vesaMode->bufferPhysical + 10)) = 0xFFFFFF00;
	while (true);

	Print("---------------------------\n");
	kernelVMM.Initialise();
	memoryManagerVMM.Initialise();
	pmm.Initialise();
	scheduler.Initialise();
	acpi.Initialise(); // Initialises CPULocalStorage.
	scheduler.SpawnThread((uintptr_t) KernelInitialisation, 0, kernelProcess, false);
	KernelLog(LOG_VERBOSE, "Starting preemption...\n");
	scheduler.Start();

	KernelAPMain();
}
