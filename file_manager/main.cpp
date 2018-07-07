#include "../api/os.h"
#define Defer(x) OSDefer(x)
#include "../api/linked_list.cpp"

#define COMMAND_NAVIGATE_BACKWARDS (1)
#define COMMAND_NAVIGATE_FORWARDS  (2)
#define COMMAND_NAVIGATE_PARENT    (3)
#define COMMAND_NAVIGATE_PATH	   (4)

#define COMMAND_NEW_FOLDER         (1)

#define OS_MANIFEST_DEFINITIONS
#include "../bin/Programs/File Manager/manifest.h"

// TODO Bug: Bookmark folder button not updated properly with multiple instances in the same folder.

struct FolderChild {
	OSDirectoryChild data;
	uint16_t state;
};

struct Instance {
	OSObject folderListing,
		 folderPath,
		 statusLabel,
		 window,
		 bookmarkList,
		 instanceObject,
		 renameTextbox;

	FolderChild *folderChildren;
	size_t folderChildCount;
	size_t selectedChildCount;

	char *path;
	size_t pathBytes;

	LinkedItem<Instance> thisItem;

#define PATH_HISTORY_MAX (64)
	char *pathBackwardHistory[PATH_HISTORY_MAX];
	size_t pathBackwardHistoryBytes[PATH_HISTORY_MAX];
	uintptr_t pathBackwardHistoryPosition;
	char *pathForwardHistory[PATH_HISTORY_MAX];
	size_t pathForwardHistoryBytes[PATH_HISTORY_MAX];
	uintptr_t pathForwardHistoryPosition;

	uintptr_t sortColumn;
	bool sortDescending;

	void Initialise(OSMessage *message);

#define LOAD_FOLDER_BACKWARDS (1)
#define LOAD_FOLDER_FORWARDS (2)
#define LOAD_FOLDER_NO_HISTORY (3)
	bool LoadFolder(char *path, size_t pathBytes, 
			char *path2 = nullptr, size_t pathBytes2 = 0,
			unsigned historyMode = 0);

#define ERROR_CANNOT_LOAD_FOLDER (0)
#define ERROR_CANNOT_CREATE_FOLDER (1)
#define ERROR_INTERNAL (2)
	void ReportError(unsigned where, OSError error);
};

struct Bookmark {
	char *path;
	size_t pathBytes;
};

struct Global {
	Bookmark *bookmarks;
	size_t bookmarkCount, bookmarkAllocated;

	LinkedList<Instance> instances;

	void AddBookmark(char *path, size_t pathBytes);
	bool RemoveBookmark(char *path, size_t pathBytes);
};

Global global;

OSListViewColumn folderListingColumns[] = {
#define COLUMN_NAME (0)
	{ OSLiteral("Name"), 270, 100, OS_LIST_VIEW_COLUMN_PRIMARY | OS_LIST_VIEW_COLUMN_ICON | OS_LIST_VIEW_COLUMN_SORT_ASCENDING, },
#define COLUMN_DATE_MODIFIED (1)
	{ OSLiteral("Date modified"), 120, 50, OS_FLAGS_DEFAULT, },
#define COLUMN_TYPE (2)
	{ OSLiteral("Type"), 120, 50, OS_FLAGS_DEFAULT, },
#define COLUMN_SIZE (3)
	{ OSLiteral("Size"), 100, 50, OS_LIST_VIEW_COLUMN_RIGHT_ALIGNED, },
};

#define GUI_STRING_BUFFER_LENGTH (1024)
char guiStringBuffer[GUI_STRING_BUFFER_LENGTH];

#define PATH_BUFFER_LENGTH (8192)
char pathBuffer[PATH_BUFFER_LENGTH];

int GetFileType(char *name, size_t bytes) {
	int lastSeparator = 0;

	for (intptr_t i = bytes - 1; i >= 0; i--) {
		if (name[i] == '.') {
			lastSeparator = i;
			break;
		}
	}

	name += lastSeparator;
	bytes -= lastSeparator;

#define MATCH_EXTENSION(a) (OSCStringLength(name) == bytes && 0 == OSCompareBytes((void *) (a), name, bytes))

#define FILE_CLASS_EXECUTABLE (0x1000)
#define FILE_CLASS_IMAGE (0x2000)
#define FILE_CLASS_TEXT (0x4000)
#define FILE_CLASS_FONT (0x8000)
#define FILE_CLASS_MISC (0x10000)

	if (MATCH_EXTENSION(".esx")) {
#define FILE_TYPE_EXECUTABLE (FILE_CLASS_EXECUTABLE | 1)
		return FILE_TYPE_EXECUTABLE;
	} else if (MATCH_EXTENSION(".esx_symbols")) {
#define FILE_TYPE_DEBUGGER_DATA (FILE_CLASS_EXECUTABLE | 2)
		return FILE_TYPE_DEBUGGER_DATA;
	} else if (MATCH_EXTENSION(".png")) {
#define FILE_TYPE_PNG_IMAGE (FILE_CLASS_IMAGE | 1)
		return FILE_TYPE_PNG_IMAGE;
	} else if (MATCH_EXTENSION(".jpg")) {
#define FILE_TYPE_JPG_IMAGE (FILE_CLASS_IMAGE | 2)
		return FILE_TYPE_JPG_IMAGE;
	} else if (MATCH_EXTENSION(".ttf")) {
#define FILE_TYPE_TTF_FONT (FILE_CLASS_FONT | 1)
		return FILE_TYPE_TTF_FONT;
	} else if (MATCH_EXTENSION(".a")) {
#define FILE_TYPE_STATIC_LIBRARY (FILE_CLASS_MISC | 1)
		return FILE_TYPE_STATIC_LIBRARY;
	} else if (MATCH_EXTENSION(".h")) {
#define FILE_TYPE_C_HEADER (FILE_CLASS_TEXT | 1)
		return FILE_TYPE_C_HEADER;
	} else if (MATCH_EXTENSION(".txt")) {
#define FILE_TYPE_PLAIN_TEXT (FILE_CLASS_TEXT | 2)
		return FILE_TYPE_PLAIN_TEXT;
	} else {
#define FILE_TYPE_UNKNOWN (0)
		return FILE_TYPE_UNKNOWN;
	}
}

