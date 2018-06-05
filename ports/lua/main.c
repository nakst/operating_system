#define OS_NO_CSTDLIB
#include "../../../../api/os.h"

#include <string.h>

#define OS_MANIFEST_DEFINITIONS
#include "lua.manifest.h"
 
#include "src/lua.h"
#include "src/lauxlib.h"
#include "src/lualib.h"

int main (int argc, char **argv);

OSCallbackResponse ProcessSystemMessage(OSObject _object, OSMessage *message) {
	(void) _object;

	if (message->type == OS_MESSAGE_CREATE_INSTANCE) {
		// OSPrint("Create lua state...\n");
		lua_State *state = luaL_newstate();
		luaL_openlibs(state);
		// OSPrint("Create lua state done.\n");
		OSCreateInstance(state, message);
		return OS_CALLBACK_HANDLED;
	} else if (message->type == OS_MESSAGE_PROCESS_REQUEST) {
		size_t argc = 0;

		for (uintptr_t i = 0; i < message->processRequest.requestBytes; i++) {
			if (message->processRequest.request[i] == '\f') {
				argc++;
			}
		}

		if (argc >= 1 && argc <= 16) {
			lua_State *state = (lua_State *) OSGetInstanceContext(message->processRequest.instance);

			char *argv[argc];
			char *next = message->processRequest.request;

			for (uintptr_t i = 0; i < argc; i++) {
				argv[i] = next;
				size_t length = OSStringLength(next, '\f');
				next[length] = 0;
				next += length + 1;
				OSPrint("argv[%d] = %s\n", i, OSCStringLength(argv[i]), argv[i]);
			}

			// TODO Error handling.

			int result;
			(void) result;

			if (0 == strcmp(argv[0], "EXECUTE") && argc == 2) {
				result = luaL_loadstring(state, argv[1]);
				result = lua_pcall(state, 0, LUA_MULTRET, 0);
				return OS_CALLBACK_HANDLED;
			} else if (0 == strcmp(argv[0], "MAP") && argc >= 2) {
				for (uintptr_t i = 2; i < argc; i++) {
					result = luaL_loadstring(state, argv[i]);
					result = lua_pcall(state, 0, LUA_MULTRET, 0);
				}

				char buffer[4096];
				strcpy(buffer, "return ");
				strcat(buffer, argv[1]);

				result = luaL_loadstring(state, buffer);
				result = lua_pcall(state, 0, LUA_MULTRET, 0);

				char *output = (char *) lua_tostring(state, -1);
				message->processRequest.response = output;
				message->processRequest.responseBytes = strlen(output);
				lua_pop(state, 1);
			}
		}

		return OS_CALLBACK_REJECTED;
	}

	return OS_CALLBACK_NOT_HANDLED;
}

void ProgramEntry() {
	OSSetMessageCallback(osSystemMessages, OS_MAKE_MESSAGE_CALLBACK(ProcessSystemMessage, nullptr));
	OSProcessMessages();
}
