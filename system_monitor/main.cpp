// TODO Implement commands.
// TODO Better task listing state retention across refreshes.

#define OS_NO_CSTDLIB
#include "../api/os.h"

#define COMMAND_OPTIONS (1)
#define COMMAND_NEW_TASK (2)
#define COMMAND_NEW_TASK_CONFIRM (3)
#define COMMAND_END_PROCESS (4)
#define COMMAND_END_PROCESS_CONFIRM (5)

#define OS_MANIFEST_DEFINITIONS
#include "../bin/Programs/System Monitor/manifest.h"

OSListViewColumn taskListingColumns[] = {
#define COLUMN_NAME (0)
	{ OSLiteral("Name"), 150, 50, OS_LIST_VIEW_COLUMN_PRIMARY | OS_LIST_VIEW_COLUMN_SORT_ASCENDING, },
#define COLUMN_PID (1)
	{ OSLiteral("PID"), 50, 50, OS_LIST_VIEW_COLUMN_RIGHT_ALIGNED, },
#define COLUMN_CPU (2)
	{ OSLiteral("CPU"), 50, 50, OS_LIST_VIEW_COLUMN_RIGHT_ALIGNED, },
#define COLUMN_MEMORY (3)
	{ OSLiteral("Memory"), 80, 50, OS_LIST_VIEW_COLUMN_RIGHT_ALIGNED, },
};

#define GUI_STRING_BUFFER_LENGTH (1024)
char guiStringBuffer[GUI_STRING_BUFFER_LENGTH];

struct ProcessInformation {
#define MAX_PROCESS_NAME_LENGTH (80)
	char name[MAX_PROCESS_NAME_LENGTH];
	size_t nameLength;
	int64_t pid, cpu, memory, cpuTimeSlices;
	uint16_t state;
};

struct Instance {
	OSObject window,
		 statusLabel,
		 taskListing,
		 newTaskTextbox;

	ProcessInformation *processes;
	size_t processCount;

	void Initialise();
};

Instance instance;
bool createdInstance;

void TerminateProcess(uint64_t pid) {
	OSHandle process = OSOpenProcess(pid);

	if (process == OS_INVALID_HANDLE) {
		OSShowDialogAlert(OSLiteral("Terminate Process"),
				OSLiteral("The process could not be terminated."),
				OSLiteral("You do not have permission to manage this process."),
				OS_ICON_ERROR, instance.window);
		return;
	}

	OSTerminateProcess(process);
	OSCloseHandle(process);
}

OSCallbackResponse Command(OSObject object, OSMessage *message) {
	(void) object;

	if (message->type != OS_NOTIFICATION_COMMAND) {
		return OS_CALLBACK_NOT_HANDLED;
	}

	switch ((uintptr_t) message->context) {
		case COMMAND_OPTIONS: {
		} break;

		case COMMAND_NEW_TASK: {
			OSShowDialogTextPrompt(OSLiteral("New Task"),
				   OSLiteral("Enter the name of the program you want to start:"),
				   OS_ICON_RUN, instance.window,
				   commandNewTaskConfirm, &instance.newTaskTextbox);
		} break;

		case COMMAND_NEW_TASK_CONFIRM: {
			OSString string;
			OSGetText(instance.newTaskTextbox, &string);
			OSExecuteProgram(string.buffer, string.bytes);

			OSCloseWindow(OSGetWindow(instance.newTaskTextbox));
		} break;

		case COMMAND_END_PROCESS: {
			OSShowDialogConfirm(OSLiteral("Terminate Process"),
				   OSLiteral("Are you sure you want to terminate this process?"),
				   OSLiteral("Any unsaved data will be lost."),
				   OS_ICON_WARNING, instance.window,
				   commandEndProcessConfirm);
		} break;

		case COMMAND_END_PROCESS_CONFIRM: {
			OSCloseWindow(message->command.window);

			for (uintptr_t i = 0; i < instance.processCount; i++) {
				if (instance.processes[i].state & OS_LIST_VIEW_ITEM_SELECTED) {
					TerminateProcess(instance.processes[i].pid);
					break;
				}
			}
		} break;
	}

	return OS_CALLBACK_HANDLED;
}

