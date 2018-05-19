#ifndef IncludedEssenceAPIHeader
#define IncludedEssenceAPIHeader

// TODO Remove constructors.

#include <stdint.h>
#include <limits.h>
#include <stddef.h>
#include <stdarg.h>
#include <stdbool.h>

#ifdef __cplusplus
#define OS_EXTERN_C extern "C"
#define OS_CONSTRUCTOR(x) x

// Scoped defer: http://www.gingerbill.org/article/defer-in-cpp.html
template <typename F> struct OSprivDefer { F f; OSprivDefer(F f) : f(f) {} ~OSprivDefer() { f(); } };
template <typename F> OSprivDefer<F> OSdefer_func(F f) { return OSprivDefer<F>(f); }
#define OSDEFER_1(x, y) x##y
#define OSDEFER_2(x, y) OSDEFER_1(x, y)
#define OSDEFER_3(x)    OSDEFER_2(x, __COUNTER__)
#define OS_Defer(code)   auto OSDEFER_3(_defer_) = OSdefer_func([&](){code;})
#define OSDefer(code)   OS_Defer(code)

#else
#define OS_EXTERN_C extern
#define OS_CONSTRUCTOR(x)
#endif

#define OSLiteral(x) (char *) x, OSCStringLength((char *) x)

#define OS_SCANCODE_A (0x1C)
#define OS_SCANCODE_B (0x32)
#define OS_SCANCODE_C (0x21)
#define OS_SCANCODE_D (0x23)
#define OS_SCANCODE_E (0x24)
#define OS_SCANCODE_F (0x2B)
#define OS_SCANCODE_G (0x34)
#define OS_SCANCODE_H (0x33)
#define OS_SCANCODE_I (0x43)
#define OS_SCANCODE_J (0x3B)
#define OS_SCANCODE_K (0x42)
#define OS_SCANCODE_L (0x4B)
#define OS_SCANCODE_M (0x3A)
#define OS_SCANCODE_N (0x31)
#define OS_SCANCODE_O (0x44)
#define OS_SCANCODE_P (0x4D)
#define OS_SCANCODE_Q (0x15)
#define OS_SCANCODE_R (0x2D)
#define OS_SCANCODE_S (0x1B)
#define OS_SCANCODE_T (0x2C)
#define OS_SCANCODE_U (0x3C)
#define OS_SCANCODE_V (0x2A)
#define OS_SCANCODE_W (0x1D)
#define OS_SCANCODE_X (0x22)
#define OS_SCANCODE_Y (0x35)
#define OS_SCANCODE_Z (0x1A)

#define OS_SCANCODE_0 (0x45)
#define OS_SCANCODE_1 (0x16)
#define OS_SCANCODE_2 (0x1E)
#define OS_SCANCODE_3 (0x26)
#define OS_SCANCODE_4 (0x25)
#define OS_SCANCODE_5 (0x2E)
#define OS_SCANCODE_6 (0x36)
#define OS_SCANCODE_7 (0x3D)
#define OS_SCANCODE_8 (0x3E)
#define OS_SCANCODE_9 (0x46)

#define OS_SCANCODE_CAPS_LOCK	(0x58)
#define OS_SCANCODE_SCROLL_LOCK	(0x7E)
#define OS_SCANCODE_NUM_LOCK	(0x77) // Also sent by the pause key.
#define OS_SCANCODE_LEFT_SHIFT	(0x12)
#define OS_SCANCODE_LEFT_CTRL	(0x14)
#define OS_SCANCODE_LEFT_ALT	(0x11)
#define OS_SCANCODE_LEFT_FLAG	(0x11F)
#define OS_SCANCODE_RIGHT_SHIFT	(0x59)
#define OS_SCANCODE_RIGHT_CTRL	(0x114)
#define OS_SCANCODE_RIGHT_ALT	(0x111)
#define OS_SCANCODE_PAUSE	(0xE1)
#define OS_SCANCODE_CONTEXT_MENU (0x127)

#define OS_SCANCODE_BACKSPACE	(0x66)
#define OS_SCANCODE_ESCAPE	(0x76)
#define OS_SCANCODE_INSERT	(0x170)
#define OS_SCANCODE_HOME	(0x16C)
#define OS_SCANCODE_PAGE_UP	(0x17D)
#define OS_SCANCODE_DELETE	(0x171)
#define OS_SCANCODE_END		(0x169)
#define OS_SCANCODE_PAGE_DOWN	(0x17A)
#define OS_SCANCODE_UP_ARROW	(0x175)
#define OS_SCANCODE_LEFT_ARROW	(0x16B)
#define OS_SCANCODE_DOWN_ARROW	(0x172)
#define OS_SCANCODE_RIGHT_ARROW	(0x174)

#define OS_SCANCODE_SPACE	(0x29)
#define OS_SCANCODE_TAB		(0x0D)
#define OS_SCANCODE_ENTER	(0x5A)

#define OS_SCANCODE_SLASH	(0x4A)
#define OS_SCANCODE_BACKSLASH	(0x5D)
#define OS_SCANCODE_LEFT_BRACE	(0x54)
#define OS_SCANCODE_RIGHT_BRACE	(0x5B)
#define OS_SCANCODE_EQUALS	(0x55)
#define OS_SCANCODE_BACKTICK	(0x0E)
#define OS_SCANCODE_HYPHEN	(0x4E)
#define OS_SCANCODE_SEMICOLON	(0x4C)
#define OS_SCANCODE_QUOTE	(0x52)
#define OS_SCANCODE_COMMA	(0x41)
#define OS_SCANCODE_PERIOD	(0x49)

#define OS_SCANCODE_NUM_DIVIDE 	 (0x14A)
#define OS_SCANCODE_NUM_MULTIPLY (0x7C)
#define OS_SCANCODE_NUM_SUBTRACT (0x7B)
#define OS_SCANCODE_NUM_ADD	 (0x79)
#define OS_SCANCODE_NUM_ENTER	 (0x15A)
#define OS_SCANCODE_NUM_POINT	 (0x71)
#define OS_SCANCODE_NUM_0	 (0x70)
#define OS_SCANCODE_NUM_1	 (0x69)
#define OS_SCANCODE_NUM_2	 (0x72)
#define OS_SCANCODE_NUM_3	 (0x7A)
#define OS_SCANCODE_NUM_4	 (0x6B)
#define OS_SCANCODE_NUM_5	 (0x73)
#define OS_SCANCODE_NUM_6	 (0x74)
#define OS_SCANCODE_NUM_7	 (0x6C)
#define OS_SCANCODE_NUM_8	 (0x75)
#define OS_SCANCODE_NUM_9	 (0x7D)

#define OS_SCANCODE_PRINT_SCREEN_1 (0x112) // Both are sent when print screen is pressed.
#define OS_SCANCODE_PRINT_SCREEN_2 (0x17C)

#define OS_SCANCODE_F1  (0x05)
#define OS_SCANCODE_F2  (0x06)
#define OS_SCANCODE_F3  (0x04)
#define OS_SCANCODE_F4  (0x0C)
#define OS_SCANCODE_F5  (0x03)
#define OS_SCANCODE_F6  (0x0B)
#define OS_SCANCODE_F7  (0x83)
#define OS_SCANCODE_F8  (0x0A)
#define OS_SCANCODE_F9  (0x01)
#define OS_SCANCODE_F10 (0x09)
#define OS_SCANCODE_F11 (0x78)
#define OS_SCANCODE_F12 (0x07)

#define OS_SCANCODE_ACPI_POWER 	(0x137)
#define OS_SCANCODE_ACPI_SLEEP 	(0x13F)
#define OS_SCANCODE_ACPI_WAKE  	(0x15E)

#define OS_SCANCODE_MM_NEXT	(0x14D)
#define OS_SCANCODE_MM_PREVIOUS	(0x115)
#define OS_SCANCODE_MM_STOP	(0x13B)
#define OS_SCANCODE_MM_PAUSE	(0x134)
#define OS_SCANCODE_MM_MUTE	(0x123)
#define OS_SCANCODE_MM_QUIETER	(0x121)
#define OS_SCANCODE_MM_LOUDER	(0x132)
#define OS_SCANCODE_MM_SELECT	(0x150)
#define OS_SCANCODE_MM_EMAIL	(0x148)
#define OS_SCANCODE_MM_CALC	(0x12B)
#define OS_SCANCODE_MM_FILES	(0x140)

#define OS_SCANCODE_WWW_SEARCH	(0x110)
#define OS_SCANCODE_WWW_HOME	(0x13A)
#define OS_SCANCODE_WWW_BACK	(0x138)
#define OS_SCANCODE_WWW_FORWARD	(0x130)
#define OS_SCANCODE_WWW_STOP	(0x128)
#define OS_SCANCODE_WWW_REFRESH	(0x120)
#define OS_SCANCODE_WWW_STARRED	(0x118)

#define OS_ICON_NONE 			(0)
#define OS_ICON_FILE 			(1)
#define OS_ICON_FOLDER 			(2)
#define OS_ICON_ERROR 			(3)
#define OS_ICON_FORWARD			(4)
#define OS_ICON_BACK 			(5)
#define OS_ICON_PARENT			(6)
#define OS_ICON_BOOKMARK		(7)
#define OS_ICON_ROTATE_CLOCKWISE 	(8)
#define OS_ICON_ROTATE_ANTI_CLOCKWISE 	(9)
#define OS_ICON_MAGNIFYING_GLASS	(10)
#define OS_ICON_SHUTDOWN		(11)

