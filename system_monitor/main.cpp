// TODO Sorting.

#include "../api/os.h"

#define MANIFEST_PARSER_LIBRARY
#include "../util/manifest_parser.cpp"

#define COMMAND_OPTIONS (1)
#define COMMAND_NEW_TASK (2)
#define COMMAND_NEW_TASK_CONFIRM (3)
#define COMMAND_END_PROCESS (4)
#define COMMAND_END_PROCESS_CONFIRM (5)
#define COMMAND_UPDATE_SPEED_HIGH (6)
#define COMMAND_UPDATE_SPEED_NORMAL (7)
#define COMMAND_UPDATE_SPEED_LOW (8)

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
	bool seenInNewSnapshot;
};

struct Instance {
	OSObject window,
		 statusLabel,
		 taskListing,
		 newTaskTextbox,
		 optionsWindow;

	volatile int updateSpeed, originalUpdateSpeed;
	bool showEndProcessConfirmationDialog;

	ProcessInformation *processes;
	size_t processCount;

	void Initialise();
};

Instance instance;
bool createdInstance;

void TerminateProcess() {
	uint64_t pid = 0;

	for (uintptr_t i = 0; i < instance.processCount; i++) {
		if (instance.processes[i].state & OS_LIST_VIEW_ITEM_SELECTED) {
			pid = instance.processes[i].pid;
			break;
		}
	}

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

OSCallbackResponse CloseOptionsWindow(OSObject object, OSMessage *message) {
	(void) object;
	(void) message;

	if (message->context == (void *) 1) {
		instance.updateSpeed = instance.originalUpdateSpeed;
	} else if (message->context == (void *) 2) {
		instance.showEndProcessConfirmationDialog = OSGetCommandCheck(instance.optionsWindow, commandShowEndProcessConfirmationDialog);
	}

	OSCloseWindow(instance.optionsWindow);
	instance.optionsWindow = nullptr;
	return OS_CALLBACK_HANDLED;
}

OSCallbackResponse Command(OSObject object, OSMessage *message) {
	(void) object;

	if (message->type != OS_NOTIFICATION_COMMAND) {
		return OS_CALLBACK_NOT_HANDLED;
	}

	switch ((uintptr_t) message->context) {
		case COMMAND_OPTIONS: {
			if (!instance.optionsWindow) {
				OSStartGUIAllocationBlock(16384);

				instance.optionsWindow = OSCreateWindow(optionsWindow);
				instance.originalUpdateSpeed = instance.updateSpeed;

				OSObject rootLayout = OSCreateGrid(1, 2, OS_GRID_STYLE_LAYOUT);
				OSSetRootGrid(instance.optionsWindow, rootLayout);

				OSObject layout5 = OSCreateGrid(1, 2, OS_GRID_STYLE_CONTAINER);
				OSAddControl(rootLayout, 0, 0, layout5, OS_CELL_FILL);

				OSObject layout1 = OSCreateGrid(1, 3, OS_GRID_STYLE_GROUP_BOX);
				OSAddControl(layout5, 0, 0, layout1, OS_FLAGS_DEFAULT);
				OSAddControl(layout1, 0, 0, OSCreateLabel(OSLiteral("Confirmations:"), false), OS_FLAGS_DEFAULT);
				OSAddControl(layout1, 0, 1, OSCreateButton(commandShowEndProcessConfirmationDialog, OS_BUTTON_STYLE_NORMAL), OS_CELL_H_INDENT_1);

				OSObject layout2 = OSCreateGrid(1, 5, OS_GRID_STYLE_GROUP_BOX);
				OSAddControl(layout5, 0, 1, layout2, OS_CELL_H_FILL);
				OSAddControl(layout2, 0, 0, OSCreateLabel(OSLiteral("Update speed:"), false), OS_FLAGS_DEFAULT);
				OSAddControl(layout2, 0, 1, OSCreateButton(commandUpdateSpeedLow, OS_BUTTON_STYLE_NORMAL), OS_CELL_H_INDENT_1);
				OSAddControl(layout2, 0, 2, OSCreateButton(commandUpdateSpeedNormal, OS_BUTTON_STYLE_NORMAL), OS_CELL_H_INDENT_1);
				OSAddControl(layout2, 0, 3, OSCreateButton(commandUpdateSpeedHigh, OS_BUTTON_STYLE_NORMAL), OS_CELL_H_INDENT_1);

				OSObject okButton;
				OSObject layout3 = OSCreateGrid(1, 1, OS_GRID_STYLE_CONTAINER_ALT);
				OSObject layout4 = OSCreateGrid(2, 1, OS_GRID_STYLE_CONTAINER_WITHOUT_BORDER);
				OSAddControl(rootLayout, 0, 1, layout3, OS_CELL_H_FILL);
				OSAddControl(layout3, 0, 0, layout4, OS_CELL_H_RIGHT | OS_CELL_H_PUSH);
				OSAddControl(layout4, 0, 0, okButton = OSCreateButton(osDialogStandardOK, OS_BUTTON_STYLE_NORMAL), OS_FLAGS_DEFAULT);
				OSAddControl(layout4, 1, 0, OSCreateButton(osDialogStandardCancel, OS_BUTTON_STYLE_NORMAL), OS_FLAGS_DEFAULT);

				OSPackWindow(instance.optionsWindow);

				switch (instance.updateSpeed) {
					case 500:  OSCheckCommand(instance.optionsWindow, commandUpdateSpeedHigh,   true); break;
					case 2000: OSCheckCommand(instance.optionsWindow, commandUpdateSpeedNormal, true); break;
					case 5000: OSCheckCommand(instance.optionsWindow, commandUpdateSpeedLow,    true); break;
				}

				OSCheckCommand(instance.optionsWindow, commandShowEndProcessConfirmationDialog, instance.showEndProcessConfirmationDialog);
				OSSetFocusedControl(okButton, false);

				OSSetCommandNotificationCallback(instance.optionsWindow, osDialogStandardCancel, OS_MAKE_CALLBACK(CloseOptionsWindow, (void *) 1));
				OSSetCommandNotificationCallback(instance.optionsWindow, osDialogStandardOK, OS_MAKE_CALLBACK(CloseOptionsWindow, (void *) 2));

				OSEndGUIAllocationBlock();
			} else {
				OSSetFocusedWindow(instance.optionsWindow);
			}
		} break;

		case COMMAND_UPDATE_SPEED_HIGH: {
			instance.updateSpeed = 500;
		} break;

		case COMMAND_UPDATE_SPEED_NORMAL: {
			instance.updateSpeed = 2000;
		} break;

		case COMMAND_UPDATE_SPEED_LOW: {
			instance.updateSpeed = 5000;
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

			if (string.bytes) {
				OSExecuteProgram(string.buffer, string.bytes);

				OSCloseWindow(OSGetWindow(instance.newTaskTextbox));
			}
		} break;

		case COMMAND_END_PROCESS: {
			if (instance.showEndProcessConfirmationDialog) {
				OSShowDialogConfirm(OSLiteral("Terminate Process"),
						OSLiteral("Are you sure you want to terminate this process?"),
						OSLiteral("Any unsaved data will be lost."),
						OS_ICON_WARNING, instance.window,
						commandEndProcessConfirm);
			} else {
				TerminateProcess();
			}
		} break;

		case COMMAND_END_PROCESS_CONFIRM: {
			OSCloseWindow(message->command.window);
			TerminateProcess();
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

	// TODO Don't rely on hardcoded paths.
	OSNodeInformation node;
	OSError error = OSOpenNode(OSLiteral("/Programs/System Monitor/Configuration.txt"), OS_OPEN_NODE_WRITE_ACCESS | OS_OPEN_NODE_RESIZE_ACCESS, &node);
	size_t length;

	if (error == OS_SUCCESS) {
		length = OSFormatString(guiStringBuffer, GUI_STRING_BUFFER_LENGTH, 
				"[config]\nupdateSpeed = %d;\nconfirmEndProcess = %z;\n",
				instance.updateSpeed, instance.showEndProcessConfirmationDialog ? "true" : "false");
		error = OSResizeFile(node.handle, length); 
	}

	if (error == OS_SUCCESS) {
		OSWriteFileSync(node.handle, 0, length, guiStringBuffer); 
	}

	if (error != OS_SUCCESS) {
		OSPrint("Warning: Could not save System Monitor configuration.");
	}

	OSTerminateProcess(OS_CURRENT_PROCESS);
	return OS_CALLBACK_HANDLED;
}

void RefreshProcessesThread(void *argument) {
	(void) argument;

	while (true) {
		OSAcquireMutex(&osMessageMutex);

		size_t bufferSize;
		OSHandle snapshotHandle = OSTakeSystemSnapshot(OS_SYSTEM_SNAPSHOT_PROCESSES, &bufferSize);
		OSSnapshotProcesses *snapshot = (OSSnapshotProcesses *) OSHeapAllocate(bufferSize, false);
		OSReadConstantBuffer(snapshotHandle, snapshot);
		OSCloseHandle(snapshotHandle);

		ProcessInformation *current = instance.processes;
		size_t currentCount = instance.processCount;
		size_t newCount = snapshot->count;

		for (uintptr_t j = 0; j < currentCount; j++) {
			current[j].seenInNewSnapshot = false;
		}

		for (uintptr_t i = 0; i < snapshot->count; i++) {
			for (uintptr_t j = 0; j < currentCount; j++) {
				if (snapshot->processes[i].pid == current[j].pid) {
					snapshot->processes[i].internal = 1; // The process was also in the previous snapshot.
					current[j].memory = snapshot->processes[i].memoryUsage;
					current[j].cpu = snapshot->processes[i].cpuTimeSlices - current[j].cpuTimeSlices;
					current[j].cpuTimeSlices = snapshot->processes[i].cpuTimeSlices;
					current[j].seenInNewSnapshot = true;
					newCount--;
					break;
				}
			}
		}

		for (uintptr_t j = 0; j < currentCount; j++) {
			if (!current[j].seenInNewSnapshot) {
				OSMoveMemory(current + j + 1, current + currentCount, -sizeof(ProcessInformation), false);
				OSListViewRemove(instance.taskListing, j, 1, 0);
				j--;
				currentCount--;
				instance.processCount--;
			}
		}

		if (newCount) {
			instance.processes = (ProcessInformation *) OSHeapAllocate((currentCount + newCount) * sizeof(ProcessInformation), false);
			OSCopyMemory(instance.processes, current, currentCount * sizeof(ProcessInformation));
			OSZeroMemory(instance.processes + currentCount, newCount * sizeof(ProcessInformation));
			uintptr_t j = currentCount;
			OSHeapFree(current);
			current = instance.processes;
			instance.processCount += newCount;

			for (uintptr_t i = 0; i < snapshot->count; i++) {
				if (!snapshot->processes[i].internal) {
					current[j].memory = snapshot->processes[i].memoryUsage;
					current[j].cpu = 0;
					current[j].cpuTimeSlices = snapshot->processes[i].cpuTimeSlices;
					current[j].nameLength = snapshot->processes[i].nameLength;
					current[j].pid = snapshot->processes[i].pid;
					OSCopyMemory(current[j].name, snapshot->processes[i].name, snapshot->processes[i].nameLength);
					j++;
				}
			}

			OSListViewInsert(instance.taskListing, currentCount, newCount);
			currentCount += newCount;
		}

		uintptr_t totalTimeSlices = 0;
		bool foundSelection = false;

		for (uintptr_t j = 0; j < currentCount; j++) {
			totalTimeSlices += current[j].cpu;

			if (current[j].state & OS_LIST_VIEW_ITEM_SELECTED) {
				foundSelection = true;
			}
		}

		if (totalTimeSlices) {
			for (uintptr_t j = 0; j < currentCount; j++) {
				current[j].cpu = current[j].cpu * 100 / totalTimeSlices;
			}
		}

		OSEnableCommand(instance.window, commandEndProcess, foundSelection);
		OSSetText(instance.statusLabel, guiStringBuffer, OSFormatString(guiStringBuffer, GUI_STRING_BUFFER_LENGTH, 
					"%d processes", currentCount), OS_RESIZE_MODE_GROW_ONLY);

		OSRepaintControl(instance.taskListing);

		OSHeapFree(snapshot);
		OSReleaseMutex(&osMessageMutex);

		OSSleep(instance.updateSpeed);
	}
}

void ParseConfiguration(Token attribute, Token section, Token name, Token value, int event) {
	(void) attribute;
	(void) section;

	if (event == EVENT_ATTRIBUTE) { 
		if (CompareTokens(name, "updateSpeed")) {
			instance.updateSpeed = OSParseInteger(value.text, value.bytes);
		} else if (CompareTokens(name, "confirmEndProcess")) {
			instance.showEndProcessConfirmationDialog = tolower(value.text[0]) == 't';
		}
	}
}

void Instance::Initialise() {
	createdInstance = true;

	OSStartGUIAllocationBlock(16384);

	instance.updateSpeed = 2000;
	instance.showEndProcessConfirmationDialog = true;

	{
		// TODO Don't rely on hardcoded paths.
		size_t fileSize;
		char *file = (char *) OSReadEntireFile(OSLiteral("/Programs/System Monitor/Configuration.txt"), &fileSize); 

		if (file) {
			ParseManifest(file, ParseConfiguration);
			OSHeapFree(file);
		}
	}

	window = OSCreateWindow(mainWindow);
	OSSetInstance(window, this);

	OSSetCommandNotificationCallback(window, osCommandDestroyWindow, OS_MAKE_CALLBACK(DestroyInstance, nullptr));

	OSObject rootLayout = OSCreateGrid(1, 3, OS_GRID_STYLE_LAYOUT);
	OSSetRootGrid(window, rootLayout);

	OSObject toolbar = OSCreateGrid(3, 1, OS_GRID_STYLE_TOOLBAR);
	OSAddGrid(rootLayout, 0, 0, toolbar, OS_CELL_H_FILL);
	OSAddControl(toolbar, 0, 0, OSCreateButton(commandNewTask, OS_BUTTON_STYLE_TOOLBAR), OS_FLAGS_DEFAULT);
	OSAddControl(toolbar, 1, 0, OSCreateButton(commandEndProcess, OS_BUTTON_STYLE_TOOLBAR), OS_FLAGS_DEFAULT);
	OSAddControl(toolbar, 2, 0, OSCreateButton(commandOptions, OS_BUTTON_STYLE_TOOLBAR), OS_FLAGS_DEFAULT);

	OSObject content = OSCreateGrid(1, 1, OS_GRID_STYLE_CONTAINER);
	OSAddGrid(rootLayout, 0, 1, content, OS_CELL_FILL);
	taskListing = OSCreateListView(OS_CREATE_LIST_VIEW_SINGLE_SELECT | OS_CREATE_LIST_VIEW_CONSTANT_HEIGHT 
			| OS_CREATE_LIST_VIEW_BORDER | OS_CREATE_LIST_VIEW_SORTABLE, OS_LIST_VIEW_ITEM_HEIGHT_DEFAULT);
	OSListViewSetColumns(taskListing, taskListingColumns, sizeof(taskListingColumns) / sizeof(taskListingColumns[0]));
	OSAddControl(content, 0, 0, taskListing, OS_CELL_FILL);
	OSSetObjectNotificationCallback(taskListing, OS_MAKE_CALLBACK(ProcessTaskListingNotification, nullptr));

	OSObject statusBar = OSCreateGrid(2, 1, OS_GRID_STYLE_STATUS_BAR);
	OSAddGrid(rootLayout, 0, 2, statusBar, OS_CELL_H_FILL);
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