const char *GetFileType(int index) {
	if (index == FILE_TYPE_EXECUTABLE) {
		return "Executable";
	} else if (index == FILE_TYPE_DEBUGGER_DATA) {
		return "Debugger data";
	} else if (index == FILE_TYPE_PNG_IMAGE) {
		return "PNG image";
	} else if (index == FILE_TYPE_JPG_IMAGE) {
		return "JPG image";
	} else if (index == FILE_TYPE_TTF_FONT) {
		return "TTF font";
	} else if (index == FILE_TYPE_STATIC_LIBRARY) {
		return "Static library";
	} else if (index == FILE_TYPE_C_HEADER) {
		return "C/C++ header";
	} else if (index == FILE_TYPE_PLAIN_TEXT) {
		return "Plain text";
	} else {
		return "File";
	}
}

void SelectedChildCountUpdated(Instance *instance) {
#define ONE (1)
#define MANY (2)
#define ENABLE_COMMAND(command, when) OSEnableCommand(instance->instanceObject, (command), (when) == ONE ? (instance->selectedChildCount == 1) : (instance->selectedChildCount));

	ENABLE_COMMAND(commandOpenItem, ONE);
	ENABLE_COMMAND(commandOpenItemProperties, MANY);
	ENABLE_COMMAND(commandRenameItem, MANY);

#undef ONE
#undef MANY
#undef ENABLE_COMMNAND
}

int SortFolder(const void *_a, const void *_b, void *argument) {
	Instance *instance = (Instance *) argument;

	FolderChild *a = (FolderChild *) _a;
	FolderChild *b = (FolderChild *) _b;

	if (a->data.information.type == OS_NODE_FILE && b->data.information.type == OS_NODE_DIRECTORY) {
		return 1;
	} else if (b->data.information.type == OS_NODE_FILE && a->data.information.type == OS_NODE_DIRECTORY) {
		return -1;
	}

	int result = 0;

	switch (instance->sortColumn) {
		case COLUMN_NAME: {
			result = OSCompareStrings(a->data.name, b->data.name, a->data.nameLengthBytes, b->data.nameLengthBytes);
		} break;

		case COLUMN_TYPE: {
			if (a->data.information.type == OS_NODE_FILE) {
				char *s1 = (char *) GetFileType(GetFileType(a->data.name, a->data.nameLengthBytes));
				char *s2 = (char *) GetFileType(GetFileType(b->data.name, b->data.nameLengthBytes));
				result = OSCompareStrings(s1, s2, OSCStringLength(s1), OSCStringLength(s2));
			}
		} break;

		case COLUMN_SIZE: {
			if (a->data.information.type == OS_NODE_FILE) {
				if (a->data.information.fileSize > b->data.information.fileSize) {
					result = 1;
				} else if (a->data.information.fileSize < b->data.information.fileSize) {
					result = -1;
				} else {
					result = 0;
				}
			}
		} break;
	}

	if (!result && instance->sortColumn) {
		// If the two strings were equal, then fallback by sorting by their names.
		result = OSCompareStrings(a->data.name, b->data.name, a->data.nameLengthBytes, b->data.nameLengthBytes);
	}

	return result * (instance->sortDescending ? -1 : 1);
}

OSCallbackResponse CallbackOpenItem(OSNotification *notification) {
	if (notification->type != OS_NOTIFICATION_COMMAND) {
		return OS_CALLBACK_NOT_HANDLED;
	}

	Instance *instance = (Instance *) notification->instanceContext;

	uintptr_t i = 0;

	for (i = 0; i < instance->folderChildCount; i++) {
		if (instance->folderChildren[i].state & OS_LIST_VIEW_ITEM_SELECTED) {
			break;
		}
	}

	if (i == instance->folderChildCount) {
		return OS_CALLBACK_REJECTED;
	}

	uintptr_t index = i;
	FolderChild *child = instance->folderChildren + index;
	OSDirectoryChild *data = &child->data;

	if (data->information.type == OS_NODE_FILE) {
		int fileType = GetFileType(data->name, data->nameLengthBytes);

		if (fileType & FILE_CLASS_EXECUTABLE) {
			size_t length = OSFormatString(guiStringBuffer, GUI_STRING_BUFFER_LENGTH, "%s/%s", 
					instance->pathBytes, instance->path, data->nameLengthBytes, data->name);
			OSExecuteProgram(guiStringBuffer, length);
		}
	} else if (data->information.type == OS_NODE_DIRECTORY) {
		char *existingPath = instance->path;
		size_t existingPathBytes = instance->pathBytes;

		char *folderName = data->name;
		size_t folderNameBytes = data->nameLengthBytes;

		instance->LoadFolder(existingPath, existingPathBytes, folderName, folderNameBytes);
	}

	return OS_CALLBACK_HANDLED;
}