#define OS_FLAGS_DEFAULT (0)

OS_EXTERN_C uintptr_t _OSSyscall(uintptr_t argument0, uintptr_t argument1, uintptr_t argument2, 
			        uintptr_t unused,    uintptr_t argument3, uintptr_t argument4);
#define OSSyscall(a, b, c, d, e) _OSSyscall((a), (b), (c), 0, (d), (e))

#define OS_CHECK_ERROR(x) (((intptr_t) (x)) < (OS_SUCCESS))

#define OS_SUCCESS 				(-1)

typedef enum OSFatalError {
	OS_FATAL_ERROR_INVALID_BUFFER,
	OS_FATAL_ERROR_UNKNOWN_SYSCALL,
	OS_FATAL_ERROR_INVALID_MEMORY_REGION,
	OS_FATAL_ERROR_MEMORY_REGION_LOCKED_BY_KERNEL,
	OS_FATAL_ERROR_PATH_LENGTH_EXCEEDS_LIMIT,
	OS_FATAL_ERROR_INVALID_HANDLE, // Note: this has to be fatal!! See the linear handle list.
	OS_FATAL_ERROR_MUTEX_NOT_ACQUIRED_BY_THREAD,
	OS_FATAL_ERROR_MUTEX_ALREADY_ACQUIRED,
	OS_FATAL_ERROR_BUFFER_NOT_ACCESSIBLE,
	OS_FATAL_ERROR_SHARED_MEMORY_REGION_TOO_LARGE,
	OS_FATAL_ERROR_SHARED_MEMORY_STILL_MAPPED,
	OS_FATAL_ERROR_COULD_NOT_LOAD_FONT,
	OS_FATAL_ERROR_COULD_NOT_DRAW_FONT,
	OS_FATAL_ERROR_COULD_NOT_ALLOCATE_MEMORY,
	OS_FATAL_ERROR_INCORRECT_FILE_ACCESS,
	OS_FATAL_ERROR_TOO_MANY_WAIT_OBJECTS,
	OS_FATAL_ERROR_INCORRECT_NODE_TYPE,
	OS_FATAL_ERROR_PROCESSOR_EXCEPTION,
	OS_FATAL_ERROR_INVALID_PANE_CHILD,
	OS_FATAL_ERROR_INVALID_PANE_OBJECT,
	OS_FATAL_ERROR_UNSUPPORTED_CALLBACK,
	OS_FATAL_ERROR_MISSING_CALLBACK,
	OS_FATAL_ERROR_UNKNOWN,
	OS_FATAL_ERROR_RECURSIVE_BATCH,
	OS_FATAL_ERROR_CORRUPT_HEAP,
	OS_FATAL_ERROR_BAD_CALLBACK_OBJECT,
	OS_FATAL_ERROR_RESIZE_GRID,
	OS_FATAL_ERROR_OUT_OF_GRID_BOUNDS,
	OS_FATAL_ERROR_OVERWRITE_GRID_OBJECT,
	OS_FATAL_ERROR_CORRUPT_LINKED_LIST,
	OS_FATAL_ERROR_NO_MENU_POSITION,
	OS_FATAL_ERROR_BAD_OBJECT_TYPE,
	OS_FATAL_ERROR_MESSAGE_SHOULD_BE_HANDLED,
	OS_FATAL_ERROR_INDEX_OUT_OF_BOUNDS,
	OS_FATAL_ERROR_COUNT,
	OS_FATAL_ERROR_INVALID_STRING_LENGTH,
	OS_FATAL_ERROR_SPINLOCK_NOT_ACQUIRED,
} OSFatalError;

// These must be negative.
// See OSReadFileSync.
#define OS_ERROR_BUFFER_TOO_SMALL		(-2)
#define OS_ERROR_UNKNOWN_OPERATION_FAILURE 	(-7)
#define OS_ERROR_NO_MESSAGES_AVAILABLE		(-9)
#define OS_ERROR_MESSAGE_QUEUE_FULL		(-10)
#define OS_ERROR_MESSAGE_NOT_HANDLED_BY_GUI	(-13)
#define OS_ERROR_PATH_NOT_WITHIN_MOUNTED_VOLUME	(-14)
#define OS_ERROR_PATH_NOT_TRAVERSABLE		(-15)
#define OS_ERROR_FILE_ALREADY_EXISTS		(-19)
#define OS_ERROR_FILE_DOES_NOT_EXIST		(-20)
#define OS_ERROR_DRIVE_ERROR_FILE_DAMAGED	(-21) 
#define OS_ERROR_ACCESS_NOT_WITHIN_FILE_BOUNDS	(-22) 
#define OS_ERROR_FILE_PERMISSION_NOT_GRANTED	(-23)
#define OS_ERROR_FILE_IN_EXCLUSIVE_USE		(-24)
#define OS_ERROR_FILE_CANNOT_GET_EXCLUSIVE_USE	(-25)
#define OS_ERROR_INCORRECT_NODE_TYPE		(-26)
#define OS_ERROR_EVENT_NOT_SET			(-27)
#define OS_ERROR_TIMEOUT_REACHED		(-29)
#define OS_ERROR_REQUEST_CLOSED_BEFORE_COMPLETE (-30)
#define OS_ERROR_NO_CHARACTER_AT_COORDINATE	(-31)
#define OS_ERROR_FILE_ON_READ_ONLY_VOLUME	(-32)
#define OS_ERROR_USER_CANCELED_IO		(-33)
#define OS_ERROR_INVALID_DIMENSIONS		(-34)
#define OS_ERROR_DRIVE_CONTROLLER_REPORTED	(-35)
#define OS_ERROR_COULD_NOT_ISSUE_PACKET		(-36)
#define OS_ERROR_HANDLE_TABLE_FULL		(-37)
#define OS_ERROR_COULD_NOT_RESIZE_FILE		(-38)
#define OS_ERROR_DIRECTORY_NOT_EMPTY		(-39)
#define OS_ERROR_UNSUPPORTED_FILESYSTEM		(-40)
#define OS_ERROR_NODE_ALREADY_DELETED		(-41)
#define OS_ERROR_NODE_IS_ROOT			(-42)
#define OS_ERROR_VOLUME_MISMATCH		(-43)
#define OS_ERROR_TARGET_WITHIN_SOURCE		(-44)
#define OS_ERROR_TARGET_INVALID_TYPE		(-45)
#define OS_ERROR_NOTHING_TO_DRAW		(-46)

typedef intptr_t OSError;

typedef enum OSSyscallType {
	OS_SYSCALL_PRINT,
	OS_SYSCALL_ALLOCATE,
	OS_SYSCALL_FREE,
	OS_SYSCALL_CREATE_PROCESS,
	OS_SYSCALL_GET_CREATION_ARGUMENT,
	OS_SYSCALL_CREATE_SURFACE,
	OS_SYSCALL_GET_LINEAR_BUFFER,
	OS_SYSCALL_INVALIDATE_RECTANGLE,
	OS_SYSCALL_COPY_TO_SCREEN,
	OS_SYSCALL_FORCE_SCREEN_UPDATE,
	OS_SYSCALL_FILL_RECTANGLE,
	OS_SYSCALL_COPY_SURFACE,
	OS_SYSCALL_CLEAR_MODIFIED_REGION,
	OS_SYSCALL_GET_MESSAGE,
	OS_SYSCALL_POST_MESSAGE,
	OS_SYSCALL_POST_MESSAGE_REMOTE,
	OS_SYSCALL_WAIT_MESSAGE,
	OS_SYSCALL_CREATE_WINDOW,
	OS_SYSCALL_UPDATE_WINDOW,
	OS_SYSCALL_DRAW_SURFACE,
	OS_SYSCALL_CREATE_MUTEX,
	OS_SYSCALL_ACQUIRE_MUTEX,
	OS_SYSCALL_RELEASE_MUTEX, 
	OS_SYSCALL_CLOSE_HANDLE,
	OS_SYSCALL_TERMINATE_THREAD,
	OS_SYSCALL_CREATE_THREAD,
	OS_SYSCALL_WAIT,
	OS_SYSCALL_SHARE_MEMORY,
	OS_SYSCALL_MAP_OBJECT,
	OS_SYSCALL_OPEN_SHARED_MEMORY,
	OS_SYSCALL_TERMINATE_PROCESS,
	OS_SYSCALL_OPEN_NODE,
	OS_SYSCALL_READ_FILE_SYNC,
	OS_SYSCALL_WRITE_FILE_SYNC,
	OS_SYSCALL_RESIZE_FILE,
	OS_SYSCALL_CREATE_EVENT,
	OS_SYSCALL_SET_EVENT,
	OS_SYSCALL_RESET_EVENT,
	OS_SYSCALL_POLL_EVENT,
	OS_SYSCALL_REFRESH_NODE_INFORMATION,
	OS_SYSCALL_SET_CURSOR_STYLE,
	OS_SYSCALL_MOVE_WINDOW,
	OS_SYSCALL_GET_WINDOW_BOUNDS,
	OS_SYSCALL_REDRAW_ALL,
	OS_SYSCALL_PAUSE_PROCESS,
	OS_SYSCALL_CRASH_PROCESS,
	OS_SYSCALL_GET_THREAD_ID,
	OS_SYSCALL_ENUMERATE_DIRECTORY_CHILDREN,
	OS_SYSCALL_READ_FILE_ASYNC,
	OS_SYSCALL_WRITE_FILE_ASYNC,
	OS_SYSCALL_GET_IO_REQUEST_PROGRESS,
	OS_SYSCALL_CANCEL_IO_REQUEST,
	OS_SYSCALL_BATCH,
	OS_SYSCALL_NEED_WM_TIMER,
	OS_SYSCALL_RESET_CLICK_CHAIN,
	OS_SYSCALL_GET_CURSOR_POSITION,
	OS_SYSCALL_COPY,
	OS_SYSCALL_GET_CLIPBOARD_HEADER,
	OS_SYSCALL_PASTE_TEXT,
	OS_SYSCALL_DELETE_NODE,
	OS_SYSCALL_MOVE_NODE,
	OS_SYSCALL_EXECUTE_PROGRAM,
	OS_SYSCALL_READ_CONSTANT_BUFFER,
	OS_SYSCALL_GET_PROCESS_STATE,
	OS_SYSCALL_SHUTDOWN,
	OS_SYSCALL_SET_FOCUSED_WINDOW,
	OS_SYSCALL_YIELD_SCHEDULER,
	OS_SYSCALL_ACQUIRE_MULTIPLE_MUTEXES,
} OSSyscallType;

