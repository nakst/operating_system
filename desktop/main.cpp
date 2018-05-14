#include "../api/os.h"

#define WALLPAPER ("/OS/Sample Images/Flower.jpg")
#define FIRST_PROGRAM ("File Manager")

#define OS_MANIFEST_DEFINITIONS
#include "../bin/OS/desktop.manifest.h"

#define MANIFEST_PARSER_LIBRARY
#include "../util/manifest_parser.cpp"

struct InstalledProgram {
	Token name, shortName, workingFolder, executablePath;
	OSHandle process;
	uintptr_t pid;
};

#define MAX_INSTALLED_PROGRAMS (8192)
InstalledProgram installedPrograms[MAX_INSTALLED_PROGRAMS];
size_t installedProgramCount;

char *errorMessages[] = {
	(char *) "INVALID_BUFFER",
	(char *) "UNKNOWN_SYSCALL",
	(char *) "INVALID_MEMORY_REGION",
	(char *) "MEMORY_REGION_LOCKED_BY_KERNEL",
	(char *) "PATH_LENGTH_EXCEEDS_LIMIT",
	(char *) "INVALID_HANDLE",
	(char *) "MUTEX_NOT_ACQUIRED_BY_THREAD",
	(char *) "MUTEX_ALREADY_ACQUIRED",
	(char *) "BUFFER_NOT_ACCESSIBLE",
	(char *) "SHARED_MEMORY_REGION_TOO_LARGE",
	(char *) "SHARED_MEMORY_STILL_MAPPED",
	(char *) "COULD_NOT_LOAD_FONT",
	(char *) "COULD_NOT_DRAW_FONT",
	(char *) "COULD_NOT_ALLOCATE_MEMORY",
	(char *) "INCORRECT_FILE_ACCESS",
	(char *) "TOO_MANY_WAIT_OBJECTS",
	(char *) "INCORRECT_NODE_TYPE",
	(char *) "PROCESSOR_EXCEPTION",
	(char *) "INVALID_PANE_CHILD",
	(char *) "INVALID_PANE_OBJECT",
	(char *) "UNSUPPORTED_CALLBACK",
	(char *) "MISSING_CALLBACK",
	(char *) "UNKNOWN",
	(char *) "RECURSIVE_BATCH",
	(char *) "CORRUPT_HEAP",
	(char *) "BAD_CALLBACK_OBJECT",
	(char *) "RESIZE_GRID",
	(char *) "OUT_OF_GRID_BOUNDS",
	(char *) "OVERWRITE_GRID_OBJECT",
	(char *) "CORRUPT_LINKED_LIST",
	(char *) "NO_MENU_POSITION",
	(char *) "BAD_OBJECT_TYPE",
	(char *) "MESSAGE_SHOULD_BE_HANDLED",
	(char *) "INDEX_OUT_OF_BOUNDS",
};