OSCallbackResponse CommandNew(OSNotification *notification) {
	if (notification->type != OS_NOTIFICATION_COMMAND) {
		return OS_CALLBACK_NOT_HANDLED;
	}

	Instance *instance = (Instance *) notification->instanceContext;

	switch ((uintptr_t) notification->context) {
		case COMMAND_NEW_FOLDER: {
			size_t length;
			uintptr_t attempt = 1;

			while (attempt < 1000) {
				if (attempt == 1) {
					length = OSFormatString(guiStringBuffer, GUI_STRING_BUFFER_LENGTH, "New folder");
				} else {
					length = OSFormatString(guiStringBuffer, GUI_STRING_BUFFER_LENGTH, "New folder %d", attempt);
				}

				for (uintptr_t i = 0; i < instance->folderChildCount; i++) {
					FolderChild *child = instance->folderChildren + i;

					if (OSCompareStrings(child->data.name, guiStringBuffer, child->data.nameLengthBytes, length) == 0) {
						goto nextAttempt;
					}
				}

				break;
				nextAttempt:;
				attempt++;
			}

			bool addSeparator = instance->pathBytes > 1;
			size_t fullPathLength = instance->pathBytes + length + (addSeparator ? 1 : 0);
			char *fullPath = (char *) OSHeapAllocate(fullPathLength, false);
			OSCopyMemory(fullPath, instance->path, instance->pathBytes);
			if (addSeparator) fullPath[instance->pathBytes] = '/';
			OSCopyMemory(fullPath + instance->pathBytes + (addSeparator ? 1 : 0), guiStringBuffer, length);

			OSNodeInformation node;
			OSError error = OSOpenNode(fullPath, fullPathLength, OS_OPEN_NODE_DIRECTORY | OS_OPEN_NODE_FAIL_IF_FOUND, &node);

			if (error != OS_SUCCESS) {
				instance->ReportError(ERROR_CANNOT_CREATE_FOLDER, error);
			} else {
				OSCloseHandle(node.handle);
				instance->LoadFolder(instance->path, instance->pathBytes, nullptr, 0, LOAD_FOLDER_NO_HISTORY);
			}

			OSHeapFree(fullPath);
		} break;
	}

	return OS_CALLBACK_HANDLED;
}

OSCallbackResponse CommandNavigate(OSNotification *notification) {
	if (notification->type != OS_NOTIFICATION_COMMAND) {
		return OS_CALLBACK_NOT_HANDLED;
	}

	Instance *instance = (Instance *) notification->instanceContext;

	switch ((uintptr_t) notification->context) {
		case COMMAND_NAVIGATE_BACKWARDS: {
			instance->pathBackwardHistoryPosition--;
			instance->LoadFolder(instance->pathBackwardHistory[instance->pathBackwardHistoryPosition],
					instance->pathBackwardHistoryBytes[instance->pathBackwardHistoryPosition],
					nullptr, 0, LOAD_FOLDER_BACKWARDS);
		} break;
	
		case COMMAND_NAVIGATE_FORWARDS: {
			instance->pathForwardHistoryPosition--;
			instance->LoadFolder(instance->pathForwardHistory[instance->pathForwardHistoryPosition],
					instance->pathForwardHistoryBytes[instance->pathForwardHistoryPosition],
					nullptr, 0, LOAD_FOLDER_FORWARDS);
		} break;
	
		case COMMAND_NAVIGATE_PARENT: {
			size_t s = instance->pathBytes;
	
			while (true) {
				if (instance->path[--s] == '/') {
					break;
				}
			}
	
			if (!s) s++;
	
			instance->LoadFolder(instance->path, s);
		} break;
	
		case COMMAND_NAVIGATE_PATH: {
			OSString string;
			OSGetText(notification->generator, &string);

			if (!instance->LoadFolder(string.buffer, string.bytes)) {
				return OS_CALLBACK_REJECTED;
			}
		} break;
	}

	OSEnableCommand(notification->instance, commandNavigateBackwards, instance->pathBackwardHistoryPosition);
	OSEnableCommand(notification->instance, commandNavigateForwards, instance->pathForwardHistoryPosition);

	return OS_CALLBACK_HANDLED;
}

OSCallbackResponse CallbackBookmarkFolder(OSNotification *notification) {
	if (notification->type != OS_NOTIFICATION_COMMAND) {
		return OS_CALLBACK_NOT_HANDLED;
	}

	Instance *instance = (Instance *) notification->instanceContext;
	bool checked = notification->command.checked;

	if (checked) {
		// Add bookmark.
		global.AddBookmark(instance->path, instance->pathBytes);
	} else {
		// Remove bookmark.
		if (!global.RemoveBookmark(instance->path, instance->pathBytes)) {
			instance->ReportError(ERROR_INTERNAL, OS_ERROR_UNKNOWN_OPERATION_FAILURE);
		}
	}

	return OS_CALLBACK_HANDLED;
}

OSCallbackResponse CallbackRenameItem(OSNotification *notification) {
	if (notification->type != OS_NOTIFICATION_COMMAND) {
		return OS_CALLBACK_NOT_HANDLED;
	}

	Instance *instance = (Instance *) notification->instanceContext;

	if (instance->selectedChildCount == 1) {
		// TODO.
		return OS_CALLBACK_REJECTED;
	} else if (instance->selectedChildCount) {
		OSShowDialogTextPrompt(OSLiteral("Rename Multiple"),
				OSLiteral("Enter a Lua expression for the new names."),
				instance->instanceObject,
				OS_ICON_RENAME, instance->window,
				commandRenameItemConfirm, &instance->renameTextbox);
		OSSetText(instance->renameTextbox, OSLiteral("name .. \".\" .. extension"), OS_RESIZE_MODE_IGNORE);
		OSSetCommandNotificationCallback(notification->instance, osDialogStandardCancel, OS_MAKE_NOTIFICATION_CALLBACK(CallbackRenameItemConfirm, (void *) 1));
	}

	return OS_CALLBACK_HANDLED;
}