#define OS_INVALID_HANDLE 		((OSHandle) (0))
#define OS_CURRENT_THREAD	 	((OSHandle) (0x1000))
#define OS_CURRENT_PROCESS	 	((OSHandle) (0x1001))
#define OS_SURFACE_UI_SHEET		((OSHandle) (0x2000))
#define OS_SURFACE_WALLPAPER		((OSHandle) (0x2001))

#define OS_INVALID_OBJECT		(nullptr)

#define OS_WAIT_NO_TIMEOUT (-1)

// Note: If you're using a timeout, then 
#define OS_MAX_WAIT_COUNT 		(16)

typedef uintptr_t OSHandle;
typedef void *OSObject;

typedef struct OSBatchCall {
	OSSyscallType index; 
	bool stopBatchIfError;
	union { uintptr_t argument0, returnValue; };
	uintptr_t argument1, argument2, argument3;
} OSBatchCall;

typedef struct OSThreadInformation {
	OSHandle handle;
	uintptr_t tid;
} OSThreadInformation;

typedef struct OSProcessInformation {
	OSHandle handle;
	uintptr_t pid;
	OSThreadInformation mainThread;
} OSProcessInformation;

typedef struct OSUniqueIdentifier {
	uint8_t d[16];
} OSUniqueIdentifier;

typedef enum OSNodeType {
	OS_NODE_FILE,
	OS_NODE_DIRECTORY,
	OS_NODE_INVALID,
} OSNodeType;

typedef struct OSNodeInformation {
	union {
		OSHandle handle;
		bool present; // From OSEnumerateDirectoryChildren.
	};

	OSNodeType type;

	union {
		uint64_t fileSize;
		uint64_t directoryChildren;
	};
} OSNodeInformation;

typedef struct OSDirectoryChild {
#define OS_MAX_DIRECTORY_CHILD_NAME_LENGTH (256)
	char name[OS_MAX_DIRECTORY_CHILD_NAME_LENGTH];
	size_t nameLengthBytes;
	OSNodeInformation information; 
} OSDirectoryChild;

typedef struct OSPoint {
	int32_t x;
	int32_t y;
} OSPoint;

typedef struct OSRectangle {
	int32_t left;   // Inclusive.
	int32_t right;  // Exclusive.
	int32_t top;    // Inclusive.
	int32_t bottom; // Exclusive.
} OSRectangle;

#define OS_MAKE_RECTANGLE_ALL(x) ((OSRectangle){(int32_t)(x),(int32_t)(x),(int32_t)(x),(int32_t)(x)})
#define OS_MAKE_RECTANGLE(l, r, t, b) ((OSRectangle){(int32_t)(l),(int32_t)(r),(int32_t)(t),(int32_t)(b)})
#define OS_MAKE_CALLBACK(a, b) ((OSCallback){(a),(b)})
#define OS_MAKE_POINT(x, y) ((OSPoint){(int32_t)(x),(int32_t)(y)})

typedef struct OSColor {
	OS_CONSTRUCTOR(OSColor() {})

	OS_CONSTRUCTOR(OSColor(uint32_t x) {
		red = (x & 0xFF0000) >> 16;
		green = (x & 0xFF00) >> 8;
		blue = (x & 0xFF) >> 0;
	})

	OS_CONSTRUCTOR(OSColor(uint8_t _red, uint8_t _green, uint8_t _blue) {
		red = _red;
		green = _green;
		blue = _blue;
	})

	uint8_t red;
	uint8_t green;
	uint8_t blue;
} OSColor;

typedef enum OSColorFormat {
	OS_COLOR_FORMAT_32_XRGB,
} OSColorFormat; 

typedef struct OSLinearBuffer {
	size_t width, height, stride;
	OSColorFormat colorFormat;
	OSHandle handle; // A shared memory region. See OSMapObject.
} OSLinearBuffer;

typedef struct _OSRectangleAndColor {
	OSRectangle rectangle;
	OSColor color;
} _OSRectangleAndColor;

typedef struct _OSDrawSurfaceArguments {
	OSRectangle source, destination, border;
	uint8_t alpha;
} _OSDrawSurfaceArguments;

typedef enum OSCursorStyle {
	OS_CURSOR_NORMAL, 
	OS_CURSOR_TEXT, 
	OS_CURSOR_RESIZE_VERTICAL, 
	OS_CURSOR_RESIZE_HORIZONTAL,
	OS_CURSOR_RESIZE_DIAGONAL_1, // '/'
	OS_CURSOR_RESIZE_DIAGONAL_2, // '\'
	OS_CURSOR_SPLIT_VERTICAL,
	OS_CURSOR_SPLIT_HORIZONTAL,
	OS_CURSOR_PAN_HOVER,
	OS_CURSOR_PAN_DRAG,
} OSCursorStyle;

typedef struct OSSpinlock {
	volatile uint8_t state;
} OSSpinlock;

typedef struct OSMutex {
	OSHandle event;
	OSSpinlock spinlock;
	volatile uint8_t state;
	volatile uint32_t queued;
} OSMutex;

typedef struct OSString {
	char *buffer;
	size_t bytes, characters;
} OSString;

typedef struct OSCaret {
	uintptr_t byte, character;
} OSCaret;

typedef struct OSCrashReason {
	OSError errorCode;
} OSCrashReason;

typedef struct OSProcessState {
	OSCrashReason crashReason;
	void *creationArgument;
	uintptr_t id;
#define OS_PROCESS_EXECUTABLE_NOT_LOADED 0
#define OS_PROCESS_EXECUTABLE_FAILED_TO_LOAD 1
#define OS_PROCESS_EXECUTABLE_LOADED 2
	uintptr_t executableState;
	uint8_t allThreadsTerminated : 1, terminating : 1, crashed : 1;
} OSProcessState;

typedef struct OSIORequestProgress {
	uint64_t accessed;
	uint64_t progress; 
	bool completed, cancelled;
	OSError error;
} OSIORequestProgress;

typedef enum OSClipboardFormat {
	OS_CLIPBOARD_FORMAT_EMPTY,
	OS_CLIPBOARD_FORMAT_TEXT,
} OSClipboardFormat;

typedef struct OSClipboardHeader {
	size_t customBytes;
	OSClipboardFormat format;
	size_t textBytes;
	uintptr_t unused;
} OSClipboardHeader;

typedef enum OSButtonStyle {
	OS_BUTTON_STYLE_NORMAL,
	OS_BUTTON_STYLE_REPEAT,
	OS_BUTTON_STYLE_TOOLBAR,
	OS_BUTTON_STYLE_TOOLBAR_ICON_ONLY,
} OSButtonStyle;

typedef enum OSTextboxStyle {
	OS_TEXTBOX_STYLE_NORMAL,
	OS_TEXTBOX_STYLE_COMMAND,
	OS_TEXTBOX_STYLE_LARGE,
} OSTextboxStyle;

typedef enum OSGridStyle {
	OS_GRID_STYLE_GROUP_BOX,
	OS_GRID_STYLE_MENU,
	OS_GRID_STYLE_MENUBAR,
	OS_GRID_STYLE_LAYOUT,
	OS_GRID_STYLE_CONTAINER,
	OS_GRID_STYLE_CONTAINER_WITHOUT_BORDER,
	OS_GRID_STYLE_CONTAINER_ALT,
	OS_GRID_STYLE_STATUS_BAR,
	OS_GRID_STYLE_TOOLBAR,
	OS_GRID_STYLE_TOOLBAR_ALT,
	OS_GRID_STYLE_BLANK_MENU,
} OSGridStyle;

