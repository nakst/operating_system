#include "../api/os.h"

#define OS_MANIFEST_DEFINITIONS
#include "../bin/OS/test.manifest.h"

#include "../freetype/ft2build.h"
#include FT_FREETYPE_H

OSObject progressBar, window, scrollbar;

int x = 5;

struct y {
	int a, b;
};

y y2 = {1, 2};

int vp = 100;

struct Word {
	char *text;
	size_t length;
	unsigned count;
	bool selected;
};

OSObject listView;
size_t wordCount;
Word words[50000];
char buffer[1024];

OSHandle mutexes[2];
OSInstance calculator;

void LocalMutexTest(void *argument) {
	int i = (uintptr_t) argument;
	
	OSPrint("i = %d\n", i);

#if 0
	OSAcquireMultipleGlobalMutexes(mutexes, 2);
	OSReleaseGlobalMutex(mutexes[0]);
	OSReleaseGlobalMutex(mutexes[1]);

	OSAcquireGlobalMutex(mutexes[0]);
	OSReleaseGlobalMutex(mutexes[0]);

	OSAcquireGlobalMutex(mutexes[1]);
	OSReleaseGlobalMutex(mutexes[1]);

	OSAcquireGlobalMutex(mutexes[1]);
	OSAcquireGlobalMutex(mutexes[0]);
	OSReleaseGlobalMutex(mutexes[0]);
	OSReleaseGlobalMutex(mutexes[1]);
#endif

	OSPrint("i = %d\n", i);

	OSTerminateThread(OS_CURRENT_THREAD);
}

OSCallbackResponse SliderValueChanged(OSNotification *message) {
	if (message->type != OS_NOTIFICATION_VALUE_CHANGED) {
		return OS_CALLBACK_NOT_HANDLED;
	}

	OSSetProgressBarValue(message->context, message->valueChanged.newValue);
	return OS_CALLBACK_HANDLED;
}

OSCallbackResponse ListViewCallback(OSNotification *message) {
	if (message->type == OS_NOTIFICATION_GET_ITEM) {
		uintptr_t index = message->listViewItem.index;
		Word *word = words + index;

		if (message->listViewItem.mask & OS_LIST_VIEW_ITEM_TEXT) {
			switch (message->listViewItem.column) {
				case 0: {
					message->listViewItem.textBytes = OSFormatString(buffer, 1024, "%s", word->length, word->text);
				} break;

				case 1: {
					message->listViewItem.textBytes = OSFormatString(buffer, 1024, "%d", word->count);
				} break;

				case 2: {
					message->listViewItem.textBytes = OSFormatString(buffer, 1024, "%d", index + 1);
				} break;
			}

			message->listViewItem.text = buffer;
		}

		if (message->listViewItem.mask & OS_LIST_VIEW_ITEM_SELECTED) {
			if (word->selected) {
				message->listViewItem.state |= OS_LIST_VIEW_ITEM_SELECTED;
			}
		}

		if (message->listViewItem.mask & OS_LIST_VIEW_ITEM_HEIGHT) {
			message->listViewItem.height = 50;
		}
	} else if (message->type == OS_NOTIFICATION_SET_ITEM) {
		uintptr_t index = message->listViewItem.index;
		Word *word = words + index;

		if (message->listViewItem.mask & OS_LIST_VIEW_ITEM_SELECTED) {
			word->selected = message->listViewItem.state & OS_LIST_VIEW_ITEM_SELECTED;
		}
	} else if (message->type == OS_NOTIFICATION_PAINT_CELL && message->paintCell.column == 2) {
		OSDrawProgressBar(message->paintCell.surface, message->paintCell.bounds, (float) message->paintCell.index / (float) wordCount, message->paintCell.clip, true);
	} else {
		return OS_CALLBACK_NOT_HANDLED;
	}

	return OS_CALLBACK_HANDLED;
}

int SortList(const void *_a, const void *_b) {
	Word *a = (Word *) _a;
	Word *b = (Word *) _b;

	char *s1 = a->text;
	char *s2 = b->text;
	size_t length1 = a->length;
	size_t length2 = b->length;

	while (length1 || length2) {
		if (!length1) return -1;
		if (!length2) return 1;

		if (*s1 != *s2) {
			return *s1 - *s2;
		}

		s1++;
		s2++;
		length1--;
		length2--;
	}

	return 0;
}

OSListViewColumn columns[] = {
	{ OSLiteral("Word"), 100, 30, true },
	{ OSLiteral("Count"), 50, 30, false },
	{ OSLiteral("Index"), 50, 30, false },
};