OSCallbackResponse ProcessSystemMessage(OSObject _object, OSMessage *message) {
	(void) _object;
	OSCallbackResponse response = OS_CALLBACK_NOT_HANDLED;

	switch (message->type) {
		case OS_MESSAGE_PROGRAM_CRASH: {
			// Terminate the crashed process.
			OSTerminateProcess(message->crash.process);
			OSCloseHandle(message->crash.process);

			char crashMessage[256];
			size_t crashMessageLength;
			OSError code = message->crash.reason.errorCode;

			if (code < OS_FATAL_ERROR_COUNT && code >= 0) {
				crashMessageLength = OSFormatString(crashMessage, 256, 
						"Error code: %d (%s)", code, OSCStringLength(errorMessages[code]), errorMessages[code]);
			} else {
				crashMessageLength = OSFormatString(crashMessage, 256, 
						"Error code: %d (user defined error)", code);
			}

			size_t processNameBytes = message->crash.processNameBytes;
			char *processName = (char *) OSHeapAllocate(processNameBytes, false);
			char *processNameBase = processName;
			OSReadConstantBuffer(message->crash.processNameBuffer, processName);
			OSCloseHandle(message->crash.processNameBuffer);

			if (*processName == '/') {
				processName += processNameBytes;
				processNameBytes = 0;

				do {
					processNameBytes++;
					processName--;
				} while (*processName != '/');

				processNameBytes--;
				processName++;
			}

			char crashMessage2[256];
			size_t crashMessage2Length;

			crashMessage2Length = OSFormatString(crashMessage2, 256, 
					"%s has crashed.", processNameBytes, processName);

			OSShowDialogAlert(OSLiteral("Program Crashed"),
					crashMessage2, crashMessage2Length,
					crashMessage, crashMessageLength,
					OS_ICON_ERROR, OS_INVALID_OBJECT);

			// If this was an installed program, we'll have to restart the process.
			for (uintptr_t i = 0; i < installedProgramCount; i++) {
				if (installedPrograms[i].pid == message->crash.pid) {
					OSCloseHandle(installedPrograms[i].process);
					installedPrograms[i].pid = 0;
					installedPrograms[i].process = 0;
				}
			}

			OSHeapFree(processNameBase);

			response = OS_CALLBACK_HANDLED;
		} break;

		case OS_MESSAGE_EXECUTE_PROGRAM: {
			size_t nameBytes = message->executeProgram.nameBytes;
			char *name = (char *) OSHeapAllocate(nameBytes, false);
			OSReadConstantBuffer(message->executeProgram.nameBuffer, name);
			OSCloseHandle(message->executeProgram.nameBuffer);

			bool found = false;
			InstalledProgram *program;
			bool executable = *name == '/';

			for (uintptr_t i = 0; i < installedProgramCount; i++) {
				program = installedPrograms + i;

				if (!executable && 0 == OSCompareStrings(program->name.text, name, program->name.bytes, nameBytes)) {
					found = true;
					break;
				} else if (executable && 0 == OSCompareStrings(program->executablePath.text, name, program->executablePath.bytes, nameBytes)) {
					found = true;
					break;
				}
			}

			if (found) {
				bool start = false, close = false;

				if (!program->process) {
					// The program hasn't been started yet.
					start = true;
				} else {
					OSProcessState state;
					OSGetProcessState(program->process, &state);

					if (state.terminating) {
						// The program has been terminated.
						close = true;
					}
				}

				if (close) {
					start = true;
					OSCloseHandle(program->process);
					program->pid = 0;
					program->process = 0;
				}

				if (start) {
					OSProcessInformation information;

					if (OS_SUCCESS == OSCreateProcess(program->executablePath.text, program->executablePath.bytes, &information, nullptr)) {
						OSCloseHandle(information.mainThread.handle);

						program->process = information.handle;
						program->pid = information.pid;
					}
				}

				if (program->process) {
					OSMessage m = {};
					m.type = OS_MESSAGE_CREATE_INSTANCE;
					OSPostMessageRemote(program->process, &m);
				}
			} else if (executable) {
				// Not an installed program.
				// Run the executable anyway.

				OSProcessInformation information;

				if (OS_SUCCESS == OSCreateProcess(name, nameBytes, &information, nullptr)) {
					OSCloseHandle(information.handle);
					OSCloseHandle(information.mainThread.handle);
				}
			} else {
				char buffer[256];
				size_t bufferLength = OSFormatString(buffer, 256, "\"%s\" could not be found.", nameBytes, name);

				OSShowDialogAlert(OSLiteral("Invalid Program"),
						OSLiteral("The program failed to start."),
						buffer, bufferLength,
						OS_ICON_ERROR, OS_INVALID_OBJECT);
			}

			OSHeapFree(name);
		} break;

		case OS_MESSAGE_PROGRAM_FAILED_TO_START: {
			OSShowDialogAlert(OSLiteral("Invalid Program"),
					OSLiteral("The program failed to start."),
					OSLiteral("The executable file was either corrupt, or not designed to run on your computer."),
					OS_ICON_ERROR, OS_INVALID_OBJECT);
		} break;

		default: {
			response = OS_CALLBACK_NOT_HANDLED;
		} break;
	}

	return response;
}

bool LoadImageIntoSurface(char *cPath, OSHandle surface, bool center, uintptr_t atX = 0, uintptr_t atY = 0) {
	size_t fileSize;
	uint8_t *loadedFile = (uint8_t *) OSReadEntireFile(cPath, OSCStringLength(cPath), &fileSize);

	if (!loadedFile) {
		return false;
	} else {
		int imageX, imageY, imageChannels;
		uint8_t *image = stbi_load_from_memory(loadedFile, fileSize, &imageX, &imageY, &imageChannels, 4);

		if (!image) {
			OSHeapFree(loadedFile);
			return false;
		} else {
			OSLinearBuffer buffer; 
			OSGetLinearBuffer(surface, &buffer);

			void *bitmap = OSMapObject(buffer.handle, 0, buffer.height * buffer.stride, OS_MAP_OBJECT_READ_WRITE);

			if (center) {
				int xOffset = 0, yOffset = 0;

				if (imageX > (int) buffer.width) {
					xOffset = imageX / 2 - buffer.width / 2;
				}

				if (imageY > (int) buffer.height) {
					yOffset = imageY / 2 - buffer.height / 2;
				}

				for (uintptr_t y = 0; y < buffer.height; y++) {
					for (uintptr_t x = 0; x < buffer.width; x++) {
						uint8_t *destination = (uint8_t *) bitmap + (y) * buffer.stride + (x) * 4;
						uint8_t *source = image + (y + yOffset) * imageX * 4 + (x + xOffset) * 4;

						destination[2] = source[0];
						destination[1] = source[1];
						destination[0] = source[2];
						destination[3] = source[3];
					}
				}
			} else {
				for (uintptr_t y = atY; y < atY + imageY; y++) {
					for (uintptr_t x = atX; x < atX + imageX; x++) {
						uint8_t *destination = (uint8_t *) bitmap + (y) * buffer.stride + (x) * 4;
						uint8_t *source = image + (y - atY) * imageX * 4 + (x - atX) * 4;

						destination[2] = source[0];
						destination[1] = source[1];
						destination[0] = source[2];
						destination[3] = source[3];
					}
				}
			}

			OSInvalidateRectangle(surface, OS_MAKE_RECTANGLE(0, imageX, 0, imageY));
			OSHeapFree(image);
			OSFree(bitmap);
			OSCloseHandle(buffer.handle);
		}

		OSHeapFree(loadedFile);
	}

	return true;
}