typedef enum OSMessageType {
	// GUI messages:

	OS_MESSAGE_LAYOUT			= 0x0200,
	OS_MESSAGE_LAYOUT_TEXT			= 0x0201,
	OS_MESSAGE_MEASURE			= 0x0202,
	OS_MESSAGE_PAINT			= 0x0203,
	OS_MESSAGE_PAINT_BACKGROUND		= 0x0204,
	OS_MESSAGE_CUSTOM_PAINT			= 0x0205,
	OS_MESSAGE_CHECK_LAYOUT			= 0x0206, // Return OS_CALLBACK_REJECTED to relayout after modifications.

	OS_MESSAGE_DESTROY			= 0x0210,
	OS_MESSAGE_PARENT_UPDATED		= 0x0211,
	OS_MESSAGE_CHILD_UPDATED		= 0x0212,
	OS_MESSAGE_TEXT_UPDATED			= 0x0213,
	OS_MESSAGE_HIT_TEST			= 0x0214,
	OS_MESSAGE_CARET_BLINK			= 0x0215,

	OS_MESSAGE_CLICKED			= 0x0240,
	OS_MESSAGE_START_HOVER			= 0x0241,
	OS_MESSAGE_END_HOVER			= 0x0242,
	OS_MESSAGE_START_PRESS			= 0x0243,
	OS_MESSAGE_END_PRESS			= 0x0244,
	OS_MESSAGE_START_DRAG			= 0x0245,
	OS_MESSAGE_START_FOCUS			= 0x0246,
	OS_MESSAGE_END_FOCUS			= 0x0247,
	OS_MESSAGE_END_LAST_FOCUS		= 0x0248,
	OS_MESSAGE_MOUSE_DRAGGED		= 0x0249,
	OS_MESSAGE_KEY_TYPED			= 0x024A,
	OS_MESSAGE_DISABLE			= 0x024B,

	// Window manager messages:
	OS_MESSAGE_MOUSE_MOVED 			= 0x1000,
	OS_MESSAGE_MOUSE_LEFT_PRESSED 		= 0x1001,
	OS_MESSAGE_MOUSE_LEFT_RELEASED 		= 0x1002,
	OS_MESSAGE_KEY_PRESSED			= 0x1003,
	OS_MESSAGE_KEY_RELEASED			= 0x1004,
	OS_MESSAGE_WINDOW_CREATED 		= 0x1005,
	OS_MESSAGE_WM_TIMER	 		= 0x1006, 
	OS_MESSAGE_WINDOW_ACTIVATED		= 0x1007,
	OS_MESSAGE_WINDOW_DEACTIVATED		= 0x1008,
	OS_MESSAGE_WINDOW_DESTROYED 		= 0x1009,
	OS_MESSAGE_MOUSE_EXIT			= 0x100A, // Sent when the mouse leaves the window's bounds.
	OS_MESSAGE_MOUSE_ENTER			= 0x100B, 
	OS_MESSAGE_MOUSE_RIGHT_PRESSED 		= 0x100C,
	OS_MESSAGE_MOUSE_RIGHT_RELEASED 	= 0x100D,
	OS_MESSAGE_MOUSE_MIDDLE_PRESSED 	= 0x100E,
	OS_MESSAGE_MOUSE_MIDDLE_RELEASED 	= 0x100F,
	OS_MESSAGE_MODAL_PARENT_CLICKED		= 0x1010,
	OS_MESSAGE_UPDATE_WINDOW		= 0x1011,
	OS_MESSAGE_CLICK_REPEAT			= 0x1012,

	// Notifications:
	OS_NOTIFICATION_COMMAND			= 0x2000,
	OS_NOTIFICATION_VALUE_CHANGED		= 0x2001,
	OS_NOTIFICATION_GET_ITEM		= 0x2002,
	OS_NOTIFICATION_SET_ITEM		= 0x2004,
	OS_NOTIFICATION_START_EDIT		= 0x2005,
	OS_NOTIFICATION_END_EDIT		= 0x2006,
	OS_NOTIFICATION_CANCEL_EDIT		= 0x2007,
	OS_NOTIFICATION_CONFIRM_EDIT		= 0x2008,
	OS_NOTIFICATION_CHOOSE_ITEM		= 0x2009,
	OS_NOTIFICATION_CONVERT_Y_TO_INDEX	= 0x200A,
	OS_NOTIFICATION_MEASURE_HEIGHT		= 0x200B,
	OS_NOTIFICATION_PAINT_ITEM		= 0x200C,
	OS_NOTIFICATION_PAINT_CELL		= 0x200D,
	OS_NOTIFICATION_SET_ITEM_RANGE		= 0x200E,
	OS_NOTIFICATION_SORT_COLUMN		= 0x200F,
	OS_NOTIFICATION_RIGHT_CLICK		= 0x2010,
	OS_NOTIFICATION_DIALOG_CLOSE		= 0x2011,

	// Desktop messages:
	OS_MESSAGE_EXECUTE_PROGRAM		= 0x4800,
	OS_MESSAGE_POWER_BUTTON_PRESSED		= 0x4801,

	// Instance messages:
	OS_MESSAGE_CREATE_INSTANCE		= 0x4A00,
	OS_MESSAGE_PROCESS_STARTED		= 0x4A01,

	// Debugger messages:
	OS_MESSAGE_PROGRAM_CRASH		= 0x4C00,
	OS_MESSAGE_PROGRAM_FAILED_TO_START	= 0x4C01,

	// Misc messages:
	OS_MESSAGE_CLIPBOARD_UPDATED		= 0x5001,
	OS_MESSAGE_SET_PROPERTY			= 0x5002,

	// User messages:
	OS_MESSAGE_USER_START			= 0x8000,
	OS_MESSAGE_USER_END			= 0xBFFF,
} OSMessageType;

typedef struct OSMessage {
	OSMessageType type;
	void *context;

	union {
		void *argument;

		struct {
			int oldPositionX;
			int newPositionX;
			int oldPositionY;
			int newPositionY;
			int newPositionXScreen;
			int newPositionYScreen;
		} mouseMoved;

		struct {
			// Structure must match `mouseMoved`.
			int originalPositionX;
			int newPositionX;
			int originalPositionY;
			int newPositionY;
			int newPositionXScreen;
			int newPositionYScreen;
		} mouseDragged;

		struct {
			int positionX;
			int positionY;
			int positionXScreen;
			int positionYScreen;
			uint8_t clickChainCount;
			uint8_t activationClick : 1;
			uint8_t alt : 1, ctrl : 1, shift : 1;
		} mousePressed;

		struct {
			int positionX;
			int positionY;
			int positionXScreen;
			int positionYScreen;
		} mouseEntered;

		struct {
			unsigned scancode; 
			bool alt, ctrl, shift;
			OSObject notHandledBy; // Used in tab traversal.
		} keyboard;

		struct {
			OSCrashReason reason;
			OSHandle process;
			OSHandle processNameBuffer;
			size_t processNameBytes;
			uintptr_t pid;
		} crash;

		struct {
			int left, right, top, bottom;
			OSRectangle clip;
			bool force;
		} layout;

		struct {
			int *widths, *heights;
		} checkLayout;

		struct {
			int parentWidth, parentHeight;
			int preferredWidth, preferredHeight;
		} measure;

		struct {
			OSHandle surface;
			OSRectangle clip;
			bool force;
		} paint;

		struct {
			OSHandle surface;
			OSRectangle clip;
		} paintBackground;

		struct {
			OSObject window;
		} parentUpdated;

		struct {
			int positionX, positionY;
			bool result;
		} hitTest;

		struct {
			OSObject window;
			struct OSCommand *command;
			bool checked;
		} command;

		struct {
			int newValue;
		} valueChanged;

#define OS_LIST_VIEW_ITEM_SELECTED	   (0x0001)
#define OS_LIST_VIEW_ITEM_CUSTOM	   (0x0002)
#define OS_LIST_VIEW_ITEM_SELECTED_2	   (0x0004) // Selected by the selection box; cleared when the mouse is released.
#define OS_LIST_VIEW_ITEM_TEXT             (0x10000)
#define OS_LIST_VIEW_ITEM_ICON		   (0x20000)
#define OS_LIST_VIEW_ITEM_WIDTH		   (0x40000)
#define OS_LIST_VIEW_ITEM_HEIGHT	   (0x80000)
#define OS_LIST_VIEW_ITEM_HEIGHT_DEFAULT   (-1)
		struct {
			char *text; 
			size_t textBytes;
			uint32_t mask;
			int32_t index, column;
			uint16_t iconID, state;
			int16_t width, height;
		} listViewItem;

		struct {
			int32_t indexFrom, indexTo;
			uint16_t state, mask;
		} listViewItemRange;

		struct {
			int32_t index;
			bool descending;
		} listViewColumn;

		struct {
			int y;
			uintptr_t index;
			int offset;

			int knownY;
			uintptr_t knownIndex;
		} convertYToIndex;

		struct {
			uintptr_t fromIndex, toIndex;
			int height;
		} measureHeight;

		struct {
			OSHandle surface;
			OSRectangle bounds;
			OSRectangle clip;
			int32_t index;
		} paintItem;

		struct {
			OSHandle surface;
			OSRectangle bounds;
			OSRectangle clip;
			int32_t index, column;
		} paintCell;

		struct {
			uintptr_t index;
			void *value;
		} setProperty;

		struct {
			bool disabled;
		} disable;

		struct OSClipboardHeader clipboard;

		struct {
			OSHandle nameBuffer;
			size_t nameBytes;
		} executeProgram;
	};
} OSMessage;

// Determines how the image is scaled.
typedef enum OSDrawMode {
	OS_DRAW_MODE_REPEAT_FIRST = 0, // The first non-border pixel is repeated.
	OS_DRAW_MODE_STRECH, 
	OS_DRAW_MODE_REPEAT,
} OSDrawMode;

typedef void (*OSThreadEntryFunction)(void *argument);

typedef struct OSDebuggerMessage {
	OSHandle process;
	OSCrashReason reason;
} OSDebuggerMessage;

typedef int OSCallbackResponse;
typedef OSCallbackResponse (*OSCallbackFunction)(OSObject object, OSMessage *);

typedef struct OSCallback {
	OSCallbackFunction function;
	void *context;
} OSCallback;

typedef struct OSMenuItem {
	enum {
		COMMAND, SUBMENU, SEPARATOR,
	} type;

	void *value;
} OSMenuItem;