void CreateList(OSObject content) {
	size_t length;
	char *file = (char *) OSReadEntireFile(OSLiteral("/OS/A Study in Scarlet.txt"), &length);

	if (!file || !length) {
		OSCrashProcess(2000);
	}

	for (uintptr_t i = 0; i < length; i++) {
		file[i] = tolower(file[i]);
	}

	int totalWords = 0;

	while (length) {
		char *start = file;
		char c = *start;

		while ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '\'') {
			file++;
			length--;
			c = *file;
		}

		if (file == start) {
			file++;
			length--;
			continue;
		}

		totalWords++;

		{
			bool found = false;
			size_t length = file - start;

#if 1
			for (uintptr_t i = 0; i < wordCount; i++) {
				if (length == words[i].length && 0 == OSCompareBytes(start, words[i].text, words[i].length)) {
					found = true;
					words[i].count++;
					break;
				}
			}
#endif

			if (!found) {
				words[wordCount].text = start;
				words[wordCount].length = length;
				words[wordCount].count = 1;
				wordCount++;
			}
		}
	}
	
	qsort(words, wordCount, sizeof(Word), SortList);

#if 0
	for (uintptr_t i = 0; i < wordCount; i++) {
		OSPrint("%s, ", words[i].length, words[i].text);
	}
#endif

#if 1
#if 1
	listView = OSCreateListView(OS_CREATE_LIST_VIEW_BORDER | OS_CREATE_LIST_VIEW_SINGLE_SELECT, 0);
#else
	listView = OSCreateListView(OS_CREATE_LIST_VIEW_BORDER | OS_CREATE_LIST_VIEW_MULTI_SELECT, 0);
#endif
	OSSetObjectNotificationCallback(listView, OS_MAKE_NOTIFICATION_CALLBACK(ListViewCallback, nullptr));
	OSAddControl(content, 0, 4, listView, OS_CELL_FILL);
	OSListViewInsert(listView, 0, wordCount/* / 16*/);

	OSListViewSetColumns(listView, columns, sizeof(columns)/sizeof(columns[0]));
#else
	(void) content;
#endif

	OSPrint("Loaded test list with %d words (%d total)\n", wordCount, totalWords);
}

OSCallbackResponse ScrollbarMoved(OSNotification *message) {
	if (message->valueChanged.newValue == 300) {
		// OSSetScrollbarPosition(object, 200, false);
		// vp = 800;
		// OSSetScrollbarMeasurements(object, 400, 800);
	}

	return OS_CALLBACK_HANDLED;
}

OSCallbackResponse Crash(OSNotification *message) {
	// OSPrint("object = %x\n", object);
	// OSListViewInsert(listView, 5, wordCount / 16);
	OSCreateMenu(menuPrograms, message->generator, OS_CREATE_MENU_AT_SOURCE, OS_FLAGS_DEFAULT);

	return OS_CALLBACK_HANDLED;
}

OSCallbackResponse Launch(OSNotification *message) {
#if 0
	OSProcessInformation information;
	OSCreateProcess((char *) message->context, OSCStringLength((char *) message->context), &information, nullptr);
	OSCloseHandle(information.handle);
	OSCloseHandle(information.mainThread.handle);
#endif
	OSExecuteProgram((char *) message->context, OSCStringLength((char *) message->context));
	return OS_CALLBACK_HANDLED;
}

OSCallbackResponse ProcessActionOK(OSNotification *message) {
	(void) message;
	static int progress = 0;
	progress++;
	if (progress <= 5) {
		// OSSetProgressBarValue(progressBar, progress);
	} else {
		OSMessage message;
		message.type = OS_MESSAGE_DESTROY;
		OSSendMessage(window, &message);
	}

	return OS_CALLBACK_HANDLED;
}

#if 0
OSObject contentPane;

void Test(OSObject generator, void *argument, OSCallbackData *data) {
	(void) argument;
	char buffer[64];
	size_t bytes = OSFormatString(buffer, 64, "Generator = %x, type = %d", generator, data->type);
	OSSetControlText(OSGetControl(contentPane, 0, 1), buffer, bytes);
	OSPrint("%s\n", bytes, buffer);
}

enum MenuID {
	MENU_FILE,
	MENU_EDIT,
	MENU_RECENT,
};

void PopulateMenu(OSObject generator, void *argument, OSCallbackData *data) {
	(void) generator;
	(void) data;

	OSObject menu = data->populateMenu.popupMenu;

	switch ((uintptr_t) argument) {
		case MENU_FILE: {
			OSObject openItem = OSCreateControl(OS_CONTROL_MENU, (char *) "Open", 4, 0);
			OSObject saveItem = OSCreateControl(OS_CONTROL_MENU, (char *) "Save", 4, 0);
			OSObject recentItem = OSCreateControl(OS_CONTROL_MENU, (char *) "Recent", 6, OS_CONTROL_MENU_HAS_CHILDREN);

			OSSetMenuItems(menu, 3);
			OSSetMenuItem(menu, 0, openItem);
			OSSetMenuItem(menu, 1, saveItem);
			OSSetMenuItem(menu, 2, recentItem);

			OSSetObjectCallback(recentItem, OS_OBJECT_CONTROL, OS_CALLBACK_POPULATE_MENU, PopulateMenu, (void *) MENU_RECENT);
		} break;

		case MENU_RECENT:
		case MENU_EDIT: {
			OSObject copyItem = OSCreateControl(OS_CONTROL_MENU, (char *) "Copy", 4, 0);
			OSObject pasteItem = OSCreateControl(OS_CONTROL_MENU, (char *) "Paste", 5, 0);
			OSObject recurseItem = OSCreateControl(OS_CONTROL_MENU, (char *) "Recurse", 7, OS_CONTROL_MENU_HAS_CHILDREN);

			OSSetMenuItems(menu, 3);
			OSSetMenuItem(menu, 0, copyItem);
			OSSetMenuItem(menu, 1, pasteItem);
			OSSetMenuItem(menu, 2, recurseItem);

			OSSetObjectCallback(recurseItem, OS_OBJECT_CONTROL, OS_CALLBACK_POPULATE_MENU, PopulateMenu, (void *) MENU_FILE);
		} break;
	}
}
#endif