OSCallbackResponse CallbackRenameItemConfirm(OSNotification *notification) {
	if (notification->type != OS_NOTIFICATION_COMMAND) {
		return OS_CALLBACK_NOT_HANDLED;
	}

	Instance *instance = (Instance *) notification->instanceContext;

	if (instance->selectedChildCount == 1) {
		// TODO.
		return OS_CALLBACK_REJECTED;
	}

	if (!notification->context) {
		OSString expression;
		OSGetText(instance->renameTextbox, &expression);

		OSObject lua = OSCreateInstance(nullptr, nullptr);
		OSOpenInstance(lua, instance->instanceObject, OSLiteral("lua"), OS_FLAGS_DEFAULT);

		size_t bufferSize = 4096;
		char *buffer = (char *) OSHeapAllocate(bufferSize, false);
		bool stop = false;

		for (uintptr_t i = 0; i < instance->folderChildCount && !stop; i++) {
			if (!(instance->folderChildren[i].state & OS_LIST_VIEW_ITEM_SELECTED)) {
				continue;
			}

			OSDirectoryChild *child = &instance->folderChildren[i].data;
			size_t extensionSeparator = child->nameLengthBytes - 1;

			for (intptr_t i = child->nameLengthBytes - 1; i >= 0; i--) {
				if (child->name[i] == '.') {
					extensionSeparator = i;
					break;
				}
			}

			size_t responseBytes;
			OSHandle response = OSIssueRequest(lua, buffer, 
					OSFormatString(buffer, bufferSize, 
						"MAP\f" "%s\f" "full = \"%s\"\f" "name = \"%s\"\f" "extension = \"%s\"\f", 
						expression.bytes, expression.buffer, 
						child->nameLengthBytes, child->name,
						extensionSeparator, child->name,
						child->nameLengthBytes - extensionSeparator - 1, child->name + extensionSeparator + 1), 
					1000 /*1 second timeout*/, &responseBytes); 

			if (response && responseBytes) {
				char *output = (char *) OSHeapAllocate(responseBytes, false);
				OSReadConstantBuffer(response, output);

				OSPrint("Output is '%s'.\n", responseBytes, output);

				if (0 == OSCompareStrings(output, child->name, responseBytes, child->nameLengthBytes)) {
					continue;
				}

				OSNodeInformation node;
				OSError error;

				error = OSOpenNode(pathBuffer, OSFormatString(pathBuffer, PATH_BUFFER_LENGTH, "%s/%s", 
							instance->pathBytes, instance->path, child->nameLengthBytes, child->name), 
						OS_OPEN_NODE_FAIL_IF_NOT_FOUND, &node);
				if (error == OS_SUCCESS) error = OSRenameNode(node.handle, output, responseBytes); 
				if (error != OS_SUCCESS) stop = true;

				OSHeapFree(output);
			} else {
				stop = true;
			}

			if (response) {
				OSCloseHandle(response);
			}
		}

		if (stop) {
			// TODO This doesn't work correctly?
			OSShowDialogAlert(OSLiteral("Error"),
					OSLiteral("The operation failed."),
					OSLiteral("TODO: write a proper error message!"),
					notification->instance,
					OS_ICON_ERROR, instance->window);
		}

		OSHeapFree(buffer);
		OSDestroyInstance(lua);
		// TODO Make this actually destroy the instance?

		// TODO Better folder refreshing.
		instance->LoadFolder(instance->path, instance->pathBytes, nullptr, 0, LOAD_FOLDER_NO_HISTORY);
	}

	OSCloseWindow(OSGetWindow(notification->generator));
	return OS_CALLBACK_HANDLED;
}

OSCallbackResponse ProcessBookmarkListingNotification(OSNotification *notification) {
	Instance *instance = (Instance *) notification->context;
	
	switch (notification->type) {
		case OS_NOTIFICATION_GET_ITEM: {
			uintptr_t index = notification->listViewItem.index;
			Bookmark *bookmark = global.bookmarks + index;

			if (notification->listViewItem.mask & OS_LIST_VIEW_ITEM_TEXT) {
				size_t length = 0;

				if (bookmark->pathBytes != 1) {
					while (bookmark->path[bookmark->pathBytes - ++length] != '/');
					length--;
				} else {
					length = 1;
				}

				notification->listViewItem.textBytes = OSFormatString(guiStringBuffer, GUI_STRING_BUFFER_LENGTH, 
						"%s", length, bookmark->path + bookmark->pathBytes - length);
				notification->listViewItem.text = guiStringBuffer;
			}

			if (notification->listViewItem.mask & OS_LIST_VIEW_ITEM_SELECTED) {
				if (!OSCompareStrings(bookmark->path, instance->path, bookmark->pathBytes, instance->pathBytes)) {
					notification->listViewItem.state |= OS_LIST_VIEW_ITEM_SELECTED;
				}
			}

			if (notification->listViewItem.mask & OS_LIST_VIEW_ITEM_ICON) {
				notification->listViewItem.iconID = OS_ICON_FOLDER;
			}

			if (notification->listViewItem.mask & OS_LIST_VIEW_ITEM_HEIGHT) {
				notification->listViewItem.height = OS_LIST_VIEW_ITEM_HEIGHT_DEFAULT;
			}

			return OS_CALLBACK_HANDLED;
		} break;

		case OS_NOTIFICATION_SET_ITEM: {
			uintptr_t index = notification->listViewItem.index;
			Bookmark *bookmark = global.bookmarks + index;

			if (notification->listViewItem.mask & OS_LIST_VIEW_ITEM_SELECTED) {
				if (notification->listViewItem.state & OS_LIST_VIEW_ITEM_SELECTED) {
					instance->LoadFolder(bookmark->path, bookmark->pathBytes);
				}
			}

			return OS_CALLBACK_HANDLED;
		} break;

		default: {
			return OS_CALLBACK_NOT_HANDLED;
		} break;
	}
}