typedef struct OSMenuSpecification {
	char *name;
	size_t nameBytes;

	int minimumWidth, minimumHeight;

	OSMenuItem *items;
	size_t itemCount;
} OSMenuSpecification;

typedef struct OSCommand {
#define OS_COMMAND_DYNAMIC (-1)
	int32_t identifier;

	char *label;
	size_t labelBytes;

	char *shortcut;
	size_t shortcutBytes;

	char *access;
	size_t accessBytes;

	char *radio;
	size_t radioBytes;

	uint8_t checkable : 1, 
		defaultCheck : 1, 
		defaultDisabled : 1,
		radioCheck : 1,
		dangerous : 1;

	uint16_t iconID;

	OSCallback callback;
} OSCommand;

typedef struct OSWindowSpecification {
	unsigned width, height;
	unsigned minimumWidth, minimumHeight;

	unsigned flags;

	char *title;
	size_t titleBytes;

	OSMenuSpecification *menubar;
	OSCommand *defaultCommand;
} OSWindowSpecification;

typedef struct OSListViewColumn {
	char *title;
	size_t titleBytes;

#define OS_LIST_VIEW_COLUMN_DEFAULT_WIDTH_PRIMARY (300)
#define OS_LIST_VIEW_COLUMN_DEFAULT_WIDTH_SECONDARY (150)
	int width;
	int minimumWidth;

#define OS_LIST_VIEW_COLUMN_PRIMARY (1)
#define OS_LIST_VIEW_COLUMN_RIGHT_ALIGNED (2)
#define OS_LIST_VIEW_COLUMN_ICON (4)
#define OS_LIST_VIEW_COLUMN_SORT_ASCENDING (8)
#define OS_LIST_VIEW_COLUMN_SORT_DESCENDING (16)
	uint32_t flags;
} OSListViewColumn;

#define OS_CALLBACK_NOT_HANDLED (-1)
#define OS_CALLBACK_HANDLED (0)
#define OS_CALLBACK_REJECTED (-2)

extern OSObject osSystemMessages;

#define OS_CREATE_WINDOW_MENU (2)
#define OS_CREATE_WINDOW_NORMAL (4)
#define OS_CREATE_WINDOW_WITH_MENUBAR (8)
#define OS_CREATE_WINDOW_DIALOG (16)

#define OS_ORIENTATION_HORIZONTAL (false)
#define OS_ORIENTATION_VERTICAL   (true)

#define OS_CELL_H_PUSH          (0x0001)
#define OS_CELL_H_EXPAND        (0x0002)
#define OS_CELL_H_LEFT          (0x0004)
#define OS_CELL_H_CENTER        (0x0008)
#define OS_CELL_H_RIGHT         (0x0010)
#define OS_CELL_V_PUSH          (0x0100)
#define OS_CELL_V_EXPAND        (0x0200)
#define OS_CELL_V_TOP           (0x0400)
#define OS_CELL_V_CENTER        (0x0800)
#define OS_CELL_V_BOTTOM        (0x1000)

// Some common layouts...
#define OS_CELL_FILL	  (OS_CELL_H_PUSH | OS_CELL_H_EXPAND | OS_CELL_V_PUSH | OS_CELL_V_EXPAND)
#define OS_CELL_CENTER	  (OS_CELL_H_CENTER | OS_CELL_V_CENTER)
#define OS_CELL_EXPAND    (OS_CELL_H_EXPAND | OS_CELL_V_EXPAND)
#define OS_CELL_CORNER	  (OS_CELL_H_LEFT | OS_CELL_V_TOP)

#define OS_CREATE_SCROLL_PANE_VERTICAL      (1)
#define OS_CREATE_SCROLL_PANE_HORIZONTAL    (2)

#define OS_CREATE_LIST_VIEW_SINGLE_SELECT   (2)
#define OS_CREATE_LIST_VIEW_MULTI_SELECT    (4) // Requires OS_CREATE_LIST_VIEW_CONSTANT_HEIGHT.
#define OS_CREATE_LIST_VIEW_ANY_SELECTIONS  (OS_CREATE_LIST_VIEW_SINGLE_SELECT | OS_CREATE_LIST_VIEW_MULTI_SELECT)
#define OS_CREATE_LIST_VIEW_CONSTANT_HEIGHT (16)
#define OS_CREATE_LIST_VIEW_BORDER	    (32)
#define OS_CREATE_LIST_VIEW_SORTABLE        (64)

#define OS_SHARED_MEMORY_MAXIMUM_SIZE ((size_t) 1024 * 1024 * 1024 * 1024)
#define OS_SHARED_MEMORY_NAME_MAX_LENGTH (32)
#define OS_SHARED_MEMORY_MAP_ALL (0)
#define OS_MAP_OBJECT_ALL (0)

#define OS_GUI_FONT_REGULAR ((char *) "Shell/Font/RegularGUI")

#define OS_DRAW_STRING_HALIGN_LEFT 	(1)
#define OS_DRAW_STRING_HALIGN_RIGHT 	(2)
#define OS_DRAW_STRING_HALIGN_CENTER 	(OS_DRAW_STRING_HALIGN_LEFT | OS_DRAW_STRING_HALIGN_RIGHT)
#define OS_DRAW_STRING_VALIGN_TOP 	(4)
#define OS_DRAW_STRING_VALIGN_BOTTOM 	(8)
#define OS_DRAW_STRING_VALIGN_CENTER 	(OS_DRAW_STRING_VALIGN_TOP | OS_DRAW_STRING_VALIGN_BOTTOM)
#define OS_DRAW_STRING_WORD_WRAP 	(16)

#define OS_OPEN_NODE_READ_NONE		(0x0)
#define OS_OPEN_NODE_READ_BLOCK		(0x1)
#define OS_OPEN_NODE_READ_ACCESS	(0x2)
#define OS_OPEN_NODE_READ_EXCLUSIVE	(0x3)
#define OS_OPEN_NODE_READ_MASK(x)	((x) & 0x7)

#define OS_OPEN_NODE_WRITE_NONE		(0x00)
#define OS_OPEN_NODE_WRITE_BLOCK	(0x10)
#define OS_OPEN_NODE_WRITE_ACCESS	(0x20)
#define OS_OPEN_NODE_WRITE_EXCLUSIVE	(0x30)
#define OS_OPEN_NODE_WRITE_MASK(x)	((x) & 0x70)

#define OS_OPEN_NODE_RESIZE_NONE	(0x000)
#define OS_OPEN_NODE_RESIZE_BLOCK	(0x100)
#define OS_OPEN_NODE_RESIZE_ACCESS	(0x200)
#define OS_OPEN_NODE_RESIZE_EXCLUSIVE	(0x300)
#define OS_OPEN_NODE_RESIZE_MASK(x)	((x) & 0x700)

#define OS_OPEN_NODE_FAIL_IF_FOUND	(0x1000)
#define OS_OPEN_NODE_FAIL_IF_NOT_FOUND	(0x2000)
#define OS_OPEN_NODE_DIRECTORY		(0x4000)
#define OS_OPEN_NODE_CREATE_DIRECTORIES	(0x8000) // Create the directories leading to the file, if they don't already exist.

#define OS_OPEN_SHARED_MEMORY_FAIL_IF_FOUND     (0x1000)
#define OS_OPEN_SHARED_MEMORY_FAIL_IF_NOT_FOUND (0x2000)

#define OS_MAP_OBJECT_READ_WRITE        (0)
#define OS_MAP_OBJECT_READ_ONLY         (1)
#define OS_MAP_OBJECT_COPY_ON_WRITE     (2)

#define OS_CREATE_MENUBAR (1)
#define OS_CREATE_SUBMENU (2)
#define OS_CREATE_MENU_BLANK (4)
#define OS_CREATE_MENU_FROM_MENUBAR (8)
#define OS_CREATE_MENU_AT_SOURCE (OS_MAKE_POINT(-1, -1))
#define OS_CREATE_MENU_AT_CURSOR (OS_MAKE_POINT(-2, -1))

#define OS_SLIDER_MODE_HORIZONTAL       (1)
#define OS_SLIDER_MODE_VERTICAL         (2)
#define OS_SLIDER_MODE_TICKS_LEFT       (4)
#define OS_SLIDER_MODE_TICKS_ABOVE      (4)
#define OS_SLIDER_MODE_TICKS_RIGHT      (8)
#define OS_SLIDER_MODE_TICKS_BENEATH    (8)
#define OS_SLIDER_MODE_TICKS_BOTH_SIDES (12)
#define OS_SLIDER_MODE_SNAP_TO_TICKS	(32)
#define OS_SLIDER_MODE_OPPOSITE_VALUE   (64)

#define OS_BLANK_CONTROL_DRAW_PARENT_BACKGROUND   (1)
#define OS_BLANK_CONTROL_IGNORE_ACTIVATION_CLICKS (2)
#define OS_BLANK_CONTROL_FOCUSABLE                (4)
#define OS_BLANK_CONTROL_TAB_STOP                 (8)

OS_EXTERN_C void OSInitialiseAPI();

OS_EXTERN_C void OSBatch(OSBatchCall *calls, size_t count); 

OS_EXTERN_C OSError OSCreateProcess(const char *executablePath, size_t executablePathLength, OSProcessInformation *information, void *argument);
OS_EXTERN_C OSError OSCreateThread(OSThreadEntryFunction entryFunction, OSThreadInformation *information, void *argument);
OS_EXTERN_C OSHandle OSCreateSurface(size_t width, size_t height);
OS_EXTERN_C OSHandle OSCreateGlobalMutex();
OS_EXTERN_C OSHandle OSCreateEvent(bool autoReset);