jmp_buf buf;
int jmpState = 0;

void Function2() {
	if (jmpState++ != 2) OSCrashProcess(232);
	longjmp(buf, 0);
	OSCrashProcess(233);
}

int CompareIntegers(const void *a, const void *b) {
	uintptr_t *c = (uintptr_t *) a;
	uintptr_t *d = (uintptr_t *) b;
	return *c - *d;
}

OSCallbackResponse ToggleEnabled(OSNotification *message) {
	OSPrint("context = %x\n", message->context);
	OSEnableControl(message->context, message->command.checked);
	// OSSetScrollbarMeasurements(scrollbar, 10, 20);
	return OS_CALLBACK_HANDLED;
}

int z = 1;

extern "C" OSObject CreateMenuItem(OSMenuItem item);

void Delete(OSHandle handle) {
	// OSPrint("Deleting %x...\n", handle);
	OSDeleteNode(handle);
}

OSCallbackResponse SendRemoteCommand(OSNotification *notification) {
	if (notification->type != OS_NOTIFICATION_COMMAND) return OS_CALLBACK_NOT_HANDLED;
	OSIssueForeignCommand(&calculator, OSLiteral("evaluate"));
	return OS_CALLBACK_HANDLED;
}

void EnumerateRootThread(void *argument) {
	(void) argument;

	OSNodeInformation node;
	OSOpenNode(OSLiteral("/"), OS_OPEN_NODE_DIRECTORY, &node);
	OSDirectoryChild *buffer = (OSDirectoryChild *) OSHeapAllocate(sizeof(OSDirectoryChild) * 1024, true);

	while (true) {
		OSEnumerateDirectoryChildren(node.handle, buffer, 1024);
	}
}