OSCallbackResponse ProcessTaskListingNotification(OSObject object, OSMessage *message) {
	(void) object;
	
	switch (message->type) {
		case OS_NOTIFICATION_GET_ITEM: {
			uintptr_t index = message->listViewItem.index;
			ProcessInformation *process = instance.processes + index;

			if (index >= instance.processCount) {
				OSCrashProcess(OS_FATAL_ERROR_INDEX_OUT_OF_BOUNDS);
			}

			if (message->listViewItem.mask & OS_LIST_VIEW_ITEM_TEXT) {
				switch (message->listViewItem.column) {
					case COLUMN_NAME: {
						message->listViewItem.textBytes = OSFormatString(guiStringBuffer, GUI_STRING_BUFFER_LENGTH, 
								"%s", process->nameLength, process->name);
					} break;

					case COLUMN_PID: {
						message->listViewItem.textBytes = OSFormatString(guiStringBuffer, GUI_STRING_BUFFER_LENGTH, 
								"%d", process->pid);
					} break;

					case COLUMN_CPU: {
						message->listViewItem.textBytes = OSFormatString(guiStringBuffer, GUI_STRING_BUFFER_LENGTH, 
								"%d%%", process->cpu);
					} break;

					case COLUMN_MEMORY: {
						int64_t memory = process->memory;

						if (memory < 1000000) {
							message->listViewItem.textBytes = OSFormatString(guiStringBuffer, GUI_STRING_BUFFER_LENGTH, 
									"%d.%d KB", memory / 1000, (memory / 100) % 10);
						} else if (memory < 1000000000) {
							message->listViewItem.textBytes = OSFormatString(guiStringBuffer, GUI_STRING_BUFFER_LENGTH, 
									"%d.%d MB", memory / 1000000, (memory / 100000) % 10);
						} else {
							message->listViewItem.textBytes = OSFormatString(guiStringBuffer, GUI_STRING_BUFFER_LENGTH, 
									"%d.%d GB", memory / 1000000000, (memory / 100000000) % 10);
						}
					} break;
				}

				message->listViewItem.text = guiStringBuffer;
			}

			message->listViewItem.state = process->state & ((uint16_t) message->listViewItem.mask);

			if (message->listViewItem.mask & OS_LIST_VIEW_ITEM_HEIGHT) {
				message->listViewItem.height = OS_LIST_VIEW_ITEM_HEIGHT_DEFAULT;
			}

			return OS_CALLBACK_HANDLED;
		} break;

		case OS_NOTIFICATION_SET_ITEM: {
			uintptr_t index = message->listViewItem.index;
			ProcessInformation *process = instance.processes + index;

			if (index >= instance.processCount) {
				OSCrashProcess(OS_FATAL_ERROR_INDEX_OUT_OF_BOUNDS);
			}

			process->state = (process->state & ~((uint16_t) message->listViewItem.mask)) | (message->listViewItem.state & message->listViewItem.mask);

			if (message->listViewItem.mask & OS_LIST_VIEW_ITEM_SELECTED) {
				OSEnableCommand(instance.window, commandEndProcess, process->state & OS_LIST_VIEW_ITEM_SELECTED);
			}

			return OS_CALLBACK_HANDLED;
		} break;

		default: {
			return OS_CALLBACK_NOT_HANDLED;
		} break;
	}
}

OSCallbackResponse DestroyInstance(OSObject object, OSMessage *message) {
	(void) object;
	(void) message;
	OSTerminateProcess(OS_CURRENT_PROCESS);
	return OS_CALLBACK_HANDLED;
}

void RefreshProcessesThread(void *argument) {
	(void) argument;

	while (true) {
		OSAcquireMutex(&osMessageMutex);
		OSListViewReset(instance.taskListing);

		size_t bufferSize;
		OSHandle snapshotHandle = OSTakeSystemSnapshot(OS_SYSTEM_SNAPSHOT_PROCESSES, &bufferSize);
		OSSnapshotProcesses *snapshot = (OSSnapshotProcesses *) OSHeapAllocate(bufferSize, false);
		OSReadConstantBuffer(snapshotHandle, snapshot);
		OSCloseHandle(snapshotHandle);

		ProcessInformation *processes = (ProcessInformation *) OSHeapAllocate(sizeof(ProcessInformation) * snapshot->count, true);
		uintptr_t totalTimeSlices = 0;

		OSEnableCommand(instance.window, commandEndProcess, false);

		for (uintptr_t i = 0; i < snapshot->count; i++) {
			OSSnapshotProcessesItem *item = snapshot->processes + i;
			ProcessInformation *process = processes + i;

			process->nameLength = item->nameLength;
			process->pid = item->pid;
			process->cpu = 0;
			process->memory = item->memoryUsage;
			process->cpuTimeSlices = item->cpuTimeSlices;
			OSCopyMemory(process->name, item->name, item->nameLength);

			for (uintptr_t i = 0; i < instance.processCount; i++) {
				if (instance.processes[i].pid == process->pid) {
					process->state = instance.processes[i].state;
					process->cpu = process->cpuTimeSlices - instance.processes[i].cpuTimeSlices;

					if (process->state & OS_LIST_VIEW_ITEM_SELECTED) {
						OSEnableCommand(instance.window, commandEndProcess, true);
					}

					break;
				}
			}

			totalTimeSlices += process->cpu;
		}

		if (totalTimeSlices) {
			for (uintptr_t i = 0; i < snapshot->count; i++) {
				ProcessInformation *process = processes + i;
				process->cpu = process->cpu * 100 / totalTimeSlices;
			}
		}

		instance.processCount = snapshot->count;
		OSHeapFree(instance.processes);
		OSHeapFree(snapshot);
		instance.processes = processes;
		OSListViewInsert(instance.taskListing, 0, instance.processCount);
		OSReleaseMutex(&osMessageMutex);
		OSSleep(2000);
	}
}