#define OS_MAX_PROGRAM_NAME_LENGTH (256)
OS_EXTERN_C void OSExecuteProgram(const char *name, size_t nameBytes);
OS_EXTERN_C void OSReadConstantBuffer(OSHandle constantBuffer, void *output);

OS_EXTERN_C OSError OSCloseHandle(OSHandle handle);

OS_EXTERN_C OSError OSOpenNode(char *path, size_t pathLength, uint64_t flags, OSNodeInformation *information);
OS_EXTERN_C void *OSReadEntireFile(const char *filePath, size_t filePathLength, size_t *fileSize); 
OS_EXTERN_C size_t OSReadFileSync(OSHandle file, uint64_t offset, size_t size, void *buffer); // If return value >= 0, number of bytes read. Otherwise, OSError.
OS_EXTERN_C size_t OSWriteFileSync(OSHandle file, uint64_t offset, size_t size, void *buffer); // If return value >= 0, number of bytes written. Otherwise, OSError.
OS_EXTERN_C OSHandle OSReadFileAsync(OSHandle file, uint64_t offset, size_t size, void *buffer); 
OS_EXTERN_C OSHandle OSWriteFileAsync(OSHandle file, uint64_t offset, size_t size, void *buffer); // TODO Message on completion.
OS_EXTERN_C OSError OSResizeFile(OSHandle file, uint64_t newSize); 
OS_EXTERN_C void OSRefreshNodeInformation(OSNodeInformation *information);
OS_EXTERN_C OSError OSEnumerateDirectoryChildren(OSHandle directory, OSDirectoryChild *buffer, size_t bufferCount);
OS_EXTERN_C void OSGetIORequestProgress(OSHandle ioRequest, OSIORequestProgress *buffer);
OS_EXTERN_C void OSCancelIORequest(OSHandle ioRequest);
OS_EXTERN_C OSError OSDeleteNode(OSHandle node); // Directories must be empty (error = OS_ERROR_DIRECTORY_NOT_EMPTY).
OS_EXTERN_C OSError OSMoveNode(OSHandle node, OSHandle newDirectory, char *newName, size_t newNameLength); // Does not handle the following cases (yet): moving between volumes, replacing existing files.
#define OSRenameNode(_node, _newName, _newNameLength) OSMoveNode(_node, OS_INVALID_HANDLE, _newName, _newNameLength)

OS_EXTERN_C OSError OSTerminateThread(OSHandle thread);
OS_EXTERN_C OSError OSTerminateProcess(OSHandle thread);
OS_EXTERN_C OSError OSTerminateThisProcess();

OS_EXTERN_C void OSPauseProcess(OSHandle process, bool resume);
OS_EXTERN_C void OSCrashProcess(OSError error);

OS_EXTERN_C uintptr_t OSGetThreadID(OSHandle thread);

OS_EXTERN_C void OSReleaseGlobalMutex(OSHandle mutex);
OS_EXTERN_C void OSAcquireGlobalMutex(OSHandle mutex);
OS_EXTERN_C void OSAcquireMultipleGlobalMutexes(OSHandle *mutexes, size_t count);
OS_EXTERN_C void OSReleaseSpinlock(OSSpinlock *spinlock);
OS_EXTERN_C void OSAcquireSpinlock(OSSpinlock *spinlock);
OS_EXTERN_C void OSReleaseMutex(OSMutex *mutex);
OS_EXTERN_C void OSAcquireMutex(OSMutex *mutex);
OS_EXTERN_C void OSDestroyMutex(OSMutex *mutex);

OS_EXTERN_C void OSYieldScheduler();

OS_EXTERN_C OSError OSSetEvent(OSHandle event);
OS_EXTERN_C OSError OSResetEvent(OSHandle event);
OS_EXTERN_C OSError OSPollEvent(OSHandle event);

OS_EXTERN_C uintptr_t OSWait(OSHandle *objects, size_t objectCount, uintptr_t timeoutMs);
#define OSWaitSingle(object) OSWait(&object, 1, OS_WAIT_NO_TIMEOUT)

OS_EXTERN_C OSHandle OSOpenSharedMemory(size_t size, char *name, size_t nameLength, unsigned flags);
OS_EXTERN_C OSHandle OSShareMemory(OSHandle sharedMemoryRegion, OSHandle targetProcess, bool readOnly);
OS_EXTERN_C void *OSMapObject(OSHandle object, uintptr_t offset, size_t size, unsigned flags);

OS_EXTERN_C void *OSAllocate(size_t size);
OS_EXTERN_C OSError OSFree(void *address);

OS_EXTERN_C void *OSGetCreationArgument(OSHandle object);
OS_EXTERN_C void OSGetProcessState(OSHandle process, OSProcessState *state);

OS_EXTERN_C OSError OSGetLinearBuffer(OSHandle surface, OSLinearBuffer *linearBuffer);
OS_EXTERN_C OSError OSInvalidateRectangle(OSHandle surface, OSRectangle rectangle);
OS_EXTERN_C OSError OSCopyToScreen(OSHandle source, OSPoint point, uint16_t depth);
OS_EXTERN_C OSError OSForceScreenUpdate();
OS_EXTERN_C OSError OSFillRectangle(OSHandle surface, OSRectangle rectangle, OSColor color);
OS_EXTERN_C OSError OSCopySurface(OSHandle destination, OSHandle source, OSPoint destinationPoint);
OS_EXTERN_C OSError OSDrawSurface(OSHandle destination, OSHandle source, OSRectangle destinationRegion, OSRectangle sourceRegion, OSRectangle borderRegion, OSDrawMode mode, uint8_t alpha);
OS_EXTERN_C OSError OSDrawSurfaceClipped(OSHandle destination, OSHandle source, OSRectangle destinationRegion, OSRectangle sourceRegion, OSRectangle borderRegion, OSDrawMode mode, uint8_t alpha, OSRectangle clipRegion);
OS_EXTERN_C OSError OSClearModifiedRegion(OSHandle surface);
OS_EXTERN_C OSError OSDrawString(OSHandle surface, OSRectangle region, OSString *string, int fontSize, unsigned flags, uint32_t color, int32_t backgroundColor, bool bold, OSRectangle clipRegion, int blur);
OS_EXTERN_C OSError OSFindCharacterAtCoordinate(OSRectangle region, OSPoint coordinate, OSString *string, unsigned flags, OSCaret *position, int fontSize, int scrollX);
OS_EXTERN_C void OSDrawProgressBar(OSHandle surface, OSRectangle bounds, float progress, OSRectangle clip, bool blue);

// You shouldn't need to call either of these...
OS_EXTERN_C void OSRedrawAll();
OS_EXTERN_C void OSInitialiseGUI();

#define OS_GRID_PROPERTY_BORDER_SIZE (1)
#define OS_GRID_PROPERTY_GAP_SIZE (2)
#define OS_GUI_OBJECT_PROPERTY_SUGGESTED_WIDTH (3)
#define OS_GUI_OBJECT_PROPERTY_SUGGESTED_HEIGHT (4)
#define OS_CONTROL_PROPERTY_CURSOR (5)
OS_EXTERN_C void OSSetProperty(OSObject object, uintptr_t index, void *value);

OS_EXTERN_C void OSSetCursor(OSObject window, OSCursorStyle cursor);

OS_EXTERN_C OSError OSPostMessage(OSMessage *message);
OS_EXTERN_C OSError OSPostMessageRemote(OSHandle process, OSMessage *message);
OS_EXTERN_C OSCallbackResponse OSSendMessage(OSObject target, OSMessage *message);
OS_EXTERN_C OSCallbackResponse OSForwardMessage(OSObject target, OSCallback callback, OSMessage *message);
OS_EXTERN_C OSCallback OSSetCallback(OSObject generator, OSCallback callback); // Returns old callback.
OS_EXTERN_C void OSProcessMessages();

#define OS_RESIZE_MODE_IGNORE (0)
#define OS_RESIZE_MODE_GROW_ONLY (1)
#define OS_RESIZE_MODE_EXACT (2)

OS_EXTERN_C void OSGetText(OSObject control, OSString *string);
OS_EXTERN_C void OSSetText(OSObject control, char *text, size_t textBytes, unsigned resizeMode);
OS_EXTERN_C void OSDisableControl(OSObject control, bool disabled);
#define OSEnableControl(_control, _enabled) OSDisableControl((_control), !(_enabled))
OS_EXTERN_C void OSDisableCommand(OSObject window, OSCommand *command, bool disabled);
OS_EXTERN_C void OSCheckCommand(OSObject window, OSCommand *command, bool checked);
#define OSEnableCommand(_window, _command, _enabled) OSDisableCommand((_window), (_command), !(_enabled))
OS_EXTERN_C void OSSetCommandNotificationCallback(OSObject _window, OSCommand *_command, OSCallback callback);
OS_EXTERN_C void OSSetObjectNotificationCallback(OSObject object, OSCallback callback);
OS_EXTERN_C void OSSetControlCommand(OSObject control, OSCommand *command);
OS_EXTERN_C void OSIssueCommand(OSCommand *command, OSObject window);

OS_EXTERN_C void OSSetInstance(OSObject window, void *instance);
OS_EXTERN_C void *OSGetInstance(OSObject window);
OS_EXTERN_C void *OSGetInstanceFromControl(OSObject object);

OS_EXTERN_C void OSDebugGUIObject(OSObject guiObject);

