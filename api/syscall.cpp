#ifdef KERNEL
uintptr_t _OSSyscall(uintptr_t argument0, uintptr_t argument1, uintptr_t argument2, 
		uintptr_t unused, uintptr_t argument3, uintptr_t argument4) {
	(void) unused;
	return DoSyscall((OSSyscallType) argument0, argument1, argument2, argument3, argument4, true, nullptr);
}
#endif

void *OSVirtualAllocate(size_t size) {
	intptr_t result = OSSyscall(OS_SYSCALL_ALLOCATE, size, 0, 0, 0);

	if (result >= 0) {
		return (void *) result;
	} else {
		return nullptr;
	}
}

OSError OSVirtualFree(void *address) {
	intptr_t result = OSSyscall(OS_SYSCALL_FREE, (uintptr_t) address, 0, 0, 0);
	return result;
}

OSError OSCreateProcess(const char *executablePath, size_t executablePathLength, OSProcessInformation *information, void *argument) {
	intptr_t result = OSSyscall(OS_SYSCALL_CREATE_PROCESS, (uintptr_t) executablePath, executablePathLength, (uintptr_t) information, (uintptr_t) argument);
	return result;
}

void *OSGetCreationArgument(OSHandle process) {
	return (void *) OSSyscall(OS_SYSCALL_GET_CREATION_ARGUMENT, process, 0, 0, 0);
}

OSHandle OSCreateSurface(size_t width, size_t height) {
	return OSSyscall(OS_SYSCALL_CREATE_SURFACE, width, height, 0, 0);
}

void OSGetLinearBuffer(OSHandle surface, OSLinearBuffer *linearBuffer) {
	OSSyscall(OS_SYSCALL_GET_LINEAR_BUFFER, surface, (uintptr_t) linearBuffer, 0, 0);
}

void OSInvalidateRectangle(OSHandle surface, OSRectangle rectangle) {
	OSSyscall(OS_SYSCALL_INVALIDATE_RECTANGLE, surface, (uintptr_t) &rectangle, 0, 0);
}

void OSCopyToScreen(OSHandle source, OSPoint point, uint16_t depth) {
	OSSyscall(OS_SYSCALL_COPY_TO_SCREEN, source, (uintptr_t) &point, depth, 0);
}

void OSForceScreenUpdate() {
	OSSyscall(OS_SYSCALL_FORCE_SCREEN_UPDATE, 0, 0, 0, 0);
}

void OSFillRectangle(OSHandle surface, OSRectangle rectangle, OSColor color) {
	_OSRectangleAndColor arg;
	arg.rectangle = rectangle;
	arg.color = color;

	OSSyscall(OS_SYSCALL_FILL_RECTANGLE, surface, (uintptr_t) &arg, 0, 0);
}

void OSCopySurface(OSHandle destination, OSHandle source, OSPoint destinationPoint) {
	OSSyscall(OS_SYSCALL_COPY_SURFACE, destination, source, (uintptr_t) &destinationPoint, 0);
}

void OSClearModifiedRegion(OSHandle surface) {
	OSSyscall(OS_SYSCALL_CLEAR_MODIFIED_REGION, surface, 0, 0, 0);
}

OSError OSGetMessage(OSMessage *message) {
	return OSSyscall(OS_SYSCALL_GET_MESSAGE, (uintptr_t) message, 0, 0, 0);
}

OSError OSPostMessage(OSMessage *message) {
	return OSSyscall(OS_SYSCALL_POST_MESSAGE, (uintptr_t) message, 0, 0, 0);
}

OSError OSPostMessageRemote(OSHandle process, OSMessage *message) {
	return OSSyscall(OS_SYSCALL_POST_MESSAGE_REMOTE, (uintptr_t) message, process, 0, 0);
}

OSError OSWaitMessage(uintptr_t timeoutMs) {
	return OSSyscall(OS_SYSCALL_WAIT_MESSAGE, timeoutMs, 0, 0, 0);
}

OSError OSDrawSurface(OSHandle destination, OSHandle source, OSRectangle destinationRegion, OSRectangle sourceRegion, OSRectangle borderRegion, OSDrawMode mode, uint8_t alpha) {
	_OSDrawSurfaceArguments arg;
	arg.destination = destinationRegion;
	arg.source = sourceRegion;
	arg.border = borderRegion;
	arg.alpha = alpha;

	if (destinationRegion.left >= destinationRegion.right || destinationRegion.top >= destinationRegion.bottom) {
		return OS_ERROR_NOTHING_TO_DRAW;
	}

	return OSSyscall(OS_SYSCALL_DRAW_SURFACE, destination, source, (uintptr_t) &arg, mode);
}