OSCallbackResponse ProcessFolderListingNotification(OSNotification *notification) {
	Instance *instance = (Instance *) notification->context;
	
	switch (notification->type) {
		case OS_NOTIFICATION_GET_ITEM: {
			uintptr_t index = notification->listViewItem.index;
			FolderChild *child = instance->folderChildren + index;
			OSDirectoryChild *data = &child->data;

			if (index >= instance->folderChildCount) {
				OSCrashProcess(OS_FATAL_ERROR_INDEX_OUT_OF_BOUNDS);
			}

			if (notification->listViewItem.mask & OS_LIST_VIEW_ITEM_TEXT) {
				switch (notification->listViewItem.column) {
					case COLUMN_NAME: {
						notification->listViewItem.textBytes = OSFormatString(guiStringBuffer, GUI_STRING_BUFFER_LENGTH, 
								"%s", data->nameLengthBytes, data->name);
					} break;

					case COLUMN_DATE_MODIFIED: {
						notification->listViewItem.textBytes = 0;
					} break;

					case COLUMN_TYPE: {
						notification->listViewItem.textBytes = OSFormatString(guiStringBuffer, GUI_STRING_BUFFER_LENGTH, 
								"%z", data->information.type == OS_NODE_FILE ? GetFileType(GetFileType(data->name, data->nameLengthBytes)) : "Folder");
					} break;

					case COLUMN_SIZE: {
						notification->listViewItem.textBytes = 0;

						if (data->information.type == OS_NODE_FILE) {
							int fileSize = data->information.fileSize;

							if (fileSize == 0) {
								notification->listViewItem.textBytes = OSFormatString(guiStringBuffer, GUI_STRING_BUFFER_LENGTH, 
										"(empty)");
							} else if (fileSize == 1) {
								notification->listViewItem.textBytes = OSFormatString(guiStringBuffer, GUI_STRING_BUFFER_LENGTH, 
										"1 byte", fileSize);
							} else if (fileSize < 1000) {
								notification->listViewItem.textBytes = OSFormatString(guiStringBuffer, GUI_STRING_BUFFER_LENGTH, 
										"%d bytes", fileSize);
							} else if (fileSize < 1000000) {
								notification->listViewItem.textBytes = OSFormatString(guiStringBuffer, GUI_STRING_BUFFER_LENGTH, 
										"%d.%d KB", fileSize / 1000, (fileSize / 100) % 10);
							} else if (fileSize < 1000000000) {
								notification->listViewItem.textBytes = OSFormatString(guiStringBuffer, GUI_STRING_BUFFER_LENGTH, 
										"%d.%d MB", fileSize / 1000000, (fileSize / 100000) % 10);
							} else {
								notification->listViewItem.textBytes = OSFormatString(guiStringBuffer, GUI_STRING_BUFFER_LENGTH, 
										"%d.%d GB", fileSize / 1000000000, (fileSize / 100000000) % 10);
							}
						} else if (data->information.type == OS_NODE_DIRECTORY) {
							uint64_t children = data->information.directoryChildren;

							if (children == 0) notification->listViewItem.textBytes = OSFormatString(guiStringBuffer, GUI_STRING_BUFFER_LENGTH, "(empty)");
							else if (children == 1) notification->listViewItem.textBytes = OSFormatString(guiStringBuffer, GUI_STRING_BUFFER_LENGTH, "1 item");
							else notification->listViewItem.textBytes = OSFormatString(guiStringBuffer, GUI_STRING_BUFFER_LENGTH, "%d items", children);
						}
					} break;
				}

				notification->listViewItem.text = guiStringBuffer;
			}

			notification->listViewItem.state = child->state & ((uint16_t) notification->listViewItem.mask);

			if (notification->listViewItem.mask & OS_LIST_VIEW_ITEM_ICON) {
				notification->listViewItem.iconID = data->information.type == OS_NODE_DIRECTORY ? OS_ICON_FOLDER : OS_ICON_FILE;
			}

			if (notification->listViewItem.mask & OS_LIST_VIEW_ITEM_HEIGHT) {
				notification->listViewItem.height = OS_LIST_VIEW_ITEM_HEIGHT_DEFAULT;
			}

			return OS_CALLBACK_HANDLED;
		} break;

		case OS_NOTIFICATION_SET_ITEM: {
			uintptr_t index = notification->listViewItem.index;

			if (index >= instance->folderChildCount) {
				OSCrashProcess(OS_FATAL_ERROR_INDEX_OUT_OF_BOUNDS);
			}

			FolderChild *child = instance->folderChildren + index;
	
			if (child->state & OS_LIST_VIEW_ITEM_SELECTED) {
				instance->selectedChildCount--;
			}

			child->state = (child->state & ~((uint16_t) notification->listViewItem.mask)) | (notification->listViewItem.state & notification->listViewItem.mask);

			if (child->state & OS_LIST_VIEW_ITEM_SELECTED) {
				instance->selectedChildCount++;
			}

			SelectedChildCountUpdated(instance);

			return OS_CALLBACK_HANDLED;
		} break;

		case OS_NOTIFICATION_CHOOSE_ITEM: {
			OSIssueCommand(notification->instance, commandOpenItem);

			return OS_CALLBACK_HANDLED;
		} break;

		case OS_NOTIFICATION_SET_ITEM_RANGE: {
			if (notification->listViewItemRange.indexFrom < 0 || notification->listViewItemRange.indexFrom > (int) instance->folderChildCount
					|| notification->listViewItemRange.indexTo < 0 || notification->listViewItemRange.indexTo > (int) instance->folderChildCount) {
				OSCrashProcess(OS_FATAL_ERROR_INDEX_OUT_OF_BOUNDS);
			}

			for (int i = notification->listViewItemRange.indexFrom; i < notification->listViewItemRange.indexTo; i++) {
				FolderChild *child = instance->folderChildren + i;

				if (child->state & OS_LIST_VIEW_ITEM_SELECTED) {
					instance->selectedChildCount--;
				}

				child->state &= ~notification->listViewItemRange.mask;
				child->state |= notification->listViewItemRange.mask & notification->listViewItemRange.state;

				if (child->state & OS_LIST_VIEW_ITEM_SELECTED) {
					instance->selectedChildCount++;
				}
			}

			SelectedChildCountUpdated(instance);

			return OS_CALLBACK_HANDLED;
		} break;

		case OS_NOTIFICATION_SORT_COLUMN: {
			OSListViewReset(instance->folderListing);
			instance->selectedChildCount = 0;

			instance->sortColumn = notification->listViewColumn.index;
			instance->sortDescending = notification->listViewColumn.descending;
			OSSort(instance->folderChildren, instance->folderChildCount, sizeof(FolderChild), SortFolder, instance);

			OSListViewInsert(instance->folderListing, 0, instance->folderChildCount);

			return OS_CALLBACK_HANDLED;
		} break;

		case OS_NOTIFICATION_RIGHT_CLICK: {
			if (instance->selectedChildCount == 1) {
				OSCreateMenu(menuItemContext, notification->generator, OS_CREATE_MENU_AT_CURSOR, OS_FLAGS_DEFAULT);
			} else if (instance->selectedChildCount) {
				OSCreateMenu(menuMultipleItemContext, notification->generator, OS_CREATE_MENU_AT_CURSOR, OS_FLAGS_DEFAULT);
			} else {
				OSCreateMenu(menuFolderListingContext, notification->generator, OS_CREATE_MENU_AT_CURSOR, OS_FLAGS_DEFAULT);
			}

			return OS_CALLBACK_HANDLED;
		} break;

		default: {
			return OS_CALLBACK_NOT_HANDLED;
		} break;
	}
}