OS_EXTERN_C OSObject OSCreateMenu(OSMenuSpecification *menuSpecification, OSObject sourceControl, OSPoint position, unsigned flags);
OS_EXTERN_C OSObject OSCreateWindow(OSWindowSpecification *specification);
OS_EXTERN_C OSObject OSCreateGrid(unsigned columns, unsigned rows, OSGridStyle style);
OS_EXTERN_C OSObject OSCreateScrollPane(OSObject content, unsigned flags);
OS_EXTERN_C void OSAddControl(OSObject grid, unsigned column, unsigned row, OSObject control, unsigned layout);
#define OSAddGrid(_grid, _column, _row, _child, _layout) OSAddControl(_grid, _column, _row, _child, _layout)
#define OSSetRootGrid(_window, _grid) OSAddControl(_window, 0, 0, _grid, OS_CELL_FILL)

OS_EXTERN_C void OSStartGUIAllocationBlock(size_t bytes);
OS_EXTERN_C size_t OSEndGUIAllocationBlock();

OS_EXTERN_C OSObject OSShowDialogAlert(char *title, size_t titleBytes,
				   char *message, size_t messageBytes,
				   char *description, size_t descriptionBytes,
				   uint16_t iconID, OSObject modalParent);
OS_EXTERN_C OSObject OSShowDialogConfirm(char *title, size_t titleBytes,
				   char *message, size_t messageBytes,
				   char *description, size_t descriptionBytes,
				   uint16_t iconID, OSObject modalParent,
				   OSCommand *command);

OS_EXTERN_C void OSCloseWindow(OSObject window);
OS_EXTERN_C void OSSetFocusedWindow(OSObject window);

OS_EXTERN_C void OSGetMousePosition(OSObject relativeWindow, OSPoint *position);
OS_EXTERN_C OSRectangle OSGetControlBounds(OSObject control);
OS_EXTERN_C void OSRepaintControl(OSObject object);
OS_EXTERN_C OSObject OSGetWindow(OSObject object);

OS_EXTERN_C OSObject OSCreateBlankControl(int width, int height, OSCursorStyle cursor, unsigned flags);
OS_EXTERN_C OSObject OSCreateLine(bool orientation);
OS_EXTERN_C OSObject OSCreateButton(OSCommand *command, OSButtonStyle style);
OS_EXTERN_C OSObject OSCreateTextbox(OSTextboxStyle style);
OS_EXTERN_C OSObject OSCreateLabel(char *label, size_t labelBytes, bool wordWrap);
OS_EXTERN_C OSObject OSCreateIconDisplay(uint16_t iconID);
OS_EXTERN_C OSObject OSCreateProgressBar(int minimum, int maximum, int initialValue, bool small);
OS_EXTERN_C OSObject OSCreateScrollbar(bool orientation, bool automaticallyUpdatePosition);
OS_EXTERN_C OSObject OSCreateListView(unsigned flags, int constantHeight);
OS_EXTERN_C OSObject OSCreateSlider(int minimum, int maximum, int initialValue, int mode, int minorTickSpacing, int majorTickSpacing);

#define OSCreateIndeterminateProgressBar(small) OSCreateProgressBar(0, 0, 0, small)
#define OSCreateSpacer(w, h) OSCreateBlankControl(0, 0, OS_CURSOR_NORMAL, OS_FLAGS_DEFAULT)

OS_EXTERN_C void OSSetFocusedControl(OSObject control, bool asDefaultForWindow);
OS_EXTERN_C void OSRemoveFocusedControl(OSObject window, bool removeWeakFocus);
OS_EXTERN_C OSObject OSGetFocusedControl(OSObject window, bool ignoreWeakFocus);

OS_EXTERN_C void OSSetProgressBarValue(OSObject control, int newValue);

OS_EXTERN_C void OSSetSliderPosition(OSObject slider, int position, bool sendValueChangedNotification);
OS_EXTERN_C int OSGetSliderPosition(OSObject slider);

OS_EXTERN_C void OSListViewInsert(OSObject listView, uintptr_t index, size_t count);
OS_EXTERN_C void OSListViewRemove(OSObject listView, uintptr_t index, size_t count);
OS_EXTERN_C void OSListViewReset(OSObject listView);
OS_EXTERN_C void OSListViewInvalidate(OSObject listView, uintptr_t index, size_t count);
OS_EXTERN_C void OSListViewSetColumns(OSObject listView, OSListViewColumn *columns, size_t count);

OS_EXTERN_C void OSSetScrollbarMeasurements(OSObject _scrollbar, int contentSize, int viewportSize);
OS_EXTERN_C void OSSetScrollbarPosition(OSObject _scrollbar, int position, bool sendValueChangedNotification);
OS_EXTERN_C int OSGetScrollbarPosition(OSObject _scrollbar);

#ifndef KERNEL
OS_EXTERN_C void *OSHeapAllocate(size_t size, bool zeroMemory);
OS_EXTERN_C void OSHeapFree(void *address);

OS_EXTERN_C size_t OSCStringLength(char *string);
OS_EXTERN_C void OSCopyMemory(void *destination, void *source, size_t bytes);
OS_EXTERN_C void OSMoveMemory(void *_start, void *_end, intptr_t amount, bool zeroEmptySpace);
OS_EXTERN_C void OSCopyMemoryReverse(void *_destination, void *_source, size_t bytes);
OS_EXTERN_C void OSZeroMemory(void *destination, size_t bytes);
OS_EXTERN_C int OSCompareBytes(void *a, void *b, size_t bytes);
OS_EXTERN_C uint8_t OSSumBytes(uint8_t *data, size_t bytes);
OS_EXTERN_C void OSPrint(const char *format, ...);
OS_EXTERN_C void OSPrintDirect(char *string, size_t stringLength);
OS_EXTERN_C size_t OSFormatString(char *buffer, size_t bufferLength, const char *format, ...);
OS_EXTERN_C void OSHelloWorld();
OS_EXTERN_C uint8_t OSGetRandomByte();
OS_EXTERN_C void OSSort(void *_base, size_t nmemb, size_t size, int (*compar)(const void *, const void *, void *), void *argument);
OS_EXTERN_C int OSCompareStrings(char *s1, char *s2, size_t length1, size_t length2);

OS_EXTERN_C int utf8_length_char(char *character);
OS_EXTERN_C int utf8_value(char *character);
OS_EXTERN_C int utf8_encode(int value, char *buffer);
OS_EXTERN_C char *utf8_advance(char *string);
OS_EXTERN_C char *utf8_retreat(char *string);
OS_EXTERN_C int utf8_length(char *string, int max_bytes);

// TODO Possibly remove all of these?
// 	Or move into a libc library?
#ifndef OS_NO_CSTDLIB
typedef volatile int sig_atomic_t;
typedef int64_t time_t;
typedef int64_t clock_t;
typedef int64_t off_t;

typedef struct lconv {
	char *decimal_point;
	char *thousands_sep;
	char *grouping;
	char *int_curr_symbol;
	char *currency_symbol;
	char *mon_decimal_point;
	char *mon_thousands_sep;
	char *mon_grouping;
	char *positive_sign;
	char *negative_sign;
	char frac_digits;
	char p_cs_precedes;
	char n_cs_precedes;
	char p_sep_by_space;
	char n_sep_by_space;
	char p_sign_posn;
	char n_sign_posn;
	char int_frac_digits;
	char int_p_cs_precedes;
	char int_n_cs_precedes;
	char int_p_sep_by_space;
	char int_n_sep_by_space;
	char int_p_sign_posn;
	char int_n_sign_posn;
} lconv;

typedef struct tm {
	int tm_sec;
	int tm_min;
	int tm_hour;
	int tm_mday;
	int tm_mon;
	int tm_year;
	int tm_wday;
	int tm_yday;
	int tm_isdst;
} tm;

struct stat {
	int64_t st_dev;
	int64_t st_ino;
	int64_t st_mode;
	int64_t st_nlink;
	int64_t st_uid;
	int64_t st_gid;
	int64_t st_rdev;
	int64_t st_size;
	int64_t st_blksize;
	int64_t st_blocks;
	time_t st_atime;
	time_t st_mtime;
	time_t st_ctime;
};

typedef struct FILE {
	OSNodeInformation node;

	uint64_t position;
	int ungotc;

	uint64_t seekToEndBeforeWrites : 1,
		 endOfFile : 1,
		 error : 1,
		 temporary : 1,
		 virtualFile : 1;
} FILE;

extern int errno;
extern FILE *stdin;
extern FILE *stdout;
extern FILE *stderr;

#define RAND_MAX (16777215)
#define EOF (-1)
#define BUFSIZ (2048)
#define SEEK_SET (1)
#define SEEK_CUR (2)
#define SEEK_END (4)
#define _IONBF (8)
#define _IOLBF (16)
#define _IOFBF (32)
#define HUGE_VAL (__builtin_huge_val())
#define INFINITY (__builtin_inff())
#define NAN (__builtin_nanf(""))
#define EXIT_SUCCESS (OS_CALLBACK_HANDLED)
#define EXIT_FAILURE (OS_CALLBACK_REJECTED)
#define CLOCKS_PER_SEC (1000)
#define L_tmpnam (16)
#define SIG_DFL (NULL)
#define SIGINT (2)
#define LC_ALL (1)
#define LC_COLLATE (2)
#define LC_CTYPE (4)
#define LC_MONETARY (8)
#define LC_NUMERIC (16)
#define LC_TIME (32)
#define PRIx8 "x"
#define PRIx16 "x"
#define PRId32 "d"
#define PRIu32 "u"
#define PRIx32 "x"
#define PRIX32 "X"
#define PRId64 "ld"
#define PRIu64 "lu"
#define PRIx64 "lx"
#define PRIX64 "lX"
#define PROT_READ (0x01)
#define PROT_WRITE (0x02)
#define PROT_EXEC (0x04)
#define PROT_NONE (0x08)
#define MAP_SHARED (0x10)
#define MAP_PRIVATE (0x20)
#define MAP_FIXED (0x40)
#define MAP_FAILED (NULL)

