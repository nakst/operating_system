#ifndef IMPLEMENTATION

#define KERNEL

#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>
#include <stdbool.h>

#include <mmintrin.h>
#include <xmmintrin.h>
#include <emmintrin.h>

#define OS_FOLDER "/os"

#define MAX_PROCESSORS (256)
#define MAX_PATH (4096)

#define DRIVE_ACCESS_READ (0)
#define DRIVE_ACCESS_WRITE (1)
#define DRIVE_ACCESS_IMPLEMENTATION (2)

#ifdef ARCH_X86_64
#define TIMER_INTERRUPT (0x40)
#define YIELD_IPI (0x41)
// Note: IRQ_BASE is currently 0x50.
#define TLB_SHOOTDOWN_IPI (0xF0)
#define KERNEL_PANIC_IPI (0x0)
#define LOW_MEMORY_MAP_START (0xFFFFFF0000000000)
#endif

enum LogLevel {
	LOG_NONE,
	LOG_VERBOSE,
	LOG_INFO,
	LOG_WARNING,
	LOG_ERROR,
};

void KernelLog(LogLevel level, const char *format, ...);
void KernelPanic(const char *format, ...);
void Print(const char *format, ...);

#include "../api/os.h"
#define Defer(x) OSDefer(x)
#define CF(x) x
#include "../api/common.cpp"
#include "../api/linked_list.cpp"

extern "C" void ProcessorDisableInterrupts();
extern "C" void ProcessorEnableInterrupts();
extern "C" bool ProcessorAreInterruptsEnabled();
extern "C" void ProcessorHalt();
extern "C" void ProcessorIdle();
extern "C" void ProcessorOut8(uint16_t port, uint8_t value);
extern "C" uint8_t ProcessorIn8(uint16_t port);
extern "C" void ProcessorOut16(uint16_t port, uint16_t value);
extern "C" uint16_t ProcessorIn16(uint16_t port);
extern "C" void ProcessorOut32(uint16_t port, uint32_t value);
extern "C" uint32_t ProcessorIn32(uint16_t port);
extern "C" void ProcessorInvalidatePage(uintptr_t virtualAddress);
extern "C" void ProcessorAPStartup();
extern "C" void ProcessorMagicBreakpoint(...);
extern "C" void ProcessorBreakpointHelper(...);
extern "C" struct CPULocalStorage *GetLocalStorage();
extern "C" struct Thread *GetCurrentThread();
extern "C" void ProcessorSetLocalStorage(struct CPULocalStorage *cls);
extern "C" void ProcessorSetThreadStorage(uintptr_t tls);
extern "C" void ProcessorSendIPI(uintptr_t interrupt, bool nmi = false, int processorID = -1);
extern "C" void ProcessorDebugOutputByte(uint8_t byte);
extern "C" void ProcessorFakeTimerInterrupt();
extern "C" uint64_t ProcessorReadTimeStamp();
extern "C" void DoContextSwitch(struct InterruptContext *context, 
		uintptr_t virtualAddressSpace, uintptr_t threadKernelStack, struct Thread *newThread);
extern "C" void ProcessorSetAddressSpace(uintptr_t virtualAddressSpaceIdentifier);
extern "C" uintptr_t ProcessorGetAddressSpace();
extern "C" uintptr_t ProcessorGetRSP();
extern "C" void ProcessorFlushCodeCache();
extern "C" uint64_t ProcessorReadMXCSR();

volatile uintptr_t ipiVector;
extern struct Spinlock ipiLock;

#ifdef ARCH_X86_64
extern "C" uint64_t ProcessorReadCR3();
extern "C" void gdt_data();

volatile uintptr_t tlbShootdownVirtualAddress;
volatile size_t tlbShootdownPageCount;
volatile uintptr_t tlbShootdownRemainingProcessors;

extern "C" CPULocalStorage *cpu_local_storage;

extern "C" bool simdSSE3Support;
extern "C" bool simdSSSE3Support;