OSError OSDrawSurfaceClipped(OSHandle destination, OSHandle source, OSRectangle destinationRegion, OSRectangle sourceRegion, OSRectangle borderRegion, OSDrawMode mode, uint8_t alpha, OSRectangle clipRegion) {
	OSClipDrawSourceRegion(&sourceRegion, &borderRegion, &destinationRegion, &clipRegion, mode);
	return OSDrawSurface(destination, source, clipRegion, sourceRegion, borderRegion, mode, alpha);
}

#ifndef KERNEL
void OSFillRectangleClipped(OSHandle surface, OSRectangle rectangle, OSColor color, OSRectangle clipRegion) {
	OSClipRectangle(rectangle, clipRegion, &rectangle);
	OSFillRectangle(surface, rectangle, color);
}
#endif

OSHandle OSCreateEvent(bool autoReset) {
	return OSSyscall(OS_SYSCALL_CREATE_EVENT, autoReset, 0, 0, 0);
}

void OSSetEvent(OSHandle handle) {
	OSSyscall(OS_SYSCALL_SET_EVENT, handle, 0, 0, 0);
}

void OSResetEvent(OSHandle handle) {
	OSSyscall(OS_SYSCALL_RESET_EVENT, handle, 0, 0, 0);
}

OSError OSPollEvent(OSHandle handle) {
	return OSSyscall(OS_SYSCALL_POLL_EVENT, handle, 0, 0, 0);
}

OSError OSCloseHandle(OSHandle handle) {
	return OSSyscall(OS_SYSCALL_CLOSE_HANDLE, handle, 0, 0, 0);
}

void OSTerminateThread(OSHandle thread) {
	OSSyscall(OS_SYSCALL_TERMINATE_THREAD, thread, 0, 0, 0);
}

void OSTerminateProcess(OSHandle process) {
	OSSyscall(OS_SYSCALL_TERMINATE_PROCESS, process, 0, 0, 0);
}

void OSTerminateThisProcess() {
	OSSyscall(OS_SYSCALL_TERMINATE_PROCESS, OS_CURRENT_PROCESS, 0, 0, 0);
}

OSError OSCreateThread(OSThreadEntryFunction entryFunction, OSThreadInformation *information, void *argument) {
	return OSSyscall(OS_SYSCALL_CREATE_THREAD, (uintptr_t) entryFunction, 0, (uintptr_t) information, (uintptr_t) argument);
}

void *OSReadEntireFile(const char *filePath, size_t filePathLength, size_t *fileSize) {
	OSNodeInformation information;

	if (OS_SUCCESS != OSOpenNode((char *) filePath, filePathLength, OS_OPEN_NODE_READ_ACCESS, &information)) {
		return nullptr;
	}

	*fileSize = information.fileSize;
	void *buffer = OSHeapAllocate(information.fileSize + 1, false);
	((char *) buffer)[information.fileSize] = 0;

	if (information.fileSize != OSReadFileSync(information.handle, 0, information.fileSize, buffer)) {
		OSHeapFree(buffer);
		buffer = nullptr;
	}
	
	OSCloseHandle(information.handle);
	return buffer;
}

OSHandle OSOpenSharedMemory(size_t size, char *name, size_t nameLength, unsigned flags) {
	return OSSyscall(OS_SYSCALL_OPEN_SHARED_MEMORY, size, (uintptr_t) name, nameLength, flags);
}

OSHandle OSShareMemory(OSHandle sharedMemoryRegion, OSHandle targetProcess, bool readOnly) {
	return OSSyscall(OS_SYSCALL_SHARE_MEMORY, sharedMemoryRegion, targetProcess, readOnly, 0);
}

void *OSMapObject(OSHandle sharedMemoryRegion, uintptr_t offset, size_t size, unsigned flags) {
	intptr_t result = OSSyscall(OS_SYSCALL_MAP_OBJECT, sharedMemoryRegion, offset, size, flags);

	if (result >= 0) {
		return (void *) result;
	} else {
		return nullptr;
	}
}

OSError OSOpenNode(char *path, size_t pathLength, uint64_t flags, OSNodeInformation *information) {
	intptr_t result = OSSyscall(OS_SYSCALL_OPEN_NODE, (uintptr_t) path, pathLength, flags, (uintptr_t) information);
	return result;
}

size_t OSReadFileSync(OSHandle handle, uint64_t offset, size_t size, void *buffer) {
	intptr_t result = OSSyscall(OS_SYSCALL_READ_FILE_SYNC, handle, offset, size, (uintptr_t) buffer);
	return result;
}

size_t OSWriteFileSync(OSHandle handle, uint64_t offset, size_t size, void *buffer) {
	intptr_t result = OSSyscall(OS_SYSCALL_WRITE_FILE_SYNC, handle, offset, size, (uintptr_t) buffer);
	return result;
}