OS_EXTERN_C int isalnum(int c);
OS_EXTERN_C int iscntrl(int c);
OS_EXTERN_C int isgraph(int c);
OS_EXTERN_C int islower(int c);
OS_EXTERN_C int isprint(int c);
OS_EXTERN_C int ispunct(int c);
OS_EXTERN_C int isupper(int c);
OS_EXTERN_C int isxdigit(int c);
OS_EXTERN_C __attribute__((noreturn)) void abort();
OS_EXTERN_C double acos(double x);
OS_EXTERN_C double asin(double x);
OS_EXTERN_C double atan2(double y, double x);
OS_EXTERN_C double ceil(double x);
OS_EXTERN_C void clearerr(FILE *stream);
OS_EXTERN_C clock_t clock();
OS_EXTERN_C double cos(double x);
OS_EXTERN_C double cosh(double x);
OS_EXTERN_C double difftime(time_t t1, time_t t2);
OS_EXTERN_C void exit(int status);
OS_EXTERN_C double exp(double x);
OS_EXTERN_C int fclose(FILE *stream);
OS_EXTERN_C int feof(FILE *stream);
OS_EXTERN_C int ferror(FILE *stream);
OS_EXTERN_C int fflush(FILE *stream);
OS_EXTERN_C char *fgets(char *s, int size, FILE *stream);
OS_EXTERN_C double floor(double x);
OS_EXTERN_C double fmod(double x, double y);
OS_EXTERN_C FILE *fopen(const char *path, const char *mode);
OS_EXTERN_C int fprintf(FILE *stream, const char *format, ...);
OS_EXTERN_C int fputs(const char *s, FILE *stream);
OS_EXTERN_C size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream);
OS_EXTERN_C FILE *freopen(const char *path, const char *mode, FILE *stream);
OS_EXTERN_C double frexp(double x, int *exp);
OS_EXTERN_C int fseek(FILE *stream, long offset, int whence);
OS_EXTERN_C int fseeko(FILE *stream, off_t offset, int whence);
OS_EXTERN_C long ftell(FILE *stream);
OS_EXTERN_C off_t ftello(FILE *stream);
OS_EXTERN_C size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream);
#define getc fgetc
OS_EXTERN_C int fgetc(FILE *stream);
OS_EXTERN_C struct tm *gmtime(const time_t *time);
OS_EXTERN_C double ldexp(double x, int exp);
OS_EXTERN_C struct tm *localtime(const time_t *time);
OS_EXTERN_C double log(double x);
OS_EXTERN_C double log10(double x);
OS_EXTERN_C double log2(double x);
OS_EXTERN_C time_t mktime(struct tm *tm);
OS_EXTERN_C double pow(double x, double y);
OS_EXTERN_C int remove(const char *path);
OS_EXTERN_C int rename(const char *oldPath, const char *newPath);
OS_EXTERN_C char *setlocale(int category, const char *locale);
OS_EXTERN_C int setvbuf(FILE *stream, char *buf, int mode, size_t size);
OS_EXTERN_C void (*signal(int sig, void (*func)(int)))(int);
OS_EXTERN_C double sin(double x);
OS_EXTERN_C double sinh(double x);
OS_EXTERN_C int snprintf(char *str, size_t size, const char *format, ...);
OS_EXTERN_C int sprintf(char *str, const char *format, ...);
OS_EXTERN_C int printf(const char *format, ...);
OS_EXTERN_C void srand(unsigned int seed);
OS_EXTERN_C int strcoll(const char *s1, const char *s2);
OS_EXTERN_C char *strerror(int errnum);
OS_EXTERN_C size_t strftime(char *s, size_t max, const char *format, const struct tm *tm);
OS_EXTERN_C char *strpbrk(const char *s, const char *accept);
OS_EXTERN_C double strtod(const char *nptr, char **endptr);
OS_EXTERN_C int system(const char *command);
OS_EXTERN_C double tan(double x);
OS_EXTERN_C double tanh(double x);
OS_EXTERN_C time_t time(time_t *tloc);
OS_EXTERN_C FILE *tmpfile();
OS_EXTERN_C char *tmpnam(char *s);
OS_EXTERN_C void *memset(void *s, int c, size_t n);
OS_EXTERN_C void *memcpy(void *dest, const void *src, size_t n);
OS_EXTERN_C size_t strlen(const char *s);
OS_EXTERN_C void *malloc(size_t size);
OS_EXTERN_C void *calloc(size_t num, size_t size);
OS_EXTERN_C void *memmove(void *dest, const void *src, size_t n);
OS_EXTERN_C void free(void *ptr);
OS_EXTERN_C void *realloc(void *ptr, size_t size);
OS_EXTERN_C double fabs(double x);
OS_EXTERN_C int abs(int n);
OS_EXTERN_C int ifloor(double x);
OS_EXTERN_C int iceil(double x);
OS_EXTERN_C double sqrt(double x);
OS_EXTERN_C char *getenv(const char *name);
OS_EXTERN_C int strcmp(const char *s1, const char *s2);
OS_EXTERN_C int strcasecmp(const char *s1, const char *s2);
OS_EXTERN_C int strncmp(const char *s1, const char *s2, size_t n);
OS_EXTERN_C int strncasecmp(const char *s1, const char *s2, size_t n);
OS_EXTERN_C int toupper(int c);
OS_EXTERN_C int tolower(int c);
OS_EXTERN_C char *strncpy(char *dest, const char *src, size_t n);
OS_EXTERN_C long int strtol(const char *nptr, char **endptr, int base);
OS_EXTERN_C unsigned long int strtoul(const char *nptr, char **endptr, int base);
OS_EXTERN_C char *strcat(char *dest, const char *src);
OS_EXTERN_C char *strsep(char **stringp, const char *delim);
OS_EXTERN_C char *strstr(const char *haystack, const char *needle);
OS_EXTERN_C char *strrchr(const char *s, int c);
OS_EXTERN_C void qsort(void *base, size_t nmemb, size_t size, int (*compar)(const void *, const void *));
OS_EXTERN_C char *strcpy(char *dest, const char *src);
OS_EXTERN_C char *stpcpy(char *dest, const char *src);
OS_EXTERN_C char *strchr(const char *s, int c);
OS_EXTERN_C size_t strnlen(const char *s, size_t maxlen);
OS_EXTERN_C size_t strspn(const char *s, const char *accept);
OS_EXTERN_C size_t strcspn(const char *s, const char *reject);
OS_EXTERN_C int memcmp(const void *s1, const void *s2, size_t n);
OS_EXTERN_C void *memchr(const void *s, int c, size_t n);
OS_EXTERN_C int isspace(int c);
OS_EXTERN_C int isalpha(int c);
OS_EXTERN_C int isdigit(int c);
OS_EXTERN_C void OSAssertionFailure();
OS_EXTERN_C struct lconv *localeconv();
OS_EXTERN_C int rand();
OS_EXTERN_C int ungetc(int c, FILE *stream);
#define putc fputc
OS_EXTERN_C int fputc(int c, FILE *stream);
OS_EXTERN_C int puts(const char *string);
OS_EXTERN_C void perror(const char *string);
OS_EXTERN_C int vsnprintf(char *string, size_t size, const char *format, va_list arguments);
OS_EXTERN_C int ftruncate(int fildes, off_t length);
OS_EXTERN_C int fileno(FILE *stream);
OS_EXTERN_C int access(const char *path, int amode);
OS_EXTERN_C int fstat(int fildes, struct stat *buffer);
OS_EXTERN_C int stat(const char *path, struct stat *buffer);
OS_EXTERN_C int getpagesize();
OS_EXTERN_C void *mmap(void *address, size_t length, int protection, int flags, int fildes, off_t offset);
OS_EXTERN_C int munmap(void *address, size_t length);
OS_EXTERN_C char *canonicalize_file_name(const char *path);
OS_EXTERN_C int atoi(const char *nptr);
OS_EXTERN_C int sscanf(const char *string, const char *format, ...);

#define assert(x) do{if (!(x)) OSAssertionFailure();}while(0)

#ifdef ARCH_X86_64
#define OS_PAGE_SIZE (4096)
typedef struct jmp_buf { uintptr_t rsp, rbp, rbx, r12, r13, r14, r15, returnAddress; } jmp_buf;
#endif
OS_EXTERN_C int _setjmp(jmp_buf *env);
OS_EXTERN_C __attribute__((noreturn)) void _longjmp(jmp_buf *env, int val);
#define setjmp(x) _setjmp(&(x))
#define longjmp(x, y) _longjmp(&(x), (y))

#endif

#define STBI_NO_STDIO
#define STBI_ONLY_PNG
#define STBI_ONLY_JPEG
#define STBI_NO_LINEAR
#include "stb_image.h"

OS_EXTERN_C uint64_t osRandomByteSeed;
OS_EXTERN_C OSMutex osMessageMutex;
#endif

OS_EXTERN_C void ProgramEntry(void); 

#endif