extern "C" void ProgramEntry() {
#if 0
	if (x != 5) OSCrashProcess(600);
	if (y2.a != 1) OSCrashProcess(601);
	if (y2.b != 2) OSCrashProcess(602);
	if (++z != 2) OSCrashProcess(603); // Is the data segment writable?

#if 0
	{
		OSThreadInformation information;
		OSCreateThread(EnumerateRootThread, &information, nullptr);
	}
#endif


	{
		OSNodeInformation node;
		OSHandle handles[16];

		for (uintptr_t i = 0; i < 16; i++) {
			char buffer[256];
			size_t length = OSFormatString(buffer, 256, "/TestFolder/%d", i);
			OSOpenNode(buffer, length, OS_OPEN_NODE_RESIZE_ACCESS | OS_OPEN_NODE_CREATE_DIRECTORIES, &node);
			handles[i] = node.handle;
		}

#if 1
		Delete(handles[0]);
		Delete(handles[1]);
		Delete(handles[2]);
		Delete(handles[3]);
		Delete(handles[4]);
		Delete(handles[5]);
		Delete(handles[6]);
		Delete(handles[7]);
		Delete(handles[8]);
		Delete(handles[9]);
		Delete(handles[10]);
		Delete(handles[11]);
		Delete(handles[12]);
		Delete(handles[13]);
		Delete(handles[14]);
		Delete(handles[15]);
#endif

		// OSError error = OSOpenNode(OSLiteral("/TestFolder/3"), OS_OPEN_NODE_FAIL_IF_NOT_FOUND, &node);
		// if (error != OS_ERROR_FILE_DOES_NOT_EXIST) OSCrashProcess(704);

		OSOpenNode(OSLiteral("/TestFolder/File.txt"), OS_FLAGS_DEFAULT, &node);
		OSCloseHandle(node.handle);
		OSOpenNode(OSLiteral("/TestFolder/File 2.txt"), OS_FLAGS_DEFAULT, &node);
		OSCloseHandle(node.handle);
		OSOpenNode(OSLiteral("/TestFolder/File 3.txt"), OS_FLAGS_DEFAULT, &node);
		OSOpenNode(OSLiteral("/TestFolder/File 10.txt"), OS_FLAGS_DEFAULT, &node);
		OSOpenNode(OSLiteral("/TestFolder/14.1.txt"), OS_FLAGS_DEFAULT, &node);
		OSOpenNode(OSLiteral("/TestFolder/14.2.txt"), OS_FLAGS_DEFAULT, &node);

		for (uintptr_t i = 0; i < 16; i++) {
			OSCloseHandle(handles[i]);
		}
	}

#if 1
	{
		OSNodeInformation node1, node2;

		OSOpenNode(OSLiteral("/TestFolder/Move"), OS_FLAGS_DEFAULT, &node1);
		OSOpenNode(OSLiteral("/"), OS_OPEN_NODE_DIRECTORY, &node2);

		OSError error = OSMoveNode(node1.handle, node2.handle, OSLiteral("stage2"));
		OSPrint("move error = %d\n", error);

		OSCloseHandle(node1.handle);
		OSCloseHandle(node2.handle);
	}

#if 1
	{
		OSNodeInformation node;
		OSOpenNode(OSLiteral("/RemoveFileTest.txt"), OS_OPEN_NODE_RESIZE_ACCESS, &node);
		OSDeleteNode(node.handle);
		OSCloseHandle(node.handle);
	}

	{
		OSNodeInformation node;
		OSOpenNode(OSLiteral("/stage2"), OS_OPEN_NODE_RESIZE_ACCESS, &node);
		OSDeleteNode(node.handle);
		OSCloseHandle(node.handle);
	}
#endif
#endif

	{
		OSNodeInformation node;
		OSOpenNode(OSLiteral("/ResizeFileTest.txt"), 
				OS_OPEN_NODE_RESIZE_ACCESS | OS_OPEN_NODE_WRITE_ACCESS | OS_OPEN_NODE_READ_ACCESS, 
				&node);

		// OSPrint("Opened node, handle = %d, error = %d\n", node.handle, error);

#if 0
		// Commented out because we don't yet the free extents allocated
		// during a failed resize.

		OSResizeFile(node.handle, (uint64_t) 0xFFFFFFFFFFFF);

		OSPrint("Attempted massive file resize\n");
#endif

		uint8_t buffer[512];

		for (uintptr_t i = 1; i < 128; i++) {
			for (uintptr_t j = 0; j < 512; j++) {
				buffer[j] = i;
			}

			// OSPrint("Resizing file to %d\n", i * 512);
			OSResizeFile(node.handle, i * 512);
			// OSPrint("Write to %d\n", (i - 1) * 512);
			OSWriteFileSync(node.handle, (i - 1) * 512, 512, buffer);
		}

		for (uintptr_t i = 1; i < 128; i++) {
			// OSPrint("Read from %d\n", (i - 1) * 512);
			OSReadFileSync(node.handle, (i - 1) * 512, 512, buffer);

			for (uintptr_t j = 0; j < 512; j++) {
				if (buffer[j] != i) {
					OSCrashProcess(700);
				}
			}
		}

#if 1
		for (uintptr_t i = 126; i > 0; i--) {
			// OSPrint("Resizing file to %d\n", i * 512);
			OSResizeFile(node.handle, i * 512);
		}
#endif

		for (uintptr_t i = 1; i < 2; i++) {
			// OSPrint("Read from %d\n", (i - 1) * 512);
			OSReadFileSync(node.handle, (i - 1) * 512, 512, buffer);

			for (uintptr_t j = 0; j < 512; j++) {
				if (buffer[j] != i) {
					OSCrashProcess(700);
				}
			}
		}

		OSCloseHandle(node.handle);
	}

	{
		void *a = malloc(0x100000);
		void *b = realloc(a, 0x1000);
		void *c = realloc(b, 0x100000);
		free(c);
	}

	if (strcasecmp("abc", "AbC")) {
		OSCrashProcess(250);
	}

	if (strspn("123abc", "123456789") != 3) {
		OSCrashProcess(251);
	}

	if (strcspn("abc123", "123456789") != 3) {
		OSCrashProcess(252);
	}

	{
		const char *s = "abcabc";
		if (strchr(s, 'b') != s + 1) {
			OSCrashProcess(254);
		}
		if (strrchr(s, 'b') != s + 4) {
			OSCrashProcess(254);
		}
	}

	{
		char b[] = "abcdef";

		if (strlen(b) != 6 || strnlen(b, 3) != 3 || strnlen(b, 10) != 6) {
			OSCrashProcess(267);
		}
	}

	if (strchr("", 'a') || strrchr("", 'a')) {
		OSCrashProcess(255);
	}

	{
		char b[32];
		strcpy(b, "he");
		strcat(b, "llo");
		if (strcmp(b, "hello")) OSCrashProcess(253);
	}

	{
		char text[] = "hello,world:test";
		const char *delim = ":,";
		char *n = text;
		(void) n;
		(void) delim;
		// OSPrint("%z\n", strsep(&n, delim));
		// OSPrint("%z\n", strsep(&n, delim));
		// OSPrint("%z\n", strsep(&n, delim));
		// OSPrint("%x\n", strsep(&n, delim));
	}

	jmpState = 1;
	if (setjmp(buf) == 0) {
		if (jmpState++ != 1) OSCrashProcess(230);
		Function2();
		OSCrashProcess(231);
	} else {
		if (jmpState++ != 3) OSCrashProcess(234);
	}

	if (jmpState++ != 4) OSCrashProcess(235);
	if (strtol("\n\f\n -0xaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaAAAAAAAaaaaaa", nullptr, 0) != LONG_MIN) OSCrashProcess(236);
	char *x = (char *) "+03", *y;
	if (strtol(x, &y, 4) != 3 || y != x + 3) OSCrashProcess(237);

	{
		char *a = (char *) "hello";
		if (strstr(a, "hello") != a) OSCrashProcess(238);
		if (strstr(a, "hell") != a) OSCrashProcess(239);
		if (strstr(a, "ello") != a + 1) OSCrashProcess(240);
		if (strstr(a, "hello!")) OSCrashProcess(241);
		if (strstr(a, "l") != a + 2) OSCrashProcess(242);
	}

	{
		char *a = (char *) "hello";

		if (strcmp(a, "hdllo") <= 0) OSCrashProcess(243);
		if (strcmp(a, "hello") != 0) OSCrashProcess(244);
		if (strcmp(a, "hfllo") >= 0) OSCrashProcess(245);
		if (strcmp(a, "h") <= 0) OSCrashProcess(246);
		if (strcmp(a, "helloo") >= 0) OSCrashProcess(247);
	}

	{
		uintptr_t array[1000];
		for (uintptr_t i = 0; i < 1000; i++) array[i] = OSGetRandomByte();
		qsort(array, 1000, sizeof(uintptr_t), CompareIntegers);
		for (uintptr_t i = 1; i < 1000; i++) if (array[i] < array[i - 1]) OSCrashProcess(248);
	}

	{
		OSNodeInformation node = {};
		OSOpenNode(OSLiteral("/MapFile.txt"), 
				  OS_OPEN_NODE_RESIZE_EXCLUSIVE 
				| OS_OPEN_NODE_WRITE_ACCESS 
				| OS_OPEN_NODE_READ_ACCESS, &node);
		OSResizeFile(node.handle, 1048576);
		uint32_t *buffer = (uint32_t *) OSHeapAllocate(1048576, false);
		for (int i = 0; i < 262144; i++) buffer[i] = i;
		OSWriteFileSync(node.handle, 0, 1048576, buffer);
		OSReadFileSync(node.handle, 0, 1048576, buffer);
		for (uintptr_t i = 0; i < 262144; i++) if (buffer[i] != i) OSCrashProcess(201);
		uint32_t *pointer = (uint32_t *) OSMapObject(node.handle, 0, OS_MAP_OBJECT_ALL, OS_MAP_OBJECT_READ_WRITE);
		uint32_t *pointer2 = (uint32_t *) OSMapObject(node.handle, 0, OS_MAP_OBJECT_ALL, OS_MAP_OBJECT_READ_WRITE);
		for (uintptr_t i = 4096; i < 262144; i++) if (pointer[i] != buffer[i]) OSCrashProcess(200);
		for (uintptr_t i = 4096; i < 262144; i++) if (pointer2[i] != buffer[i]) OSCrashProcess(200);
		// pointer[0]++;
		OSUnmapObject(pointer);
		OSCloseHandle(node.handle);
		OSUnmapObject(pointer2);
	}

	{
		char m[256];
		char n[256];
		n[255] = 0;
		for (int i = 0; i < 256; i++) m[i] = i;
		OSCopyMemory(n, m, 255);
		m[255] = 0;
		for (int i = 0; i < 256; i++) if (m[i] != n[i]) OSCrashProcess(202);
	}

	char *path = (char *) "/OS/new_dir/test2.txt";
	OSNodeInformation node;
	OSError error = OSOpenNode(path, OSCStringLength(path), 
			OS_OPEN_NODE_READ_ACCESS | OS_OPEN_NODE_RESIZE_ACCESS | OS_OPEN_NODE_WRITE_ACCESS | OS_OPEN_NODE_CREATE_DIRECTORIES, &node);
	if (error != OS_SUCCESS) OSCrashProcess(150);
	error = OSResizeFile(node.handle, 8);
	if (error != OS_SUCCESS) OSCrashProcess(151);
	char buffer[8];
	buffer[0] = 'a';
	buffer[1] = 'b';
	OSWriteFileSync(node.handle, 0, 1, buffer);
	buffer[0] = 'b';
	buffer[1] = 'c';
	size_t bytesRead = OSReadFileSync(node.handle, 0, 8, buffer);
	if (bytesRead != 8) OSCrashProcess(152);
	if (buffer[0] != 'a' || buffer[1] != 0) OSCrashProcess(101);
	OSRefreshNodeInformation(&node);

	{
		OSBatchCall calls[] = {
			{ OS_SYSCALL_PRINT, /*Return immediately on error?*/ true, /*Argument 0 and return value*/ (uintptr_t) "Hello ",    /*Argument 1*/ 6, /*Argument 2*/ 0, /*Argument 3*/ 0, },
			{ OS_SYSCALL_PRINT, /*Return immediately on error?*/ true, /*Argument 0 and return value*/ (uintptr_t) "batched",   /*Argument 1*/ 7, /*Argument 2*/ 0, /*Argument 3*/ 0, },
			{ OS_SYSCALL_PRINT, /*Return immediately on error?*/ true, /*Argument 0 and return value*/ (uintptr_t) " world!\n", /*Argument 1*/ 8, /*Argument 2*/ 0, /*Argument 3*/ 0, },
		};

		OSBatch(calls, sizeof(calls) / sizeof(OSBatchCall));
	}

	{
		char *path = (char *) "/OS/sample_images";
		OSNodeInformation node;
		OSError error = OSOpenNode(path, OSCStringLength(path), OS_OPEN_NODE_DIRECTORY, &node);
		OSDirectoryChild buffer[16];

		if (error == OS_SUCCESS) {
			error = OSEnumerateDirectoryChildren(node.handle, buffer, 16);
		}

		if (error != OS_SUCCESS) OSCrashProcess(100);
	}

	{
		char *path = (char *) "/OS/act.txt";

		OSNodeInformation node1, node2, node3;

		OSError error1 = OSOpenNode(path, OSCStringLength(path),
				OS_OPEN_NODE_READ_ACCESS, &node1);
		if (error1 != OS_SUCCESS) OSCrashProcess(120);

		OSError error2 = OSOpenNode(path, OSCStringLength(path),
				OS_OPEN_NODE_READ_ACCESS, &node2);
		if (error2 != OS_SUCCESS) OSCrashProcess(121);

		OSError error3 = OSOpenNode(path, OSCStringLength(path),
				OS_OPEN_NODE_READ_BLOCK, &node3);
		if (error3 != OS_ERROR_FILE_CANNOT_GET_EXCLUSIVE_USE) OSCrashProcess(122);

		OSCloseHandle(node1.handle);
		OSCloseHandle(node2.handle);

		OSError error6 = OSOpenNode(path, OSCStringLength(path),
				OS_OPEN_NODE_READ_BLOCK, &node1);
		if (error6 != OS_SUCCESS) OSCrashProcess(125);

		OSError error7 = OSOpenNode(path, OSCStringLength(path),
				OS_OPEN_NODE_READ_BLOCK, &node2);
		if (error7 != OS_SUCCESS) OSCrashProcess(126);

		OSCloseHandle(node1.handle);
		OSCloseHandle(node2.handle);

		OSError error8 = OSOpenNode(path, OSCStringLength(path),
				OS_OPEN_NODE_READ_EXCLUSIVE, &node1);
		if (error8 != OS_SUCCESS) OSCrashProcess(127);

		OSError error9 = OSOpenNode(path, OSCStringLength(path),
				OS_OPEN_NODE_READ_BLOCK, &node2);
		if (error9 != OS_ERROR_FILE_CANNOT_GET_EXCLUSIVE_USE) OSCrashProcess(128);

		OSError error10 = OSOpenNode(path, OSCStringLength(path),
				OS_OPEN_NODE_READ_EXCLUSIVE, &node2);
		if (error10 != OS_ERROR_FILE_CANNOT_GET_EXCLUSIVE_USE) OSCrashProcess(129);

		OSError error11 = OSOpenNode(path, OSCStringLength(path),
				OS_OPEN_NODE_READ_ACCESS, &node2);
		if (error11 != OS_ERROR_FILE_IN_EXCLUSIVE_USE) OSCrashProcess(130);
	}

#if 0
	{
		char *path = (char *) "/OS/test.txt";
		OSNodeInformation node;
		OSError error = OSOpenNode(path, OSCStringLength(path), 
				OS_OPEN_NODE_READ_ACCESS | OS_OPEN_NODE_RESIZE_ACCESS | OS_OPEN_NODE_WRITE_ACCESS,
				&node);
		if (error != OS_SUCCESS) OSCrashProcess(102);
		// error = OSResizeFile(node.handle, 2048);
		if (error != OS_SUCCESS) OSCrashProcess(103);
		uint16_t buffer[1024];
		for (int i = 0; i < 1024; i++) buffer[i] = i;
		OSWriteFileSync(node.handle, 0, 2048, buffer);
		for (int i = 0; i < 1024; i++) buffer[i] = i + 1024;
		OSHandle handle = OSWriteFileAsync(node.handle, 256, 2048 - 256 - 256, buffer + 128);
		// OSCancelIORequest(handle);
		OSWaitSingle(handle);
		OSIORequestProgress progress;
		OSGetIORequestProgress(handle, &progress);
		OSCloseHandle(handle);
		for (int i = 0; i < 1024; i++) buffer[i] = i + 2048;
		OSReadFileSync(node.handle, 0, 2048, buffer);
		for (int i = 0; i < 128; i++) if (buffer[i] != i) OSCrashProcess(107);
		for (int i = 1024 - 128; i < 1024; i++) if (buffer[i] != i) OSCrashProcess(108);
		for (int i = 128; i < 1024 - 128; i++) if (buffer[i] == i) OSCrashProcess(110); else if (buffer[i] != i + 1024) OSCrashProcess(109);

		// OSCreateWindow((char *) "Test Program", 12, 320, 200, 0);
	}
#endif

	{
		OSHandle region = OSOpenSharedMemory(512 * 1024 * 1024, nullptr, 0, 0);
		void *pointer = OSMapObject(region, 0, 0, OS_MAP_OBJECT_READ_WRITE);
		// Commented out because of the memory requirements.
#if 0
		OSResizeSharedMemory(region, 300 * 1024 * 1024); // Big -> big
		OSResizeSharedMemory(region, 200 * 1024 * 1024); // Big -> small
		OSResizeSharedMemory(region, 100 * 1024 * 1024); // Small -> small
		OSResizeSharedMemory(region, 400 * 1024 * 1024); // Small -> big
#endif
		OSCloseHandle(region);
		OSUnmapObject(pointer);
	}

#if 1
	{
		OSHandle region = OSOpenSharedMemory(512 * 1024 * 1024, nullptr, 0, 0);
		void *pointer = OSMapObject(region, 0, 0, OS_MAP_OBJECT_READ_WRITE);
		// OSZeroMemory(pointer, 512 * 1024 * 1024);
		OSCloseHandle(region);
		OSUnmapObject(pointer);
	}
#endif

	for (int i = 0; i < 3; i++) {
		OSHandle handle = OSOpenSharedMemory(1024, (char *) "Test", 4, OS_OPEN_SHARED_MEMORY_FAIL_IF_FOUND);
		OSHandle handle3 = OSOpenSharedMemory(0, (char *) "Test", 4, OS_OPEN_SHARED_MEMORY_FAIL_IF_NOT_FOUND);
		if (handle3 == OS_INVALID_HANDLE) OSCrashProcess(141);
		OSCloseHandle(handle);
		OSCloseHandle(handle3);
		OSHandle handle2 = OSOpenSharedMemory(0, (char *) "Test", 4, OS_OPEN_NODE_FAIL_IF_NOT_FOUND);
		if (handle2 != OS_INVALID_HANDLE) OSCrashProcess(140);
	}

	OSPrint("All tests completed successfully.\n");
#endif
	// OSCrashProcess(OS_FATAL_ERROR_INVALID_BUFFER);
	
	OSWindowSpecification ws = {};
	ws.width = 900;
	ws.height = 650;
	ws.minimumWidth = 100;
	ws.minimumHeight = 100;
	ws.title = (char *) "Hello, world!";
	ws.titleBytes = OSCStringLength(ws.title);
	ws.menubar = myMenuBar;
	OSInstance *instance = (OSInstance *) OSHeapAllocate(sizeof(OSInstance), true);
	OSInitialiseInstance(instance, 0);
	window = OSCreateWindow(&ws, instance);

	OSObject b;
	OSObject content = OSCreateGrid(4, 6, OS_GRID_STYLE_CONTAINER);
	OSObject scrollPane = OSCreateScrollPane(content, OS_CREATE_SCROLL_PANE_VERTICAL | OS_CREATE_SCROLL_PANE_HORIZONTAL);
	OSSetRootGrid(window, scrollPane);
	OSAddControl(content, 1, 1, b = OSCreateButton(actionOK, OS_BUTTON_STYLE_NORMAL), 0);

	OSAddControl(content, 1, 0, OSCreateLine(OS_ORIENTATION_HORIZONTAL), OS_CELL_H_EXPAND);

	OSAddControl(content, 0, 2, b = OSCreateTextbox(OS_TEXTBOX_STYLE_NORMAL), 0);

	OSSetCommandNotificationCallback(instance, actionToggleEnabled, OS_MAKE_NOTIFICATION_CALLBACK(ToggleEnabled, b));
	OSAddControl(content, 0, 1, OSCreateButton(actionToggleEnabled, OS_BUTTON_STYLE_NORMAL), 0);

	OSAddControl(content, 1, 2, OSCreateTextbox(OS_TEXTBOX_STYLE_NORMAL), OS_CELL_H_PUSH | OS_CELL_H_EXPAND);

	OSAddControl(content, 0, 0, progressBar = OSCreateProgressBar(0, 100, 20, false), 0);
	OSAddControl(content, 0, 3, OSCreateButton(commandDeleteEverything, OS_BUTTON_STYLE_NORMAL), 0);

	OSObject sliderH, sliderV;
	OSAddControl(content, 2, 4, sliderV = OSCreateSlider(0, 100, 50, 
				OS_SLIDER_MODE_VERTICAL | OS_SLIDER_MODE_TICKS_BOTH_SIDES | OS_SLIDER_MODE_OPPOSITE_VALUE, 
				5, 4), 0);
#if 0
	OSAddControl(content, 0, 5, sliderH = OSCreateSlider(0, 100, 50, 
				OS_SLIDER_MODE_HORIZONTAL | OS_SLIDER_MODE_TICKS_BOTH_SIDES | OS_SLIDER_MODE_SNAP_TO_TICKS, 
				5, 4), 0);
#endif
	(void) sliderH;

	OSAddControl(content, 0, 5, OSCreateButton(sendRemoteCommand, OS_BUTTON_STYLE_NORMAL), OS_FLAGS_DEFAULT);

	OSSetObjectNotificationCallback(sliderV, OS_MAKE_NOTIFICATION_CALLBACK(SliderValueChanged, progressBar));

	{
		OSObject textbox = OSCreateTextbox(OS_TEXTBOX_STYLE_NORMAL);
		// OSDebugGUIObject(textbox);
		OSObject grid = OSCreateGrid(2, 5, OS_GRID_STYLE_GROUP_BOX);
		OSAddGrid(content, 1, 4, grid, OS_CELL_H_RIGHT);
		OSAddControl(grid, 0, 0, textbox, OS_CELL_H_LEFT);
		OSAddControl(grid, 0, 1, OSCreateButton(actionToggleEnabled, OS_BUTTON_STYLE_NORMAL), OS_CELL_H_LEFT);
		OSAddControl(grid, 0, 2, OSCreateButton(r1, OS_BUTTON_STYLE_NORMAL), OS_CELL_H_LEFT);
		OSAddControl(grid, 0, 3, OSCreateButton(r2, OS_BUTTON_STYLE_NORMAL), OS_CELL_H_LEFT);
		OSAddControl(grid, 0, 4, OSCreateButton(r3, OS_BUTTON_STYLE_NORMAL), OS_CELL_H_LEFT);
		OSSetText(textbox, OSLiteral("abcdefghijklmnopqrstuvwxyz"), OS_RESIZE_MODE_IGNORE);
	}

#if 0
#if 0
	scrollbar = OSCreateScrollbar(OS_ORIENTATION_HORIZONTAL);
	OSAddControl(content, 0, 4, scrollbar, OS_CELL_H_PUSH | OS_CELL_H_EXPAND | OS_CELL_V_CENTER);
#else
	scrollbar = OSCreateScrollbar(OS_ORIENTATION_VERTICAL);
	OSAddControl(content, 0, 4, scrollbar, OS_CELL_V_PUSH | OS_CELL_V_EXPAND | OS_CELL_H_CENTER);
#endif
	OSSetScrollbarMeasurements(scrollbar, 400, 100);
	OSSetObjectNotificationCallback(scrollbar, OS_MAKE_NOTIFICATION_CALLBACK(ScrollbarMoved, nullptr));
	// OSDebugGUIObject(scrollbar);
#endif

	CreateList(content);

#if 0
	OSPrint("%F, %F, %F, %F, %F, %F\n", floor(2.5), floor(-2.5), floor(3), ceil(2.5), ceil(-2.5), ceil(3));
	OSPrint("sin(3) = %F\n", sin(3));

	{
		FILE *f = fopen("/ctest.txt", "wb");
		fputs("hey!", f);
		fclose(f);
		f = fopen("/ctest.txt", "rb");
		char b[16];
		fgets(b, 16, f);
		OSPrint("%s\n", OSCStringLength(b), b);
		fclose(f);
	}

	{
		double x = strtod("    -0x12.3aA4P-1", nullptr);
		OSPrint("x = %F\n", x);
	}

#if 0
	mutexes[0] = OSCreateGlobalMutex();
	mutexes[1] = OSCreateGlobalMutex();
#endif

	for (int i = 0; i < 20; i++) {
		OSThreadInformation information;
		OSCreateThread(LocalMutexTest, &information, (void *) (uintptr_t) i);
	}
#endif

	OSDisableCommand(instance, actionToggleEnabled, false);
	OSDisableCommand(instance, actionOK, false);

#if 0
	{
		OSObject window = OSCreateWindow(windowTest2, nullptr);
		OSObject content = OSCreateGrid(1, 1, OS_GRID_STYLE_CONTAINER);
#if 1
		OSObject scrollPane = OSCreateScrollPane(content, OS_CREATE_SCROLL_PANE_VERTICAL);
		OSSetRootGrid(window, scrollPane);
#else
		OSSetRootGrid(window, content);
#endif
		const char *loremIpsum = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.";
		// OSAddControl(content, 0, 0, OSCreateLabel(OSLiteral(loremIpsum), true), OS_CELL_FILL);
		// OSAddControl(content, 0, 0, OSCreateLabel(OSLiteral("Hello!"), false), 0);

		OSObject textbox = OSCreateTextbox(OS_TEXTBOX_STYLE_NORMAL);
		OSAddControl(content, 0, 0, textbox, OS_CELL_H_PUSH | OS_CELL_V_PUSH | OS_CELL_H_EXPAND);
		OSSetText(textbox, OSLiteral(loremIpsum), OS_RESIZE_MODE_IGNORE);
	}
#endif

	{
		OSError error = OSOpenInstance(&calculator, instance, OSLiteral("calculator"));
		OSPrint("error = %d\n", error);
	}

	OSProcessMessages();
}