void Instance::Initialise() {
	createdInstance = true;

	OSStartGUIAllocationBlock(16384);

	window = OSCreateWindow(mainWindow);
	OSSetInstance(window, this);

	OSSetCommandNotificationCallback(window, osCommandDestroyWindow, OS_MAKE_CALLBACK(DestroyInstance, nullptr));

	OSObject rootLayout = OSCreateGrid(1, 3, OS_GRID_STYLE_LAYOUT);
	OSSetRootGrid(window, rootLayout);

	OSObject toolbar = OSCreateGrid(3, 1, OS_GRID_STYLE_TOOLBAR);
	OSAddGrid(rootLayout, 0, 0, toolbar, OS_CELL_FILL_H);
	OSAddControl(toolbar, 0, 0, OSCreateButton(commandNewTask, OS_BUTTON_STYLE_TOOLBAR), OS_CELL_V_CENTER | OS_CELL_V_PUSH);
	OSAddControl(toolbar, 1, 0, OSCreateButton(commandEndProcess, OS_BUTTON_STYLE_TOOLBAR), OS_CELL_V_CENTER | OS_CELL_V_PUSH);
	OSAddControl(toolbar, 2, 0, OSCreateButton(commandOptions, OS_BUTTON_STYLE_TOOLBAR), OS_CELL_V_CENTER | OS_CELL_V_PUSH);

	OSObject content = OSCreateGrid(1, 1, OS_GRID_STYLE_CONTAINER);
	OSAddGrid(rootLayout, 0, 1, content, OS_CELL_FILL);
	taskListing = OSCreateListView(OS_CREATE_LIST_VIEW_SINGLE_SELECT | OS_CREATE_LIST_VIEW_CONSTANT_HEIGHT 
			| OS_CREATE_LIST_VIEW_BORDER | OS_CREATE_LIST_VIEW_SORTABLE, OS_LIST_VIEW_ITEM_HEIGHT_DEFAULT);
	OSListViewSetColumns(taskListing, taskListingColumns, sizeof(taskListingColumns) / sizeof(taskListingColumns[0]));
	OSAddControl(content, 0, 0, taskListing, OS_CELL_FILL);
	OSSetObjectNotificationCallback(taskListing, OS_MAKE_CALLBACK(ProcessTaskListingNotification, nullptr));

	OSObject statusBar = OSCreateGrid(2, 1, OS_GRID_STYLE_STATUS_BAR);
	OSAddGrid(rootLayout, 0, 2, statusBar, OS_CELL_FILL_H);
	statusLabel = OSCreateLabel(OSLiteral(""), false);
	OSAddControl(statusBar, 1, 0, statusLabel, OS_FLAGS_DEFAULT);

	OSEndGUIAllocationBlock();

	{
		OSThreadInformation thread;
		OSCreateThread(RefreshProcessesThread, &thread, nullptr);
		OSCloseHandle(thread.handle);
	}
}

OSCallbackResponse ProcessSystemMessage(OSObject _object, OSMessage *message) {
	(void) _object;

	if (message->type == OS_MESSAGE_CREATE_INSTANCE) {
		if (createdInstance) {
			OSSetFocusedWindow(instance.window);
		} else {
			instance.Initialise();
		}

		return OS_CALLBACK_HANDLED;
	} 

	return OS_CALLBACK_NOT_HANDLED;
}

void ProgramEntry() {
	OSSetCallback(osSystemMessages, OS_MAKE_CALLBACK(ProcessSystemMessage, nullptr));
	OSProcessMessages();
}
