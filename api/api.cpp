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
	OSMessageCallback callback;
	APIObject *parent;
};

OSMutex osMessageMutex;

APIObject _osSystemMessages;
OSObject osSystemMessages;

uint64_t osSystemConstants[256];

static bool sendIdleMessages, draggingWindowResizeHandle;
static uint64_t lastIdleTimeStamp;

#include "utf8.h"
#include "heap.cpp"
#include "linked_list.cpp"
#include "font.cpp"
#include "gui.cpp"
#include "common.cpp"
#include "syscall.cpp"

extern "C" void ProgramEntry();

extern "C" void OSInitialiseAPI() {
	// TODO Seed random number generator.

	osSystemMessages = &_osSystemMessages;

	OSSyscall(OS_SYSCALL_GET_SYSTEM_CONSTANTS, (uintptr_t) osSystemConstants, 0, 0, 0);

	OSInitialiseGUI();
	InitialiseCStandardLibrary();

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

OSCallbackResponse OSForwardMessage(OSObject target, OSMessageCallback callback, OSMessage *message) {
	if (!callback.function) {
		return OS_CALLBACK_NOT_HANDLED;
	}

	message->context = callback.context;
	return callback.function(target, message);
}

void OSSendIdleMessages(bool enabled) {
	sendIdleMessages = enabled;
	lastIdleTimeStamp = OSProcessorReadTimeStamp();
}

void OSProcessMessages() {
	while (true) {
		OSMessage message;

		if (sendIdleMessages && !draggingWindowResizeHandle) {
			uint64_t timeStamp = OSProcessorReadTimeStamp();
			message.type = OS_MESSAGE_IDLE;
			message.idle.microsecondsSinceLastIdleMessage = 
				(timeStamp - lastIdleTimeStamp) / osSystemConstants[OS_SYSTEM_CONSTANT_TIME_STAMP_UNITS_PER_MICROSECOND];
			lastIdleTimeStamp = timeStamp;
			OSSendMessage(osSystemMessages, &message);
		} else {
			OSWaitMessage(OS_WAIT_NO_TIMEOUT);
		}

		if (OSGetMessage(&message) == OS_SUCCESS) {
			OSAcquireMutex(&osMessageMutex);

			if (message.type == OS_MESSAGE_SYSTEM_CONSTANT_UPDATED) {
				osSystemConstants[message.systemConstantUpdated.index] = message.systemConstantUpdated.newValue;
				RefreshAllWindows();
				goto done;
			} else if (message.type == OS_MESSAGE_ISSUE_COMMAND) {
				IssueCommand(&message);
				goto done;
			} else if (message.type == OS_MESSAGE_ISSUE_REQUEST) {
				OSInstance *instance = (OSInstance *) message.context;
				OSHandle handle = message.issueRequest.requestBuffer;
				size_t s = message.issueRequest.requestBytes;
				char *data = (char *) OSHeapAllocate(s, false);
				OSReadConstantBuffer(handle, data);
				message.type = OS_MESSAGE_PROCESS_REQUEST;
				message.processRequest.request = data;
				message.processRequest.requestBytes = s;
				message.processRequest.instance = instance;
				message.processRequest.response = nullptr;
				message.processRequest.responseBytes = 0;
				OSPrint("Request: %s\n", message.processRequest.requestBytes, message.processRequest.request);
				OSSendMessage(osSystemMessages, &message);
				OSPrint("Response: %s\n", message.processRequest.responseBytes, message.processRequest.response);
				OSSyscall(OS_SYSCALL_SET_REQUEST_RESPONSE, instance->handle, (uintptr_t) message.processRequest.response, message.processRequest.responseBytes, 0);
				OSHeapFree(data);
				OSCloseHandle(handle);
				goto done;
			}

			if (message.context) {
				OSSendMessage(message.context, &message);
				goto done;
			}

			OSSendMessage(osSystemMessages, &message);

			done:;
			OSReleaseMutex(&osMessageMutex);
		}
	}
}

OSMessageCallback OSSetMessageCallback(OSObject generator, OSMessageCallback callback) {
	OSMessageCallback old;
	APIObject *object = (APIObject *) generator;
	old = object->callback;
	object->callback = callback;

	return old;
}

OSCallbackResponse OSSendNotification(OSObject generator, OSNotificationCallback callback, OSNotification *notification, OSInstance *instance) {
	if (!callback.function) {
		return OS_CALLBACK_NOT_HANDLED;
	}

	notification->context = callback.context;
	notification->generator = generator;
	notification->instance = instance;

	return callback.function(notification);
}