extern "C" void SSSE3Framebuffer32To24Copy(volatile uint8_t *destination, volatile uint8_t *source, size_t pixelGroups);
#endif

// Returns the start address of the executable.
uintptr_t LoadELF(char *imageName, size_t imageNameLength);

bool HandlePageFault(uintptr_t page);

void NextTimer(size_t ms); // Receive a TIMER_INTERRUPT in ms ms.
void Delay1MS(); // Spin for 1ms. Use only during initialisation. Not thread-safe.

typedef bool (*IRQHandler)(uintptr_t interruptIndex);
bool RegisterIRQHandler(uintptr_t interruptIndex, IRQHandler handler);

struct Mutex {
	void Acquire();
	void Release();
	void AssertLocked();

	struct Thread *volatile owner;
	uintptr_t acquireAddress, releaseAddress; // TODO Remove in non-debug builds?

	size_t handles;

	LinkedList<struct Thread> blockedThreads;
};

struct Spinlock {
	void Acquire();
	void Release(bool force = false);
	void AssertLocked();

	volatile uint8_t state;
	volatile bool interruptsEnabled;
	struct Thread *volatile owner;
	volatile uintptr_t acquireAddress, releaseAddress;
	volatile uint8_t ownerCPU;
};

struct Event {
	bool Set(bool schedulerAlreadyLocked = false, bool maybeAlreadySet = false);
	void Reset(); 
	bool Poll();

	bool Wait(uint64_t timeoutMs); // See Scheduler::WaitEvents to wait for multiple events.
				       // Returns false if the wait timed out.

	volatile bool autoReset; // This should be first field in the structure,
			         // so that the type of Event can be easily declared with {autoReset}.

	volatile uintptr_t state;
	volatile size_t handles;

	LinkedList<Thread> blockedThreads;
};

typedef void (*AsyncTaskCallback)(void *argument);

struct AsyncTask {
	AsyncTaskCallback callback;
	void *argument;
	struct VirtualAddressSpace *addressSpace;
};

struct CPULocalStorage {
	struct Thread *currentThread, 
		      *idleThread, 
		      *asyncTaskThread;

	bool irqSwitchThread;
	unsigned processorID;
	bool schedulerReady;
	size_t spinlockCount;

	struct ACPIProcessor *acpiProcessor;

#define MAX_ASYNC_TASKS (256)
	volatile AsyncTask asyncTasks[MAX_ASYNC_TASKS];
	volatile uint8_t asyncTasksRead, asyncTasksWrite;
};

struct UniqueIdentifier {
	// Don't mess with this structure, it's used in filesystems.
	uint8_t d[16];
};

struct ConstantBuffer {
	size_t bytes;
	// Data follows.
};

struct ProgramInstance {
	// TODO Do we need a handle to the owner?
	struct Process *volatile owner; // The process to which messages are sent.
	ProgramInstance *parent; // The owner of this process is the only process that can send messages to this instance.
	void *apiObject;
	uintptr_t handles;
	Mutex mutex;
	Event event;
	volatile OSHandle lastResponse;
	volatile size_t lastResponseBytes;
};

UniqueIdentifier installationID; // The identifier of this OS installation, given to us by the bootloader.

struct Process *desktopProcess;

#endif

#include "bitset.cpp"
#include "memory.cpp"
#include "object_manager.cpp"
#include "scheduler.cpp"
#include "terminal.cpp"
#include "vga.cpp"
#include "graphics.cpp"
#include "pci.cpp"
#include "acpi.cpp"

#ifndef IMPLEMENTATION
#include "../api/heap.cpp"
#endif

#ifdef ARCH_X86_64
#include "x86_64.cpp"
#endif

#include "ata.cpp"
#include "ahci.cpp"
#include "vfs.cpp"
#include "esfs.cpp"
#include "ps2.cpp"
#include "devices.cpp"
#include "elf.cpp"
#include "window_manager.cpp"
#include "syscall.cpp"

#ifdef IMPLEMENTATION
#include "../api/syscall.cpp"
#endif