bool Global::RemoveBookmark(char *path, size_t pathBytes) {
	for (uintptr_t i = 0; i < bookmarkCount; i++) {
		if (OSCompareStrings(bookmarks[i].path, path, bookmarks[i].pathBytes, pathBytes) == 0) {
			OSMoveMemory(bookmarks + i + 1, bookmarks + bookmarkCount, -1 * sizeof(Bookmark), false);
			bookmarkCount--;

			{
				LinkedItem<Instance> *instance = instances.firstItem;

				while (instance) {
					OSListViewRemove(instance->thisItem->bookmarkList, i, 1, 0);
					instance = instance->nextItem;
				}
			}

			return true;
		}
	}

	return false;
}

void Global::AddBookmark(char *path, size_t pathBytes) {
	if (bookmarkAllocated == bookmarkCount) {
		bookmarkAllocated = (bookmarkAllocated + 8) * 2;
		Bookmark *replacement = (Bookmark *) OSHeapAllocate(bookmarkAllocated * sizeof(Bookmark), false);
		OSCopyMemory(replacement, bookmarks, bookmarkCount * sizeof(Bookmark));
		OSHeapFree(bookmarks);
		bookmarks = replacement;
	}

	Bookmark *bookmark = bookmarks + bookmarkCount;

	bookmark->pathBytes = pathBytes;
	bookmark->path = (char *) OSHeapAllocate(pathBytes, false);
	OSCopyMemory(bookmark->path, path, pathBytes);

	bookmarkCount++;

	{
		LinkedItem<Instance> *instance = instances.firstItem;

		while (instance) {
			OSListViewInsert(instance->thisItem->bookmarkList, bookmarkCount - 1, 1);
			instance = instance->nextItem;
		}
	}
}

void Instance::ReportError(unsigned where, OSError error) {
	const char *message = "An unknown error occurred.";
	const char *description = "Please try again.";

	switch (where) {
		case ERROR_CANNOT_LOAD_FOLDER: {
			message = "Could not open the folder.";
			description = "The specified path was invalid.";
		} break;

		case ERROR_CANNOT_CREATE_FOLDER: {
			message = "Could not create a new folder.";
		} break;

		case ERROR_INTERNAL: {
			message = "An internal error occurred.";
		} break;
	}

	switch (error) {
		case OS_ERROR_PATH_NOT_TRAVERSABLE: {
			description = "One or more of the leading folders did not exist.";
		} break;

		case OS_ERROR_FILE_DOES_NOT_EXIST: {
			description = "The folder does not exist.";
		} break;

		case OS_ERROR_FILE_ALREADY_EXISTS: {
			description = "The folder already exists.";
		} break;

		case OS_ERROR_FILE_PERMISSION_NOT_GRANTED: {
			description = "You do not have permission to view the contents of this folder.";
		} break;

		case OS_ERROR_INCORRECT_NODE_TYPE: {
			description = "This is not a valid folder.";
		} break;

		case OS_ERROR_DRIVE_CONTROLLER_REPORTED: {
			description = "An error occurred while accessing your drive.";
		} break;
	}

	OSShowDialogAlert(OSLiteral("Error"), OSLiteral(message), OSLiteral(description), 
			instanceObject, OS_ICON_ERROR, window);
}