OSHandle OSReadFileAsync(OSHandle handle, uint64_t offset, size_t size, void *buffer) {
	intptr_t result = OSSyscall(OS_SYSCALL_READ_FILE_ASYNC, handle, offset, size, (uintptr_t) buffer);
	return result;
}

OSHandle OSWriteFileAsync(OSHandle handle, uint64_t offset, size_t size, void *buffer) {
	intptr_t result = OSSyscall(OS_SYSCALL_WRITE_FILE_ASYNC, handle, offset, size, (uintptr_t) buffer);
	return result;
}

OSError OSResizeFile(OSHandle handle, uint64_t newSize) {
	return OSSyscall(OS_SYSCALL_RESIZE_FILE, handle, newSize, 0, 0);
}

uintptr_t OSWait(OSHandle *handles, size_t count, uintptr_t timeoutMs) {
	return OSSyscall(OS_SYSCALL_WAIT, (uintptr_t) handles, count, timeoutMs, 0);
}

void OSRefreshNodeInformation(OSNodeInformation *information) {
	OSSyscall(OS_SYSCALL_REFRESH_NODE_INFORMATION, (uintptr_t) information, 0, 0, 0);
}

void OSRedrawAll() {
	OSSyscall(OS_SYSCALL_REDRAW_ALL, 0, 0, 0, 0);
}

void OSPauseProcess(OSHandle process, bool resume) {
	OSSyscall(OS_SYSCALL_PAUSE_PROCESS, process, resume, 0, 0);
}

void OSCrashProcess(OSError error) {
	OSSyscall(OS_SYSCALL_CRASH_PROCESS, error, 0, 0, 0);
}

uintptr_t OSGetThreadID(OSHandle thread) {
	return OSSyscall(OS_SYSCALL_GET_THREAD_ID, thread, 0, 0, 0);
}

OSError OSEnumerateDirectoryChildren(OSHandle directory, OSDirectoryChild *buffer, size_t size) {
	return OSSyscall(OS_SYSCALL_ENUMERATE_DIRECTORY_CHILDREN, directory, (uintptr_t) buffer, size, 0);
}

void OSGetIORequestProgress(OSHandle ioRequest, OSIORequestProgress *buffer) {
	OSSyscall(OS_SYSCALL_GET_IO_REQUEST_PROGRESS, ioRequest, (uintptr_t) buffer, 0, 0);
}

void OSCancelIORequest(OSHandle ioRequest) {
	OSSyscall(OS_SYSCALL_CANCEL_IO_REQUEST, ioRequest, 0, 0, 0);
}

void OSBatch(OSBatchCall *calls, size_t count) {
#if 0
	for (uintptr_t i = 0; i < count; i++) {
		OSBatchCall *call = calls + i;
		// ... modify system call for version changes ... 
	}
#endif

	OSSyscall(OS_SYSCALL_BATCH, (uintptr_t) calls, count, 0, 0);
}

OSError OSDeleteNode(OSHandle node) {
	return OSSyscall(OS_SYSCALL_DELETE_NODE, node, 0, 0, 0);
}

OSError OSMoveNode(OSHandle node, OSHandle directory, char *newPath, size_t newPathLength) {
	return OSSyscall(OS_SYSCALL_MOVE_NODE, node, directory, (uintptr_t) newPath, newPathLength);
}

void OSExecuteProgram(const char *name, size_t nameBytes) {
	OSSyscall(OS_SYSCALL_EXECUTE_PROGRAM, (uintptr_t) name, nameBytes, 0, 0);
}

void OSReadConstantBuffer(OSHandle buffer, void *output) {
	OSSyscall(OS_SYSCALL_READ_CONSTANT_BUFFER, buffer, (uintptr_t) output, 0, 0);
}

void OSGetProcessState(OSHandle process, OSProcessState *state) {
	OSSyscall(OS_SYSCALL_GET_PROCESS_STATE, process, (uintptr_t) state, 0, 0);
}

void OSYieldScheduler() {
	OSSyscall(OS_SYSCALL_YIELD_SCHEDULER, 0, 0, 0, 0);
}

void OSSleep(uint64_t milliseconds) {
	OSSyscall(OS_SYSCALL_SLEEP, milliseconds, 0, 0, 0);
}

OSHandle OSTakeSystemSnapshot(int type, size_t *bufferSize) {
	return OSSyscall(OS_SYSCALL_TAKE_SYSTEM_SNAPSHOT, type, (uintptr_t) bufferSize, 0, 0);
}

OSHandle OSOpenProcess(uint64_t pid) {
	return OSSyscall(OS_SYSCALL_OPEN_PROCESS, pid, 0, 0, 0);
}
