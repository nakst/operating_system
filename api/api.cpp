#include "os.h"

#define Defer(x) OSDefer(x)

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_STATIC
#include "stb_image.h"

#define STB_SPRINTF_IMPLEMENTATION
#define STB_SPRINTF_STATIC
#include "stb_sprintf.h"

#ifndef CF
#define CF(x) OS ## x
#endif

enum APIObjectType {
	API_OBJECT_WINDOW,
	API_OBJECT_GRID,
	API_OBJECT_CONTROL,
};

struct APIObject {
	APIObjectType type;
	OSCallback callback;
	APIObject *parent;
};

#include "utf8.h"
#include "heap.cpp"
#include "linked_list.cpp"
#include "font.cpp"
#include "gui.cpp"
#include "common.cpp"
#include "syscall.cpp"

OSHandle osMessageMutex;

APIObject _osSystemMessages;
OSObject osSystemMessages;

extern "C" void ProgramEntry();

extern "C" void OSInitialiseAPI() {
	// TODO Seed random number generator.

	void OSFPInitialise();
	OSFPInitialise();

	heapMutex = OSCreateMutex();
	printMutex = OSCreateMutex();
	osMessageMutex = OSCreateMutex();
	osSystemMessages = &_osSystemMessages;

	OSInitialiseGUI();

	OSMessage m = {};
	m.type = OS_MESSAGE_PROCESS_STARTED;
	OSPostMessage(&m);
}

extern "C" void _start() {
	// Call global constructors.
	void _init();
	_init();

	OSInitialiseAPI();
	ProgramEntry();
	OSTerminateThread(OS_CURRENT_THREAD);
}

OSCallbackResponse OSSendMessage(OSObject target, OSMessage *message) {
	APIObject *object = (APIObject *) target;
	if (!object) return OS_CALLBACK_NOT_HANDLED;
	if (!object->callback.function) return OS_CALLBACK_NOT_HANDLED;
	message->context = object->callback.context;
	return object->callback.function(target, message);
}

OSCallbackResponse OSForwardMessage(OSObject target, OSCallback callback, OSMessage *message) {
	if (!callback.function) {
		return OS_CALLBACK_NOT_HANDLED;
	}

	message->context = callback.context;
	return callback.function(target, message);
}

void OSProcessMessages() {
	while (true) {
		OSMessage message;
		OSWaitMessage(OS_WAIT_NO_TIMEOUT);

		if (OSGetMessage(&message) == OS_SUCCESS) {
			OSAcquireMutex(osMessageMutex);

			if (message.context) {
				OSSendMessage(message.context, &message);
				goto done;
			}

			OSSendMessage(osSystemMessages, &message);

			done:;
			OSReleaseMutex(osMessageMutex);
		}
	}
}

OSCallback OSSetCallback(OSObject generator, OSCallback callback) {
	OSCallback old;
	APIObject *object = (APIObject *) generator;
	old = object->callback;
	object->callback = callback;

	return old;
}