bool Instance::LoadFolder(char *path1, size_t pathBytes1, char *path2, size_t pathBytes2, unsigned historyMode) {
	if (!pathBytes1) return false;

	char *oldPath = path;
	size_t oldPathBytes = pathBytes;
	char *newPath;

	{
		goto normal;
		fail:;
		OSHeapFree(newPath);
		return false;
		normal:;
	}

	// Fix the paths.
	if (path2 || pathBytes1 != 1) {
		if (path1[pathBytes1 - 1] == '/') pathBytes1--;
		if (path2 && path2[pathBytes2 - 1] == '/') pathBytes2--;
	}

	// Create the path.
	newPath = (char *) OSHeapAllocate(pathBytes1 + (path2 ? (pathBytes2 + 1) : 0), false);
	OSCopyMemory(newPath, path1, pathBytes1);
	size_t newPathBytes = pathBytes1;

	if (path2) {
		newPath[newPathBytes] = '/';
		OSCopyMemory(newPath + newPathBytes + 1, path2, pathBytes2);
		newPathBytes += pathBytes2 + 1;
	}

	OSNodeInformation node;
	OSError error;

	// Open the directory.
	error = OSOpenNode(newPath, newPathBytes, OS_OPEN_NODE_DIRECTORY | OS_OPEN_NODE_FAIL_IF_NOT_FOUND, &node);

	if (error != OS_SUCCESS) {
		ReportError(ERROR_CANNOT_LOAD_FOLDER, error);
		goto fail;
	}

	OSDefer(OSCloseHandle(node.handle));

	// Get the directory's children.
	size_t childCount = node.directoryChildren + 1024 /*Just in case extra files are created between OSOpenNode and here.*/;
	OSDirectoryChild *children = (OSDirectoryChild *) OSHeapAllocate(childCount * sizeof(OSDirectoryChild), true);
	OSDefer(OSHeapFree(children));
	error = OSEnumerateDirectoryChildren(node.handle, children, childCount);

	if (error != OS_SUCCESS) {
		ReportError(ERROR_CANNOT_LOAD_FOLDER, error);
		goto fail;
	}

	// Work out how many there were.
	for (uintptr_t i = 0; i < childCount; i++) {
		if (!children[i].information.present) {
			childCount = i;
			break;
		}
	}

	// Allocate memory to store the children.
	OSHeapFree(folderChildren);
	folderChildren = (FolderChild *) OSHeapAllocate(childCount * sizeof(FolderChild), true);
	folderChildCount = childCount;

	// Copy across the data.
	for (uintptr_t i = 0; i < childCount; i++) {
		OSCopyMemory(&folderChildren[i].data, children + i, sizeof(OSDirectoryChild));
	}

	// Sort the folder.
	OSSort(folderChildren, folderChildCount, sizeof(FolderChild), SortFolder, this);

	// Confirm the new path.
	path = newPath;
	pathBytes = newPathBytes;

	// Update the UI.
	OSListViewReset(folderListing);
	OSListViewInsert(folderListing, 0, childCount);
	OSSetText(folderPath, path, pathBytes, OS_RESIZE_MODE_IGNORE);
	OSEnableCommand(instanceObject, commandNavigateParent, pathBytes1 != 1);
	OSListViewInvalidate(bookmarkList, 0, global.bookmarkCount);

	selectedChildCount = 0;
	SelectedChildCountUpdated(this);

	{
		bool found = false;

		for (uintptr_t i = 0; i < global.bookmarkCount; i++) {
			if (OSCompareStrings(global.bookmarks[i].path, path, global.bookmarks[i].pathBytes, pathBytes) == 0) {
				found = true;
				break;
			}
		}

		OSCheckCommand(instanceObject, commandBookmarkFolder, found);
	}

	// Add the previous folder to the history.
	if (oldPath && historyMode != LOAD_FOLDER_NO_HISTORY) {
		char **history = historyMode == LOAD_FOLDER_BACKWARDS ? pathForwardHistory : pathBackwardHistory;
		size_t *historyBytes = historyMode == LOAD_FOLDER_BACKWARDS ? pathForwardHistoryBytes : pathBackwardHistoryBytes;
		uintptr_t &historyPosition = historyMode == LOAD_FOLDER_BACKWARDS ? pathForwardHistoryPosition : pathBackwardHistoryPosition;

		if (historyPosition == PATH_HISTORY_MAX) {
			OSHeapFree(history[0]);
			OSCopyMemoryReverse(history, history + 1, sizeof(char *) * (PATH_HISTORY_MAX - 1));
		}

		history[historyPosition] = oldPath;
		historyBytes[historyPosition] = oldPathBytes;
		historyPosition++;

		OSEnableCommand(instanceObject, historyMode == LOAD_FOLDER_BACKWARDS ? commandNavigateForwards : commandNavigateBackwards, true);

		// If this was a normal navigation, clear the forward history.
		if (!historyMode) {
			while (pathForwardHistoryPosition) {
				OSHeapFree(pathForwardHistory[--pathForwardHistoryPosition]);
			}

			OSEnableCommand(instanceObject, commandNavigateForwards, false);
		}
	}

	// Update the status label.
	{
		size_t length;
		if (folderChildCount == 0) length = OSFormatString(guiStringBuffer, GUI_STRING_BUFFER_LENGTH, "(empty)");
		else if (folderChildCount == 1) length = OSFormatString(guiStringBuffer, GUI_STRING_BUFFER_LENGTH, "1 item");
		else length = OSFormatString(guiStringBuffer, GUI_STRING_BUFFER_LENGTH, "%d items", folderChildCount);
		OSSetText(statusLabel, guiStringBuffer, length, OS_RESIZE_MODE_GROW_ONLY);
	}

	return true;
}

OSCallbackResponse DestroyInstance(OSNotification *notification) {
	if (notification->type != OS_NOTIFICATION_WINDOW_CLOSE) return OS_CALLBACK_NOT_HANDLED;
	Instance *instance = (Instance *) notification->context;
	OSHeapFree(instance->folderChildren);
	OSHeapFree(instance->path);
	global.instances.Remove(&instance->thisItem);
	OSDestroyInstance(notification->instance);
	OSHeapFree(instance);
	return OS_CALLBACK_HANDLED;
}