void InitialiseGUI() {
	LoadImageIntoSurface((char *) "/OS/Visual Styles/Default.png", OS_SURFACE_UI_SHEET, false);
	LoadImageIntoSurface((char *) "/OS/Icons/Tango Icons 16x16.png", OS_SURFACE_UI_SHEET, false, 512, 0);

#ifdef WALLPAPER
	LoadImageIntoSurface((char *) WALLPAPER, OS_SURFACE_WALLPAPER, true);
#else
	OSHandle surface = OS_SURFACE_WALLPAPER;
	OSLinearBuffer buffer; OSGetLinearBuffer(surface, &buffer);
	OSFillRectangle(surface, OS_MAKE_RECTANGLE(0, buffer.width, 0, buffer.height), OSColor(0x5372A6));
#endif

	OSInitialiseGUI();
	OSRedrawAll();
}

Token RemoveQuotes(Token token) {
	if (*token.text == '"') {
		token.text++;
		token.bytes--;
	}

	if (token.text[token.bytes - 1] == '"') {
		token.bytes--;
	}

	return token;
}

void LoadInstalledPrograms(Token attribute, Token section, Token name, Token value, int event) {
	if (installedProgramCount == MAX_INSTALLED_PROGRAMS) {
		return;
	}

	if (event == EVENT_START_SECTION) { 
		if (CompareTokens(attribute, "program")) {
			installedPrograms[installedProgramCount].shortName = section;
		}
	}

	if (event == EVENT_ATTRIBUTE) { 
		if (CompareTokens(attribute, "program")) {
			if (CompareTokens(name, "name")) {
				installedPrograms[installedProgramCount].name = RemoveQuotes(value);
			} else if (CompareTokens(name, "workingFolder")) {
				installedPrograms[installedProgramCount].workingFolder = RemoveQuotes(value);
			} else if (CompareTokens(name, "executablePath")) {
				installedPrograms[installedProgramCount].executablePath = RemoveQuotes(value);
			}
		}
	}

	if (event == EVENT_END_SECTION) { 
		if (CompareTokens(attribute, "program")) {
			installedProgramCount++;
		}
	}
}

extern "C" void ProgramEntry() {
	if (sizeof(OSMessage) > 64) {
		OSPrint("Warning: OSMessage is larger than 64 bytes.\n");
	}

	OSHandle handle = OSOpenSharedMemory(1, OSLiteral("DesktopInstance"), OS_OPEN_SHARED_MEMORY_FAIL_IF_FOUND);

	if (handle == OS_INVALID_HANDLE) {
		OSShowDialogAlert(OSLiteral("Desktop Multiple Processes"),
				OSLiteral("You attempted to launch multiple desktop processes."),
				OSLiteral("Only one desktop process can exist per user account."),
				OS_ICON_ERROR, OS_INVALID_OBJECT);
		OSProcessMessages();
		return;
	}

	InitialiseGUI();

	{
		size_t fileSize;
		char *file = (char *) OSReadEntireFile(OSLiteral("/OS/Installed Programs.dat"), &fileSize); 

		if (!file || !ParseManifest(file, LoadInstalledPrograms)
				|| installedProgramCount == MAX_INSTALLED_PROGRAMS) {
			OSShowDialogAlert(OSLiteral("System Configuration Error"),
					OSLiteral("The system configuration could not be loaded."),
					OSLiteral(""),
					OS_ICON_ERROR, OS_INVALID_OBJECT);
			OSProcessMessages();
		}
	}

	OSExecuteProgram(OSLiteral(FIRST_PROGRAM));

	OSSetCallback(osSystemMessages, OS_MAKE_CALLBACK(ProcessSystemMessage, nullptr));
	OSProcessMessages();
}
