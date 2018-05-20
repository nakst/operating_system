#define OS_NO_CSTDLIB
#include "../api/os.h"

#define OS_MANIFEST_DEFINITIONS
#include "../bin/Programs/System Monitor/manifest.h"

struct Instance {
	OSObject window;

	void Initialise();
};

OSCallbackResponse DestroyInstance(OSObject object, OSMessage *message) {
	(void) object;
	Instance *instance = (Instance *) message->context;
	OSHeapFree(instance);
	return OS_CALLBACK_HANDLED;
}

void Instance::Initialise() {
	OSStartGUIAllocationBlock(32768);

	window = OSCreateWindow(mainWindow);
	OSSetInstance(window, this);

	OSSetCommandNotificationCallback(window, osCommandDestroyWindow, OS_MAKE_CALLBACK(DestroyInstance, this));

	OSObject rootLayout = OSCreateGrid(1, 1, OS_GRID_STYLE_LAYOUT);
	OSSetRootGrid(window, rootLayout);

	OSEndGUIAllocationBlock();
}

OSCallbackResponse ProcessSystemMessage(OSObject _object, OSMessage *message) {
	(void) _object;

	if (message->type == OS_MESSAGE_CREATE_INSTANCE) {
		((Instance *) OSHeapAllocate(sizeof(Instance), true))->Initialise();
		return OS_CALLBACK_HANDLED;
	} 

	return OS_CALLBACK_NOT_HANDLED;
}

void ProgramEntry() {
	OSSetCallback(osSystemMessages, OS_MAKE_CALLBACK(ProcessSystemMessage, nullptr));
	OSProcessMessages();
}