void Instance::Initialise(OSMessage *message) {
	thisItem.thisItem = this;
	global.instances.InsertEnd(&thisItem);
	instanceObject = OSCreateInstance(this, message);

	OSStartGUIAllocationBlock(32768);

	window = OSCreateWindow(mainWindow, instanceObject);
	OSSetObjectNotificationCallback(window, OS_MAKE_NOTIFICATION_CALLBACK(DestroyInstance, this));

	OSObject rootLayout = OSCreateGrid(1, 6, OS_GRID_STYLE_LAYOUT);
	OSObject contentSplit = OSCreateGrid(3, 1, OS_GRID_STYLE_LAYOUT);
	OSObject toolbar1 = OSCreateGrid(5, 1, OS_GRID_STYLE_TOOLBAR);
	OSObject toolbar2 = OSCreateGrid(2, 1, OS_GRID_STYLE_TOOLBAR_ALT);
	OSObject statusBar = OSCreateGrid(2, 1, OS_GRID_STYLE_STATUS_BAR);

	OSSetRootGrid(window, rootLayout);
	OSAddGrid(rootLayout, 0, 3, contentSplit, OS_CELL_FILL);
	OSAddGrid(rootLayout, 0, 0, toolbar1, OS_CELL_H_EXPAND | OS_CELL_H_PUSH);
	OSAddGrid(rootLayout, 0, 1, toolbar2, OS_CELL_H_EXPAND | OS_CELL_H_PUSH);
	OSAddGrid(rootLayout, 0, 5, statusBar, OS_CELL_H_EXPAND | OS_CELL_H_PUSH);

	bookmarkList = OSCreateListView(OS_CREATE_LIST_VIEW_SINGLE_SELECT | OS_CREATE_LIST_VIEW_CONSTANT_HEIGHT, OS_LIST_VIEW_ITEM_HEIGHT_DEFAULT);
	OSSetObjectNotificationCallback(bookmarkList, OS_MAKE_NOTIFICATION_CALLBACK(ProcessBookmarkListingNotification, this));
	OSAddControl(contentSplit, 0, 0, bookmarkList, OS_CELL_H_EXPAND | OS_CELL_V_EXPAND);
	OSSetProperty(bookmarkList, OS_GUI_OBJECT_PROPERTY_SUGGESTED_WIDTH, (void *) 160);
	OSListViewInsert(bookmarkList, 0, global.bookmarkCount);

	folderListing = OSCreateListView(OS_CREATE_LIST_VIEW_MULTI_SELECT | OS_CREATE_LIST_VIEW_CONSTANT_HEIGHT | OS_CREATE_LIST_VIEW_SORTABLE, OS_LIST_VIEW_ITEM_HEIGHT_DEFAULT);
	OSSetObjectNotificationCallback(folderListing, OS_MAKE_NOTIFICATION_CALLBACK(ProcessFolderListingNotification, this));
	OSAddControl(contentSplit, 2, 0, folderListing, OS_CELL_FILL);
	OSListViewSetColumns(folderListing, folderListingColumns, sizeof(folderListingColumns) / sizeof(folderListingColumns[0]));

	OSAddControl(contentSplit, 1, 0, OSCreateLine(OS_ORIENTATION_VERTICAL), OS_CELL_V_EXPAND | OS_CELL_V_PUSH);

	OSObject backButton = OSCreateButton(commandNavigateBackwards, OS_BUTTON_STYLE_TOOLBAR);
	OSAddControl(toolbar1, 0, 0, backButton, OS_CELL_V_CENTER | OS_CELL_V_PUSH);
	OSObject forwardButton = OSCreateButton(commandNavigateForwards, OS_BUTTON_STYLE_TOOLBAR_ICON_ONLY);
	OSAddControl(toolbar1, 1, 0, forwardButton, OS_CELL_V_CENTER | OS_CELL_V_PUSH);
	OSObject parentButton = OSCreateButton(commandNavigateParent, OS_BUTTON_STYLE_TOOLBAR_ICON_ONLY);
	OSAddControl(toolbar1, 2, 0, parentButton, OS_CELL_V_CENTER | OS_CELL_V_PUSH);

	folderPath = OSCreateTextbox(OS_TEXTBOX_STYLE_COMMAND);
	OSSetControlCommand(folderPath, commandNavigatePath);
	OSAddControl(toolbar1, 3, 0, folderPath, OS_CELL_H_EXPAND | OS_CELL_H_PUSH | OS_CELL_V_CENTER | OS_CELL_V_PUSH);

	OSObject bookmarkFolderButton = OSCreateButton(commandBookmarkFolder, OS_BUTTON_STYLE_TOOLBAR_ICON_ONLY);
	OSAddControl(toolbar1, 4, 0, bookmarkFolderButton, OS_CELL_V_CENTER | OS_CELL_V_PUSH);

	OSAddControl(toolbar2, 0, 0, OSCreateButton(commandNewFolder, OS_BUTTON_STYLE_TOOLBAR), OS_CELL_V_CENTER | OS_CELL_V_PUSH);
	OSAddControl(toolbar2, 1, 0, OSCreateButton(commandOpenItem, OS_BUTTON_STYLE_TOOLBAR), OS_CELL_V_CENTER | OS_CELL_V_PUSH);

	statusLabel = OSCreateLabel(OSLiteral(""), false);
	OSAddControl(statusBar, 1, 0, statusLabel, OS_FLAGS_DEFAULT);

	LoadFolder(OSLiteral("/"));
	OSSetFocusedControl(folderListing, true);

	OSEndGUIAllocationBlock();
}

OSCallbackResponse ProcessSystemMessage(OSObject _object, OSMessage *message) {
	(void) _object;

	if (message->type == OS_MESSAGE_CREATE_INSTANCE) {
		((Instance *) OSHeapAllocate(sizeof(Instance), true))->Initialise(message);
		return OS_CALLBACK_HANDLED;
	} 

	return OS_CALLBACK_NOT_HANDLED;
}

void ProgramEntry() {
	global.AddBookmark(OSLiteral("/OS"));
	global.AddBookmark(OSLiteral("/Programs"));

	OSSetMessageCallback(osSystemMessages, OS_MAKE_MESSAGE_CALLBACK(ProcessSystemMessage, nullptr));
	OSProcessMessages();
}
