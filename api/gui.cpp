#include "../bin/OS/standard.manifest.h"

static void EnterDebugger() {
	asm volatile ("xchg %bx,%bx");
}

#define DIMENSION_PUSH (65535)

#define CARET_BLINK_HZ (1)
#define CARET_BLINK_PAUSE (2)

#define RESIZE_LEFT 		(1)
#define RESIZE_RIGHT 		(2)
#define RESIZE_TOP 		(4)
#define RESIZE_BOTTOM 		(8)
#define RESIZE_TOP_LEFT 	(5)
#define RESIZE_TOP_RIGHT 	(6)
#define RESIZE_BOTTOM_LEFT 	(9)
#define RESIZE_BOTTOM_RIGHT 	(10)
#define RESIZE_MOVE		(0)
#define RESIZE_NONE		(11)

#define SCROLLBAR_SIZE (17)
#define SCROLLBAR_MINIMUM (16)
#define SCROLLBAR_BUTTON_AMOUNT (64)

#define STANDARD_BORDER_SIZE (2)

#define ICON_TEXT_GAP (5)

uint32_t STANDARD_BACKGROUND_COLOR = 0xF5F6F9;

uint32_t TEXT_COLOR_DEFAULT = 0x000515;
uint32_t TEXT_COLOR_DISABLED = 0x777777;
uint32_t TEXT_COLOR_DISABLED_SHADOW = 0xEEEEEE;
uint32_t TEXT_COLOR_HEADING = 0x003296;
uint32_t TEXT_COLOR_TITLEBAR = 0xFFFFFF;
uint32_t TEXT_COLOR_TOOLBAR = 0xFFFFFF;

uint32_t TEXTBOX_SELECTED_COLOR_1 = 0xFFC4D9F9;
uint32_t TEXTBOX_SELECTED_COLOR_2 = 0xFFDDDDDD;

uint32_t DISABLE_TEXT_SHADOWS = 1;

// TODO Keyboard controls.
// 	- Access keys.
// 	- List view navigation.
// 	- Change "default command" to be a special case of dialog box and menu keyboard controls.
// TODO Scroll-selections in textboxes and list views.
// TODO Textboxes.
// 	- Undo/redo.
// 	- Multi-line.
// 	- Limit the length to 1024 characters?
// TODO Multiple-cell positions.
// TODO Wrapping.

struct UIImage {
	OSRectangle region;
	OSRectangle border;
	OSDrawMode drawMode;

	UIImage Translate(int x, int y) {
		UIImage a = *this;
		a.region.left += x;
		a.region.right += x;
		a.border.left += x;
		a.border.right += x;
		a.region.top += y;
		a.region.bottom += y;
		a.border.top += y;
		a.border.bottom += y;
		return a;
	}
};

static UIImage activeWindowBorder11	= {{1, 1 + 6, 144, 144 + 6}, 	{1, 1, 144, 144}};
static UIImage activeWindowBorder12	= {{8, 8 + 1, 144, 144 + 6}, 	{8, 9, 144, 144}};
static UIImage activeWindowBorder13	= {{10, 10 + 6, 144, 144 + 6}, 	{10, 10, 144, 144}};
static UIImage activeWindowBorder21	= {{1, 1 + 6, 151, 151 + 24}, 	{1, 1, 151, 151}};
static UIImage activeWindowBorder22	= {{8, 8 + 1, 151, 151 + 24}, 	{8, 9, 151, 151}};
static UIImage activeWindowBorder23	= {{10, 10 + 6, 151, 151 + 24},	{10, 10, 151, 151}};
static UIImage activeWindowBorder31	= {{1, 1 + 6, 176, 176 + 1}, 	{1, 1, 176, 177}};
static UIImage activeWindowBorder33	= {{10, 10 + 6, 176, 176 + 1}, 	{10, 10, 176, 177}};
static UIImage activeWindowBorder41	= {{1, 1 + 6, 178, 178 + 6}, 	{1, 1, 178, 178}};
static UIImage activeWindowBorder42	= {{8, 8 + 1, 178, 178 + 6}, 	{8, 9, 178, 178}};
static UIImage activeWindowBorder43	= {{10, 10 + 6, 178, 178 + 6}, 	{10, 10, 178, 178}};

static UIImage inactiveWindowBorder11	= {{16 + 1, 16 + 1 + 6, 144, 144 + 6}, 	{16 + 1, 16 + 1, 144, 144}};
static UIImage inactiveWindowBorder12	= {{16 + 8, 16 + 8 + 1, 144, 144 + 6}, 	{16 + 8, 16 + 9, 144, 144}};
static UIImage inactiveWindowBorder13	= {{16 + 10, 16 + 10 + 6, 144, 144 + 6},{16 + 10, 16 + 10, 144, 144}};
static UIImage inactiveWindowBorder21	= {{16 + 1, 16 + 1 + 6, 151, 151 + 24}, {16 + 1, 16 + 1, 151, 151}};
static UIImage inactiveWindowBorder22	= {{16 + 8, 16 + 8 + 1, 151, 151 + 24}, {16 + 8, 16 + 9, 151, 151}};
static UIImage inactiveWindowBorder23	= {{16 + 10, 16 + 10 + 6, 151, 151 + 24},{16 + 10, 16 + 10, 151, 151}};
static UIImage inactiveWindowBorder31	= {{16 + 1, 16 + 1 + 6, 176, 176 + 1}, 	{16 + 1, 16 + 1, 176, 177}};
static UIImage inactiveWindowBorder33	= {{16 + 10, 16 + 10 + 6, 176, 176 + 1}, {16 + 10, 16 + 10, 176, 177}};
static UIImage inactiveWindowBorder41	= {{16 + 1, 16 + 1 + 6, 178, 178 + 6}, 	{16 + 1, 16 + 1, 178, 178}};
static UIImage inactiveWindowBorder42	= {{16 + 8, 16 + 8 + 1, 178, 178 + 6}, 	{16 + 8, 16 + 9, 178, 178}};
static UIImage inactiveWindowBorder43	= {{16 + 10, 16 + 10 + 6, 178, 178 + 6}, {16 + 10, 16 + 10, 178, 178}};

static UIImage activeDialogBorder11	= {{1, 1 + 3, 144, 144 + 3}, 	{1, 1, 144, 144}};
static UIImage activeDialogBorder12	= {{8, 8 + 1, 144, 144 + 3}, 	{8, 9, 144, 144}};
static UIImage activeDialogBorder13	= {{13, 13 + 3, 144, 144 + 3}, 	{13, 13, 144, 144}};
static UIImage activeDialogBorder21	= {{1, 1 + 3, 151, 151 + 24}, 	{1, 1, 151, 151}};
static UIImage activeDialogBorder23	= {{13, 13 + 3, 151, 151 + 24},	{13, 13, 151, 151}};
static UIImage activeDialogBorder31	= {{1, 1 + 3, 185, 185 + 1}, 	{1, 1, 185, 186}};
static UIImage activeDialogBorder33	= {{1, 1 + 3, 187, 187 + 1}, 	{1, 1, 187, 188}};
static UIImage activeDialogBorder41	= {{1, 1 + 3, 181, 181 + 3}, 	{1, 1, 181, 181}};
static UIImage activeDialogBorder42	= {{5, 5 + 1, 185, 185 + 3}, 	{5, 6, 185, 185}};
static UIImage activeDialogBorder43	= {{13, 13 + 3, 181, 181 + 3}, 	{13, 13, 181, 181}};

static UIImage inactiveDialogBorder11	= {{16 + 1, 16 + 1 + 3, 144, 144 + 3}, 	{16 + 1, 16 + 1, 144, 144}};
static UIImage inactiveDialogBorder12	= {{16 + 8, 16 + 8 + 1, 144, 144 + 3}, 	{16 + 8, 16 + 9, 144, 144}};
static UIImage inactiveDialogBorder13	= {{16 + 10 + 3, 16 + 10 + 3 + 3, 144, 144 + 3},{16 + 10 + 3, 16 + 10 + 3, 144, 144}};
static UIImage inactiveDialogBorder21	= {{16 + 1, 16 + 1 + 3, 151, 151 + 24}, {16 + 1, 16 + 1, 151, 151}};
static UIImage inactiveDialogBorder23	= {{16 + 10 + 3, 16 + 10 + 3 + 3, 151, 151 + 24},{16 + 10 + 3, 16 + 10 + 3, 151, 151}};
static UIImage inactiveDialogBorder31	= {{1, 1 + 3, 189, 189 + 1}, 	{1, 1, 189, 190}};
static UIImage inactiveDialogBorder33	= {{1, 1 + 3, 191, 191 + 1}, {1, 1, 191, 192}};
static UIImage inactiveDialogBorder41	= {{16 + 1, 16 + 1 + 3, 181, 181 + 3}, 	{16 + 1, 16 + 1, 181, 181}};
static UIImage inactiveDialogBorder42	= {{5, 5 + 1, 189, 189 + 3}, 	{5, 5 + 1, 189, 189}};
static UIImage inactiveDialogBorder43	= {{16 + 10 + 3, 16 + 10 + 3 + 3, 181, 181 + 3}, {16 + 10 + 3, 16 + 10 + 3, 181, 181}};

static UIImage progressBarBackground 	= {{34, 62, 0, 16}, {36, 60, 8, 14}, OS_DRAW_MODE_STRECH};
static UIImage progressBarDisabled 	= {{29 + 34, 29 + 62, 0, 16}, {29 + 36, 29 + 60, 8, 14}, OS_DRAW_MODE_STRECH};
static UIImage progressBarFilled 	= {{1 + 29 + 29 + 34, 29 + 29 + 62 - 1, 1, 15}, {29 + 29 + 36, 29 + 29 + 60, 8, 14}, OS_DRAW_MODE_STRECH};
static UIImage progressBarFilledBlue 	= {{29 + 1 + 29 + 29 + 34, 29 + 29 + 29 + 62 - 1, 1, 15}, {29 + 29 + 29 + 36, 29 + 29 + 29 + 60, 8, 14}, OS_DRAW_MODE_STRECH};
static UIImage progressBarMarquee 	= {{95, 185, 17, 30}, {95, 185, 24, 29}, OS_DRAW_MODE_STRECH};

static UIImage buttonNormal		= {{0 * 9 + 0, 0 * 9 + 8, 88, 109}, {0 * 9 + 3, 0 * 9 + 5, 91, 106}, OS_DRAW_MODE_STRECH };
static UIImage buttonDisabled		= {{1 * 9 + 0, 1 * 9 + 8, 88, 109}, {1 * 9 + 3, 1 * 9 + 5, 91, 106}, OS_DRAW_MODE_STRECH };
static UIImage buttonHover		= {{2 * 9 + 0, 2 * 9 + 8, 88, 109}, {2 * 9 + 3, 2 * 9 + 5, 91, 106}, OS_DRAW_MODE_STRECH };
static UIImage buttonPressed		= {{3 * 9 + 0, 3 * 9 + 8, 88, 109}, {3 * 9 + 3, 3 * 9 + 5, 91, 106}, OS_DRAW_MODE_STRECH };
static UIImage buttonFocused		= {{4 * 9 + 0, 4 * 9 + 8, 88, 109}, {4 * 9 + 3, 4 * 9 + 5, 91, 106}, OS_DRAW_MODE_STRECH };
static UIImage buttonDangerousHover	= {{5 * 9 + 0, 5 * 9 + 8, 88, 109}, {5 * 9 + 3, 5 * 9 + 5, 91, 106}, OS_DRAW_MODE_STRECH };
static UIImage buttonDangerousPressed	= {{6 * 9 + 0, 6 * 9 + 8, 88, 109}, {6 * 9 + 3, 6 * 9 + 5, 91, 106}, OS_DRAW_MODE_STRECH };
static UIImage buttonDangerousFocused	= {{7 * 9 + 0, 7 * 9 + 8, 88, 109}, {7 * 9 + 3, 7 * 9 + 5, 91, 106}, OS_DRAW_MODE_STRECH };
static UIImage buttonDefault		= {{8 * 9 + 0, 8 * 9 + 8, 88, 109}, {8 * 9 + 3, 8 * 9 + 5, 91, 106}, OS_DRAW_MODE_STRECH };

static UIImage checkboxHover		= {{48, 61, 242, 255}, {48, 49, 242, 243}};
static UIImage radioboxHover		= {{48, 61, 242 - 13, 255 - 13}, {48, 49, 242 - 13, 243 - 13}};

static UIImage textboxNormal		= {{52, 61, 166, 189}, {55, 58, 169, 186}};
static UIImage textboxFocus		= {{11 + 52, 11 + 61, 166, 189}, {11 + 55, 11 + 58, 169, 186}};
static UIImage textboxHover		= {{-11 + 52, -11 + 61, 166, 189}, {-11 + 55, -11 + 58, 169, 186}};
static UIImage textboxDisabled		= {{22 + 52, 22 + 61, 166, 189}, {22 + 55, 22 + 58, 169, 186}};
static UIImage textboxCommand		= {{33 + 52, 33 + 61, 166, 189}, {33 + 55, 33 + 58, 169, 186}};
static UIImage textboxCommandHover	= {{44 + 52, 44 + 61, 166, 189}, {44 + 55, 44 + 58, 169, 186}};

static UIImage gridBox 			= {{1, 7, 17, 23}, {3, 4, 19, 20}};
static UIImage menuBox 			= {{1, 31, 1, 14}, {28, 30, 4, 6}};
static UIImage menuBoxBlank		= {{22, 27, 16, 24}, {24, 25, 19, 20}};
static UIImage menubarBackground	= {{34, 40, 124, 145}, {35, 38, 128, 144}, OS_DRAW_MODE_STRECH};
static UIImage dialogAltAreaBox		= {{18, 19, 17, 22}, {18, 19, 21, 22}};

static UIImage menuIconCheck 		= {{0, 16, 32, 32 + 16}, {0, 0, 32, 32}};
static UIImage menuIconRadio 		= {{0, 16, 17 + 32, 17 + 32 + 16}, {0, 0, 17 + 32, 17 + 32}};
static UIImage menuIconSub 		= {{0, 16, 17 + 17 + 32, 17 + 17 + 32 + 16}, {0, 0, 17 + 17 + 32, 17 + 17 + 32}};

static UIImage menuItemHover		= {{42, 50, 142, 159}, {45, 46, 151, 157}, OS_DRAW_MODE_STRECH};
static UIImage menuItemDragged		= {{18 + 42, 18 + 50, 142, 159}, {18 + 45, 18 + 46, 151, 157}, OS_DRAW_MODE_STRECH};

static UIImage toolbarBackground	= {{0, 0 + 60, 195, 195 + 31}, {0 + 1, 0 + 59, 195 + 1, 195 + 29}, OS_DRAW_MODE_STRECH};
static UIImage toolbarBackgroundAlt	= {{98 + 0, 98 + 0 + 60, 195, 195 + 31}, {98 + 0 + 1, 98 + 0 + 59, 195 + 1, 195 + 29}, OS_DRAW_MODE_STRECH};

static UIImage toolbarHover		= {{73, 84, 195, 226}, {78, 79, 203, 204}};
static UIImage toolbarPressed		= {{73 - 12, 84 - 12, 195, 226}, {78 - 12, 79 - 12, 203, 204}};
static UIImage toolbarNormal		= {{73 + 12, 84 + 12, 195, 226}, {78 + 12, 79 + 12, 203, 204}};

static UIImage scrollbarTrackHorizontalEnabled  = {{121, 122, 62, 79}, {121, 122, 62, 62}};
static UIImage scrollbarTrackHorizontalPressed  = {{117, 118, 62, 79}, {117, 118, 62, 62}};
static UIImage scrollbarTrackHorizontalDisabled = {{119, 120, 62, 79}, {119, 120, 62, 62}};
static UIImage scrollbarTrackVerticalEnabled    = {{174, 191, 82, 83}, {174, 174, 82, 83}};
static UIImage scrollbarTrackVerticalPressed    = {{174, 191, 84, 85}, {174, 174, 84, 85}};
static UIImage scrollbarTrackVerticalDisabled   = {{174, 191, 80, 81}, {174, 174, 80, 81}};
static UIImage scrollbarButtonHorizontalNormal  = {{159, 166, 62, 79}, {162, 163, 62, 62}};
static UIImage scrollbarButtonHorizontalHover   = {{167, 174, 62, 79}, {170, 171, 62, 62}};
static UIImage scrollbarButtonHorizontalPressed = {{175, 182, 62, 79}, {178, 179, 62, 62}};
static UIImage scrollbarButtonVerticalNormal    = {{141, 158, 62, 69}, {141, 141, 65, 66}};
static UIImage scrollbarButtonVerticalHover     = {{141, 158, 70, 77}, {141, 141, 73, 74}};
static UIImage scrollbarButtonVerticalPressed   = {{141, 158, 78, 85}, {141, 141, 81, 82}};
static UIImage scrollbarButtonDisabled          = {{183, 190, 62, 79}, {186, 187, 62, 62}};
static UIImage scrollbarNotchesHorizontal       = {{159, 164, 80, 88}, {159, 159, 80, 80}};
static UIImage scrollbarNotchesVertical         = {{165, 173, 80, 85}, {165, 165, 80, 80}};

#if 0
static UIImage resizePad               = {{123, 140, 62, 79}, {123, 123, 62, 62}};
#endif

static UIImage smallArrowUpNormal      = {{206, 217, 25, 34}, {206, 206, 25, 25}};
static UIImage smallArrowDownNormal    = {{206, 217, 35, 44}, {206, 206, 35, 35}};
static UIImage smallArrowLeftNormal    = {{204, 213, 14, 25}, {204, 204, 14, 14}};
static UIImage smallArrowRightNormal   = {{204, 213, 3, 14}, {204, 204, 3, 3}};

static UIImage lineHorizontal		= {{40, 52, 115, 116}, {41, 41, 115, 115}};
static UIImage *lineHorizontalBackgrounds[] = { &lineHorizontal, &lineHorizontal, &lineHorizontal, &lineHorizontal, };
static UIImage lineVertical		= {{35, 36, 110, 122}, {35, 35, 111, 111}};
static UIImage *lineVerticalBackgrounds[] = { &lineVertical, &lineVertical, &lineVertical, &lineVertical, };

static UIImage sliderBox = {{98, 126, 132, 160}, {112, 113, 145, 146}};
static UIImage tickHorizontal = {{208, 213, 151, 153}, {209, 211, 151, 153}};
static UIImage tickVertical = {{209, 211, 155, 160}, {209, 211, 156, 158}};

static UIImage sliderDownHover = {{206, 219, 171, 191}, {206, 206, 171, 171}};
static UIImage sliderDownNormal = {{12 + 206, 12 + 219, 171, 191}, {12 + 206, 12 + 206, 171, 171}};
static UIImage sliderDownFocused = {{24 + 206, 24 + 219, 171, 191}, {24 + 206, 24 + 206, 171, 171}};
static UIImage sliderDownPressed = {{36 + 206, 36 + 219, 171, 191}, {36 + 206, 36 + 206, 171, 171}};
static UIImage sliderDownDisabled = {{110, 123, 161, 181}, {110, 110, 161, 161}};

static UIImage sliderUpHover = {{206, 219, 19 + 171, 19 + 191}, {206, 206, 19 + 171, 19 + 171}};
static UIImage sliderUpNormal = {{12 + 206, 12 + 219, 19 + 171, 19 + 191}, {12 + 206, 12 + 206, 19 + 171, 19 + 171}};
static UIImage sliderUpFocused = {{24 + 206, 24 + 219, 19 + 171, 19 + 191}, {24 + 206, 24 + 206, 19 + 171, 19 + 171}};
static UIImage sliderUpPressed = {{36 + 206, 36 + 219, 19 + 171, 19 + 191}, {36 + 206, 36 + 206, 19 + 171, 19 + 171}};
static UIImage sliderUpDisabled = {{72, 85, 140, 160}, {72, 72, 140, 140}};

static UIImage sliderVerticalHover = {{206, 219, 19 + 19 + 171, 19 + 19 + 191}, {206, 206, 19 + 19 + 171, 19 + 19 + 171}};
static UIImage sliderVerticalNormal = {{12 + 206, 12 + 219, 19 + 19 + 171, 19 + 19 + 191}, {12 + 206, 12 + 206, 19 + 19 + 171, 19 + 19 + 171}};
static UIImage sliderVerticalFocused = {{24 + 206, 24 + 219, 19 + 19 + 171, 19 + 19 + 191}, {24 + 206, 24 + 206, 19 + 19 + 171, 19 + 19 + 171}};
static UIImage sliderVerticalPressed = {{36 + 206, 36 + 219, 19 + 19 + 171, 19 + 19 + 191}, {36 + 206, 36 + 206, 19 + 19 + 171, 19 + 19 + 171}};
static UIImage sliderVerticalDisabled = {{12 + 72, 12 + 85, 140, 160}, {12 + 72, 12 + 72, 140, 140}};

static UIImage sliderRightHover = {{147, 167, 145, 158}, {147, 147, 145, 145}};
static UIImage sliderRightNormal = {{147, 167, 12 + 145, 12 + 158}, {147, 147, 12 + 145, 12 + 145}};
static UIImage sliderRightFocused = {{147, 167, 24 + 145, 24 + 158}, {147, 147, 24 + 145, 24 + 145}};
static UIImage sliderRightPressed = {{147, 167, 36 + 145, 36 + 158}, {147, 147, 36 + 145, 36 + 145}};
static UIImage sliderRightDisabled = {{127, 147, 12 + 145, 12 + 158}, {127, 127, 12 + 145, 12 + 145}};

static UIImage sliderLeftHover = {{20 + 147, 20 + 167, 145, 158}, {20 + 147, 20 + 147, 145, 145}};
static UIImage sliderLeftNormal = {{20 + 147, 20 + 167, 12 + 145, 12 + 158}, {20 + 147, 20 + 147, 12 + 145, 12 + 145}};
static UIImage sliderLeftFocused = {{20 + 147, 20 + 167, 24 + 145, 24 + 158}, {20 + 147, 20 + 147, 24 + 145, 24 + 145}};
static UIImage sliderLeftPressed = {{20 + 147, 20 + 167, 36 + 145, 36 + 158}, {20 + 147, 20 + 147, 36 + 145, 36 + 145}};
static UIImage sliderLeftDisabled = {{127, 147, 145, 158}, {127, 127, 145, 145}};

static UIImage sliderHorizontalHover = {{40 + 147, 40 + 167, 145, 158}, {40 + 147, 40 + 147, 145, 145}};
static UIImage sliderHorizontalNormal = {{40 + 147, 40 + 167, 12 + 145, 12 + 158}, {40 + 147, 40 + 147, 12 + 145, 12 + 145}};
static UIImage sliderHorizontalFocused = {{40 + 147, 40 + 167, 24 + 145, 24 + 158}, {40 + 147, 40 + 147, 24 + 145, 24 + 145}};
static UIImage sliderHorizontalPressed = {{40 + 147, 40 + 167, 36 + 145, 36 + 158}, {40 + 147, 40 + 147, 36 + 145, 36 + 145}};
static UIImage sliderHorizontalDisabled = {{127, 148, 24 + 145, 24 + 158}, {127, 127, 24 + 145, 24 + 145}};

static struct UIImage *sliderDown[] = {
	&sliderDownNormal,
	&sliderDownDisabled,
	&sliderDownHover,
	&sliderDownPressed,
	&sliderDownFocused,
};

static struct UIImage *sliderUp[] = {
	&sliderUpNormal,
	&sliderUpDisabled,
	&sliderUpHover,
	&sliderUpPressed,
	&sliderUpFocused,
};

static struct UIImage *sliderVertical[] = {
	&sliderVerticalNormal,
	&sliderVerticalDisabled,
	&sliderVerticalHover,
	&sliderVerticalPressed,
	&sliderVerticalFocused,
};

static struct UIImage *sliderRight[] = {
	&sliderRightNormal,
	&sliderRightDisabled,
	&sliderRightHover,
	&sliderRightPressed,
	&sliderRightFocused,
};

static struct UIImage *sliderLeft[] = {
	&sliderLeftNormal,
	&sliderLeftDisabled,
	&sliderLeftHover,
	&sliderLeftPressed,
	&sliderLeftFocused,
};

static struct UIImage *sliderHorizontal[] = {
	&sliderHorizontalNormal,
	&sliderHorizontalDisabled,
	&sliderHorizontalHover,
	&sliderHorizontalPressed,
	&sliderHorizontalFocused,
};

// static UIImage testImage = {{57, 61, 111, 115}, {58, 60, 112, 114}};

static struct UIImage *scrollbarTrackVerticalBackgrounds[] = { 
	&scrollbarTrackVerticalEnabled, 
	&scrollbarTrackVerticalDisabled, 
	&scrollbarTrackVerticalEnabled, 
	&scrollbarTrackVerticalPressed, 
	&scrollbarTrackVerticalEnabled, 
};
static struct UIImage *scrollbarTrackHorizontalBackgrounds[] = { 
	&scrollbarTrackHorizontalEnabled, 
	&scrollbarTrackHorizontalDisabled, 
	&scrollbarTrackHorizontalEnabled, 
	&scrollbarTrackHorizontalPressed, 
	&scrollbarTrackHorizontalEnabled, 
};

#define ICON16(x, y) {{x, x + 16, y, y + 16}, {x, x, y, y}}
#define ICON24(x, y) {{x, x + 24, y, y + 24}, {x, x, y, y}}
#define ICON32(x, y) {{x, x + 32, y, y + 32}, {x, x, y, y}}

static UIImage icons16[] = {
	{{}, {}},
	ICON16(237, 117),
	ICON16(220, 117),
	{{}, {}},
	ICON16(512 + 320, 208),
	ICON16(512 + 320, 160),
	ICON16(512 + 64, 192),
	ICON16(512 + 320, 288),
	ICON16(512 + 192, 16),
	ICON16(512 + 0, 432),
	ICON16(512 + 64, 288),
};

static UIImage icons32[] = {
	{{}, {}},
	{{}, {}},
	{{}, {}},
	ICON32(220, 135),
	{{}, {}},
	{{}, {}},
	{{}, {}},
	{{}, {}},
	{{}, {}},
	{{}, {}},
	{{}, {}},
	ICON32(512 + 0, 480),
};

static UIImage *scrollbarButtonHorizontalBackgrounds[] = {
	&scrollbarButtonHorizontalNormal,
	&scrollbarButtonDisabled,
	&scrollbarButtonHorizontalHover,
	&scrollbarButtonHorizontalPressed,
	&scrollbarButtonVerticalNormal,
};

static UIImage *scrollbarButtonVerticalBackgrounds[] = {
	&scrollbarButtonVerticalNormal,
	&scrollbarButtonDisabled,
	&scrollbarButtonVerticalHover,
	&scrollbarButtonVerticalPressed,
	&scrollbarButtonVerticalNormal,
};

#if 0
static UIImage *scrollbarResizePadBackgrounds[] = {
	&scrollbarResizePad,
	&scrollbarResizePad,
	&scrollbarResizePad,
	&scrollbarResizePad,
};
#endif

static UIImage *menuItemBackgrounds[] = {
	nullptr,
	nullptr,
#if 1
	&menuItemHover,
	&menuItemDragged,
	&menuItemHover,
#else
	&listViewHighlight,
	&listViewSelected,
	&listViewHighlight,
#endif
};

struct UIImage *toolbarItemBackgrounds[] = {
	nullptr,
	nullptr,
	&toolbarHover,
	&toolbarPressed,
	&toolbarNormal,
	nullptr,
	&toolbarHover,
	&toolbarPressed,
};

static UIImage *textboxBackgrounds[] = {
	&textboxNormal,
	&textboxDisabled,
	&textboxHover,
	&textboxFocus,
	&textboxFocus,
};

static UIImage *textboxCommandBackgrounds[] = {
	&textboxCommand,
	&textboxDisabled,
	&textboxCommandHover,
	&textboxFocus,
	&textboxFocus,
};

static UIImage *buttonBackgrounds[] = {
	&buttonNormal,
	&buttonDisabled,
	&buttonHover,
	&buttonPressed,
	&buttonFocused,
};

static UIImage *buttonDefaultBackgrounds[] = {
	&buttonDefault,
	&buttonDisabled,
	&buttonHover,
	&buttonPressed,
	&buttonFocused,
};

static UIImage *buttonDangerousBackgrounds[] = {
	&buttonNormal,
	&buttonDisabled,
	&buttonDangerousHover,
	&buttonDangerousPressed,
	&buttonDangerousFocused,
};

static UIImage *windowBorder11[] = {&activeWindowBorder11, &inactiveWindowBorder11, &activeWindowBorder11, &activeWindowBorder11, &activeWindowBorder11};
static UIImage *windowBorder12[] = {&activeWindowBorder12, &inactiveWindowBorder12, &activeWindowBorder12, &activeWindowBorder12, &activeWindowBorder12};
static UIImage *windowBorder13[] = {&activeWindowBorder13, &inactiveWindowBorder13, &activeWindowBorder13, &activeWindowBorder13, &activeWindowBorder13};
static UIImage *windowBorder21[] = {&activeWindowBorder21, &inactiveWindowBorder21, &activeWindowBorder21, &activeWindowBorder21, &activeWindowBorder21};
static UIImage *windowBorder22[] = {&activeWindowBorder22, &inactiveWindowBorder22, &activeWindowBorder22, &activeWindowBorder22, &activeWindowBorder22};
static UIImage *windowBorder23[] = {&activeWindowBorder23, &inactiveWindowBorder23, &activeWindowBorder23, &activeWindowBorder23, &activeWindowBorder23};
static UIImage *windowBorder31[] = {&activeWindowBorder31, &inactiveWindowBorder31, &activeWindowBorder31, &activeWindowBorder31, &activeWindowBorder31};
static UIImage *windowBorder33[] = {&activeWindowBorder33, &inactiveWindowBorder33, &activeWindowBorder33, &activeWindowBorder33, &activeWindowBorder33};
static UIImage *windowBorder41[] = {&activeWindowBorder41, &inactiveWindowBorder41, &activeWindowBorder41, &activeWindowBorder41, &activeWindowBorder41};
static UIImage *windowBorder42[] = {&activeWindowBorder42, &inactiveWindowBorder42, &activeWindowBorder42, &activeWindowBorder42, &activeWindowBorder42};
static UIImage *windowBorder43[] = {&activeWindowBorder43, &inactiveWindowBorder43, &activeWindowBorder43, &activeWindowBorder43, &activeWindowBorder43};
                                                                                                                                
static UIImage *dialogBorder11[] = {&activeDialogBorder11, &inactiveDialogBorder11, &activeDialogBorder11, &activeDialogBorder11, &activeDialogBorder11};
static UIImage *dialogBorder12[] = {&activeDialogBorder12, &inactiveDialogBorder12, &activeDialogBorder12, &activeDialogBorder12, &activeDialogBorder12};
static UIImage *dialogBorder13[] = {&activeDialogBorder13, &inactiveDialogBorder13, &activeDialogBorder13, &activeDialogBorder13, &activeDialogBorder13};
static UIImage *dialogBorder21[] = {&activeDialogBorder21, &inactiveDialogBorder21, &activeDialogBorder21, &activeDialogBorder21, &activeDialogBorder21};
static UIImage *dialogBorder23[] = {&activeDialogBorder23, &inactiveDialogBorder23, &activeDialogBorder23, &activeDialogBorder23, &activeDialogBorder23};
static UIImage *dialogBorder31[] = {&activeDialogBorder31, &inactiveDialogBorder31, &activeDialogBorder31, &activeDialogBorder31, &activeDialogBorder31};
static UIImage *dialogBorder33[] = {&activeDialogBorder33, &inactiveDialogBorder33, &activeDialogBorder33, &activeDialogBorder33, &activeDialogBorder33};
static UIImage *dialogBorder41[] = {&activeDialogBorder41, &inactiveDialogBorder41, &activeDialogBorder41, &activeDialogBorder41, &activeDialogBorder41};
static UIImage *dialogBorder42[] = {&activeDialogBorder42, &inactiveDialogBorder42, &activeDialogBorder42, &activeDialogBorder42, &activeDialogBorder42};
static UIImage *dialogBorder43[] = {&activeDialogBorder43, &inactiveDialogBorder43, &activeDialogBorder43, &activeDialogBorder43, &activeDialogBorder43};

static const int totalBorderWidth = 6 + 6;
static const int totalBorderHeight = 6 + 24 + 6;

struct GUIObject : APIObject {
	OSCallback notificationCallback;
	struct Window *window;
	OSRectangle bounds, cellBounds, inputBounds;
	uint16_t descendentInvalidationFlags;
	uint16_t layout;
	uint16_t preferredWidth, preferredHeight;
	uint16_t horizontalMargin : 4, verticalMargin : 4,
		verbose : 1,
		suggestWidth : 1, // Prevent recalculation of the preferred[Width/Height]
		suggestHeight : 1, // on MEASURE messages with grids.
		relayout : 1, repaint : 1,
		tabStop : 1, disabled : 1,
		isMenubar : 1;
};

#define DESCENDENT_REPAINT  (1)
#define DESCENDENT_RELAYOUT (2)

struct Control : GUIObject {
	// Current size: ~290 bytes.
	// Is this too big?
		
#define UI_IMAGE_NORMAL (0)
#define UI_IMAGE_DISABLED (1)
#define UI_IMAGE_HOVER (2)
#define UI_IMAGE_DRAG (3)
	UIImage **backgrounds;
	UIImage *icon;

	void *context;

	OSCommand *command;
	LinkedItem<Control> commandItem;

	OSMenuSpecification *rightClickMenu;

	OSString text;
	OSRectangle textBounds;
	uint32_t textColor;
	uint8_t textSize, textAlign;
	uint8_t textShadow : 1, 
		textBold : 1,
		textShadowBlur : 1,
		customTextRendering : 1;

	// Configuration:

	uint32_t focusable : 1,
		checkable : 1,
		radioCheck : 1,
		useClickRepeat : 1,

		noAnimations : 1,
		noDisabledTextColorChange : 1,
		ignoreActivationClicks : 1,
		drawParentBackground : 1,

		additionalCheckedBackgrounds : 1,
		additionalCheckedIcons : 1,
		hasFocusedBackground : 1,
		centerIcons : 1,
		iconHasVariants : 1,
		iconHasFocusVariant : 1,

		keepCustomCursorWhenDisabled : 1,
		cursor : 5;

	// State:

	uint32_t isChecked : 1, 
		firstPaint : 1,
		repaintCustomOnly : 1,
		pressedByKeyboard : 1;

	LinkedItem<Control> timerControlItem;

	// Animation data:
	uint8_t timerHz, timerStep;
	uint8_t animationStep, finalAnimationStep;
	uint8_t from1, from2, from3, from4, from5;
	uint8_t current1, current2, current3, current4, current5;
};

#if 0
struct ListView : Control {
	unsigned flags;
	size_t itemCount;
	OSObject scrollbar;
	int scrollY;
	int32_t highlightRow, lastClickedRow;

	OSPoint selectionBoxAnchor;
	OSPoint selectionBoxPosition;
	OSRectangle selectionBox;

	OSListViewColumn *columns;
	int32_t columnsCount;
	int32_t rowWidth;

	int repaintFirstRow, repaintLastRow;
	int draggingColumnIndex, draggingColumnX;
	bool repaintSelectionBox;
	OSRectangle oldSelectionBox;
	
	enum {
		DRAGGING_NONE,
		DRAGGING_ITEMS,
		DRAGGING_SELECTION,
		DRAGGING_COLUMN,
	} dragging;
};
#endif

struct MenuItem : Control {
	OSMenuItem item;
	Window *child;
	bool menubar;
};

struct Textbox : Control {
	OSCaret caret, caret2;
	OSCaret wordSelectionAnchor, wordSelectionAnchor2;
	uint8_t caretBlink : 1,
		sentEditResultNotification : 1;
	int scrollX;
	OSString previousString;
	OSTextboxStyle style;
};

struct ProgressBar : Control {
	int minimum, maximum, value;
};

struct WindowResizeControl : Control {
	unsigned direction;
};

struct Grid : GUIObject {
	unsigned columns, rows;
	GUIObject **objects;
	int *widths, *heights;
	int *minimumWidths, *minimumHeights;
	OSGridStyle style;
	OSRectangle borderSize;
	uint16_t gapSize;
	UIImage *background;
	uint32_t backgroundColor;
	int xOffset, yOffset;
	bool treatPreferredDimensionsAsMinima; // Used with scroll panes for PUSH objects.
};

struct Slider : Grid {
	int minimum, maximum, value, minorTickSpacing, majorTickSpacing, mode, previousClickAdjustment;
	struct SliderHandle *handle;
};

struct SliderHandle : Control {
	Slider *slider;
};

struct Scrollbar : Grid {
	bool enabled;
	bool orientation;
	bool automaticallyUpdatePosition;

	int contentSize;
	int viewportSize;
	int height;

	int anchor;
	float position;
	int maxPosition;
	int size;
};

struct CommandWindow {
	// Data each window stores for each command specified in the program's manifest.
	LinkedList<Control> controls; // Controls bound to this command.
	OSCallback notificationCallback;
	uintptr_t disabled : 1, checked : 1;
	uintptr_t state;
};

struct Window : GUIObject {
	OSHandle window, surface;

	// For OS_CREATE_WINDOW_MENU: 1 x 1.
	// For OS_CREATE_WINDOW_NORMAL: 3 x 4.
	// 	- Content pane at 1, 2.
	// 	- Or if OS_CREATE_WINDOW_WITH_MENUBAR,
	// 		- This is a subgrid, 1 x 2
	// 		- Menubar at 0, 0.
	// 		- Content pane at 0, 1.
	Grid *root;

	unsigned flags;
	OSCursorStyle cursor, cursorOld;

	struct Control *pressed,      // Mouse is pressing the control.
		       *hover,        // Mouse is hovering over the control.
		       *focus, 	      // Control has strong focus.
		       *lastFocus,    // Control has weak focus.
		       *defaultFocus; // Control receives focus if no other control has focus.

	struct Grid *hoverGrid;

	bool destroyed, created, activated;

	int width, height;
	int minimumWidth, minimumHeight;

	LinkedList<Control> timerControls;
	int timerHz, caretBlinkStep, caretBlinkPause;

	CommandWindow *commands;
	OSCommand *defaultCommand;
	void *instance;
	bool hasMenuParent;

	bool willUpdateAfterMessageProcessing;
};

struct OpenMenu {
	Window *window;
	Control *source;
	bool fromMenubar;
};

static OpenMenu openMenus[8];
static unsigned openMenuCount;
static bool navigateMenuMode;
static bool navigateMenuUsedKey;
static MenuItem *navigateMenuItem;
static MenuItem *navigateMenuFirstItem;

struct GUIAllocationBlock {
	size_t allocationCount;
	size_t totalBytes;
	size_t allocatedBytes;
};

static GUIAllocationBlock *guiAllocationBlock;

#include "list_view.cpp"
#define IMPLEMENTATION

static void SetParentDescendentInvalidationFlags(GUIObject *object, uint16_t mask) {
	do {
		if (object->type == API_OBJECT_WINDOW) {
			Window *window = (Window *) object;

			if (!window->willUpdateAfterMessageProcessing) {
				window->willUpdateAfterMessageProcessing = true;

				OSMessage message;
				message.type = OS_MESSAGE_UPDATE_WINDOW;
				message.context = window;
				OSPostMessage(&message);
			}
		}

		object->descendentInvalidationFlags |= mask;
		object = (GUIObject *) object->parent;
	} while (object);
}

void OSStartGUIAllocationBlock(size_t bytes) {
	guiAllocationBlock = (GUIAllocationBlock *) OSHeapAllocate(bytes + sizeof(GUIAllocationBlock), false);
	guiAllocationBlock->allocationCount = 0;
	guiAllocationBlock->totalBytes = bytes;
	guiAllocationBlock->allocatedBytes = 0;
}

size_t OSEndGUIAllocationBlock() {
	size_t allocatedBytes = guiAllocationBlock->allocatedBytes;

	OSPrint("End GUI block %x, using %d/%d bytes (%d allocations).\n", guiAllocationBlock, allocatedBytes, guiAllocationBlock->totalBytes, guiAllocationBlock->allocationCount);

	if (!guiAllocationBlock->allocationCount) {
		OSHeapFree(guiAllocationBlock);
	}

	guiAllocationBlock = nullptr;
	return allocatedBytes;
}

static void *GUIAllocate(size_t bytes, bool clear) {
	bytes += 8;

	if (guiAllocationBlock) {
		guiAllocationBlock->allocatedBytes += bytes;

		if (guiAllocationBlock->allocatedBytes < guiAllocationBlock->totalBytes) {
			guiAllocationBlock->allocationCount++;
			uintptr_t *a = (uintptr_t *) (((uint8_t *) (guiAllocationBlock + 1)) + guiAllocationBlock->allocatedBytes - bytes);
			if (clear) OSZeroMemory(a, bytes);
			*a = (uintptr_t) guiAllocationBlock;
			return a + 1;
		}
	}

	uintptr_t *a = (uintptr_t *) OSHeapAllocate(bytes, clear);
	*a = (uintptr_t) -1;
	return a + 1;
}

static void GUIFree(void *address) {
	if (!address) {
		return;
	}

	uintptr_t *a = (uintptr_t *) address - 1;

	if (*a == (uintptr_t) -1) {
		OSHeapFree(a);
	} else {
		GUIAllocationBlock *block = (GUIAllocationBlock *) *a;
		block->allocationCount--;

		if (block->allocationCount == 0) {
			// OSPrint("Freeing GUI block %x\n", block);
			OSHeapFree(block);
		}
	}
}

static bool ClipRectangle(OSRectangle parent, OSRectangle rectangle, OSRectangle *output) {
	OSRectangle current = parent;
	OSRectangle intersection;

	if (!((current.left > rectangle.right && current.right > rectangle.left)
			|| (current.top > rectangle.bottom && current.bottom > rectangle.top))) {
		intersection.left = current.left > rectangle.left ? current.left : rectangle.left;
		intersection.top = current.top > rectangle.top ? current.top : rectangle.top;
		intersection.right = current.right < rectangle.right ? current.right : rectangle.right;
		intersection.bottom = current.bottom < rectangle.bottom ? current.bottom : rectangle.bottom;
	} else {
		intersection = OS_MAKE_RECTANGLE(0, 0, 0, 0);
	}

	if (output) {
		*output = intersection;
	}

	return intersection.left < intersection.right && intersection.top < intersection.bottom;
}

void OSDebugGUIObject(OSObject _guiObject) {
	GUIObject *guiObject = (GUIObject *) _guiObject;
	guiObject->verbose = true;
}

static void CopyText(OSString buffer) {
	OSClipboardHeader header = {};
	header.format = OS_CLIPBOARD_FORMAT_TEXT;
	header.textBytes = buffer.bytes;
	OSSyscall(OS_SYSCALL_COPY, (uintptr_t) buffer.buffer, (uintptr_t) &header, 0, 0);
}

static size_t ClipboardTextBytes() {
	OSClipboardHeader clipboard;
	OSSyscall(OS_SYSCALL_GET_CLIPBOARD_HEADER, 0, (uintptr_t) &clipboard, 0, 0);
	return clipboard.textBytes;
}

static inline void RepaintControl(OSObject _control, bool customOnly = false) {
	Control *control = (Control *) _control;

	control->repaint = true;
	SetParentDescendentInvalidationFlags(control, DESCENDENT_REPAINT);

	if (!customOnly) {
		control->repaintCustomOnly = false;
	}
}

void OSRepaintControl(OSObject object) {
	RepaintControl(object, false);
}

static inline bool IsPointInRectangle(OSRectangle rectangle, int x, int y) {
	if (rectangle.left > x || rectangle.right <= x || rectangle.top > y || rectangle.bottom <= y) {
		return false;
	}
	
	return true;
}

void OSSetFocusedWindow(OSObject object) {
	Window *window = (Window *) object;
	OSSyscall(OS_SYSCALL_SET_FOCUSED_WINDOW, window->window, 0, 0, 0);
}

void OSSetControlCommand(OSObject _control, OSCommand *_command) {
	Control *control = (Control *) _control;
	
	if (_command->iconID) {
		control->icon = icons16 + _command->iconID;
		control->iconHasVariants = true;
	}

	if (_command->identifier == OS_COMMAND_DYNAMIC) {
		control->isChecked = _command->defaultCheck;
		control->disabled = _command->defaultDisabled;
		control->notificationCallback = _command->callback;

		return;
	}

	control->command = _command;
	control->commandItem.thisItem = control;
	control->checkable = _command->checkable;
	control->radioCheck = _command->radioCheck;

	if (control->window) {
		CommandWindow *command = control->window->commands + _command->identifier;
		command->controls.InsertEnd(&control->commandItem);
		control->isChecked = command->checked;
		control->disabled = command->disabled;
		control->notificationCallback = command->notificationCallback;
	}
}

void OSAnimateControl(OSObject _control, bool fast) {
	Control *control = (Control *) _control;

	// OSPrint("Animate control %x\n", control);
	
	if (control->firstPaint && !control->noAnimations) {
		control->from1 = control->current1;
		control->from2 = control->current2;
		control->from3 = control->current3;
		control->from4 = control->current4;
		control->from5 = control->current5;

		control->animationStep = 0;
		control->finalAnimationStep = fast ? 4 : 16;

		if (!control->timerControlItem.list) {
			control->timerHz = 30; // TODO Make this 60?
			control->window->timerControls.InsertStart(&control->timerControlItem);
			control->timerControlItem.thisItem = control;
		}
	} else {
		control->animationStep = 16;
		control->finalAnimationStep = 16;
	}

	RepaintControl(control);
}

static void StandardCellLayout(GUIObject *object) {
	object->cellBounds = object->bounds;

	int width = object->bounds.right - object->bounds.left;
	int height = object->bounds.bottom - object->bounds.top;

	int preferredWidth = (object->layout & OS_CELL_H_EXPAND) ? width : (object->preferredWidth + object->horizontalMargin * 2);
	int preferredHeight = (object->layout & OS_CELL_V_EXPAND) ? height : (object->preferredHeight + object->verticalMargin * 2);

	if (width > preferredWidth) {
		if (object->layout & OS_CELL_H_EXPAND) {
		} else if (object->layout & OS_CELL_H_LEFT) {
			object->bounds.right = object->bounds.left + preferredWidth;
		} else if (object->layout & OS_CELL_H_RIGHT) {
			object->bounds.left = object->bounds.right - preferredWidth;
		} else {
			object->bounds.left = object->bounds.left + width / 2 - preferredWidth / 2;
			object->bounds.right = object->bounds.left + preferredWidth;
		}
	}

	if (height > preferredHeight) {
		if (object->layout & OS_CELL_V_EXPAND) {
		} else if (object->layout & OS_CELL_V_TOP) {
			object->bounds.bottom = object->bounds.top + preferredHeight;
		} else if (object->layout & OS_CELL_V_BOTTOM) {
			object->bounds.top = object->bounds.bottom - preferredHeight;
		} else {
			object->bounds.top = object->bounds.top + height / 2 - preferredHeight / 2;
			object->bounds.bottom = object->bounds.top + preferredHeight;
		}
	}
}

static void SetGUIObjectProperty(GUIObject *object, OSMessage *message) {
	uintptr_t value = (uintptr_t) message->setProperty.value;

	switch (message->setProperty.index) {
		case OS_GUI_OBJECT_PROPERTY_SUGGESTED_WIDTH: {
			object->suggestWidth = true;
			object->preferredWidth = value;
			object->relayout = true;

			{
				OSMessage message;
				message.type = OS_MESSAGE_CHILD_UPDATED;
				OSSendMessage(object->parent, &message);
			}
		} break;

		case OS_GUI_OBJECT_PROPERTY_SUGGESTED_HEIGHT: {
			object->suggestHeight = true;
			object->preferredHeight = value;
			object->relayout = true;

			{
				OSMessage message;
				message.type = OS_MESSAGE_CHILD_UPDATED;
				OSSendMessage(object->parent, &message);
			}
		} break;
	}
}

OSObject OSGetFocusedControl(OSObject _window, bool ignoreWeakFocus) {
	Window *window = (Window *) _window;

	if (ignoreWeakFocus) {
		return window->focus;
	} else {
		return window->lastFocus;
	}
}

void OSRemoveFocusedControl(OSObject _window, bool removeWeakFocus) {
	Window *window = (Window *) _window;
	OSMessage message;

	// Remove any focus.
	if (window->focus) {
		message.type = OS_MESSAGE_END_FOCUS;
		OSSendMessage(window->focus, &message);
		window->focus = nullptr;
	}

	// Remove any last focus.
	if (window->lastFocus && removeWeakFocus) {
		message.type = OS_MESSAGE_END_LAST_FOCUS;
		OSSendMessage(window->lastFocus, &message);
		window->lastFocus = nullptr;

		if (window->defaultFocus) {
			OSSetFocusedControl(window->defaultFocus, false);
		}
	}
}

void OSSetFocusedControl(OSObject _control, bool asDefaultForWindow) {
	if (!_control) {
		return;
	}

	Control *control = (Control *) _control;
	Window *window = control->window;
	OSMessage message;

	if (control != window->focus) {
		OSRemoveFocusedControl(window, true);
	}

	// Give this control focus, if it doesn't already have it.
	if (control != window->focus) {
		window->focus = control;
		window->lastFocus = control;
		message.type = OS_MESSAGE_START_FOCUS;
		OSSendMessage(control, &message);
	}

	if (asDefaultForWindow) {
		window->defaultFocus = control;
	}
}

static OSCallbackResponse ProcessControlMessage(OSObject _object, OSMessage *message) {
	OSCallbackResponse response = OS_CALLBACK_HANDLED;
	Control *control = (Control *) _object;

	switch (message->type) {
		case OS_MESSAGE_LAYOUT: {
			if (control->cellBounds.left == message->layout.left
					&& control->cellBounds.right == message->layout.right
					&& control->cellBounds.top == message->layout.top
					&& control->cellBounds.bottom == message->layout.bottom
					&& !control->relayout) {
				break;
			}

			control->bounds = OS_MAKE_RECTANGLE(
					message->layout.left, message->layout.right,
					message->layout.top, message->layout.bottom);

			if (control->verbose) {
				OSPrint("Cell layout for control %x: %d->%d, %d->%d\n", control, control->bounds.left, control->bounds.right, control->bounds.top, control->bounds.bottom);
			}

			StandardCellLayout(control);
			ClipRectangle(message->layout.clip, control->bounds, &control->inputBounds);

			if (control->verbose) {
				OSPrint("Layout control %x: %d->%d, %d->%d; input %d->%d, %d->%d\n", control, control->bounds.left, control->bounds.right, control->bounds.top, control->bounds.bottom,
						control->inputBounds.left, control->inputBounds.right, control->inputBounds.top, control->inputBounds.bottom);
			}

			control->relayout = false;
			RepaintControl(control);
			SetParentDescendentInvalidationFlags(control, DESCENDENT_REPAINT);

			{
				OSMessage m = *message;
				m.type = OS_MESSAGE_LAYOUT_TEXT;
				OSSendMessage(control, &m);
			}
		} break;

		case OS_MESSAGE_LAYOUT_TEXT: {
			OSRectangle contentBounds = control->bounds;
			contentBounds.left += control->horizontalMargin;
			contentBounds.right -= control->horizontalMargin;
			contentBounds.top += control->verticalMargin;
			contentBounds.bottom -= control->verticalMargin;

			control->textBounds = contentBounds;

			if (control->icon) {
				control->textBounds.left += control->icon->region.right - control->icon->region.left + ICON_TEXT_GAP;
			}
		} break;

		case OS_MESSAGE_MEASURE: {
			message->measure.preferredWidth = control->preferredWidth + control->horizontalMargin * 2;
			message->measure.preferredHeight = control->preferredHeight + control->verticalMargin * 2;
		} break;

		case OS_MESSAGE_KEY_PRESSED: {
			if (control->tabStop && control->window->lastFocus != control) {
				OSSetFocusedControl(control, false);
			} else {
				response = OS_CALLBACK_NOT_HANDLED;
			}
		} break;

		case OS_MESSAGE_PAINT: {
			if (control->repaint || message->paint.force) {
				// if (!control->firstPaint) OSPrint("first paint %x\n", control);
				control->firstPaint = true;

#if 0
				if (control->window->focus == control) {
					OSFillRectangle(message->paint.surface, message->paint.clip, OSColor(255, 0, 255));
					break;
				}
#endif

				bool isDefaultCommand = !control->pressedByKeyboard && control->window->defaultCommand == control->command && control->command;

				bool menuSource;
				bool normal, hover, pressed, disabled, focused;
				uint32_t textShadowColor, textColor;

				OSRectangle contentBounds = control->bounds;
				contentBounds.left += control->horizontalMargin;
				contentBounds.right -= control->horizontalMargin;
				contentBounds.top += control->verticalMargin;
				contentBounds.bottom -= control->verticalMargin;
				if (control->repaintCustomOnly && !message->paint.force) {
					goto repaintCustom;
				} else {
					control->repaintCustomOnly = false;
				}
				
				if (control->drawParentBackground) {
					OSMessage m = *message;
					m.type = OS_MESSAGE_PAINT_BACKGROUND;
					m.paintBackground.surface = message->paint.surface;
					ClipRectangle(message->paint.clip, control->bounds, &m.paintBackground.clip);
					OSSendMessage(control->parent, &m);
				}

				menuSource = false;

				for (uintptr_t i = 0; i < openMenuCount; i++) {
					if (openMenus[i].source == control) {
						menuSource = true;
						break;
					}
				}

				disabled = control->disabled;
				pressed = ((control->window->pressed == control && (control->window->hover == control || control->pressedByKeyboard)) 
						|| (control->window->focus == control && !control->hasFocusedBackground) || menuSource) && !disabled;
				hover = (control->window->hover == control || control->window->pressed == control) && !pressed && !disabled;
				focused = ((control->window->focus == control || (navigateMenuItem == control && navigateMenuMode)) 
						&& control->hasFocusedBackground) && !pressed && !hover && (!disabled || (navigateMenuItem == control && navigateMenuMode)) && !isDefaultCommand;
				normal = !hover && !pressed && !disabled && !focused;

				if (focused) disabled = false;

				control->current1 = ((normal   ? 15 : 0) - control->from1) * control->animationStep / control->finalAnimationStep + control->from1;
				control->current2 = ((hover    ? 15 : 0) - control->from2) * control->animationStep / control->finalAnimationStep + control->from2;
				control->current3 = ((pressed  ? 15 : 0) - control->from3) * control->animationStep / control->finalAnimationStep + control->from3;
				control->current4 = ((disabled ? 15 : 0) - control->from4) * control->animationStep / control->finalAnimationStep + control->from4;
				control->current5 = ((focused  ? 15 : 0) - control->from5) * control->animationStep / control->finalAnimationStep + control->from5;

				if (control->backgrounds) {
					uintptr_t offset = (control->isChecked && control->additionalCheckedBackgrounds) ? 4 : 0;

					if (control->backgrounds[offset + 0]) {
						OSDrawSurfaceClipped(message->paint.surface, OS_SURFACE_UI_SHEET, control->bounds, control->backgrounds[offset + 0]->region, 
								control->backgrounds[offset + 0]->border, control->backgrounds[offset + 0]->drawMode, 0xFF, message->paint.clip);
					}

					if (control->backgrounds[offset + 2] && control->current2) {
						OSDrawSurfaceClipped(message->paint.surface, OS_SURFACE_UI_SHEET, control->bounds, control->backgrounds[offset + 2]->region, 
								control->backgrounds[offset + 2]->border, control->backgrounds[offset + 2]->drawMode, 
								control->current2 == 15 ? 0xFF : 0xF * control->current2, message->paint.clip);
					}

					if (control->backgrounds[offset + 3] && control->current3) {
						OSDrawSurfaceClipped(message->paint.surface, OS_SURFACE_UI_SHEET, control->bounds, control->backgrounds[3]->region, 
								control->backgrounds[3]->border, control->backgrounds[3]->drawMode, 
								control->current3 == 15 ? 0xFF : 0xF * control->current3, message->paint.clip);
					}

					if (control->backgrounds[offset + 1] && control->current4) {
						OSDrawSurfaceClipped(message->paint.surface, OS_SURFACE_UI_SHEET, control->bounds, control->backgrounds[offset + 1]->region, 
								control->backgrounds[offset + 1]->border, control->backgrounds[offset + 1]->drawMode, 
								control->current4 == 15 ? 0xFF : 0xF * control->current4, message->paint.clip);
					}

					if (control->current5 && control->backgrounds[offset + 4]) {
						OSDrawSurfaceClipped(message->paint.surface, OS_SURFACE_UI_SHEET, control->bounds, control->backgrounds[offset + 4]->region, 
								control->backgrounds[offset + 4]->border, control->backgrounds[offset + 4]->drawMode, 
								control->current5 == 15 ? 0xFF : 0xF * control->current5, message->paint.clip);
					}
				}

				if (control->icon) {
					OSRectangle bounds = contentBounds;
					UIImage *icon = control->icon;
					
					if (control->centerIcons) {
						bounds.left += (bounds.right - bounds.left) / 2 - (icon->region.right - icon->region.left) / 2;
						bounds.right = bounds.left + icon->region.right - icon->region.left;
						bounds.top += (bounds.bottom - bounds.top) / 2 - (icon->region.bottom - icon->region.top) / 2;
						bounds.bottom = bounds.top + icon->region.bottom - icon->region.top;
					} else {
						bounds.right = bounds.left + icon->region.right - icon->region.left;
						bounds.top += (bounds.bottom - bounds.top) / 2 - (icon->region.bottom - icon->region.top) / 2;
						bounds.bottom = bounds.top + icon->region.bottom - icon->region.top;
					}

					uintptr_t offset = (control->isChecked && control->additionalCheckedIcons) ? (control->iconHasFocusVariant ? 5 : 4) : 0;

					if (control->current1 || !control->iconHasVariants) {
						UIImage icon = control->icon->Translate((control->icon->region.right - control->icon->region.left) * (0 + offset), 0);
						OSDrawSurfaceClipped(message->paint.surface, OS_SURFACE_UI_SHEET, bounds, icon.region, 
								icon.border, icon.drawMode, 0xFF, message->paint.clip);
					}

					if (control->current2 && control->iconHasVariants) {
						UIImage icon = control->icon->Translate((control->icon->region.right - control->icon->region.left) * (3 + offset), 0);
						OSDrawSurfaceClipped(message->paint.surface, OS_SURFACE_UI_SHEET, bounds, icon.region,
								icon.border, icon.drawMode, control->current2 == 15 ? 0xFF : 0xF * control->current2, message->paint.clip);
					}

					if (control->current3 && control->iconHasVariants) {
						UIImage icon = control->icon->Translate((control->icon->region.right - control->icon->region.left) * (2 + offset), 0);
						OSDrawSurfaceClipped(message->paint.surface, OS_SURFACE_UI_SHEET, bounds, icon.region,
								icon.border, icon.drawMode, control->current3 == 15 ? 0xFF : 0xF * control->current3, message->paint.clip);
					}

					if (control->current4 && control->iconHasVariants) {
						UIImage icon = control->icon->Translate((control->icon->region.right - control->icon->region.left) * (1 + offset), 0);
						OSDrawSurfaceClipped(message->paint.surface, OS_SURFACE_UI_SHEET, bounds, icon.region,
								icon.border, icon.drawMode, control->current4 == 15 ? 0xFF : 0xF * control->current4, message->paint.clip);
					}

					if (control->current5 && control->iconHasVariants) {
						UIImage icon = control->icon->Translate((control->icon->region.right - control->icon->region.left) * ((control->iconHasFocusVariant ? 4 : 3) + offset), 0);
						OSDrawSurfaceClipped(message->paint.surface, OS_SURFACE_UI_SHEET, bounds, icon.region,
								icon.border, icon.drawMode, control->current5 == 15 ? 0xFF : 0xF * control->current5, message->paint.clip);
					}
				}

				textColor = control->textColor ? control->textColor : TEXT_COLOR_DEFAULT;
				textShadowColor = 0xFFFFFFFF - textColor;

				if (control->disabled && !control->noDisabledTextColorChange) {
#if 0
					textColor ^= TEXT_COLOR_DISABLED;
					textShadowColor = TEXT_COLOR_DISABLED_SHADOW;
#endif
					textColor &= 0xFFFFFF;
					textColor |= 0x80000000;
					textShadowColor &= 0xFFFFFF;
					textShadowColor |= 0x80000000;
				}

				if (!control->customTextRendering) {
					OSRectangle textBounds = control->textBounds;

					if (control->textShadow && !DISABLE_TEXT_SHADOWS) {
						OSRectangle bounds = textBounds;
						bounds.top++; bounds.bottom++; 
						// bounds.left++; bounds.right++;

						OSDrawString(message->paint.surface, bounds, &control->text, control->textSize,
								control->textAlign, textShadowColor, -1, control->textBold, message->paint.clip, 
								control->textShadowBlur ? 3 : 0);
					}

					OSDrawString(message->paint.surface, textBounds, &control->text, control->textSize,
							control->textAlign, textColor, -1, control->textBold, message->paint.clip, 0);
				}

				repaintCustom:;

				{
					OSMessage m = *message;
					m.type = OS_MESSAGE_CUSTOM_PAINT;
					OSSendMessage(control, &m);
				}

				control->repaint = false;
				control->repaintCustomOnly = true;
			}
		} break;

		case OS_MESSAGE_PARENT_UPDATED: {
			control->window = (Window *) message->parentUpdated.window;
			control->animationStep = 16;
			control->finalAnimationStep = 16;
			RepaintControl(control);

			if (control->command) {
				OSSetControlCommand(control, control->command);
			}
		} break;

		case OS_MESSAGE_DESTROY: {
			if (control->timerControlItem.list) {
				control->window->timerControls.Remove(&control->timerControlItem);
			}

			if (control->commandItem.list) {
				control->commandItem.RemoveFromList();
			}

			if (control->window->hover == control) control->window->hover = nullptr;
			if (control->window->pressed == control) control->window->pressed = nullptr;
			if (control->window->defaultFocus == control) control->window->defaultFocus = nullptr;
			if (control->window->focus == control) OSRemoveFocusedControl(control->window, true);

			GUIFree(control->text.buffer);
			GUIFree(control);
		} break;

		case OS_MESSAGE_HIT_TEST: {
			message->hitTest.result = IsPointInRectangle(control->inputBounds, message->hitTest.positionX, message->hitTest.positionY);
		} break;

		case OS_MESSAGE_MOUSE_RIGHT_RELEASED: {
			if (control->rightClickMenu) {
				OSCreateMenu(control->rightClickMenu, control, OS_CREATE_MENU_AT_CURSOR, OS_FLAGS_DEFAULT);
			}
		} break;

		case OS_MESSAGE_START_HOVER:
		case OS_MESSAGE_END_HOVER:
		case OS_MESSAGE_START_FOCUS:
		case OS_MESSAGE_END_FOCUS:
		case OS_MESSAGE_END_LAST_FOCUS:
		case OS_MESSAGE_START_PRESS:
		case OS_MESSAGE_END_PRESS: {
			OSAnimateControl(control, message->type == OS_MESSAGE_START_PRESS || message->type == OS_MESSAGE_START_FOCUS);
		} break;

		case OS_MESSAGE_MOUSE_MOVED: {
			if (!IsPointInRectangle(control->inputBounds, message->mouseMoved.newPositionX, message->mouseMoved.newPositionY)) {
				break;
			}

			if (control->window->hover != control) {
				if (!control->disabled || control->keepCustomCursorWhenDisabled) {
					control->window->cursor = (OSCursorStyle) control->cursor;
				}

				control->window->hover = control;

				{
					OSMessage message;
					message.type = OS_MESSAGE_START_HOVER;
					OSSendMessage(control, &message);
				}
			}
		} break;

		case OS_MESSAGE_WM_TIMER: {
			if (control->animationStep == control->finalAnimationStep) {
				control->window->timerControls.Remove(&control->timerControlItem);
			} else {
				control->animationStep++;
				RepaintControl(control);
			}
		} break;

		case OS_MESSAGE_SET_PROPERTY: {
			if (message->setProperty.index == OS_CONTROL_PROPERTY_CURSOR) {
				control->cursor = (uintptr_t) message->setProperty.value;
			}

			SetGUIObjectProperty(control, message);
		} break;

		case OS_MESSAGE_DISABLE: {
			if (control->disabled == message->disable.disabled) break;
			control->disabled = message->disable.disabled;
			
			OSAnimateControl(control, false);
			
			if (control->window->focus == control) {
				OSRemoveFocusedControl(control->window, true);
			}
		} break;

		default: {
			response = OS_CALLBACK_NOT_HANDLED;
		} break;
	}

	return response;
}

void OSSetCursor(OSObject object, OSCursorStyle cursor) {
	Window *window = (Window *) object;
	window->cursor = cursor;
}

static void CreateString(char *text, size_t textBytes, OSString *string, size_t characterCount = 0) {
	GUIFree(string->buffer);
	string->buffer = (char *) GUIAllocate(textBytes, false);
	string->bytes = textBytes;
	string->characters = characterCount;
	OSCopyMemory(string->buffer, text, textBytes);

	char *m = string->buffer;

	if (!string->characters) {
		while (m < string->buffer + textBytes) {
			m = utf8_advance(m);
			string->characters++;
		}
	}
}

static OSCallbackResponse ProcessWindowResizeHandleMessage(OSObject _object, OSMessage *message) {
	OSCallbackResponse response = OS_CALLBACK_HANDLED;
	WindowResizeControl *control = (WindowResizeControl *) _object;

	if (message->type == OS_MESSAGE_MOUSE_DRAGGED && control->direction != RESIZE_NONE) {
		Window *window = control->window;
		OSRectangle bounds, bounds2;
		OSSyscall(OS_SYSCALL_GET_WINDOW_BOUNDS, window->window, (uintptr_t) &bounds, 0, 0);

		bounds2 = bounds;

		int oldWidth = bounds.right - bounds.left;
		int oldHeight = bounds.bottom - bounds.top;

		if (control->direction & RESIZE_LEFT) bounds.left = message->mouseMoved.newPositionXScreen;
		if (control->direction & RESIZE_RIGHT) bounds.right = message->mouseMoved.newPositionXScreen;
		if (control->direction & RESIZE_TOP) bounds.top = message->mouseMoved.newPositionYScreen;
		if (control->direction & RESIZE_BOTTOM) bounds.bottom = message->mouseMoved.newPositionYScreen;

		int newWidth = bounds.right - bounds.left;
		int newHeight = bounds.bottom - bounds.top;

		if (newWidth < window->minimumWidth && control->direction & RESIZE_LEFT) bounds.left = bounds.right - window->minimumWidth;
		if (newWidth < window->minimumWidth && control->direction & RESIZE_RIGHT) bounds.right = bounds.left + window->minimumWidth;
		if (newHeight < window->minimumHeight && control->direction & RESIZE_TOP) bounds.top = bounds.bottom - window->minimumHeight;
		if (newHeight < window->minimumHeight && control->direction & RESIZE_BOTTOM) bounds.bottom = bounds.top + window->minimumHeight;

		bool relayout = true;

		if (control->direction == RESIZE_MOVE) {
			bounds.left = message->mouseDragged.newPositionXScreen - message->mouseDragged.originalPositionX;
			bounds.top = message->mouseDragged.newPositionYScreen - message->mouseDragged.originalPositionY;
			bounds.right = bounds.left + oldWidth;
			bounds.bottom = bounds.top + oldHeight;
			relayout = false;
		}
		
		OSSyscall(OS_SYSCALL_MOVE_WINDOW, window->window, (uintptr_t) &bounds, 0, 0);

		if (relayout) {
			window->width = bounds.right - bounds.left;
			window->height = bounds.bottom - bounds.top;
		
			OSMessage layout;
			layout.type = OS_MESSAGE_LAYOUT;
			layout.layout.left = 0;
			layout.layout.top = 0;
			layout.layout.right = window->width;
			layout.layout.bottom = window->height;
			layout.layout.force = true;
			layout.layout.clip = OS_MAKE_RECTANGLE(0, window->width, 0, window->height);
			OSSendMessage(window->root, &layout);
		}
	} else if (message->type == OS_MESSAGE_LAYOUT_TEXT) {
		control->textBounds = control->bounds;
		control->textBounds.top -= 3;
		control->textBounds.bottom -= 3;
	} else {
		response = OSForwardMessage(_object, OS_MAKE_CALLBACK(ProcessControlMessage, nullptr), message);
	}

	return response;
}

static OSObject CreateWindowResizeHandle(UIImage **images, unsigned direction) {
	WindowResizeControl *control = (WindowResizeControl *) GUIAllocate(sizeof(WindowResizeControl), true);
	control->type = API_OBJECT_CONTROL;
	control->backgrounds = images;
	control->preferredWidth = images[0]->region.right - images[0]->region.left;
	control->preferredHeight = images[0]->region.bottom - images[0]->region.top;
	control->direction = direction;
	control->noAnimations = true;
	control->noDisabledTextColorChange = true;
	control->keepCustomCursorWhenDisabled = true;
	OSSetCallback(control, OS_MAKE_CALLBACK(ProcessWindowResizeHandleMessage, nullptr));

	switch (direction) {
		case RESIZE_LEFT:
		case RESIZE_RIGHT:
			control->cursor = OS_CURSOR_RESIZE_HORIZONTAL;
			break;

		case RESIZE_TOP:
		case RESIZE_BOTTOM:
			control->cursor = OS_CURSOR_RESIZE_VERTICAL;
			break;

		case RESIZE_TOP_RIGHT:
		case RESIZE_BOTTOM_LEFT:
			control->cursor = OS_CURSOR_RESIZE_DIAGONAL_1;
			break;

		case RESIZE_TOP_LEFT:
		case RESIZE_BOTTOM_RIGHT:
			control->cursor = OS_CURSOR_RESIZE_DIAGONAL_2;
			break;

		case RESIZE_MOVE: {
			control->textColor = TEXT_COLOR_TITLEBAR;
			control->textShadow = true;
			control->textBold = true;
			control->textSize = 10;
			control->textShadowBlur = true;
		} break;
	}

	return control;
}

enum CharacterType {
	CHARACTER_INVALID,
	CHARACTER_IDENTIFIER, // A-Z, a-z, 0-9, _, >= 0x7F
	CHARACTER_WHITESPACE, // space, tab, newline
	CHARACTER_OTHER,
};

static CharacterType GetCharacterType(int character) {
	if ((character >= '0' && character <= '9') 
			|| (character >= 'a' && character <= 'z')
			|| (character >= 'A' && character <= 'Z')
			|| (character == '_')
			|| (character >= 0x80)) {
		return CHARACTER_IDENTIFIER;
	}

	if (character == '\n' || character == '\t' || character == ' ') {
		return CHARACTER_WHITESPACE;
	}

	// TODO Treat escape-sequence-likes in the textbox as words?
	return CHARACTER_OTHER;
}

static void MoveCaret(OSString *string, OSCaret *caret, bool right, bool word, bool strongWhitespace = false) {
	if (!string->bytes) return;

	CharacterType type = CHARACTER_INVALID;

	if (word && right) goto checkCharacterType;

	while (true) {
		if (!right) {
			if (caret->character) {
				caret->character--;
				caret->byte = utf8_retreat(string->buffer + caret->byte) - string->buffer;
			} else {
				return; // We cannot move any further left.
			}
		} else {
			if (caret->character != string->characters) {
				caret->character++;
				caret->byte = utf8_advance(string->buffer + caret->byte) - string->buffer;
			} else {
				return; // We cannot move any further right.
			}
		}

		if (!word) {
			return;
		}

		checkCharacterType:;
		CharacterType newType = GetCharacterType(utf8_value(string->buffer + caret->byte));

		if (type == CHARACTER_INVALID) {
			if (newType != CHARACTER_WHITESPACE || strongWhitespace) {
				type = newType;
			}
		} else {
			if (newType != type) {
				if (!right) {
					// We've gone too far.
					MoveCaret(string, caret, true, false);
				}

				break;
			}
		}
	}
}

static void FindCaret(Textbox *control, int positionX, int positionY, bool secondCaret, unsigned clickChainCount) {
	if (clickChainCount >= 3) {
		control->caret.byte = 0;
		control->caret.character = 0;
		control->caret2.byte = control->text.bytes;
		control->caret2.character = control->text.characters;
	} else {
		OSFindCharacterAtCoordinate(control->textBounds, OS_MAKE_POINT(positionX, positionY), 
				&control->text, control->textAlign, &control->caret2, control->textSize, control->scrollX);

		if (!secondCaret) {
			control->caret = control->caret2;

			if (clickChainCount == 2) {
				MoveCaret(&control->text, &control->caret, false, true, true);
				MoveCaret(&control->text, &control->caret2, true, true, true);
				control->wordSelectionAnchor  = control->caret;
				control->wordSelectionAnchor2 = control->caret2;
			}
		} else {
			if (clickChainCount == 2) {
				if (control->caret2.byte < control->caret.byte) {
					MoveCaret(&control->text, &control->caret2, false, true);
					control->caret = control->wordSelectionAnchor2;
				} else {
					MoveCaret(&control->text, &control->caret2, true, true);
					control->caret = control->wordSelectionAnchor;
				}
			}
		}
	}

	control->window->caretBlinkPause = CARET_BLINK_PAUSE;
}

static void RemoveSelectedText(Textbox *control) {
	if (control->caret.byte == control->caret2.byte) return;
	if (control->caret.byte < control->caret2.byte) {
		OSCaret temp = control->caret2;
		control->caret2 = control->caret;
		control->caret = temp;
	}
	int bytes = control->caret.byte - control->caret2.byte;
	OSCopyMemory(control->text.buffer + control->caret2.byte, control->text.buffer + control->caret.byte, control->text.bytes - control->caret.byte);
	control->text.characters -= control->caret.character - control->caret2.character;
	control->text.bytes -= bytes;
	control->caret = control->caret2;
}

OSCallbackResponse ProcessTextboxMessage(OSObject object, OSMessage *message) {
	Textbox *control = (Textbox *) message->context;

	OSCallbackResponse result = OS_CALLBACK_NOT_HANDLED;
	static int lastClickChainCount = 1;
	bool ensureCaretVisible = false;

	if (message->type == OS_MESSAGE_CUSTOM_PAINT) {
#define TEXTBOX_MARGIN (4)
		DrawString(message->paint.surface, OS_MAKE_RECTANGLE(control->textBounds.left - TEXTBOX_MARGIN, 
					control->textBounds.right + TEXTBOX_MARGIN, control->textBounds.top, control->textBounds.bottom), 
				&control->text, control->textAlign, control->textColor, -1, 
				control->window->focus == control ? TEXTBOX_SELECTED_COLOR_1 : TEXTBOX_SELECTED_COLOR_2,
				{0, 0}, nullptr, control->caret.character, (control->window->lastFocus == control 
					&& (control->style != OS_TEXTBOX_STYLE_COMMAND || control->window->focus == control))
				&& !control->disabled ? control->caret2.character : control->caret.character, 
				control->window->focus != control || control->caretBlink || control->disabled,
				control->textSize, fontRegular, message->paint.clip, 0, control->scrollX - TEXTBOX_MARGIN);

#if 0
		OSDrawSurfaceClipped(message->paint.surface, OS_SURFACE_UI_SHEET, control->textBounds, 
				testImage.region, testImage.border, OS_DRAW_MODE_STRECH, 0xFF, message->paint.clip);
#endif

		result = OS_CALLBACK_HANDLED;
	} else if (message->type == OS_MESSAGE_LAYOUT_TEXT) {
		control->textBounds = control->bounds;
		control->textBounds.left += 6;
		control->textBounds.right -= 6;
		result = OS_CALLBACK_HANDLED;
	} else if (message->type == OS_MESSAGE_CARET_BLINK) {
		if (control->window->caretBlinkPause) {
			control->window->caretBlinkPause--;
			control->caretBlink = false;
		} else {
			control->caretBlink = !control->caretBlink;
		}

		result = OS_CALLBACK_HANDLED;
		RepaintControl(control);
	} else if (message->type == OS_MESSAGE_END_FOCUS) {
		ensureCaretVisible = true;

		OSMessage message;

		if (!control->sentEditResultNotification) {
			message.type = OS_NOTIFICATION_CANCEL_EDIT;
			OSForwardMessage(control, control->notificationCallback, &message);

			if (control->style == OS_TEXTBOX_STYLE_COMMAND) {
				CreateString(control->previousString.buffer, control->previousString.bytes, &control->text, control->previousString.characters);
			}
		}

		message.type = OS_NOTIFICATION_END_EDIT;
		OSForwardMessage(control, control->notificationCallback, &message);
		control->cursor = control->style == OS_TEXTBOX_STYLE_COMMAND ? OS_CURSOR_NORMAL : OS_CURSOR_TEXT;
		control->window->cursor = (OSCursorStyle) control->cursor;

		GUIFree(control->previousString.buffer);
		control->previousString.buffer = nullptr;
	} else if (message->type == OS_MESSAGE_START_FOCUS) {
		ensureCaretVisible = true;

		control->caretBlink = false;
		control->window->caretBlinkPause = CARET_BLINK_PAUSE;

		control->sentEditResultNotification = false;

		control->caret.byte = 0;
		control->caret.character = 0;
		control->caret2.byte = control->text.bytes;
		control->caret2.character = control->text.characters;

		OSEnableCommand(control->window, osCommandPaste, ClipboardTextBytes());
		OSEnableCommand(control->window, osCommandSelectAll, true);

		OSSetCommandNotificationCallback(control->window, osCommandPaste, OS_MAKE_CALLBACK(ProcessTextboxMessage, control));
		OSSetCommandNotificationCallback(control->window, osCommandSelectAll, OS_MAKE_CALLBACK(ProcessTextboxMessage, control));
		OSSetCommandNotificationCallback(control->window, osCommandCopy, OS_MAKE_CALLBACK(ProcessTextboxMessage, control));
		OSSetCommandNotificationCallback(control->window, osCommandCut, OS_MAKE_CALLBACK(ProcessTextboxMessage, control));
		OSSetCommandNotificationCallback(control->window, osCommandDelete, OS_MAKE_CALLBACK(ProcessTextboxMessage, control));

		{
			OSMessage message;
			message.type = OS_NOTIFICATION_START_EDIT;
			OSForwardMessage(control, control->notificationCallback, &message);
			control->cursor = OS_CURSOR_TEXT;
			control->window->cursor = (OSCursorStyle) control->cursor;

			if (control->style == OS_TEXTBOX_STYLE_COMMAND) {
				CreateString(control->text.buffer, control->text.bytes, &control->previousString, control->text.characters);
			}
		}

		OSSyscall(OS_SYSCALL_RESET_CLICK_CHAIN, 0, 0, 0, 0);
	} else if (message->type == OS_MESSAGE_CLIPBOARD_UPDATED) {
		OSEnableCommand(control->window, osCommandPaste, ClipboardTextBytes());
	} else if (message->type == OS_MESSAGE_END_LAST_FOCUS) {
		ensureCaretVisible = true;

		OSDisableCommand(control->window, osCommandPaste, true);
		OSDisableCommand(control->window, osCommandSelectAll, true);
		OSDisableCommand(control->window, osCommandCopy, true);
		OSDisableCommand(control->window, osCommandCut, true);
		OSDisableCommand(control->window, osCommandDelete, true);
	} else if (message->type == OS_NOTIFICATION_COMMAND) {
		ensureCaretVisible = true;
		// TODO Code copy-and-pasted from KEY_TYPED below.

		if (message->command.command == osCommandSelectAll) {
			control->caret.byte = 0;
			control->caret.character = 0;

			control->caret2.byte = control->text.bytes;
			control->caret2.character = control->text.characters;
		}

		if (message->command.command == osCommandCopy || message->command.command == osCommandCut) {
			OSString string;
			int length = control->caret.byte - control->caret2.byte;
			if (length < 0) length = -length;
			string.bytes = length;
			if (control->caret.byte > control->caret2.byte) string.buffer = control->text.buffer + control->caret2.byte;
			else string.buffer = control->text.buffer + control->caret.byte;
			CopyText(string);
		}

		if (message->command.command == osCommandDelete || message->command.command == osCommandCut || message->command.command == osCommandPaste) {
			RemoveSelectedText(control);
		}

		if (message->command.command == osCommandPaste) {
			int bytes = ClipboardTextBytes();
			char *old = control->text.buffer;
			control->text.buffer = (char *) GUIAllocate(control->text.bytes + bytes, false);
			OSCopyMemory(control->text.buffer, old, control->caret.byte);
			OSCopyMemory(control->text.buffer + control->caret.byte + bytes, old + control->caret.byte, control->text.bytes - control->caret.byte);
			GUIFree(old);
			char *c = control->text.buffer + control->caret.byte;
			char *d = c;
			OSSyscall(OS_SYSCALL_PASTE_TEXT, bytes, (uintptr_t) c, 0, 0);
			size_t characters = 0;
			while (c < d + bytes) { characters++; c = utf8_advance(c); }  
			control->text.characters += characters;
			control->caret.character += characters;
			control->caret.byte += bytes;
			control->text.bytes += bytes;
			control->caret2 = control->caret;
		}

		RepaintControl(control);
	} else if (message->type == OS_MESSAGE_MOUSE_LEFT_PRESSED) {
		FindCaret(control, message->mousePressed.positionX, message->mousePressed.positionY, false, message->mousePressed.clickChainCount);
		lastClickChainCount = message->mousePressed.clickChainCount;
		RepaintControl(control);
	} else if (message->type == OS_MESSAGE_START_DRAG 
			|| (message->type == OS_MESSAGE_MOUSE_RIGHT_PRESSED && control->caret.byte == control->caret2.byte)) {
		ensureCaretVisible = true;
		FindCaret(control, message->mouseDragged.originalPositionX, message->mouseDragged.originalPositionY, true, lastClickChainCount);
		control->caret = control->caret2;
		RepaintControl(control);
	} else if (message->type == OS_MESSAGE_MOUSE_DRAGGED) {
		ensureCaretVisible = true;
		FindCaret(control, message->mouseDragged.newPositionX, message->mouseDragged.newPositionY, true, lastClickChainCount);
		RepaintControl(control);
	} else if (message->type == OS_MESSAGE_TEXT_UPDATED) {
		if (control->caret2.byte > control->text.bytes) {
			control->caret2.byte = control->text.bytes;
		}

		control->caret = control->caret2;
		ensureCaretVisible = true;
	} else if (message->type == OS_MESSAGE_DESTROY) {
		GUIFree(control->previousString.buffer);
	} else if (message->type == OS_MESSAGE_KEY_TYPED) {
		ensureCaretVisible = true;

		control->caretBlink = false;
		control->window->caretBlinkPause = CARET_BLINK_PAUSE;

		int ic = -1, isc = -1;
		bool withAltOrCtrl = false;

		result = OS_CALLBACK_HANDLED;

		switch (message->keyboard.scancode) {
			case OS_SCANCODE_A: ic = 'a'; isc = 'A'; break;
			case OS_SCANCODE_B: ic = 'b'; isc = 'B'; break;
			case OS_SCANCODE_C: ic = 'c'; isc = 'C'; break;
			case OS_SCANCODE_D: ic = 'd'; isc = 'D'; break;
			case OS_SCANCODE_E: ic = 'e'; isc = 'E'; break;
			case OS_SCANCODE_F: ic = 'f'; isc = 'F'; break;
			case OS_SCANCODE_G: ic = 'g'; isc = 'G'; break;
			case OS_SCANCODE_H: ic = 'h'; isc = 'H'; break;
			case OS_SCANCODE_I: ic = 'i'; isc = 'I'; break;
			case OS_SCANCODE_J: ic = 'j'; isc = 'J'; break;
			case OS_SCANCODE_K: ic = 'k'; isc = 'K'; break;
			case OS_SCANCODE_L: ic = 'l'; isc = 'L'; break;
			case OS_SCANCODE_M: ic = 'm'; isc = 'M'; break;
			case OS_SCANCODE_N: ic = 'n'; isc = 'N'; break;
			case OS_SCANCODE_O: ic = 'o'; isc = 'O'; break;
			case OS_SCANCODE_P: ic = 'p'; isc = 'P'; break;
			case OS_SCANCODE_Q: ic = 'q'; isc = 'Q'; break;
			case OS_SCANCODE_R: ic = 'r'; isc = 'R'; break;
			case OS_SCANCODE_S: ic = 's'; isc = 'S'; break;
			case OS_SCANCODE_T: ic = 't'; isc = 'T'; break;
			case OS_SCANCODE_U: ic = 'u'; isc = 'U'; break;
			case OS_SCANCODE_V: ic = 'v'; isc = 'V'; break;
			case OS_SCANCODE_W: ic = 'w'; isc = 'W'; break;
			case OS_SCANCODE_X: ic = 'x'; isc = 'X'; break;
			case OS_SCANCODE_Y: ic = 'y'; isc = 'Y'; break;
			case OS_SCANCODE_Z: ic = 'z'; isc = 'Z'; break;
			case OS_SCANCODE_0: ic = '0'; isc = ')'; break;
			case OS_SCANCODE_1: ic = '1'; isc = '!'; break;
			case OS_SCANCODE_2: ic = '2'; isc = '@'; break;
			case OS_SCANCODE_3: ic = '3'; isc = '#'; break;
			case OS_SCANCODE_4: ic = '4'; isc = '$'; break;
			case OS_SCANCODE_5: ic = '5'; isc = '%'; break;
			case OS_SCANCODE_6: ic = '6'; isc = '^'; break;
			case OS_SCANCODE_7: ic = '7'; isc = '&'; break;
			case OS_SCANCODE_8: ic = '8'; isc = '*'; break;
			case OS_SCANCODE_9: ic = '9'; isc = '('; break;
			case OS_SCANCODE_SLASH: 	ic = '/';  isc = '?'; break;
			case OS_SCANCODE_BACKSLASH: 	ic = '\\'; isc = '|'; break;
			case OS_SCANCODE_LEFT_BRACE: 	ic = '[';  isc = '{'; break;
			case OS_SCANCODE_RIGHT_BRACE: 	ic = ']';  isc = '}'; break;
			case OS_SCANCODE_EQUALS: 	ic = '=';  isc = '+'; break;
			case OS_SCANCODE_BACKTICK: 	ic = '`';  isc = '~'; break;
			case OS_SCANCODE_HYPHEN: 	ic = '-';  isc = '_'; break;
			case OS_SCANCODE_SEMICOLON: 	ic = ';';  isc = ':'; break;
			case OS_SCANCODE_QUOTE: 	ic = '\''; isc = '"'; break;
			case OS_SCANCODE_COMMA: 	ic = ',';  isc = '<'; break;
			case OS_SCANCODE_PERIOD: 	ic = '.';  isc = '>'; break;
			case OS_SCANCODE_SPACE: 	ic = ' ';  isc = ' '; break;

			case OS_SCANCODE_BACKSPACE: {
				if (control->caret.byte == control->caret2.byte && control->caret.byte) {
					MoveCaret(&control->text, &control->caret2, false, message->keyboard.ctrl);
					int bytes = control->caret.byte - control->caret2.byte;
					OSCopyMemory(control->text.buffer + control->caret2.byte, control->text.buffer + control->caret.byte, control->text.bytes - control->caret.byte);
					control->text.characters -= 1;
					control->text.bytes -= bytes;
					control->caret = control->caret2;
				} else {
					RemoveSelectedText(control);
				}

				withAltOrCtrl = true;
			} break;

			case OS_SCANCODE_DELETE: {
				if (control->caret.byte == control->caret2.byte && control->caret.byte != control->text.bytes) {
					MoveCaret(&control->text, &control->caret, true, message->keyboard.ctrl);
					int bytes = control->caret.byte - control->caret2.byte;
					OSCopyMemory(control->text.buffer + control->caret2.byte, control->text.buffer + control->caret.byte, control->text.bytes - control->caret.byte);
					control->text.characters -= 1;
					control->text.bytes -= bytes;
					control->caret = control->caret2;
				} else {
					RemoveSelectedText(control);
				}

				withAltOrCtrl = true;
			} break;

			case OS_SCANCODE_LEFT_ARROW: {
				if (message->keyboard.shift) {
					MoveCaret(&control->text, &control->caret2, false, message->keyboard.ctrl);
				} else {
					bool move = control->caret2.byte == control->caret.byte;

					if (control->caret2.byte < control->caret.byte) control->caret = control->caret2;
					else control->caret2 = control->caret;

					if (move) {
						MoveCaret(&control->text, &control->caret2, false, message->keyboard.ctrl);
						control->caret = control->caret2;
					}
				}

				withAltOrCtrl = true;
			} break;

			case OS_SCANCODE_RIGHT_ARROW: {
				if (message->keyboard.shift) {
					MoveCaret(&control->text, &control->caret2, true, message->keyboard.ctrl);
				} else {
					bool move = control->caret2.byte == control->caret.byte;

					if (control->caret2.byte > control->caret.byte) control->caret = control->caret2;
					else control->caret2 = control->caret;

					if (move) {
						MoveCaret(&control->text, &control->caret2, true, message->keyboard.ctrl);
						control->caret = control->caret2;
					}
				}

				withAltOrCtrl = true;
			} break;

			case OS_SCANCODE_HOME: {
				control->caret2.byte = 0;
				control->caret2.character = 0;
				if (!message->keyboard.shift) control->caret = control->caret2;

				withAltOrCtrl = true;
			} break;

			case OS_SCANCODE_END: {
				control->caret2.byte = control->text.bytes;
				control->caret2.character = control->text.characters;
				if (!message->keyboard.shift) control->caret = control->caret2;

				withAltOrCtrl = true;
			} break;

			case OS_SCANCODE_ENTER: {
				OSMessage message;
				message.type = OS_NOTIFICATION_CONFIRM_EDIT;
				OSForwardMessage(control, control->notificationCallback, &message);
				control->sentEditResultNotification = true;

				message.type = OS_NOTIFICATION_COMMAND;
				message.command.window = control->window;
				message.command.command = control->command;

				OSCallbackResponse response = OSForwardMessage(control, control->notificationCallback, &message);

				if (response == OS_CALLBACK_REJECTED) {
					CreateString(control->previousString.buffer, control->previousString.bytes, &control->text, control->previousString.characters);

					OSMessage m;
					m.type = OS_MESSAGE_TEXT_UPDATED;
					OSSendMessage(control, &m);
				}

				OSRemoveFocusedControl(control->window, true);

				withAltOrCtrl = true;
			} break;

			default: {
				result = OS_CALLBACK_NOT_HANDLED;
			} break;
		}

		if ((message->keyboard.ctrl || message->keyboard.alt) && !withAltOrCtrl) {
			return OS_CALLBACK_NOT_HANDLED;
		}

		if (ic != -1 && !message->keyboard.alt && !message->keyboard.ctrl) {
			RemoveSelectedText(control);

			char data[4];
			int bytes = utf8_encode(message->keyboard.shift ? isc : ic, data);
			char *old = control->text.buffer;
			control->text.buffer = (char *) GUIAllocate(control->text.bytes + bytes, false);
			OSCopyMemory(control->text.buffer, old, control->caret.byte);
			OSCopyMemory(control->text.buffer + control->caret.byte + bytes, old + control->caret.byte, control->text.bytes - control->caret.byte);
			GUIFree(old);
			OSCopyMemory(control->text.buffer + control->caret.byte, data, bytes);
			control->text.characters += 1;
			control->caret.character++;
			control->caret.byte += bytes;
			control->text.bytes += bytes;
			control->caret2 = control->caret;
		}

		RepaintControl(control);
	}

	if (control->window && control->window->lastFocus == control) {
		bool noSelection = control->caret.byte == control->caret2.byte;
		OSDisableCommand(control->window, osCommandCopy, noSelection);
		OSDisableCommand(control->window, osCommandCut, noSelection);
		OSDisableCommand(control->window, osCommandDelete, noSelection);
	}

	if (ensureCaretVisible) {
		int caretX = MeasureStringWidth(control->text.buffer, control->caret2.byte, control->textSize, fontRegular) - control->scrollX;

		int fullWidth = MeasureStringWidth(control->text.buffer, control->text.bytes, control->textSize, fontRegular);
		int controlWidth = control->textBounds.right - control->textBounds.left;

		if (caretX < 0) {
			control->scrollX += caretX;
		} else if (caretX > controlWidth) {
			control->scrollX += caretX - controlWidth;
		} else if (fullWidth - control->scrollX < controlWidth && fullWidth > controlWidth) {
			control->scrollX = fullWidth - controlWidth;
		} else if (fullWidth <= controlWidth) {
			control->scrollX = 0;
		}
	}

	if (result == OS_CALLBACK_NOT_HANDLED) {
		result = ProcessControlMessage(object, message);
	}

	return result;
}

OSObject OSCreateTextbox(OSTextboxStyle style) {
	Textbox *control = (Textbox *) GUIAllocate(sizeof(Textbox), true);

	control->type = API_OBJECT_CONTROL;
	control->tabStop = true;
	control->preferredWidth = 160;
	control->drawParentBackground = true;
	control->backgrounds = style == OS_TEXTBOX_STYLE_COMMAND ? textboxCommandBackgrounds : textboxBackgrounds;
	control->cursor = style == OS_TEXTBOX_STYLE_COMMAND ? OS_CURSOR_NORMAL : OS_CURSOR_TEXT;
	control->focusable = true;
	control->customTextRendering = true;
	control->textAlign = OS_DRAW_STRING_VALIGN_CENTER | OS_DRAW_STRING_HALIGN_LEFT;
	control->textSize = style == OS_TEXTBOX_STYLE_LARGE ? FONT_SIZE * 2 : FONT_SIZE;
	control->textColor = TEXT_COLOR_DEFAULT;
	control->preferredHeight = GetLineHeight(fontRegular, control->textSize) + 5;
	control->style = style;
	control->rightClickMenu = osMenuTextboxContext;

	OSSetCallback(control, OS_MAKE_CALLBACK(ProcessTextboxMessage, control));

	return control;
}

static void IssueCommand(Control *control, OSCommand *command = nullptr, Window *window = nullptr) {
	if (!window) window = control->window;
	if (!command) command = control->command;

	if (control ? (control->disabled) : (window->commands[command->identifier].disabled)) {
		return;
	}

	bool checkable = control ? control->checkable : command->checkable;
	bool radioCheck = control ? control->radioCheck : command->radioCheck;
	bool isChecked = control ? control->isChecked : window->commands[command->identifier].checked;

	if (radioCheck) {
		isChecked = true;

		if (command) {
			OSCheckCommand(window, command, isChecked);
		}
	} else if (checkable) {
		// Update the checked state.
		isChecked = !isChecked;

		if (command) {
			// Update the command.
			OSCheckCommand(window, command, isChecked);
		} else {
			control->isChecked = isChecked;
		}
	}

	OSMessage message;
	message.type = OS_NOTIFICATION_COMMAND;
	message.command.checked = isChecked;
	message.command.window = window;
	message.command.command = command;

	if (control) {
		OSForwardMessage(control, control->notificationCallback, &message);
	} else {
		OSForwardMessage(nullptr, window->commands[command->identifier].notificationCallback, &message);
	}

	if (window->flags & OS_CREATE_WINDOW_MENU) {
		message.type = OS_MESSAGE_DESTROY;
		OSSendMessage(openMenus[0].window, &message);
	}
}

void OSIssueCommand(OSCommand *command, OSObject window) {
	IssueCommand(nullptr, command, (Window *) window);
}

OSObject OSGetWindow(OSObject object) {
	return ((GUIObject *) object)->window;
}

OSCallbackResponse ProcessButtonMessage(OSObject object, OSMessage *message) {
	Control *control = (Control *) object;
	OSCallbackResponse result = OS_CALLBACK_NOT_HANDLED;
	
	if (message->type == OS_MESSAGE_CLICKED) {
		IssueCommand(control);
	} else if (message->type == OS_MESSAGE_KEY_PRESSED) {
		if (message->keyboard.scancode == OS_SCANCODE_SPACE) {
			control->window->pressed = control;
			control->pressedByKeyboard = true;
			OSAnimateControl(control, true);
			result = OS_CALLBACK_HANDLED;
		}
	} else if (message->type == OS_MESSAGE_KEY_RELEASED) {
		if (message->keyboard.scancode == OS_SCANCODE_SPACE) {
			control->window->pressed = nullptr;
			control->pressedByKeyboard = false;
			OSAnimateControl(control, false);
			result = OS_CALLBACK_HANDLED;
			IssueCommand(control);
		}
	} 

	if (result == OS_CALLBACK_NOT_HANDLED) {
		result = OSForwardMessage(object, OS_MAKE_CALLBACK(ProcessControlMessage, nullptr), message);
	}

	if (message->type == OS_MESSAGE_PARENT_UPDATED) {
		if (control->window && control->window->defaultCommand == control->command && control->command) {
			if (control->backgrounds == buttonBackgrounds) {
				control->backgrounds = buttonDefaultBackgrounds;
			}
		}
	}

	return result;
}

OSObject OSCreateBlankControl(int width, int height, OSCursorStyle cursor, unsigned flags) {
	Control *control = (Control *) GUIAllocate(sizeof(Control), true);
	control->type = API_OBJECT_CONTROL;
	control->preferredWidth = width;
	control->preferredHeight = height;
	control->drawParentBackground = flags & OS_BLANK_CONTROL_DRAW_PARENT_BACKGROUND;
	control->ignoreActivationClicks = flags & OS_BLANK_CONTROL_IGNORE_ACTIVATION_CLICKS;
	control->focusable = flags & OS_BLANK_CONTROL_FOCUSABLE;
	control->tabStop = flags & OS_BLANK_CONTROL_TAB_STOP;
	control->noAnimations = true;
	control->customTextRendering = true;
	control->cursor = cursor;
	OSSetCallback(control, OS_MAKE_CALLBACK(ProcessControlMessage, nullptr));
	return control;
}

OSObject OSCreateButton(OSCommand *command, OSButtonStyle style) {
	Control *control = (Control *) GUIAllocate(sizeof(Control), true);
	control->type = API_OBJECT_CONTROL;
	control->tabStop = true;

	control->preferredWidth = 80;
	control->preferredHeight = 21;
	control->textColor = TEXT_COLOR_DEFAULT;

	control->drawParentBackground = true;
	control->ignoreActivationClicks = true;

	OSSetControlCommand(control, command);

	if (style == OS_BUTTON_STYLE_TOOLBAR) {
		control->textColor = TEXT_COLOR_TOOLBAR;
		control->horizontalMargin = 12;
		control->preferredWidth = 0;
		control->preferredHeight = 31;
		control->textShadowBlur = true;
		control->textShadow = true;
		control->backgrounds = toolbarItemBackgrounds;
		control->additionalCheckedBackgrounds = true;
	} else if (style == OS_BUTTON_STYLE_TOOLBAR_ICON_ONLY) {
		control->horizontalMargin = 6;
		control->preferredWidth = 32;
		control->preferredHeight = 31;
		control->centerIcons = true;
		control->backgrounds = toolbarItemBackgrounds;
		control->additionalCheckedBackgrounds = true;
	} else {
		control->focusable = true;
		control->hasFocusedBackground = true;

		if (control->checkable) {
			control->textAlign = OS_DRAW_STRING_VALIGN_CENTER | OS_DRAW_STRING_HALIGN_LEFT;
			control->icon = control->radioCheck ? &radioboxHover : &checkboxHover;
			control->iconHasVariants = true;
			control->iconHasFocusVariant = true;
			control->additionalCheckedIcons = true;
		} else {
			control->backgrounds = command->dangerous ? buttonDangerousBackgrounds : buttonBackgrounds;
		}

		if (style == OS_BUTTON_STYLE_REPEAT) {
			control->useClickRepeat = true;
		}
	}

	OSSetCallback(control, OS_MAKE_CALLBACK(ProcessButtonMessage, nullptr));

	if (style != OS_BUTTON_STYLE_TOOLBAR_ICON_ONLY) {
		OSSetText(control, command->label, command->labelBytes, OS_RESIZE_MODE_GROW_ONLY);
	} 

	return control;
}

static int FindObjectInGrid(Grid *grid, OSObject object) {
	if (grid->type != API_OBJECT_GRID) {
		OSCrashProcess(OS_FATAL_ERROR_INVALID_PANE_OBJECT);
	}

	for (uintptr_t i = 0; i < grid->columns * grid->rows; i++) {
		if (grid->objects[i] == object && object) {
			return i;
		}
	}

	return grid->columns * grid->rows;
}

OSCallbackResponse ProcessMenuItemMessage(OSObject object, OSMessage *message) {
	MenuItem *control = (MenuItem *) object;
	OSCallbackResponse result = OS_CALLBACK_NOT_HANDLED;

	if (message->type == OS_MESSAGE_LAYOUT_TEXT) {
		if (!control->menubar) {
			control->textBounds = control->bounds;
			// Leave room for the icons.
			control->textBounds.left += 24;
			control->textBounds.right -= 4;
			result = OS_CALLBACK_HANDLED;
		}
	} else if (message->type == OS_MESSAGE_END_HOVER) {
		if (navigateMenuItem == control) {
			navigateMenuItem = nullptr;
		}
	} else if (message->type == OS_MESSAGE_CLICKED 
			|| message->type == OS_MESSAGE_START_HOVER || message->type == OS_MESSAGE_START_FOCUS) {
		if (control->window->hover != control) {
			OSMessage m;
			m.type = OS_MESSAGE_END_HOVER;
			OSSendMessage(control->window->hover, &m);
			control->window->hover = control;
		}

		if (navigateMenuItem) {
			OSAnimateControl(navigateMenuItem, false);
		}

		bool openMenu = control->item.type == OSMenuItem::SUBMENU;

		if (message->type == OS_MESSAGE_START_HOVER && !openMenuCount) {
			openMenu = false;
		} else {
			navigateMenuMode = true;
			navigateMenuItem = control;
		}

		if (openMenuCount && openMenus[openMenuCount - 1].source == control) {
			openMenu = false;
		}

		if (message->type == OS_MESSAGE_START_FOCUS && !control->menubar) {
			openMenu = false;
		}

		if (openMenu) {
			OSCreateMenu((OSMenuSpecification *) control->item.value, control, OS_CREATE_MENU_AT_SOURCE, 
					control->menubar ? OS_CREATE_MENU_FROM_MENUBAR : OS_CREATE_SUBMENU);

			OSAnimateControl(control, true);

			if (message->type == OS_MESSAGE_CLICKED) {
				result = OS_CALLBACK_HANDLED;
			}
		}
	} else if (message->type == OS_MESSAGE_CUSTOM_PAINT && !control->menubar) {
		int xOffset = 4, yOffset = 2;
		OSRectangle iconRegion = OS_MAKE_RECTANGLE(control->bounds.left + xOffset, control->bounds.left + xOffset + 16, control->bounds.top + yOffset, control->bounds.top + yOffset + 16);
		OSRectangle clip;
		ClipRectangle(iconRegion, message->paint.clip, &clip);

		if (control->item.type == OSMenuItem::SUBMENU) {
			OSDrawSurfaceClipped(message->paint.surface, OS_SURFACE_UI_SHEET, iconRegion, 
					menuIconSub.region, menuIconSub.border, menuIconSub.drawMode, 0xFF, clip);
		} else if (control->isChecked && control->radioCheck) {
			OSDrawSurfaceClipped(message->paint.surface, OS_SURFACE_UI_SHEET, iconRegion, 
					menuIconRadio.region, menuIconRadio.border, menuIconRadio.drawMode, 0xFF, clip);
		} else if (control->isChecked) {
			OSDrawSurfaceClipped(message->paint.surface, OS_SURFACE_UI_SHEET, iconRegion, 
					menuIconCheck.region, menuIconCheck.border, menuIconCheck.drawMode, 0xFF, clip);
		}
	}

	if (result == OS_CALLBACK_NOT_HANDLED) {
		result = OSForwardMessage(object, OS_MAKE_CALLBACK(ProcessButtonMessage, nullptr), message);
	}

	return result;
}

static OSObject CreateMenuItem(OSMenuItem item, bool menubar) {
	MenuItem *control = (MenuItem *) GUIAllocate(sizeof(MenuItem), true);
	control->type = API_OBJECT_CONTROL;

	control->preferredWidth = !menubar ? 70 : 21;
	control->preferredHeight = 21;
	control->drawParentBackground = true;
	control->textAlign = menubar ? OS_FLAGS_DEFAULT : (OS_DRAW_STRING_VALIGN_CENTER | OS_DRAW_STRING_HALIGN_LEFT);
	control->backgrounds = menuItemBackgrounds;
	control->ignoreActivationClicks = menubar;
	control->noAnimations = true;
	control->hasFocusedBackground = true;
	control->tabStop = true;

	control->item = item;
	control->menubar = menubar;
	// control->horizontalMargin = menubar ? 0 : 4;

	if (item.type == OSMenuItem::COMMAND) {
		OSCommand *command = (OSCommand *) item.value;
		OSSetControlCommand(control, command);
		OSSetText(control, command->label, command->labelBytes, OS_RESIZE_MODE_GROW_ONLY);
	} else if (item.type == OSMenuItem::SUBMENU) {
		OSMenuSpecification *menu = (OSMenuSpecification *) item.value;
		OSSetText(control, menu->name, menu->nameBytes, OS_RESIZE_MODE_GROW_ONLY);
	}

	if (menubar) {
		control->preferredWidth += 4;
	} else {
		control->preferredWidth += 32;
	}

	OSSetCallback(control, OS_MAKE_CALLBACK(ProcessMenuItemMessage, nullptr));

	return control;
}

OSCallbackResponse ProcessMenuSeparatorMessage(OSObject object, OSMessage *message) {
	Control *control = (Control *) object;
	OSCallbackResponse response = OS_CALLBACK_NOT_HANDLED;

	if (message->type == OS_MESSAGE_CUSTOM_PAINT) {
		OSDrawSurface(message->paint.surface, OS_SURFACE_UI_SHEET, 
				OS_MAKE_RECTANGLE(control->bounds.left, control->bounds.right + 1, control->bounds.top + 1, control->bounds.bottom - 1),
				lineHorizontal.region, lineHorizontal.border, lineHorizontal.drawMode, 0xFF);
		response = OS_CALLBACK_HANDLED;
	}

	if (response == OS_CALLBACK_NOT_HANDLED) {
		response = OSForwardMessage(object, OS_MAKE_CALLBACK(ProcessControlMessage, nullptr), message);
	}

	return response;
}

OSObject CreateMenuSeparator() {
	Control *control = (Control *) GUIAllocate(sizeof(Control), true);
	control->type = API_OBJECT_CONTROL;
	control->drawParentBackground = true;
	control->preferredWidth = 1;
	control->preferredHeight = 3;
	OSSetCallback(control, OS_MAKE_CALLBACK(ProcessMenuSeparatorMessage, nullptr));
	return control;
}

OSObject OSCreateLine(bool orientation) {
	Control *control = (Control *) GUIAllocate(sizeof(Control), true);
	control->type = API_OBJECT_CONTROL;
	control->backgrounds = orientation ? lineVerticalBackgrounds : lineHorizontalBackgrounds;
	control->drawParentBackground = true;

	control->preferredWidth = 1;
	control->preferredHeight = 1;

	OSSetCallback(control, OS_MAKE_CALLBACK(ProcessControlMessage, nullptr));

	return control;
}

static OSCallbackResponse ProcessIconDisplayMessage(OSObject _object, OSMessage *message) {
	uint16_t iconID = (uintptr_t) message->context;
	Control *control = (Control *) _object;

	if (message->type == OS_MESSAGE_CUSTOM_PAINT) {
		OSDrawSurfaceClipped(message->paint.surface, OS_SURFACE_UI_SHEET, control->bounds, 
				icons32[iconID].region, icons32[iconID].border,
				OS_DRAW_MODE_REPEAT_FIRST, 0xFF, message->paint.clip);
		return OS_CALLBACK_HANDLED;
	} else {
		return OSForwardMessage(_object, OS_MAKE_CALLBACK(ProcessControlMessage, nullptr), message);
	}
}

OSObject OSCreateIconDisplay(uint16_t iconID) {
	Control *control = (Control *) GUIAllocate(sizeof(Control), true);
	control->type = API_OBJECT_CONTROL;
	control->drawParentBackground = true;
	control->preferredWidth = 32;
	control->preferredHeight = 32;

	OSSetCallback(control, OS_MAKE_CALLBACK(ProcessIconDisplayMessage, (void *) (uintptr_t) iconID));
	return control;
}

OSObject OSCreateLabel(char *text, size_t textBytes) {
	Control *control = (Control *) GUIAllocate(sizeof(Control), true);
	control->type = API_OBJECT_CONTROL;
	control->drawParentBackground = true;
	control->textAlign = OS_DRAW_STRING_HALIGN_LEFT;

	OSSetText(control, text, textBytes, OS_RESIZE_MODE_EXACT);
	OSSetCallback(control, OS_MAKE_CALLBACK(ProcessControlMessage, nullptr));

	return control;
}

OSRectangle OSGetControlBounds(OSObject _control) {
	Control *control = (Control *) _control;
	return control->bounds;
}

void OSDrawProgressBar(OSHandle surface, OSRectangle bounds, float progress, OSRectangle clip, bool blue) {
	OSDrawSurfaceClipped(surface, OS_SURFACE_UI_SHEET, bounds, 
			progressBarBackground.region, progressBarBackground.border, progressBarBackground.drawMode, 0xFF, clip);

	if (progress > 0.99) progress = 1;

	OSRectangle filled = OS_MAKE_RECTANGLE(
			bounds.left + 1, bounds.left + 1 + (int) (progress * (bounds.right - bounds.left - 2)),
			bounds.top + 1, bounds.bottom - 1);

	UIImage image = blue ? progressBarFilledBlue : progressBarFilled;

	OSDrawSurfaceClipped(surface, OS_SURFACE_UI_SHEET, filled, 
			image.region, image.border, image.drawMode, 0xFF, clip);
}

static OSCallbackResponse ProcessProgressBarMessage(OSObject _object, OSMessage *message) {
	OSCallbackResponse response = OS_CALLBACK_NOT_HANDLED;
	ProgressBar *control = (ProgressBar *) _object;

	if (message->type == OS_MESSAGE_PAINT) {
		response = OS_CALLBACK_HANDLED;

		if (control->repaint || message->paint.force) {
			{
				OSMessage m = *message;
				m.type = OS_MESSAGE_PAINT_BACKGROUND;
				m.paintBackground.surface = message->paint.surface;
				ClipRectangle(message->paint.clip, control->bounds, &m.paintBackground.clip);
				OSSendMessage(control->parent, &m);
			}

			if (control->disabled) {
				OSDrawSurfaceClipped(message->paint.surface, OS_SURFACE_UI_SHEET, control->bounds, 
						progressBarDisabled.region, progressBarDisabled.border, progressBarDisabled.drawMode, 0xFF, message->paint.clip);
			} else {
				OSDrawSurfaceClipped(message->paint.surface, OS_SURFACE_UI_SHEET, control->bounds, 
						progressBarBackground.region, progressBarBackground.border, progressBarBackground.drawMode, 0xFF, message->paint.clip);

				if (control->maximum) {
					float progress = (float) (control->value - control->minimum) / (float) (control->maximum - control->minimum);
					if (progress > 0.99) progress = 1;
					OSRectangle filled = OS_MAKE_RECTANGLE(
							control->bounds.left + 1, control->bounds.left + 1 + (int) (progress * (control->bounds.right - control->bounds.left - 2)),
							control->bounds.top + 1, control->bounds.bottom - 1);

					OSDrawSurfaceClipped(message->paint.surface, OS_SURFACE_UI_SHEET, filled, 
							progressBarFilled.region, progressBarFilled.border, progressBarFilled.drawMode, 0xFF, message->paint.clip);
				} else {
					OSRectangle clip;
					ClipRectangle(message->paint.clip, OS_MAKE_RECTANGLE(control->bounds.left + 1, control->bounds.right - 1, control->bounds.top + 1, control->bounds.bottom - 1), &clip);
					if (control->value >= control->bounds.right - control->bounds.left) control->value = -100;
					OSRectangle marquee = OS_MAKE_RECTANGLE(
							control->bounds.left + 1 + control->value, control->bounds.left + 101 + control->value,
							control->bounds.top + 1, control->bounds.bottom - 1);
					OSDrawSurfaceClipped(message->paint.surface, OS_SURFACE_UI_SHEET, marquee, 
							progressBarMarquee.region, progressBarMarquee.border, progressBarMarquee.drawMode, 0xFF, clip);
				}
			}

			control->repaint = false;
			control->repaintCustomOnly = true;
		}
	} else if (message->type == OS_MESSAGE_PARENT_UPDATED) {
		if (!control->maximum) {
			OSForwardMessage(_object, OS_MAKE_CALLBACK(ProcessControlMessage, nullptr), message);
			control->timerHz = 30;
			control->window->timerControls.InsertStart(&control->timerControlItem);
		}
	} else if (message->type == OS_MESSAGE_WM_TIMER) {
		response = OS_CALLBACK_HANDLED;
		OSSetProgressBarValue(control, control->value + 2);
	}

	if (response == OS_CALLBACK_NOT_HANDLED) {
		response = OSForwardMessage(_object, OS_MAKE_CALLBACK(ProcessControlMessage, nullptr), message);
	}

	return response;
}

void OSSetProgressBarValue(OSObject _control, int newValue) {
	ProgressBar *control = (ProgressBar *) _control;
	control->value = newValue;
	RepaintControl(control);
}

OSObject OSCreateProgressBar(int minimum, int maximum, int initialValue, bool small) {
	ProgressBar *control = (ProgressBar *) GUIAllocate(sizeof(ProgressBar), true);

	control->type = API_OBJECT_CONTROL;

	control->minimum = minimum;
	control->maximum = maximum;
	control->value = initialValue;

	control->preferredWidth = 168;
	control->preferredHeight = small ? 16 : 21;

	if (!control->maximum) {
		// Indeterminate progress bar.
		control->timerControlItem.thisItem = control;
		control->value = -50;
	}

	OSSetCallback(control, OS_MAKE_CALLBACK(ProcessProgressBarMessage, nullptr));

	return control;
}

void OSSetInstance(OSObject _window, void *instance) {
	((Window *) _window)->instance = instance;
}

void *OSGetInstance(OSObject _window) {
	return ((Window *) _window)->instance;
}

void *OSGetInstanceFromControl(OSObject object) {
	return ((Control *) object)->window->instance;
}

void OSGetText(OSObject _control, OSString *string) {
	Control *control = (Control *) _control;
	*string = control->text;
}

void OSSetText(OSObject _control, char *text, size_t textBytes, unsigned resizeMode) {
	Control *control = (Control *) _control;
	CreateString(text, textBytes, &control->text);

	int suggestedWidth = MeasureStringWidth(text, textBytes, FONT_SIZE, fontRegular) + 8;
	int suggestedHeight = GetLineHeight(fontRegular, FONT_SIZE);

	if (control->icon) {
		suggestedWidth += control->icon->region.right - control->icon->region.left + ICON_TEXT_GAP;
	}

	OSMessage message;

	if (resizeMode) {
		if (resizeMode == OS_RESIZE_MODE_EXACT || suggestedWidth > control->preferredWidth) {
			control->preferredWidth = suggestedWidth;
		}

		if (resizeMode == OS_RESIZE_MODE_EXACT || suggestedHeight > control->preferredHeight) {
			control->preferredHeight = suggestedHeight;
		}

		control->relayout = true;
		message.type = OS_MESSAGE_CHILD_UPDATED;
		OSSendMessage(control->parent, &message);
	}

	RepaintControl(control);
	message.type = OS_MESSAGE_TEXT_UPDATED;
	OSSendMessage(control, &message);
}

void OSAddControl(OSObject _grid, unsigned column, unsigned row, OSObject _control, unsigned layout) {
	GUIObject *_object = (GUIObject *) _grid;

	if (_object->type == API_OBJECT_WINDOW) {
		Window *window = (Window *) _grid;
		_grid = window->root;

		if (window->flags & OS_CREATE_WINDOW_MENU) {
			column = 0;
			row = 0;
		} else if (window->flags & OS_CREATE_WINDOW_WITH_MENUBAR) {
			_grid = window->root->objects[7];
			column = 0;
			row = 1;
		} else if (window->flags & OS_CREATE_WINDOW_NORMAL) {
			column = 1;
			row = 2;
		}
	}

	Grid *grid = (Grid *) _grid;

	grid->relayout = true;
	SetParentDescendentInvalidationFlags(grid, DESCENDENT_RELAYOUT);

	if (column >= grid->columns || row >= grid->rows) {
		OSCrashProcess(OS_FATAL_ERROR_OUT_OF_GRID_BOUNDS);
	}

	GUIObject *control = (GUIObject *) _control;
	if (control->type != API_OBJECT_CONTROL && control->type != API_OBJECT_GRID) OSCrashProcess(OS_FATAL_ERROR_INVALID_PANE_OBJECT);
	control->layout = layout;
	control->parent = grid;

	GUIObject **object = grid->objects + (row * grid->columns + column);
	if (*object) OSCrashProcess(OS_FATAL_ERROR_OVERWRITE_GRID_OBJECT);
	*object = control;

	{
		OSMessage message;
		message.parentUpdated.window = grid->window;
		message.type = OS_MESSAGE_PARENT_UPDATED;
		OSSendMessage(control, &message);
	}
}

static OSCallbackResponse ProcessGridMessage(OSObject _object, OSMessage *message) {
	OSCallbackResponse response = OS_CALLBACK_HANDLED;
	Grid *grid = (Grid *) _object;

	switch (message->type) {
		case OS_MESSAGE_SET_PROPERTY: {
			void *value = message->setProperty.value;
			int valueInt = (int) (uintptr_t) value;
			bool repaint = false;

			switch (message->setProperty.index) {
				case OS_GRID_PROPERTY_BORDER_SIZE: {
					grid->borderSize = OS_MAKE_RECTANGLE_ALL(valueInt);
					repaint = true;
				} break;

				case OS_GRID_PROPERTY_GAP_SIZE: {
					grid->gapSize = valueInt;
					repaint = true;
				} break;

				default: {
					SetGUIObjectProperty(grid, message);
				} break;
			}

			if (repaint) {
				grid->repaint = true;
				SetParentDescendentInvalidationFlags(grid, DESCENDENT_REPAINT);
			}
		} break;

		case OS_MESSAGE_LAYOUT: {
			if (grid->relayout || message->layout.force) {
				grid->descendentInvalidationFlags &= ~DESCENDENT_RELAYOUT;
				grid->relayout = false;

				grid->bounds = OS_MAKE_RECTANGLE(
						message->layout.left, message->layout.right,
						message->layout.top, message->layout.bottom);

				if (!grid->layout) grid->layout = OS_CELL_H_EXPAND | OS_CELL_V_EXPAND;
				StandardCellLayout(grid);

				tryAgain:;

				OSZeroMemory(grid->widths, sizeof(int) * grid->columns);
				OSZeroMemory(grid->heights, sizeof(int) * grid->rows);
				OSZeroMemory(grid->minimumWidths, sizeof(int) * grid->columns);
				OSZeroMemory(grid->minimumHeights, sizeof(int) * grid->rows);

				int pushH = 0, pushV = 0;

#if 0
				OSPrint("->Laying out grid %x (%d by %d) into %d->%d, %d->%d (given %d->%d, %d->%d), layout = %X%X\n", 
 						grid, grid->columns, grid->rows, grid->bounds.left, grid->bounds.right, grid->bounds.top, grid->bounds.bottom,
 						message->layout.left, message->layout.right, message->layout.top, message->layout.bottom, grid->layout);
#endif

				for (uintptr_t i = 0; i < grid->columns; i++) {
					for (uintptr_t j = 0; j < grid->rows; j++) {
						GUIObject **object = grid->objects + (j * grid->columns + i);
						OSMessage message;
						message.type = OS_MESSAGE_MEASURE;
						if (OSSendMessage(*object, &message) == OS_CALLBACK_NOT_HANDLED) continue;

						int width = message.measure.preferredWidth;
						int height = message.measure.preferredHeight;

						// OSPrint("Measuring %d, %d: %d, %d, %d, %d\n", i, j, width, height, message.measure.minimumWidth, message.measure.minimumHeight);

						if ((*object)->layout & OS_CELL_H_PUSH) { bool a = grid->widths[i] == DIMENSION_PUSH; grid->widths[i] = DIMENSION_PUSH; if (!a) pushH++; }
						else if (grid->widths[i] < width && grid->widths[i] != DIMENSION_PUSH) grid->widths[i] = width;
						if ((*object)->layout & OS_CELL_V_PUSH) { bool a = grid->heights[j] == DIMENSION_PUSH; grid->heights[j] = DIMENSION_PUSH; if (!a) pushV++; }
						else if (grid->heights[j] < height && grid->heights[j] != DIMENSION_PUSH) grid->heights[j] = height;

						if (grid->minimumWidths[i] < message.measure.preferredWidth) grid->minimumWidths[i] = message.measure.preferredWidth;
						if (grid->minimumHeights[j] < message.measure.preferredHeight) grid->minimumHeights[j] = message.measure.preferredHeight;
					}
				}

#if 0
				OSPrint("->Results for grid %x (%d by %d)\n", grid, grid->columns, grid->rows);

				for (uintptr_t i = 0; i < grid->columns; i++) {
					OSPrint("Column %d is pref: %dpx, min: %dpx\n", i, grid->widths[i], grid->minimumWidths[i]);
				}

				for (uintptr_t j = 0; j < grid->rows; j++) {
					OSPrint("Row %d is pref: %dpx, min: %dpx\n", j, grid->heights[j], grid->minimumHeights[j]);
				}
#endif

				if (pushH) {
					int usedWidth = grid->borderSize.left + grid->borderSize.right + grid->gapSize * (grid->columns - 1); 
					for (uintptr_t i = 0; i < grid->columns; i++) if (grid->widths[i] != DIMENSION_PUSH) usedWidth += grid->widths[i];
					int widthPerPush = (grid->bounds.right - grid->bounds.left - usedWidth) / pushH;

					for (uintptr_t i = 0; i < grid->columns; i++) {
						if (grid->widths[i] == DIMENSION_PUSH) {
							if (widthPerPush < grid->minimumWidths[i] && grid->treatPreferredDimensionsAsMinima) {
								grid->widths[i] = grid->minimumWidths[i];
							} else {
								grid->widths[i] = widthPerPush;
							}
						}
					}
				}

				if (pushV) {
					int usedHeight = grid->borderSize.top + grid->borderSize.bottom + grid->gapSize * (grid->rows - 1); 
					for (uintptr_t j = 0; j < grid->rows; j++) if (grid->heights[j] != DIMENSION_PUSH) usedHeight += grid->heights[j];
					int heightPerPush = (grid->bounds.bottom - grid->bounds.top - usedHeight) / pushV; 

					for (uintptr_t j = 0; j < grid->rows; j++) {
						if (grid->heights[j] == DIMENSION_PUSH) {
							if (heightPerPush >= grid->minimumHeights[j] || !grid->treatPreferredDimensionsAsMinima) {
								grid->heights[j] = heightPerPush;
							} else {
								grid->heights[j] = grid->minimumHeights[j];
							}
						}
					}
				}

				{
					OSMessage message;
					message.type = OS_MESSAGE_CHECK_LAYOUT;
					message.checkLayout.widths = grid->widths;
					message.checkLayout.heights = grid->heights;

					OSCallbackResponse response = OSSendMessage(grid, &message);

					if (response == OS_CALLBACK_REJECTED) {
						goto tryAgain;
					}
				}

				OSRectangle clip;
				ClipRectangle(message->layout.clip, grid->bounds, &clip);

				OSMessage message2;

				int posX = grid->bounds.left + grid->borderSize.left - grid->xOffset;

				for (uintptr_t i = 0; i < grid->columns; i++) {
					int posY = grid->bounds.top + grid->borderSize.top - grid->yOffset;

					for (uintptr_t j = 0; j < grid->rows; j++) {
						GUIObject **object = grid->objects + (j * grid->columns + i);

						message2.type = OS_MESSAGE_LAYOUT;
						message2.layout.clip = clip;
						message2.layout.force = true;
						message2.layout.left = posX;
						message2.layout.right = posX + grid->widths[i];
						message2.layout.top = posY;
						message2.layout.bottom = posY + grid->heights[j];

						OSSendMessage(*object, &message2);

						posY += grid->heights[j] + grid->gapSize;
					}

					posX += grid->widths[i] + grid->gapSize;
				}

				grid->repaint = true;
				SetParentDescendentInvalidationFlags(grid, DESCENDENT_REPAINT);
			} else if (grid->descendentInvalidationFlags & DESCENDENT_RELAYOUT) {
				grid->descendentInvalidationFlags &= ~DESCENDENT_RELAYOUT;

				for (uintptr_t i = 0; i < grid->columns * grid->rows; i++) {
					if (grid->objects[i]) {
						message->layout.left   = ((GUIObject *) grid->objects[i])->cellBounds.left;
						message->layout.right  = ((GUIObject *) grid->objects[i])->cellBounds.right;
						message->layout.top    = ((GUIObject *) grid->objects[i])->cellBounds.top;
						message->layout.bottom = ((GUIObject *) grid->objects[i])->cellBounds.bottom;

						OSSendMessage(grid->objects[i], message);
					}
				}
			}
		} break;

		case OS_MESSAGE_MEASURE: {
			OSZeroMemory(grid->widths, sizeof(int) * grid->columns);
			OSZeroMemory(grid->heights, sizeof(int) * grid->rows);

			for (uintptr_t i = 0; i < grid->columns; i++) {
				for (uintptr_t j = 0; j < grid->rows; j++) {
					GUIObject **object = grid->objects + (j * grid->columns + i);
					if (OSSendMessage(*object, message) == OS_CALLBACK_NOT_HANDLED) continue;

					int width = message->measure.preferredWidth;
					int height = message->measure.preferredHeight;

					if (grid->widths[i] < width) grid->widths[i] = width;
					if (grid->heights[j] < height) grid->heights[j] = height;
				}
			}

			int width = grid->borderSize.left, height = grid->borderSize.top;

			for (uintptr_t i = 0; i < grid->columns; i++) width += grid->widths[i] + (i == grid->columns - 1 ? grid->borderSize.left : grid->gapSize);
			for (uintptr_t j = 0; j < grid->rows; j++) height += grid->heights[j] + (j == grid->rows - 1 ? grid->borderSize.top : grid->gapSize);

			if (!grid->suggestWidth) grid->preferredWidth = message->measure.preferredWidth = width;
			else message->measure.preferredWidth = grid->preferredWidth;
			if (!grid->suggestHeight) grid->preferredHeight = message->measure.preferredHeight = height;
			else message->measure.preferredHeight = grid->preferredHeight;
		} break;

		case OS_MESSAGE_PAINT: {
			if (grid->descendentInvalidationFlags & DESCENDENT_REPAINT || grid->repaint || message->paint.force) {
				grid->descendentInvalidationFlags &= ~DESCENDENT_REPAINT;

				OSMessage m = *message;
				m.paint.force = message->paint.force || grid->repaint;
				grid->repaint = false;

				OSRectangle clip;

				if (ClipRectangle(message->paint.clip, grid->bounds, &clip)) {
					if (m.paint.force) {
						OSMessage m;
						m.type = OS_MESSAGE_PAINT_BACKGROUND;
						m.paintBackground.surface = message->paint.surface;
						ClipRectangle(message->paint.clip, grid->bounds, &m.paintBackground.clip);
						OSSendMessage(grid, &m);
					}

					for (uintptr_t i = 0; i < grid->columns * grid->rows; i++) {
						if (grid->objects[i]) {
							if (ClipRectangle(clip, grid->objects[i]->bounds, &m.paint.clip)) {
								OSSendMessage(grid->objects[i], &m);
							}
						}
					}
				}
			}
		} break;

		case OS_MESSAGE_PAINT_BACKGROUND: {
			if (grid->background) {
				OSDrawSurfaceClipped(message->paint.surface, OS_SURFACE_UI_SHEET, grid->bounds, grid->background->region,
						grid->background->border, grid->background->drawMode, 0xFF, message->paintBackground.clip);
			} else if (grid->backgroundColor) {
				OSFillRectangle(message->paint.surface, message->paintBackground.clip, OSColor(grid->backgroundColor));
			}
		} break;

		case OS_MESSAGE_PARENT_UPDATED: {
			grid->window = (Window *) message->parentUpdated.window;

			for (uintptr_t i = 0; i < grid->columns * grid->rows; i++) {
				OSSendMessage(grid->objects[i], message);
			}
		} break;

		case OS_MESSAGE_CHILD_UPDATED: {
			grid->relayout = true;
			SetParentDescendentInvalidationFlags(grid, DESCENDENT_RELAYOUT);
		} break;

		case OS_MESSAGE_DESTROY: {
			for (uintptr_t i = 0; i < grid->columns * grid->rows; i++) {
				OSSendMessage(grid->objects[i], message);
			}

			if (grid->window->hoverGrid == grid) {
				grid->window->hoverGrid = nullptr;
			}

			GUIFree(grid);
		} break;

		case OS_MESSAGE_DISABLE: {
			for (uintptr_t i = 0; i < grid->columns * grid->rows; i++) {
				OSSendMessage(grid->objects[i], message);
			}
		} break;

		case OS_MESSAGE_MOUSE_MOVED: {
			if (!IsPointInRectangle(grid->bounds, message->mouseMoved.newPositionX, message->mouseMoved.newPositionY)) {
				if (grid->window->hoverGrid == grid) grid->window->hoverGrid = nullptr;
				break;
			}

			grid->window->hoverGrid = grid;

			for (uintptr_t i = 0; i < grid->columns * grid->rows; i++) {
				OSSendMessage(grid->objects[i], message);
			}
		} break;

		case OS_MESSAGE_KEY_PRESSED: {
			response = OS_CALLBACK_NOT_HANDLED;

			if (message->keyboard.ctrl || message->keyboard.alt) {
				break;
			}

			OSObject previousFocus = message->keyboard.notHandledBy;
			int delta, start, end, i = FindObjectInGrid(grid, previousFocus);
			bool loopAround, foundFirstTime = i != (int) (grid->columns * grid->rows);

			if (message->keyboard.scancode == OS_SCANCODE_TAB) {
				delta = message->keyboard.shift ? -1 : 1;
				start = message->keyboard.shift ? grid->columns * grid->rows - 1 : 0;
				end = message->keyboard.shift ? -1 : grid->columns * grid->rows;
				loopAround = false;
			} else if (message->keyboard.scancode == OS_SCANCODE_LEFT_ARROW && foundFirstTime) {
				delta = -1;
				end = i - (i % grid->columns) - 1;
				start = end + grid->columns;
				loopAround = true;

				if (grid->columns == 1) {
					break;
				}
			} else if (message->keyboard.scancode == OS_SCANCODE_UP_ARROW && foundFirstTime) {
				delta = -grid->columns;
				end = (i % grid->columns) - grid->columns;
				start = end + grid->columns * grid->rows;
				loopAround = true;

				if (grid->rows == 1) {
					break;
				}
			} else if (message->keyboard.scancode == OS_SCANCODE_DOWN_ARROW && foundFirstTime) {
				delta = grid->columns;
				start = (i % grid->columns);
				end = start + grid->columns * grid->rows;
				loopAround = true;

				if (grid->rows == 1) {
					break;
				}
			} else if (message->keyboard.scancode == OS_SCANCODE_RIGHT_ARROW && foundFirstTime) {
				delta = 1;
				start = i - (i % grid->columns);
				end = start + grid->columns;
				loopAround = true;

				if (grid->columns == 1) {
					break;
				}
			} else {
				break;
			}

			retryTab:;
			i = FindObjectInGrid(grid, previousFocus);

			if (i == (int) (grid->columns * grid->rows)) {
				i = start;
			} else {
				i += delta;
			}

			while (i != end) {
				if (grid->objects[i] && grid->objects[i]->tabStop && !grid->objects[i]->disabled) {
					break;
				} else {
					i += delta;
				}
			}

			if (i == end && !loopAround) {
				// We're out of tab-stops in this grid.
				response = OS_CALLBACK_NOT_HANDLED;
			} else {
				if (loopAround && i == end) {
					loopAround = false;
					i = start;
				}

				OSObject focus = grid->objects[i];
				response = OSSendMessage(focus, message);

				if (response == OS_CALLBACK_NOT_HANDLED) {
					previousFocus = focus;
					goto retryTab;
				}
			}
		} break;

		default: {
			response = OS_CALLBACK_NOT_HANDLED;
		} break;
	}

	return response;
}

OSObject OSCreateGrid(unsigned columns, unsigned rows, OSGridStyle style) {
	uint8_t *memory = (uint8_t *) GUIAllocate(sizeof(Grid) + sizeof(OSObject) * columns * rows + 2 * sizeof(int) * (columns + rows), true);

	Grid *grid = (Grid *) memory;
	grid->type = API_OBJECT_GRID;
	grid->tabStop = true;

	grid->backgroundColor = STANDARD_BACKGROUND_COLOR;
	grid->columns = columns;
	grid->rows = rows;
	grid->objects = (GUIObject **) (memory + sizeof(Grid));
	grid->widths = (int *) (memory + sizeof(Grid) + sizeof(OSObject) * columns * rows);
	grid->heights = (int *) (memory + sizeof(Grid) + sizeof(OSObject) * columns * rows + sizeof(int) * columns);
	grid->minimumWidths = (int *) (memory + sizeof(Grid) + sizeof(OSObject) * columns * rows + sizeof(int) * columns + sizeof(int) * rows);
	grid->minimumHeights = (int *) (memory + sizeof(Grid) + sizeof(OSObject) * columns * rows + sizeof(int) * columns + sizeof(int) * rows + sizeof(int) * columns);
	grid->style = style;

	switch (style) {
		case OS_GRID_STYLE_GROUP_BOX: {
			grid->borderSize = OS_MAKE_RECTANGLE_ALL(12);
			grid->gapSize = 6;
			grid->background = &gridBox;
		} break;

		case OS_GRID_STYLE_MENU: {
			grid->borderSize = OS_MAKE_RECTANGLE(4, 4, 3, 1);
			grid->gapSize = 0;
			grid->background = &menuBox;
		} break;

		case OS_GRID_STYLE_BLANK_MENU: {
			grid->borderSize = OS_MAKE_RECTANGLE(2, 2, 2, 2);
			grid->gapSize = 0;
			grid->background = &menuBoxBlank;
		} break;

		case OS_GRID_STYLE_MENUBAR: {
			grid->borderSize = OS_MAKE_RECTANGLE(0, 0, -1, 2);
			grid->gapSize = 0;
			grid->background = &menubarBackground;
		} break;

		case OS_GRID_STYLE_LAYOUT: {
			grid->borderSize = OS_MAKE_RECTANGLE_ALL(0);
			grid->gapSize = 0;
			grid->backgroundColor = 0;
		} break;

		case OS_GRID_STYLE_CONTAINER: {
			grid->borderSize = OS_MAKE_RECTANGLE_ALL(8);
			grid->gapSize = 6;
		} break;

		case OS_GRID_STYLE_CONTAINER_WITHOUT_BORDER: {
			grid->borderSize = OS_MAKE_RECTANGLE_ALL(0);
			grid->gapSize = 6;
		} break;

		case OS_GRID_STYLE_CONTAINER_ALT: {
			grid->borderSize = OS_MAKE_RECTANGLE_ALL(8);
			grid->gapSize = 6;
			grid->background = &dialogAltAreaBox;
		} break;

		case OS_GRID_STYLE_STATUS_BAR: {
			grid->borderSize = OS_MAKE_RECTANGLE_ALL(4);
			grid->gapSize = 6;
			grid->background = &dialogAltAreaBox;
		} break;

		case OS_GRID_STYLE_TOOLBAR: {
			grid->borderSize = OS_MAKE_RECTANGLE(5, 5, 0, 0);
			grid->gapSize = 4;
			grid->preferredHeight = 31;
			grid->suggestHeight = true;
			grid->background = &toolbarBackground;
		} break;

		case OS_GRID_STYLE_TOOLBAR_ALT: {
			grid->borderSize = OS_MAKE_RECTANGLE(5, 5, 0, 0);
			grid->gapSize = 4;
			grid->preferredHeight = 31;
			grid->suggestHeight = true;
			grid->background = &toolbarBackgroundAlt;
		} break;
	}

	OSSetCallback(grid, OS_MAKE_CALLBACK(ProcessGridMessage, nullptr));

	return grid;
}

int OSGetSliderPosition(OSObject _slider) {
	Slider *slider = (Slider *) _slider;

	if (slider->mode & OS_SLIDER_MODE_OPPOSITE_VALUE) {
		return slider->maximum - slider->value + slider->minimum;
	} else {
		return slider->value;
	}
}

static void SliderValueModified(Slider *grid, bool sendNotification = true) {
	if (grid->value < grid->minimum) grid->value = grid->minimum;
	if (grid->value > grid->maximum) grid->value = grid->maximum;
	if (grid->mode & OS_SLIDER_MODE_SNAP_TO_TICKS) grid->value -= grid->value % grid->minorTickSpacing;

	{
		OSMessage message;
		message.type = OS_MESSAGE_CHILD_UPDATED;
		OSSendMessage(grid, &message);

		if (sendNotification) {
			message.type = OS_NOTIFICATION_VALUE_CHANGED;
			message.valueChanged.newValue = OSGetSliderPosition(grid);
			OSForwardMessage(grid, grid->notificationCallback, &message);
		}
	}
}

void OSSetSliderPosition(Slider *slider, int position, bool sendValueChangedNotification) {
	if (slider->mode & OS_SLIDER_MODE_OPPOSITE_VALUE) {
		slider->value = slider->maximum - position + slider->minimum;
	} else {
		slider->value = position;
	}

	SliderValueModified(slider, sendValueChangedNotification);
}

OSCallbackResponse ProcessSliderMessage(OSObject object, OSMessage *message) {
	Slider *grid = (Slider *) object;
	OSCallbackResponse response = OS_CALLBACK_NOT_HANDLED;

	if (message->type == OS_MESSAGE_LAYOUT) {
		if (grid->relayout || message->layout.force) {
			grid->relayout = false;
			grid->bounds = OS_MAKE_RECTANGLE(
					message->layout.left, message->layout.right,
					message->layout.top, message->layout.bottom);
			grid->cellBounds = grid->bounds;
			grid->repaint = true;
			SetParentDescendentInvalidationFlags(grid, DESCENDENT_REPAINT);

			int start, end;

			if (grid->mode & OS_SLIDER_MODE_HORIZONTAL) {
				start = grid->bounds.left + 14;
				end = grid->bounds.right - 14;
			} else {
				start = grid->bounds.top + 14;
				end = grid->bounds.bottom - 14;
			}

			int length = end - start;
			int position = length * (grid->value - grid->minimum) / (grid->maximum - grid->minimum) + start;

			OSMessage m;
			m.type = OS_MESSAGE_LAYOUT;
			m.layout.force = true;
			m.layout.clip = message->layout.clip;

			int *dStart, *dEnd, sStart, sEnd;

			if (grid->mode & OS_SLIDER_MODE_HORIZONTAL) {
				dStart = &m.layout.top;
				dEnd = &m.layout.bottom;
				sStart = message->layout.top;
				sEnd = message->layout.bottom;

				m.layout.left = position - 6;
				m.layout.right = position - 6 + 13;
			} else {
				dStart = &m.layout.left;
				dEnd = &m.layout.right;
				sStart = message->layout.left;
				sEnd = message->layout.right;

				m.layout.top = position - 6;
				m.layout.bottom = position - 6 + 13;
			}

			if ((grid->mode & OS_SLIDER_MODE_TICKS_BENEATH) && (grid->mode & OS_SLIDER_MODE_TICKS_ABOVE)) {
				*dStart = sStart + 4;
				*dEnd = sEnd - 4;
			} else if (grid->mode & OS_SLIDER_MODE_TICKS_ABOVE) {
				*dStart = sEnd - 2 - 20;
				*dEnd = sEnd - 2;
			} else if (grid->mode & OS_SLIDER_MODE_TICKS_BENEATH) {
				*dStart = sStart + 2;
				*dEnd = sStart + 2 + 20;
			} else {
				*dStart = sStart + 4;
				*dEnd = sEnd - 4;
			}

			OSSendMessage(grid->objects[0], &m);

			response = OS_CALLBACK_HANDLED;
		}
	} else if (message->type == OS_MESSAGE_MEASURE) {
		message->measure.preferredWidth = grid->preferredWidth;
		message->measure.preferredHeight = grid->preferredHeight;
		response = OS_CALLBACK_HANDLED;
	} else if (message->type == OS_MESSAGE_PAINT_BACKGROUND) {
		OSDrawSurfaceClipped(message->paint.surface, OS_SURFACE_UI_SHEET, grid->bounds, sliderBox.region,
				sliderBox.border, sliderBox.drawMode, 0xFF, message->paintBackground.clip);

		int start, end;

		if (grid->mode & OS_SLIDER_MODE_HORIZONTAL) {
			start = grid->bounds.left + 14;
			end = grid->bounds.right - 14;
		} else {
			start = grid->bounds.top + 14;
			end = grid->bounds.bottom - 14;
		}

		int length = end - start;
		float lengthPerTick = (float) length / (float) (grid->maximum - grid->minimum);
		float lengthPerMinorTick = lengthPerTick * grid->minorTickSpacing;

		{
			float i = start;
			int j = 0;

			while (i - 0.5f <= end) {
				if (grid->mode & OS_SLIDER_MODE_HORIZONTAL) {
					if (grid->mode & OS_SLIDER_MODE_TICKS_ABOVE) {
						OSDrawSurfaceClipped(message->paint.surface, OS_SURFACE_UI_SHEET, 
								OS_MAKE_RECTANGLE((int) i, (int) i + 2, grid->bounds.top + 11 - (j ? 5 : 7), grid->bounds.top + 11), 
								tickHorizontal.region, tickHorizontal.border, tickHorizontal.drawMode, 0xFF, message->paintBackground.clip);
					}

					if (grid->mode & OS_SLIDER_MODE_TICKS_BENEATH) {
						OSDrawSurfaceClipped(message->paint.surface, OS_SURFACE_UI_SHEET, 
								OS_MAKE_RECTANGLE((int) i, (int) i + 2, grid->bounds.top + 17, grid->bounds.top + 17 + (j ? 5 : 7)), 
								tickHorizontal.region, tickHorizontal.border, tickHorizontal.drawMode, 0xFF, message->paintBackground.clip);
					}
				} else {
					if (grid->mode & OS_SLIDER_MODE_TICKS_ABOVE) {
						OSDrawSurfaceClipped(message->paint.surface, OS_SURFACE_UI_SHEET, 
								OS_MAKE_RECTANGLE(grid->bounds.left + 11 - (j ? 5 : 7), grid->bounds.left + 11, (int) i, (int) i + 2), 
								tickVertical.region, tickVertical.border, tickVertical.drawMode, 0xFF, message->paintBackground.clip);
					}

					if (grid->mode & OS_SLIDER_MODE_TICKS_BENEATH) {
						OSDrawSurfaceClipped(message->paint.surface, OS_SURFACE_UI_SHEET, 
								OS_MAKE_RECTANGLE(grid->bounds.left + 17, grid->bounds.left + 17 + (j ? 5 : 7), (int) i, (int) i + 2), 
								tickVertical.region, tickVertical.border, tickVertical.drawMode, 0xFF, message->paintBackground.clip);
					}
				}

				i += lengthPerMinorTick;
				j++;

				if (j == grid->majorTickSpacing) {
					j = 0;
				}
			}
		}
	} else if (message->type == OS_MESSAGE_KEY_PRESSED) {
		if (!message->keyboard.ctrl && !message->keyboard.alt && !message->keyboard.shift) {
			response = OS_CALLBACK_HANDLED;

			if (message->keyboard.scancode == OS_SCANCODE_LEFT_ARROW || message->keyboard.scancode == OS_SCANCODE_UP_ARROW) {
				grid->value -= grid->minorTickSpacing;
			} else if (message->keyboard.scancode == OS_SCANCODE_RIGHT_ARROW || message->keyboard.scancode == OS_SCANCODE_DOWN_ARROW) {
				grid->value += grid->minorTickSpacing;
			} else {
				response = OS_CALLBACK_NOT_HANDLED;
			}

			SliderValueModified(grid);
		}
	} else if (message->type == OS_MESSAGE_CLICK_REPEAT) {
		grid->value += grid->previousClickAdjustment;
		SliderValueModified(grid);
	} else if (message->type == OS_MESSAGE_MOUSE_LEFT_PRESSED) {
		response = OS_CALLBACK_HANDLED;

		if (grid->mode & OS_SLIDER_MODE_HORIZONTAL) {
			if (message->mousePressed.positionX < grid->handle->bounds.left) {
				grid->previousClickAdjustment = -grid->minorTickSpacing;
			} else {
				grid->previousClickAdjustment = grid->minorTickSpacing;
			}
		} else {
			if (message->mousePressed.positionY < grid->handle->bounds.top) {
				grid->previousClickAdjustment = -grid->minorTickSpacing;
			} else {
				grid->previousClickAdjustment = grid->minorTickSpacing;
			}
		}


		grid->value += grid->previousClickAdjustment;
		SliderValueModified(grid);
	}

	if (response == OS_CALLBACK_NOT_HANDLED) {
		response = OSForwardMessage(object, OS_MAKE_CALLBACK(ProcessGridMessage, nullptr), message);
	}

	return response;
}

OSCallbackResponse ProcessSliderHandleMessage(OSObject object, OSMessage *message) {
	SliderHandle *control = (SliderHandle *) object;
	Slider *slider = control->slider;
	OSCallbackResponse response = OS_CALLBACK_NOT_HANDLED;

	if (message->type == OS_MESSAGE_MOUSE_DRAGGED) {
		int start, end, mouse;

		if (slider->mode & OS_SLIDER_MODE_HORIZONTAL) {
			start = slider->bounds.left + 14;
			end = slider->bounds.right - 14;
			mouse = message->mouseDragged.newPositionX;
		} else {
			start = slider->bounds.top + 14;
			end = slider->bounds.bottom - 14;
			mouse = message->mouseDragged.newPositionY;
		}

		float position = (float) (mouse + 3 - start) / (float) (end - start);
		slider->value = (int) (position * (slider->maximum - slider->minimum) + slider->minimum);

		SliderValueModified(slider);
		response = OS_CALLBACK_HANDLED;
	}

	if (response == OS_CALLBACK_NOT_HANDLED) {
		response = OSForwardMessage(object, OS_MAKE_CALLBACK(ProcessControlMessage, nullptr), message);
	}

	return response;
}

OSObject OSCreateSlider(int minimum, int maximum, int initialValue, 
		int mode, int minorTickSpacing, int majorTickSpacing) {
	uint8_t *memory = (uint8_t *) GUIAllocate(sizeof(Slider) + sizeof(OSObject) * 1, true);

	Slider *slider = (Slider *) memory;
	slider->type = API_OBJECT_GRID;

	slider->tabStop = true;
	slider->columns = 1;
	slider->rows = 1;
	slider->objects = (GUIObject **) (memory + sizeof(Slider));

	slider->minimum = minimum;
	slider->maximum = maximum;
	slider->mode = mode;
	slider->minorTickSpacing = minorTickSpacing;
	slider->majorTickSpacing = majorTickSpacing;

	OSSetSliderPosition(slider, initialValue, false);

	if (mode & OS_SLIDER_MODE_HORIZONTAL) {
		slider->preferredWidth = 168;
		slider->preferredHeight = 28;
	} else {
		slider->preferredHeight = 168;
		slider->preferredWidth = 28;
	}

	OSSetCallback(slider, OS_MAKE_CALLBACK(ProcessSliderMessage, nullptr));

	{
		SliderHandle *handle = (SliderHandle *) GUIAllocate(sizeof(SliderHandle), true);
		slider->handle = handle;
		handle->type = API_OBJECT_CONTROL;
		handle->slider = slider;
		handle->focusable = true;
		handle->tabStop = true;
		handle->drawParentBackground = true;
		handle->hasFocusedBackground = true;
		OSAddControl(slider, 0, 0, handle, 0);
		OSSetCallback(handle, OS_MAKE_CALLBACK(ProcessSliderHandleMessage, nullptr));

		if (mode & OS_SLIDER_MODE_HORIZONTAL) {
			if ((mode & OS_SLIDER_MODE_TICKS_BENEATH) && (mode & OS_SLIDER_MODE_TICKS_ABOVE)) {
				handle->backgrounds = sliderVertical;
			} else if (mode & OS_SLIDER_MODE_TICKS_BENEATH) {
				handle->backgrounds = sliderDown;
			} else if (mode & OS_SLIDER_MODE_TICKS_ABOVE) {
				handle->backgrounds = sliderUp;
			} else {
				handle->backgrounds = sliderVertical;
			}

			handle->preferredWidth = 13;
			handle->preferredHeight = 20;
		} else {
			if ((mode & OS_SLIDER_MODE_TICKS_BENEATH) && (mode & OS_SLIDER_MODE_TICKS_ABOVE)) {
				handle->backgrounds = sliderHorizontal;
			} else if (mode & OS_SLIDER_MODE_TICKS_BENEATH) {
				handle->backgrounds = sliderRight;
			} else if (mode & OS_SLIDER_MODE_TICKS_ABOVE) {
				handle->backgrounds = sliderLeft;
			} else {
				handle->backgrounds = sliderHorizontal;
			}

			handle->preferredWidth = 20;
			handle->preferredHeight = 13;
		}
	}

	return slider;
}

static OSCallbackResponse ProcessScrollPaneMessage(OSObject _object, OSMessage *message) {
	OSCallbackResponse response = OS_CALLBACK_NOT_HANDLED;
	Grid *grid = (Grid *) _object;

	if (message->type == OS_MESSAGE_LAYOUT) {
		if (grid->relayout || message->layout.force) {
			grid->relayout = false;
			grid->bounds = OS_MAKE_RECTANGLE(
					message->layout.left, message->layout.right,
					message->layout.top, message->layout.bottom);
			grid->repaint = true;
			SetParentDescendentInvalidationFlags(grid, DESCENDENT_REPAINT);

#if 0
			OSPrint("->Laying out grid %x (%d by %d) into %d->%d, %d->%d (given %d->%d, %d->%d), layout = %X\n", 
					grid, grid->columns, grid->rows, grid->bounds.left, grid->bounds.right, grid->bounds.top, grid->bounds.bottom,
					message->layout.left, message->layout.right, message->layout.top, message->layout.bottom, grid->layout);
#endif

			OSMessage m = {};
			m.type = OS_MESSAGE_MEASURE;
			OSCallbackResponse r = OSSendMessage(grid->objects[0], &m);
			if (r != OS_CALLBACK_HANDLED) OSCrashProcess(OS_FATAL_ERROR_MESSAGE_SHOULD_BE_HANDLED);

			int contentWidth = message->layout.right - message->layout.left - (grid->objects[1] ? SCROLLBAR_SIZE : 0);
			int contentHeight = message->layout.bottom - message->layout.top - (grid->objects[2] ? SCROLLBAR_SIZE : 0);
			int minimumWidth = m.measure.preferredWidth;
			int minimumHeight = m.measure.preferredHeight;

			m.type = OS_MESSAGE_LAYOUT;
			m.layout.force = true;
			m.layout.clip = message->layout.clip;

			if (grid->objects[1]) {
				m.layout.top = message->layout.top;
				m.layout.bottom = message->layout.top + contentHeight;
				m.layout.left = message->layout.right - SCROLLBAR_SIZE;
				m.layout.right = message->layout.right;
				OSSendMessage(grid->objects[1], &m);
				OSSetScrollbarMeasurements(grid->objects[1], minimumHeight, contentHeight);
			}

			if (grid->objects[2]) {
				m.layout.top = message->layout.bottom - SCROLLBAR_SIZE;
				m.layout.bottom = message->layout.bottom;
				m.layout.left = message->layout.left;
				m.layout.right = message->layout.left + contentWidth;
				OSSendMessage(grid->objects[2], &m);
				OSSetScrollbarMeasurements(grid->objects[2], minimumWidth, contentWidth);
			}

			{
				m.layout.top = message->layout.bottom - SCROLLBAR_SIZE;
				m.layout.bottom = message->layout.bottom;
				m.layout.left = message->layout.right - SCROLLBAR_SIZE;
				m.layout.right = message->layout.right;
				OSSendMessage(grid->objects[3], &m);
			}

			m.layout.top = message->layout.top;
			m.layout.bottom = message->layout.top + contentHeight;
			m.layout.left = message->layout.left;
			m.layout.right = message->layout.left + contentWidth;
			OSSendMessage(grid->objects[0], &m);

			response = OS_CALLBACK_HANDLED;
		}
	} else if (message->type == OS_MESSAGE_MEASURE) {
		message->measure.preferredWidth = SCROLLBAR_SIZE * 3;
		message->measure.preferredHeight = SCROLLBAR_SIZE * 3;
		response = OS_CALLBACK_HANDLED;
	}

	if (response == OS_CALLBACK_NOT_HANDLED) {
		response = OSForwardMessage(_object, OS_MAKE_CALLBACK(ProcessGridMessage, nullptr), message);
	}

	return response;
}

OSCallbackResponse ScrollPaneBarMoved(OSObject _object, OSMessage *message) {
	Scrollbar *scrollbar = (Scrollbar *) _object;
	Grid *grid = (Grid *) message->context;

	if (scrollbar->orientation) {
		// Vertical scrollbar.
		grid->yOffset = message->valueChanged.newValue;
	} else {
		// Horizontal scrollbar.
		grid->xOffset = message->valueChanged.newValue;
	}

	SetParentDescendentInvalidationFlags(grid, DESCENDENT_RELAYOUT);
	grid->relayout = true;

	return OS_CALLBACK_HANDLED;
}

OSObject OSCreateScrollPane(OSObject content, unsigned flags) {
	OSObject grid = OSCreateGrid(2, 2, OS_GRID_STYLE_LAYOUT);
	OSSetCallback(grid, OS_MAKE_CALLBACK(ProcessScrollPaneMessage, nullptr));

	OSAddGrid(grid, 0, 0, content, OS_CELL_FILL);
	((Grid *) content)->treatPreferredDimensionsAsMinima = true;

	if (flags & OS_CREATE_SCROLL_PANE_VERTICAL) {
		OSObject scrollbar = OSCreateScrollbar(OS_ORIENTATION_VERTICAL, true);
		OSAddGrid(grid, 1, 0, scrollbar, OS_CELL_V_PUSH | OS_CELL_V_EXPAND);
		OSSetObjectNotificationCallback(scrollbar, OS_MAKE_CALLBACK(ScrollPaneBarMoved, content));
		// OSPrint("vertical %x\n", scrollbar);
	}

	if (flags & OS_CREATE_SCROLL_PANE_HORIZONTAL) {
		OSObject scrollbar = OSCreateScrollbar(OS_ORIENTATION_HORIZONTAL, true);
		OSAddGrid(grid, 0, 1, scrollbar, OS_CELL_H_PUSH | OS_CELL_H_EXPAND);
		OSSetObjectNotificationCallback(scrollbar, OS_MAKE_CALLBACK(ScrollPaneBarMoved, content));
		// OSPrint("horizontal %x\n", scrollbar);
	}

	{
		OSObject corner = OSCreateGrid(1, 1, OS_GRID_STYLE_CONTAINER);
		OSAddGrid(grid, 1, 1, corner, OS_CELL_H_EXPAND | OS_CELL_V_EXPAND);
	}

	return grid;
}

#define SCROLLBAR_BUTTON_UP   ((void *) 1)
#define SCROLLBAR_BUTTON_DOWN ((void *) 2)
#define SCROLLBAR_NUDGE_UP    ((void *) 3)
#define SCROLLBAR_NUDGE_DOWN  ((void *) 4)

int OSGetScrollbarPosition(OSObject object) {
	Scrollbar *scrollbar = (Scrollbar *) object;

	int position = 0;

	if (scrollbar->enabled) {
		float fraction = (float) scrollbar->position / (float) scrollbar->maxPosition;
		float range = (float) (scrollbar->contentSize - scrollbar->viewportSize);
		position = (int) (fraction * range);
	}

	return position;
}

static void ScrollbarPositionChanged(Scrollbar *scrollbar) {
	OSMessage message;
	message.type = OS_NOTIFICATION_VALUE_CHANGED;
	message.valueChanged.newValue = OSGetScrollbarPosition(scrollbar);
	OSForwardMessage(scrollbar, scrollbar->notificationCallback, &message);
}

static OSCallbackResponse ScrollbarButtonPressed(OSObject object, OSMessage *message) {
	Control *button = (Control *) object;
	Scrollbar *scrollbar = (Scrollbar *) button->context;

	int position = OSGetScrollbarPosition(scrollbar);
	int amount = SCROLLBAR_BUTTON_AMOUNT;

	if (message->context == SCROLLBAR_BUTTON_UP) {
		position -= amount;
	} else if (message->context == SCROLLBAR_BUTTON_DOWN) {
		position += amount;
	} else if (message->context == SCROLLBAR_NUDGE_UP) {
		position -= scrollbar->viewportSize;
	} else if (message->context == SCROLLBAR_NUDGE_DOWN) {
		position += scrollbar->viewportSize;
	}

	OSSetScrollbarPosition(scrollbar, position, true);

	return OS_CALLBACK_HANDLED;
}

static void RelayoutScrollbar(Scrollbar *grid) {
	OSMessage message;
	message.type = OS_MESSAGE_LAYOUT;
	message.layout.force = true;
	message.layout.clip = grid->inputBounds;

	if (grid->orientation) {
		message.layout.left = grid->bounds.left;
		message.layout.right = grid->bounds.right;

		message.layout.top = grid->bounds.top;
		message.layout.bottom = message.layout.top + SCROLLBAR_SIZE;
		OSSendMessage(grid->objects[0], &message);

		if (!grid->enabled) {
			message.layout.top = 0;
			message.layout.bottom = 0;
			OSSendMessage(grid->objects[1], &message);
			OSSendMessage(grid->objects[3], &message);

			message.layout.top = grid->bounds.top + SCROLLBAR_SIZE;
			message.layout.bottom = grid->bounds.bottom - SCROLLBAR_SIZE;
			OSSendMessage(grid->objects[4], &message);
		} else {
			int x = grid->bounds.top + SCROLLBAR_SIZE + grid->position + grid->size;
			message.layout.top = grid->bounds.top + SCROLLBAR_SIZE + grid->position;
			message.layout.bottom = x;
			OSSendMessage(grid->objects[1], &message);
			message.layout.bottom = message.layout.top;
			message.layout.top = grid->bounds.top + SCROLLBAR_SIZE;
			OSSendMessage(grid->objects[3], &message);
			message.layout.top = x;
			message.layout.bottom = grid->bounds.bottom - SCROLLBAR_SIZE;
			OSSendMessage(grid->objects[4], &message);
		}

		message.layout.bottom = grid->bounds.bottom;
		message.layout.top = message.layout.bottom - SCROLLBAR_SIZE;
		OSSendMessage(grid->objects[2], &message);
	} else {
		message.layout.top = grid->bounds.top;
		message.layout.bottom = grid->bounds.bottom;

		message.layout.left = grid->bounds.left;
		message.layout.right = message.layout.left + SCROLLBAR_SIZE;
		OSSendMessage(grid->objects[0], &message);

		if (!grid->enabled) {
			message.layout.left = 0;
			message.layout.right = 0;
			OSSendMessage(grid->objects[1], &message);
			OSSendMessage(grid->objects[3], &message);

			message.layout.left = grid->bounds.left + SCROLLBAR_SIZE;
			message.layout.right = grid->bounds.right - SCROLLBAR_SIZE;
			OSSendMessage(grid->objects[4], &message);
		} else {
			int x = grid->bounds.left + SCROLLBAR_SIZE + grid->position + grid->size;
			message.layout.left = grid->bounds.left + SCROLLBAR_SIZE + grid->position;
			message.layout.right = x;
			OSSendMessage(grid->objects[1], &message);
			message.layout.right = message.layout.left;
			message.layout.left = grid->bounds.left + SCROLLBAR_SIZE;
			OSSendMessage(grid->objects[3], &message);
			message.layout.left = x;
			message.layout.right = grid->bounds.right - SCROLLBAR_SIZE;
			OSSendMessage(grid->objects[4], &message);
		}

		message.layout.right = grid->bounds.right;
		message.layout.left = message.layout.right - SCROLLBAR_SIZE;
		OSSendMessage(grid->objects[2], &message);
	}
}

void OSSetScrollbarPosition(OSObject object, int newPosition, bool sendValueChangedNotification) {
	Scrollbar *scrollbar = (Scrollbar *) object;

	if (scrollbar->enabled) {
		// OSPrint("Set scrollbar position to %d\n", newPosition);

		float range = (float) (scrollbar->contentSize - scrollbar->viewportSize);
		float fraction = (float) newPosition / range;
		scrollbar->position = (fraction * (float) scrollbar->maxPosition);

		if (scrollbar->position < 0) scrollbar->position = 0;
		else if (scrollbar->position >= scrollbar->maxPosition) scrollbar->position = scrollbar->maxPosition;

		if (sendValueChangedNotification) {
			ScrollbarPositionChanged(scrollbar);
		}

		RelayoutScrollbar(scrollbar);
	}
}

static OSCallbackResponse ProcessScrollbarGripMessage(OSObject object, OSMessage *message) {
	Control *grip = (Control *) object;
	Scrollbar *scrollbar = (Scrollbar *) grip->context;

	if (scrollbar->enabled) {
		if (message->type == OS_MESSAGE_START_DRAG) {
			scrollbar->anchor = scrollbar->orientation ? message->mouseMoved.newPositionY : message->mouseMoved.newPositionX;
		} else if (message->type == OS_MESSAGE_MOUSE_DRAGGED) {
			{
				OSMessage message;
				message.type = OS_MESSAGE_CHILD_UPDATED;
				OSSendMessage(scrollbar, &message);
			}

			int mouse = scrollbar->orientation ? message->mouseDragged.newPositionY : message->mouseDragged.newPositionX;
			scrollbar->position += mouse - scrollbar->anchor;

			if (scrollbar->position < 0) scrollbar->position = 0;
			else if (scrollbar->position >= scrollbar->maxPosition) scrollbar->position = scrollbar->maxPosition;
			else scrollbar->anchor = mouse;

			ScrollbarPositionChanged(scrollbar);
		} else if (message->type == OS_MESSAGE_CUSTOM_PAINT) {
			OSRectangle bounds;

			if (scrollbar->orientation) {
				bounds.left = grip->bounds.left + 6;
				bounds.right = bounds.left + 5;
				bounds.top = (grip->bounds.top + grip->bounds.bottom) / 2 - 4;
				bounds.bottom = bounds.top + 8;
			} else {
				bounds.top = grip->bounds.top + 6;
				bounds.bottom = bounds.top + 5;
				bounds.left = (grip->bounds.left + grip->bounds.right) / 2 - 4;
				bounds.right = bounds.left + 8;
			}

			OSDrawSurfaceClipped(message->paint.surface, OS_SURFACE_UI_SHEET, bounds, 
					scrollbar->orientation ? scrollbarNotchesHorizontal.region : scrollbarNotchesVertical.region,
					scrollbar->orientation ? scrollbarNotchesHorizontal.border : scrollbarNotchesVertical.border,
					OS_DRAW_MODE_REPEAT_FIRST, 0xFF, message->paint.clip);
		} else if (message->type == OS_MESSAGE_LAYOUT) {
			// OSPrint("Layout scrollbar grip, %d, %d, %d, %d\n",
					// message->layout.left, message->layout.right, message->layout.top, message->layout.bottom);
		}
	}

	return OSForwardMessage(object, OS_MAKE_CALLBACK(ProcessControlMessage, nullptr), message);
}

static OSCallbackResponse ProcessScrollbarMessage(OSObject object, OSMessage *message) {
	Scrollbar *grid = (Scrollbar *) object;
	OSCallbackResponse result = OS_CALLBACK_NOT_HANDLED;

	switch (message->type) {
		case OS_MESSAGE_MEASURE: {
			message->measure.preferredWidth = grid->preferredWidth;
			message->measure.preferredHeight = grid->preferredHeight;
			result = OS_CALLBACK_HANDLED;
		} break;

		case OS_MESSAGE_LAYOUT: {
			if (grid->relayout || message->layout.force) {
				grid->relayout = false;

				grid->bounds = OS_MAKE_RECTANGLE(
						message->layout.left, message->layout.right,
						message->layout.top, message->layout.bottom);
				grid->inputBounds = message->layout.clip;

				StandardCellLayout(grid);

				if (grid->enabled) {
					OSSetScrollbarMeasurements(grid, grid->contentSize, grid->viewportSize);
				}

				grid->repaint = true;
				SetParentDescendentInvalidationFlags(grid, DESCENDENT_REPAINT);

				// OSPrint("layout scrollbar %x\n", object);

				RelayoutScrollbar(grid);

				result = OS_CALLBACK_HANDLED;
			}
		} break;

		default: {} break;
	}

	if (result == OS_CALLBACK_NOT_HANDLED) {
		result = OSForwardMessage(object, OS_MAKE_CALLBACK(ProcessGridMessage, nullptr), message);
	}

	return result;
}

void OSSetScrollbarMeasurements(OSObject _scrollbar, int contentSize, int viewportSize) {
	Scrollbar *scrollbar = (Scrollbar *) _scrollbar;

	// OSPrint("Set scrollbar %x to %dpx in %dpx viewport\n", scrollbar, contentSize, viewportSize);

	if (contentSize <= viewportSize) {
		scrollbar->enabled = false;
		scrollbar->height = 0;
		scrollbar->position = 0;

		OSDisableControl(scrollbar->objects[0], true);
		OSDisableControl(scrollbar->objects[2], true);
		OSDisableControl(scrollbar->objects[3], true);
		OSDisableControl(scrollbar->objects[4], true);
	} else {
		int height = scrollbar->orientation ? (scrollbar->bounds.bottom - scrollbar->bounds.top) : (scrollbar->bounds.right - scrollbar->bounds.left);
		height -= SCROLLBAR_SIZE * 2;

		float fraction = -1;

		if (height != scrollbar->height && scrollbar->height > 0 && scrollbar->automaticallyUpdatePosition) {
			fraction = (float) scrollbar->position / (float) scrollbar->maxPosition;
		}

		scrollbar->enabled = true;
		scrollbar->height = height;
		scrollbar->contentSize = contentSize;
		scrollbar->viewportSize = viewportSize;

		float screens = (float) scrollbar->contentSize / (float) scrollbar->viewportSize;
		scrollbar->size = height / screens;

		if (scrollbar->size < SCROLLBAR_MINIMUM) scrollbar->size = SCROLLBAR_MINIMUM;

		scrollbar->maxPosition = height - scrollbar->size;
		if (fraction != -1) scrollbar->position = fraction * scrollbar->maxPosition;

		if (scrollbar->position < 0) scrollbar->position = 0;
		else if (scrollbar->position >= scrollbar->maxPosition) scrollbar->position = scrollbar->maxPosition;

		OSEnableControl(scrollbar->objects[0], true);
		OSEnableControl(scrollbar->objects[2], true);
		OSEnableControl(scrollbar->objects[3], true);
		OSEnableControl(scrollbar->objects[4], true);
	}

	RepaintControl(scrollbar->objects[0]);
	RepaintControl(scrollbar->objects[1]);
	RepaintControl(scrollbar->objects[2]);
	RepaintControl(scrollbar->objects[3]);
	RepaintControl(scrollbar->objects[4]);

	if (scrollbar->automaticallyUpdatePosition) {
		ScrollbarPositionChanged(scrollbar);
	} else {
		RelayoutScrollbar(scrollbar);
	}
}

OSObject OSCreateScrollbar(bool orientation, bool automaticallyUpdatePosition) {
	uint8_t *memory = (uint8_t *) GUIAllocate(sizeof(Scrollbar) + sizeof(OSObject) * 5, true);

	Scrollbar *scrollbar = (Scrollbar *) memory;
	scrollbar->type = API_OBJECT_GRID;
	OSSetCallback(scrollbar, OS_MAKE_CALLBACK(ProcessScrollbarMessage, nullptr));

	scrollbar->orientation = orientation;
	scrollbar->automaticallyUpdatePosition = automaticallyUpdatePosition;

	scrollbar->columns = 1;
	scrollbar->rows = 5;
	scrollbar->objects = (GUIObject **) (memory + sizeof(Scrollbar));
	scrollbar->preferredWidth = !orientation ? 64 : SCROLLBAR_SIZE;
	scrollbar->preferredHeight = !orientation ? SCROLLBAR_SIZE : 64;

	OSCommand command = {};
	command.defaultDisabled = true;
	command.identifier = OS_COMMAND_DYNAMIC;

	Control *nudgeUp = (Control *) GUIAllocate(sizeof(Control), true);
	nudgeUp->type = API_OBJECT_CONTROL;
	nudgeUp->context = scrollbar;
	nudgeUp->notificationCallback = OS_MAKE_CALLBACK(ScrollbarButtonPressed, SCROLLBAR_NUDGE_UP);
	nudgeUp->backgrounds = orientation ? scrollbarTrackVerticalBackgrounds : scrollbarTrackHorizontalBackgrounds;
	nudgeUp->useClickRepeat = true;
	OSSetCallback(nudgeUp, OS_MAKE_CALLBACK(ProcessButtonMessage, nullptr));

	command.callback = OS_MAKE_CALLBACK(ScrollbarButtonPressed, SCROLLBAR_NUDGE_DOWN);
	Control *nudgeDown = (Control *) GUIAllocate(sizeof(Control), true);
	nudgeDown->type = API_OBJECT_CONTROL;
	nudgeDown->context = scrollbar;
	nudgeDown->notificationCallback = OS_MAKE_CALLBACK(ScrollbarButtonPressed, SCROLLBAR_NUDGE_DOWN);
	nudgeDown->backgrounds = orientation ? scrollbarTrackVerticalBackgrounds : scrollbarTrackHorizontalBackgrounds;
	nudgeDown->useClickRepeat = true;
	OSSetCallback(nudgeDown, OS_MAKE_CALLBACK(ProcessButtonMessage, nullptr));

	command.callback = OS_MAKE_CALLBACK(ScrollbarButtonPressed, SCROLLBAR_BUTTON_UP);
	Control *up = (Control *) OSCreateButton(&command, OS_BUTTON_STYLE_REPEAT);
	up->backgrounds = scrollbarButtonHorizontalBackgrounds;
	up->context = scrollbar;
	up->icon = orientation ? &smallArrowUpNormal : &smallArrowLeftNormal;
	up->iconHasVariants = true;
	up->centerIcons = true;
	up->focusable = false;

	Control *grip = (Control *) GUIAllocate(sizeof(Control), true);
	grip->type = API_OBJECT_CONTROL;
	grip->context = scrollbar;
	grip->backgrounds = orientation ? scrollbarButtonVerticalBackgrounds : scrollbarButtonHorizontalBackgrounds;
	OSSetCallback(grip, OS_MAKE_CALLBACK(ProcessScrollbarGripMessage, nullptr));

	command.callback = OS_MAKE_CALLBACK(ScrollbarButtonPressed, SCROLLBAR_BUTTON_DOWN);
	Control *down = (Control *) OSCreateButton(&command, OS_BUTTON_STYLE_REPEAT);
	down->backgrounds = scrollbarButtonHorizontalBackgrounds;
	down->context = scrollbar;
	down->icon = orientation ? &smallArrowDownNormal : &smallArrowRightNormal;
	down->iconHasVariants = true;
	down->centerIcons = true;
	down->focusable = false;

	OSAddControl(scrollbar, 0, 0, up, OS_CELL_H_EXPAND);
	OSAddControl(scrollbar, 0, 1, grip, OS_CELL_V_EXPAND | OS_CELL_H_EXPAND);
	OSAddControl(scrollbar, 0, 2, down, OS_CELL_H_EXPAND);
	OSAddControl(scrollbar, 0, 3, nudgeUp, OS_CELL_V_EXPAND | OS_CELL_H_EXPAND);
	OSAddControl(scrollbar, 0, 4, nudgeDown, OS_CELL_V_EXPAND | OS_CELL_H_EXPAND);

	scrollbar->enabled = false;

	return scrollbar;
}

void OSDisableControl(OSObject _control, bool disabled) {
	OSMessage message;
	message.type = OS_MESSAGE_DISABLE;
	message.disable.disabled = disabled;
	OSSendMessage(_control, &message);
}

void OSSetObjectNotificationCallback(OSObject _object, OSCallback callback) {
	GUIObject *object = (GUIObject *) _object;
	object->notificationCallback = callback;
}

void OSSetCommandNotificationCallback(OSObject _window, OSCommand *_command, OSCallback callback) {
	Window *window = (Window *) _window;
	CommandWindow *command = window->commands + _command->identifier;
	command->notificationCallback = callback;

	LinkedItem<Control> *item = command->controls.firstItem;

	while (item) {
		Control *control = item->thisItem;
		control->notificationCallback = callback;
		item = item->nextItem;
	}
}

void OSDisableCommand(OSObject _window, OSCommand *_command, bool disabled) {
	Window *window = (Window *) _window;
	CommandWindow *command = window->commands + _command->identifier;
	if (command->disabled == disabled) return;
	command->disabled = disabled;

	LinkedItem<Control> *item = command->controls.firstItem;

	while (item) {
		Control *control = item->thisItem;
		OSDisableControl(control, disabled);
		item = item->nextItem;
	}
}

void OSCheckCommand(OSObject _window, OSCommand *_command, bool checked) {
	Window *window = (Window *) _window;

	if (_command->radioCheck) {
		for (uintptr_t i = 0; i < _commandCount; i++) {
			if ((int) i == _command->identifier) {
				continue;
			}

			if (_command->radioBytes != _commands[i]->radioBytes ||
					0 != OSCompareBytes(_command->radio, _commands[i]->radio, _command->radioBytes)) {
				continue;
			}

			CommandWindow *command = window->commands + i;
			command->checked = false;
			LinkedItem<Control> *item = command->controls.firstItem;

			while (item) {
				item->thisItem->isChecked = false;
				RepaintControl(item->thisItem);
				item = item->nextItem;
			}
		}
	} 

	CommandWindow *command = window->commands + _command->identifier;
	command->checked = checked;
	LinkedItem<Control> *item = command->controls.firstItem;

	while (item) {
		item->thisItem->isChecked = checked;
		RepaintControl(item->thisItem);
		item = item->nextItem;
	}
}

static inline int DistanceSquared(int x, int y) {
	return x * x + y * y;
}

static bool CompareKeyboardShortcut(OSCommand *command, OSMessage *message) {
	bool alt = message->keyboard.alt;
	bool shift = message->keyboard.shift;
	bool ctrl = message->keyboard.ctrl;
	unsigned scancode = message->keyboard.scancode;

	if (scancode == OS_SCANCODE_EQUALS && shift) {
		EnterDebugger();
	}

	size_t shortcutBytes = command->shortcutBytes;

	if (!shortcutBytes) {
		// This command does not have a shortcut.
		return false;
	}

	if (shortcutBytes >= 64) {
		// This command's shortcut is too long.
		return false;
	}

	char shortcut[64];
	OSCopyMemory(shortcut, command->shortcut, command->shortcutBytes);
	shortcut[command->shortcutBytes] = 0;

	for (uintptr_t i = 0; i < command->shortcutBytes; i++) {
		shortcut[i] = tolower(shortcut[i]);
	}

	// Look for prefixes.

	if (strstr(shortcut, "ctrl+")) {
		if (!ctrl) {
			return false;
		}
	} else if (ctrl) {
		return false;
	}

	if (strstr(shortcut, "shift+")) {
		if (!shift) {
			return false;
		}
	} else if (shift) {
		return false;
	}

	if (strstr(shortcut, "alt+")) {
		if (!alt) {
			return false;
		}
	} else if (alt) {
		return false;
	}

	// Then compare the actual scancode.

	char *position = shortcut + command->shortcutBytes;

	while (--position != shortcut) {
		if (*position == '+') {
			position++;
			break;
		}
	}

	const char *expected = nullptr;
	const char *alias = nullptr;

	switch (scancode) {
		case OS_SCANCODE_A:
			expected = "a";
			break;
		case OS_SCANCODE_B:
			expected = "b";
			break;
		case OS_SCANCODE_C:
			expected = "c";
			break;
		case OS_SCANCODE_D:
			expected = "d";
			break;
		case OS_SCANCODE_E:
			expected = "e";
			break;
		case OS_SCANCODE_F:
			expected = "f";
			break;
		case OS_SCANCODE_G:
			expected = "g";
			break;
		case OS_SCANCODE_H:
			expected = "h";
			break;
		case OS_SCANCODE_I:
			expected = "i";
			break;
		case OS_SCANCODE_J:
			expected = "j";
			break;
		case OS_SCANCODE_K:
			expected = "k";
			break;
		case OS_SCANCODE_L:
			expected = "l";
			break;
		case OS_SCANCODE_M:
			expected = "m";
			break;
		case OS_SCANCODE_N:
			expected = "n";
			break;
		case OS_SCANCODE_O:
			expected = "o";
			break;
		case OS_SCANCODE_P:
			expected = "p";
			break;
		case OS_SCANCODE_Q:
			expected = "q";
			break;
		case OS_SCANCODE_R:
			expected = "r";
			break;
		case OS_SCANCODE_S:
			expected = "s";
			break;
		case OS_SCANCODE_T:
			expected = "t";
			break;
		case OS_SCANCODE_U:
			expected = "u";
			break;
		case OS_SCANCODE_V:
			expected = "v";
			break;
		case OS_SCANCODE_W:
			expected = "w";
			break;
		case OS_SCANCODE_X:
			expected = "x";
			break;
		case OS_SCANCODE_Y:
			expected = "y";
			break;
		case OS_SCANCODE_Z:
			expected = "z";
			break;
		case OS_SCANCODE_0:
			expected = "0";
			alias = ")";
			break;
		case OS_SCANCODE_1:
			expected = "1";
			alias = "!";
			break;
		case OS_SCANCODE_2:
			expected = "2";
			alias = "@";
			break;
		case OS_SCANCODE_3:
			expected = "3";
			alias = "#";
			break;
		case OS_SCANCODE_4:
			expected = "4";
			alias = "$";
			break;
		case OS_SCANCODE_5:
			expected = "5";
			alias = "%";
			break;
		case OS_SCANCODE_6:
			expected = "6";
			alias = "^";
			break;
		case OS_SCANCODE_7:
			expected = "7";
			alias = "&";
			break;
		case OS_SCANCODE_8:
			expected = "8";
			alias = "*";
			break;
		case OS_SCANCODE_9:
			expected = "9";
			alias = "(";
			break;
		case OS_SCANCODE_CAPS_LOCK:
			break;
		case OS_SCANCODE_SCROLL_LOCK:
			break;
		case OS_SCANCODE_NUM_LOCK:
			break;
		case OS_SCANCODE_LEFT_SHIFT:
			break;
		case OS_SCANCODE_LEFT_CTRL:
			break;
		case OS_SCANCODE_LEFT_ALT:
			break;
		case OS_SCANCODE_LEFT_FLAG:
			break;
		case OS_SCANCODE_RIGHT_SHIFT:
			break;
		case OS_SCANCODE_RIGHT_CTRL:
			break;
		case OS_SCANCODE_RIGHT_ALT:
			break;
		case OS_SCANCODE_PAUSE:
			expected = "pause";
			break;
		case OS_SCANCODE_CONTEXT_MENU:
			break;
		case OS_SCANCODE_BACKSPACE:
			expected = "backspace";
			alias = "bkspc";
			break;
		case OS_SCANCODE_ESCAPE:
			expected = "escape";
			alias = "esc";
			break;
		case OS_SCANCODE_INSERT:
			expected = "insert";
			alias = "ins";
			break;
		case OS_SCANCODE_HOME:
			expected = "home";
			break;
		case OS_SCANCODE_PAGE_UP:
			expected = "page up";
			alias = "pgup";
			break;
		case OS_SCANCODE_DELETE:
			expected = "delete";
			alias = "del";
			break;
		case OS_SCANCODE_END:
			expected = "end";
			break;
		case OS_SCANCODE_PAGE_DOWN:
			expected = "page down";
			alias = "pgdn";
			break;
		case OS_SCANCODE_UP_ARROW:
			expected = "up";
			break;
		case OS_SCANCODE_LEFT_ARROW:
			expected = "left";
			break;
		case OS_SCANCODE_DOWN_ARROW:
			expected = "down";
			break;
		case OS_SCANCODE_RIGHT_ARROW:
			expected = "right";
			break;
		case OS_SCANCODE_SPACE:
			expected = "space";
			break;
		case OS_SCANCODE_TAB:
			expected = "tab";
			break;
		case OS_SCANCODE_ENTER:
			expected = "enter";
			alias = "return";
			break;
		case OS_SCANCODE_SLASH:
			expected = "/";
			alias = "?";
			break;
		case OS_SCANCODE_BACKSLASH:
			expected = "\\";
			alias = "|";
			break;
		case OS_SCANCODE_LEFT_BRACE:
			expected = "[";
			alias = "{";
			break;
		case OS_SCANCODE_RIGHT_BRACE:
			expected = "]";
			alias = "}";
			break;
		case OS_SCANCODE_EQUALS:
			expected = "=";
			alias = "+";
			break;
		case OS_SCANCODE_BACKTICK:
			expected = "`";
			alias = "~";
			break;
		case OS_SCANCODE_HYPHEN:
			expected = "-";
			alias = "_";
			break;
		case OS_SCANCODE_SEMICOLON:
			expected = ";";
			alias = ":";
			break;
		case OS_SCANCODE_QUOTE:
			expected = "'";
			alias = "\"";
			break;
		case OS_SCANCODE_COMMA:
			expected = ",";
			alias = "<";
			break;
		case OS_SCANCODE_PERIOD:
			expected = ".";
			alias = ">";
			break;
		case OS_SCANCODE_NUM_DIVIDE:
			break;
		case OS_SCANCODE_NUM_MULTIPLY:
			break;
		case OS_SCANCODE_NUM_SUBTRACT:
			break;
		case OS_SCANCODE_NUM_ADD:
			break;
		case OS_SCANCODE_NUM_ENTER:
			break;
		case OS_SCANCODE_NUM_POINT:
			break;
		case OS_SCANCODE_NUM_0:
			break;
		case OS_SCANCODE_NUM_1:
			break;
		case OS_SCANCODE_NUM_2:
			break;
		case OS_SCANCODE_NUM_3:
			break;
		case OS_SCANCODE_NUM_4:
			break;
		case OS_SCANCODE_NUM_5:
			break;
		case OS_SCANCODE_NUM_6:
			break;
		case OS_SCANCODE_NUM_7:
			break;
		case OS_SCANCODE_NUM_8:
			break;
		case OS_SCANCODE_NUM_9:
			break;
		case OS_SCANCODE_PRINT_SCREEN_1:
			break;
		case OS_SCANCODE_PRINT_SCREEN_2:
			break;
		case OS_SCANCODE_F1:
			expected = "f1";
			break;
		case OS_SCANCODE_F2:
			expected = "f2";
			break;
		case OS_SCANCODE_F3:
			expected = "f3";
			break;
		case OS_SCANCODE_F4:
			expected = "f4";
			break;
		case OS_SCANCODE_F5:
			expected = "f5";
			break;
		case OS_SCANCODE_F6:
			expected = "f6";
			break;
		case OS_SCANCODE_F7:
			expected = "f7";
			break;
		case OS_SCANCODE_F8:
			expected = "f8";
			break;
		case OS_SCANCODE_F9:
			expected = "f9";
			break;
		case OS_SCANCODE_F10:
			expected = "f10";
			break;
		case OS_SCANCODE_F11:
			expected = "f11";
			break;
		case OS_SCANCODE_F12:
			expected = "f12";
			break;
		case OS_SCANCODE_ACPI_POWER:
			break;
		case OS_SCANCODE_ACPI_SLEEP:
			break;
		case OS_SCANCODE_ACPI_WAKE:
			break;
		case OS_SCANCODE_MM_NEXT:
			break;
		case OS_SCANCODE_MM_PREVIOUS:
			break;
		case OS_SCANCODE_MM_STOP:
			break;
		case OS_SCANCODE_MM_PAUSE:
			break;
		case OS_SCANCODE_MM_MUTE:
			break;
		case OS_SCANCODE_MM_QUIETER:
			break;
		case OS_SCANCODE_MM_LOUDER:
			break;
		case OS_SCANCODE_MM_SELECT:
			break;
		case OS_SCANCODE_MM_EMAIL:
			break;
		case OS_SCANCODE_MM_CALC:
			break;
		case OS_SCANCODE_MM_FILES:
			break;
		case OS_SCANCODE_WWW_SEARCH:
			break;
		case OS_SCANCODE_WWW_HOME:
			break;
		case OS_SCANCODE_WWW_BACK:
			break;
		case OS_SCANCODE_WWW_FORWARD:
			break;
		case OS_SCANCODE_WWW_STOP:
			break;
		case OS_SCANCODE_WWW_REFRESH:
			break;
		case OS_SCANCODE_WWW_STARRED:
			break;
	}

	if (expected && 0 == strcmp(position, expected)) {
		// The strings matched.
		return true;
	}

	if (alias && 0 == strcmp(position, alias)) {
		// The strings matched.
		return true;
	}

	return false;
}

static OSCallbackResponse ProcessWindowMessage(OSObject _object, OSMessage *message) {
	OSCallbackResponse response = OS_CALLBACK_HANDLED;
	Window *window = (Window *) _object;
	window->willUpdateAfterMessageProcessing = true;

	static int lastClickX = 0, lastClickY = 0;
	static bool draggingStarted = false;
	bool updateWindow = false;
	bool rightClick = false;

	if (window->destroyed && message->type != OS_MESSAGE_WINDOW_DESTROYED) {
		return OS_CALLBACK_NOT_HANDLED;
	}

	switch (message->type) {
		case OS_MESSAGE_WINDOW_CREATED: {
			window->created = true;
			updateWindow = true;
		} break;

		case OS_MESSAGE_WINDOW_ACTIVATED: {
			if (!window->activated) {
				for (int i = 0; i < 12; i++) {
					if (i == 7 || (window->flags & OS_CREATE_WINDOW_MENU)) continue;
					OSDisableControl(window->root->objects[i], false);
				}

				if (window->lastFocus) {
					OSMessage message;
					message.type = OS_MESSAGE_CLIPBOARD_UPDATED;
					OSSyscall(OS_SYSCALL_GET_CLIPBOARD_HEADER, 0, (uintptr_t) &message.clipboard, 0, 0);
					OSSendMessage(window->lastFocus, &message);
				}

				OSSetFocusedControl(window->defaultFocus, false);

				window->activated = true;
			}
		} break;

		case OS_MESSAGE_WINDOW_DEACTIVATED: {
			for (int i = 0; i < 12; i++) {
				if (i == 7 || (window->flags & OS_CREATE_WINDOW_MENU)) continue;
				OSDisableControl(window->root->objects[i], true);
			}

			OSRemoveFocusedControl(window, true);

			if (window->flags & OS_CREATE_WINDOW_MENU) {
				message->type = OS_MESSAGE_DESTROY;
				OSSendMessage(window, message);
			}

			if (openMenuCount && openMenus[0].window == window) {
				navigateMenuMode = false;
			}

			window->activated = false;
		} break;

		case OS_MESSAGE_WINDOW_DESTROYED: {
			{
				OSMessage m;
				m.type = OS_NOTIFICATION_DIALOG_CLOSE;
				response = OSForwardMessage(window, window->notificationCallback, &m);
			}

			if (!window->hasMenuParent) {
				GUIFree(window->commands);
			}

			GUIFree(window);
			return response;
		} break;

		case OS_MESSAGE_KEY_RELEASED: {
			if (navigateMenuMode) {
				if (message->keyboard.scancode == OS_SCANCODE_ENTER && navigateMenuItem) {
					window->pressed = nullptr;
					navigateMenuItem->pressedByKeyboard = false;
					OSAnimateControl(navigateMenuItem, false);
					IssueCommand(navigateMenuItem);
				}
			} else {
				GUIObject *control = window->lastFocus;
				OSCallbackResponse response = OSSendMessage(control, message);

				if (response == OS_CALLBACK_NOT_HANDLED && message->keyboard.scancode == OS_SCANCODE_LEFT_ALT) {
					if (window->flags & OS_CREATE_WINDOW_WITH_MENUBAR) {
						navigateMenuUsedKey = true;
						MenuItem *control = (MenuItem *) ((Grid *) ((Grid *) window->root->objects[7])->objects[0])->objects[0];
						OSCreateMenu((OSMenuSpecification *) control->item.value, control, OS_CREATE_MENU_AT_SOURCE, OS_CREATE_MENU_FROM_MENUBAR);
					}
				}
			}
		} break;

		case OS_MESSAGE_KEY_PRESSED: {
			if (message->keyboard.scancode == OS_SCANCODE_F2 && message->keyboard.alt) {
				EnterDebugger();
			}

			if (navigateMenuMode) {
				if (navigateMenuItem) {
					RepaintControl(navigateMenuItem);
				}
				
				navigateMenuUsedKey = true;

				if (message->keyboard.scancode == OS_SCANCODE_ESCAPE) {
					if (window->hasMenuParent) {
						OSMessage m;
						m.type = OS_MESSAGE_DESTROY;
						OSSendMessage(window, &m);
					} 

					if (openMenuCount <= 1) {
						navigateMenuMode = false;
					}
				} else if (message->keyboard.scancode == OS_SCANCODE_LEFT_ARROW) {
					if (openMenuCount == 1) {
						if (((GUIObject *) openMenus[0].source->parent)->isMenubar) {
							Grid *parent = (Grid *) openMenus[0].source->parent;
							int i = FindObjectInGrid(parent, openMenus[0].source) - 1;

							if (i == -1) {
								i += parent->columns;
							}

							MenuItem *control = (MenuItem *) parent->objects[i];
							OSCreateMenu((OSMenuSpecification *) control->item.value, control, OS_CREATE_MENU_AT_SOURCE, OS_CREATE_MENU_FROM_MENUBAR);
						}
					} else {
						OSMessage m;
						m.type = OS_MESSAGE_DESTROY;
						OSSendMessage(window, &m);
					}
				} else if (message->keyboard.scancode == OS_SCANCODE_RIGHT_ARROW) {
					if (navigateMenuItem && navigateMenuItem->item.type == OSMenuItem::SUBMENU) {
						OSCreateMenu((OSMenuSpecification *) navigateMenuItem->item.value, navigateMenuItem, OS_CREATE_MENU_AT_SOURCE, OS_CREATE_SUBMENU);
					} else if (openMenuCount == 1 && ((GUIObject *) openMenus[0].source->parent)->isMenubar) {
						Grid *parent = (Grid *) openMenus[0].source->parent;
						int i = FindObjectInGrid(parent, openMenus[0].source) + 1;

						if (i == (int) parent->columns) {
							i -= parent->columns;
						}

						MenuItem *control = (MenuItem *) parent->objects[i];
						OSCreateMenu((OSMenuSpecification *) control->item.value, control, OS_CREATE_MENU_AT_SOURCE, OS_CREATE_MENU_FROM_MENUBAR);
					}
				} else if (message->keyboard.scancode == OS_SCANCODE_UP_ARROW) {
					if (!navigateMenuItem) {
						navigateMenuItem = navigateMenuFirstItem;
					}

					Grid *parent = (Grid *) navigateMenuItem->parent;

					do {
						int i = FindObjectInGrid(parent, navigateMenuItem) - 1;

						if (i == -1) {
							i += parent->rows;
						}

						navigateMenuItem = (MenuItem *) parent->objects[i];
					} while (!navigateMenuItem->tabStop);
				} else if (message->keyboard.scancode == OS_SCANCODE_DOWN_ARROW) {
					bool f = false;

					if (!navigateMenuItem) {
						navigateMenuItem = navigateMenuFirstItem;
						f = true;
					}

					Grid *parent = (Grid *) navigateMenuItem->parent;

					do {
						int i = FindObjectInGrid(parent, navigateMenuItem) + (f ? 0 : 1);

						if (i == (int) parent->rows) {
							i -= parent->rows;
						}

						navigateMenuItem = (MenuItem *) parent->objects[i];
					} while (!navigateMenuItem->tabStop);
				} else if (message->keyboard.scancode == OS_SCANCODE_ENTER && navigateMenuItem) {
					window->pressed = navigateMenuItem;
					navigateMenuItem->pressedByKeyboard = true;
					OSAnimateControl(navigateMenuItem, true);
				}

				if (navigateMenuItem) {
					RepaintControl(navigateMenuItem);
				}

				break;
			}
			
			if (message->keyboard.scancode == OS_SCANCODE_F4 && message->keyboard.alt) {
				message->type = OS_MESSAGE_DESTROY;
				OSSendMessage(window, message);
			} else if (message->keyboard.scancode == OS_SCANCODE_ESCAPE) {
				if (window->hasMenuParent || (window->flags & OS_CREATE_WINDOW_DIALOG)) {
					OSMessage m;
					m.type = OS_MESSAGE_DESTROY;
					OSSendMessage(window, &m);
				} else {
					OSRemoveFocusedControl(window, true);
				}
			} else {
				OSCallbackResponse response = OS_CALLBACK_NOT_HANDLED;

				if (window->focus) {
					message->type = OS_MESSAGE_KEY_TYPED;
					response = OSSendMessage(window->focus, message);
				}

				GUIObject *control = window->lastFocus;
				message->keyboard.notHandledBy = nullptr;
				message->type = OS_MESSAGE_KEY_PRESSED;

				while (control && control != window && response == OS_CALLBACK_NOT_HANDLED) {
					response = OSSendMessage(control, message);
					message->keyboard.notHandledBy = control;
					control = (GUIObject *) control->parent;
				}

				if (response == OS_CALLBACK_NOT_HANDLED) {
					for (uintptr_t i = 0; i < _commandCount; i++) {
						if (CompareKeyboardShortcut(_commands[i], message)) {
							IssueCommand(nullptr, _commands[i], window);
							response = OS_CALLBACK_HANDLED;
							break;
						}
					}
				}

				if (response == OS_CALLBACK_NOT_HANDLED) {
					if (message->keyboard.scancode == OS_SCANCODE_TAB 
							&& !message->keyboard.ctrl && !message->keyboard.alt) {
						message->keyboard.notHandledBy = nullptr;
						OSSendMessage(window->root, message);
					} else if (message->keyboard.scancode == OS_SCANCODE_ENTER
							&& !message->keyboard.ctrl && !message->keyboard.alt && !message->keyboard.shift
							&& window->defaultCommand) {
						IssueCommand(nullptr, window->defaultCommand, window);
					}

					// TODO Access keys.
				}
			}
		} break;

		case OS_MESSAGE_DESTROY: {
			for (uintptr_t i = 0; i < openMenuCount; i++) {
				if (openMenus[i].window == window) {
					if (i + 1 != openMenuCount) OSSendMessage(openMenus[i + 1].window, message);
					
					if (openMenus[i].source) {
						OSAnimateControl(openMenus[i].source, true);

						if (i >= 1) {
							navigateMenuItem = (MenuItem *) openMenus[i].source;
						} else {
							navigateMenuItem = nullptr;
							navigateMenuMode = false;
							navigateMenuUsedKey = false;
						}
					}

					openMenuCount = i;
					break;
				}
			}

			if (!window->hasMenuParent) {
				OSMessage message = {};
				message.type = OS_NOTIFICATION_COMMAND;
				message.command.window = window;
				message.command.command = osCommandDestroyWindow;
				int32_t identifier = osCommandDestroyWindow->identifier;
				OSForwardMessage(nullptr, window->commands[identifier].notificationCallback, &message);
			}

			OSSendMessage(window->root, message);
			OSCloseHandle(window->surface);
			OSCloseHandle(window->window);
			window->destroyed = true;
		} break;

		case OS_MESSAGE_CLICK_REPEAT: {
			if (window->pressed) {
				OSSendMessage(window->pressed, message);

				if (window->pressed == window->hover && window->pressed->useClickRepeat) {
					OSMessage clicked;
					clicked.type = OS_MESSAGE_CLICKED;
					OSSendMessage(window->pressed, &clicked);
				}
			} else if (window->hoverGrid) {
				OSSendMessage(window->hoverGrid, message);
			}
		} break;

		case OS_MESSAGE_MOUSE_RIGHT_PRESSED:
			rightClick = true;
			// Fallthrough
		case OS_MESSAGE_MOUSE_LEFT_PRESSED: {
			bool allowFocus = false;

			// If there is a control we're hovering over that isn't disabled,
			// and this either isn't an window activation click or the control is fine with activation clicks:
			if (window->hover && !window->hover->disabled
					&& (!message->mousePressed.activationClick || !window->hover->ignoreActivationClicks)) {
				// Send the raw WM message to the control.
				OSSendMessage(window->hover, message);

				// This control can get the focus from this message.
				allowFocus = true;

				// Press the control.
				window->pressed = window->hover;

				draggingStarted = false;
				lastClickX = message->mousePressed.positionX;
				lastClickY = message->mousePressed.positionY;

				OSMessage m = *message;
				m.type = OS_MESSAGE_START_PRESS;
				OSSendMessage(window->pressed, &m);

				if (!rightClick && window->hover->useClickRepeat) {
					OSMessage clicked;
					clicked.type = OS_MESSAGE_CLICKED;
					OSSendMessage(window->pressed, &clicked);
				}
			}

			// If there isn't a control we're hovering over,
			// but we are hovering over a grid,
			// send it the raw message.
			if (!window->hover && window->hoverGrid) {
				OSSendMessage(window->hoverGrid, message);
			}

			// If a different control had focus, it must lose it.
			if (window->focus != window->hover) {
				OSRemoveFocusedControl(window, false);
			}

			// If this control should be given focus...
			if (allowFocus) {
				Control *control = window->hover;

				// And it is focusable...
				if (control->focusable) {
					OSSetFocusedControl(control, false);
				}
			}
		} break;

		case OS_MESSAGE_MOUSE_MIDDLE_RELEASED:
		case OS_MESSAGE_MOUSE_RIGHT_RELEASED:
			rightClick = true; // Fallthrough.
		case OS_MESSAGE_MOUSE_LEFT_RELEASED: {
			if (window->pressed) {
				// Send the raw message.
				OSSendMessage(window->pressed, message);

				RepaintControl(window->pressed);

				if (!rightClick) {
					if (window->pressed == window->hover && !window->pressed->useClickRepeat) {
						OSMessage clicked;
						clicked.type = OS_MESSAGE_CLICKED;
						OSSendMessage(window->pressed, &clicked);
					}
				}

				OSMessage message;
				message.type = OS_MESSAGE_END_PRESS;
				OSSendMessage(window->pressed, &message);
				window->pressed = nullptr;
			}

			OSMessage m = *message;
			m.type = OS_MESSAGE_MOUSE_MOVED;
			m.mouseMoved.oldPositionX = message->mousePressed.positionX;
			m.mouseMoved.oldPositionY = message->mousePressed.positionY;
			m.mouseMoved.newPositionX = message->mousePressed.positionX;
			m.mouseMoved.newPositionY = message->mousePressed.positionY;
			m.mouseMoved.newPositionXScreen = message->mousePressed.positionXScreen;
			m.mouseMoved.newPositionYScreen = message->mousePressed.positionYScreen;
			OSSendMessage(window->root, &m);
		} break;

		case OS_MESSAGE_MOUSE_EXIT: {
			if (window->hover) {
				OSMessage message;
				message.type = OS_MESSAGE_END_HOVER;
				OSSendMessage(window->hover, &message);
				window->hover = nullptr;
			}
		} break;

		case OS_MESSAGE_MOUSE_MOVED: {
			if (window->pressed) {
				OSMessage hitTest;
				hitTest.type = OS_MESSAGE_HIT_TEST;
				hitTest.hitTest.positionX = message->mouseMoved.newPositionX;
				hitTest.hitTest.positionY = message->mouseMoved.newPositionY;
				OSCallbackResponse response = OSSendMessage(window->pressed, &hitTest);

				if (response == OS_CALLBACK_HANDLED && !hitTest.hitTest.result && window->hover) {
					OSMessage message;
					message.type = OS_MESSAGE_END_HOVER;
					OSSendMessage(window->hover, &message);
					window->hover = nullptr;
				} else if (response == OS_CALLBACK_HANDLED && hitTest.hitTest.result && !window->hover) {
					OSMessage message;
					message.type = OS_MESSAGE_START_HOVER;
					window->hover = window->pressed;
					OSSendMessage(window->hover, &message);
				}

				if (draggingStarted || DistanceSquared(message->mouseMoved.newPositionX, message->mouseMoved.newPositionY) >= 4) {
					message->mouseDragged.originalPositionX = lastClickX;
					message->mouseDragged.originalPositionY = lastClickY;

					if (!draggingStarted) {
						draggingStarted = true;
						message->type = OS_MESSAGE_START_DRAG;
						OSSendMessage(window->pressed, message);
					}

					message->type = OS_MESSAGE_MOUSE_DRAGGED;
					OSSendMessage(window->pressed, message);
				}
			} else {
				Control *old = window->hover;

				OSMessage hitTest;
				hitTest.type = OS_MESSAGE_HIT_TEST;
				hitTest.hitTest.positionX = message->mouseMoved.newPositionX;
				hitTest.hitTest.positionY = message->mouseMoved.newPositionY;
				OSCallbackResponse response = OSSendMessage(old, &hitTest);

				if (!hitTest.hitTest.result || response == OS_CALLBACK_NOT_HANDLED) {
					window->hover = nullptr;
					OSSendMessage(window->root, message);
				} else {
					OSSendMessage(old, message);
				}

				if (!window->hover) {
					window->cursor = OS_CURSOR_NORMAL;
				}

				if (window->hover != old && old) {
					OSMessage message;
					message.type = OS_MESSAGE_END_HOVER;
					OSSendMessage(old, &message);
				}
			}
		} break;

		case OS_MESSAGE_WM_TIMER: {
			LinkedItem<Control> *item = window->timerControls.firstItem;

			while (item) {
				int ticksPerMessage = window->timerHz / item->thisItem->timerHz;
				item->thisItem->timerStep++;

				if (item->thisItem->timerStep >= ticksPerMessage) {
					item->thisItem->timerStep = 0;
					OSSendMessage(item->thisItem, message);
				}

				item = item->nextItem;
			}

			if (window->lastFocus) {
				window->caretBlinkStep++;

				if (window->caretBlinkStep >= window->timerHz / CARET_BLINK_HZ) {
					window->caretBlinkStep = 0;

					OSMessage message;
					message.type = OS_MESSAGE_CARET_BLINK;
					OSSendMessage(window->lastFocus, &message);
				}
			}
		} break;

		case OS_MESSAGE_CLIPBOARD_UPDATED: {
			if (window->lastFocus) {
				OSSendMessage(window->lastFocus, message);
			}
		} break;

		default: {
			response = OS_CALLBACK_NOT_HANDLED;
		} break;
	}

	if (window->destroyed) {
		return response;
	}

	{
		// TODO Only do this if the list or focus has changed.

		LinkedItem<Control> *item = window->timerControls.firstItem;
		int largestHz = window->focus ? CARET_BLINK_HZ : 0;

		while (item) {
			int thisHz = item->thisItem->timerHz;
			if (thisHz > largestHz) largestHz = thisHz;
			item = item->nextItem;
		}

		if (window->timerHz != largestHz) {
			window->timerHz = largestHz;
			OSSyscall(OS_SYSCALL_NEED_WM_TIMER, window->window, largestHz, 0, 0);
		}
	}

	if (window->descendentInvalidationFlags & DESCENDENT_RELAYOUT) {
		window->descendentInvalidationFlags &= ~DESCENDENT_RELAYOUT;

		OSMessage message;
		message.type = OS_MESSAGE_LAYOUT;
		message.layout.clip = OS_MAKE_RECTANGLE(0, window->width, 0, window->height);
		message.layout.left = 0;
		message.layout.top = 0;
		message.layout.right = window->width;
		message.layout.bottom = window->height;
		message.layout.force = false;
		OSSendMessage(window->root, &message);
	}

	if (window->cursor != window->cursorOld) {
		OSSyscall(OS_SYSCALL_SET_CURSOR_STYLE, window->window, window->cursor, 0, 0);
		window->cursorOld = window->cursor;
		updateWindow = true;
	}

	if (window->descendentInvalidationFlags & DESCENDENT_REPAINT) {
		window->descendentInvalidationFlags &= ~DESCENDENT_REPAINT;

		OSMessage message;
		message.type = OS_MESSAGE_PAINT;
		message.paint.clip = OS_MAKE_RECTANGLE(0, window->width, 0, window->height);
		message.paint.surface = window->surface;
		message.paint.force = false;
		OSSendMessage(window->root, &message);
		updateWindow = true;
	}

	if (updateWindow) {
		OSSyscall(OS_SYSCALL_UPDATE_WINDOW, window->window, 0, 0, 0);
	}

	window->willUpdateAfterMessageProcessing = false;

	return response;
}

static Window *CreateWindow(OSWindowSpecification *specification, Window *menuParent, unsigned x = 0, unsigned y = 0, Window *modalParent = nullptr) {
	unsigned flags = specification->flags;

	if (!flags) {
		flags = OS_CREATE_WINDOW_NORMAL;
	}

	if (specification->menubar) {
		flags |= OS_CREATE_WINDOW_WITH_MENUBAR;
	}

	int width = specification->width;
	int height = specification->height;
	int minimumWidth = specification->minimumWidth;
	int minimumHeight = specification->minimumHeight;

	if (!(flags & OS_CREATE_WINDOW_MENU)) {
		width += totalBorderWidth;
		minimumWidth += totalBorderWidth;
		height += totalBorderHeight;
		minimumHeight += totalBorderHeight;
	}

	Window *window = (Window *) GUIAllocate(sizeof(Window), true);
	window->type = API_OBJECT_WINDOW;

	if (menuParent) {
		window->instance = menuParent->instance;
	}

	if (modalParent) {
		window->instance = modalParent->instance;
	}

	OSRectangle bounds;
	bounds.left = x;
	bounds.right = x + width;
	bounds.top = y;
	bounds.bottom = y + height;

	window->window = modalParent ? modalParent->window : OS_INVALID_HANDLE;
	OSSyscall(OS_SYSCALL_CREATE_WINDOW, (uintptr_t) &window->window, (uintptr_t) &bounds, (uintptr_t) window, menuParent ? (uintptr_t) menuParent->window : 0);
	OSSyscall(OS_SYSCALL_GET_WINDOW_BOUNDS, window->window, (uintptr_t) &bounds, 0, 0);

	window->width = bounds.right - bounds.left;
	window->height = bounds.bottom - bounds.top;
	window->flags = flags;
	window->cursor = OS_CURSOR_NORMAL;
	window->minimumWidth = minimumWidth;
	window->minimumHeight = minimumHeight;
	window->defaultCommand = specification->defaultCommand;

	if (!menuParent) {
		window->commands = (CommandWindow *) GUIAllocate(sizeof(CommandWindow) * _commandCount, true);

		for (uintptr_t i = 0; i < _commandCount; i++) {
			CommandWindow *command = window->commands + i;
			command->disabled = _commands[i]->defaultDisabled;
			command->checked = _commands[i]->defaultCheck;
			command->notificationCallback = _commands[i]->callback;
		}
	} else {
		window->commands = menuParent->commands;
		window->hasMenuParent = true;
	}

	OSSetCallback(window, OS_MAKE_CALLBACK(ProcessWindowMessage, nullptr));

	window->root = (Grid *) OSCreateGrid(3, 4, OS_GRID_STYLE_LAYOUT);
	window->root->parent = window;

	{
		OSMessage message;
		message.parentUpdated.window = window;
		message.type = OS_MESSAGE_PARENT_UPDATED;
		OSSendMessage(window->root, &message);
	}

	if (flags & OS_CREATE_WINDOW_NORMAL) {
		OSObject titlebar = CreateWindowResizeHandle(windowBorder22, RESIZE_MOVE);
		OSAddControl(window->root, 1, 1, titlebar, OS_CELL_H_PUSH | OS_CELL_H_EXPAND | OS_CELL_V_EXPAND);
		OSSetText(titlebar, specification->title, specification->titleBytes, OS_RESIZE_MODE_IGNORE);

		if (flags & OS_CREATE_WINDOW_DIALOG) {
			OSAddControl(window->root, 0, 0, CreateWindowResizeHandle(dialogBorder11, RESIZE_MOVE), 0);
			OSAddControl(window->root, 1, 0, CreateWindowResizeHandle(dialogBorder12, RESIZE_MOVE), OS_CELL_H_PUSH | OS_CELL_H_EXPAND);
			OSAddControl(window->root, 2, 0, CreateWindowResizeHandle(dialogBorder13, RESIZE_MOVE), 0);
			OSAddControl(window->root, 0, 1, CreateWindowResizeHandle(dialogBorder21, RESIZE_MOVE), 0);
			OSAddControl(window->root, 2, 1, CreateWindowResizeHandle(dialogBorder23, RESIZE_MOVE), 0);
			OSAddControl(window->root, 0, 2, CreateWindowResizeHandle(dialogBorder31, RESIZE_NONE), OS_CELL_V_PUSH | OS_CELL_V_EXPAND);
			OSAddControl(window->root, 2, 2, CreateWindowResizeHandle(dialogBorder33, RESIZE_NONE), OS_CELL_V_PUSH | OS_CELL_V_EXPAND);
			OSAddControl(window->root, 0, 3, CreateWindowResizeHandle(dialogBorder41, RESIZE_NONE), 0);
			OSAddControl(window->root, 1, 3, CreateWindowResizeHandle(dialogBorder42, RESIZE_NONE), OS_CELL_H_PUSH | OS_CELL_H_EXPAND);
			OSAddControl(window->root, 2, 3, CreateWindowResizeHandle(dialogBorder43, RESIZE_NONE), 0);
		} else {
			OSAddControl(window->root, 0, 0, CreateWindowResizeHandle(windowBorder11, RESIZE_TOP_LEFT), 0);
			OSAddControl(window->root, 1, 0, CreateWindowResizeHandle(windowBorder12, RESIZE_TOP), OS_CELL_H_PUSH | OS_CELL_H_EXPAND);
			OSAddControl(window->root, 2, 0, CreateWindowResizeHandle(windowBorder13, RESIZE_TOP_RIGHT), 0);
			OSAddControl(window->root, 0, 1, CreateWindowResizeHandle(windowBorder21, RESIZE_LEFT), 0);
			OSAddControl(window->root, 2, 1, CreateWindowResizeHandle(windowBorder23, RESIZE_RIGHT), 0);
			OSAddControl(window->root, 0, 2, CreateWindowResizeHandle(windowBorder31, RESIZE_LEFT), OS_CELL_V_PUSH | OS_CELL_V_EXPAND);
			OSAddControl(window->root, 2, 2, CreateWindowResizeHandle(windowBorder33, RESIZE_RIGHT), OS_CELL_V_PUSH | OS_CELL_V_EXPAND);
			OSAddControl(window->root, 0, 3, CreateWindowResizeHandle(windowBorder41, RESIZE_BOTTOM_LEFT), 0);
			OSAddControl(window->root, 1, 3, CreateWindowResizeHandle(windowBorder42, RESIZE_BOTTOM), OS_CELL_H_PUSH | OS_CELL_H_EXPAND);
			OSAddControl(window->root, 2, 3, CreateWindowResizeHandle(windowBorder43, RESIZE_BOTTOM_RIGHT), 0);
		}

		if (flags & OS_CREATE_WINDOW_WITH_MENUBAR) {
			OSObject grid = OSCreateGrid(1, 2, OS_GRID_STYLE_LAYOUT);
			OSAddGrid(window->root, 1, 2, grid, OS_CELL_FILL);
			OSAddGrid(grid, 0, 0, OSCreateMenu(specification->menubar, nullptr, OS_MAKE_POINT(0, 0), OS_CREATE_MENUBAR), OS_CELL_H_PUSH | OS_CELL_H_EXPAND);
		}
	}

	OSRemoveFocusedControl(window, true);

	return window;
}

OSObject OSCreateWindow(OSWindowSpecification *specification) {
	return CreateWindow(specification, nullptr);
}

static OSCallbackResponse CommandDialogAlertOK(OSObject object, OSMessage *message) {
	(void) object;

	if (message->type == OS_NOTIFICATION_COMMAND) {
		OSCloseWindow(message->context);
		return OS_CALLBACK_HANDLED;
	}

	return OS_CALLBACK_NOT_HANDLED;
}

static OSCallbackResponse CommandDialogAlertCancel(OSObject object, OSMessage *message) {
	(void) object;

	if (message->type == OS_NOTIFICATION_COMMAND) {
		OSCloseWindow(message->context);
		return OS_CALLBACK_HANDLED;
	}

	return OS_CALLBACK_NOT_HANDLED;
}

void OSCloseWindow(OSObject window) {
	OSMessage m;
	m.type = OS_MESSAGE_DESTROY;
	OSSendMessage(window, &m);
}

OSObject OSShowDialogConfirm(char *title, size_t titleBytes,
				   char *message, size_t messageBytes,
				   char *description, size_t descriptionBytes,
				   uint16_t iconID, OSObject modalParent, OSCommand *command) {
	OSWindowSpecification specification = *osDialogStandard;
	specification.title = title;
	specification.titleBytes = titleBytes;

	OSObject dialog = CreateWindow(&specification, nullptr, 0, 0, (Window *) modalParent);

	OSObject layout1 = OSCreateGrid(1, 2, OS_GRID_STYLE_LAYOUT);
	OSObject layout2 = OSCreateGrid(3, 1, OS_GRID_STYLE_CONTAINER);
	OSObject layout3 = OSCreateGrid(1, 2, OS_GRID_STYLE_CONTAINER_WITHOUT_BORDER);
	OSObject layout4 = OSCreateGrid(1, 1, OS_GRID_STYLE_CONTAINER_ALT);
	OSObject layout5 = OSCreateGrid(2, 1, OS_GRID_STYLE_CONTAINER_WITHOUT_BORDER);

	OSSetRootGrid(dialog, layout1);
	OSAddGrid(layout1, 0, 0, layout2, OS_CELL_FILL);
	OSAddGrid(layout2, 2, 0, layout3, OS_CELL_FILL);
	OSAddGrid(layout1, 0, 1, layout4, OS_CELL_H_EXPAND);
	OSAddGrid(layout4, 0, 0, layout5, OS_CELL_H_RIGHT | OS_CELL_H_PUSH);

	OSObject okButton = OSCreateButton(command, OS_BUTTON_STYLE_NORMAL);
	OSAddControl(layout5, 0, 0, okButton, OS_CELL_H_RIGHT);
	OSSetCommandNotificationCallback(dialog, osDialogStandardOK, OS_MAKE_CALLBACK(CommandDialogAlertOK, dialog));

	OSObject cancelButton = OSCreateButton(osDialogStandardCancel, OS_BUTTON_STYLE_NORMAL);
	OSAddControl(layout5, 1, 0, cancelButton, OS_CELL_H_RIGHT);
	OSSetCommandNotificationCallback(dialog, osDialogStandardCancel, OS_MAKE_CALLBACK(CommandDialogAlertCancel, dialog));

	Control *label = (Control *) OSCreateLabel(message, messageBytes);
	label->textSize = 10;
	label->textColor = TEXT_COLOR_HEADING;
	OSAddControl(layout3, 0, 0, label, OS_CELL_H_EXPAND | OS_CELL_H_PUSH);

	OSAddControl(layout3, 0, 1, OSCreateLabel(description, descriptionBytes), OS_CELL_H_EXPAND | OS_CELL_H_PUSH);
	OSAddControl(layout2, 0, 0, OSCreateIconDisplay(iconID), OS_CELL_V_TOP);

	OSSetFocusedControl(cancelButton, false);

	return dialog;
}

OSObject OSShowDialogAlert(char *title, size_t titleBytes,
				   char *message, size_t messageBytes,
				   char *description, size_t descriptionBytes,
				   uint16_t iconID, OSObject modalParent) {
	OSWindowSpecification specification = *osDialogStandard;
	specification.title = title;
	specification.titleBytes = titleBytes;

	OSObject dialog = CreateWindow(&specification, nullptr, 0, 0, (Window *) modalParent);

	OSObject layout1 = OSCreateGrid(1, 2, OS_GRID_STYLE_LAYOUT);
	OSObject layout2 = OSCreateGrid(3, 1, OS_GRID_STYLE_CONTAINER);
	OSObject layout3 = OSCreateGrid(1, 2, OS_GRID_STYLE_CONTAINER_WITHOUT_BORDER);
	OSObject layout4 = OSCreateGrid(1, 1, OS_GRID_STYLE_CONTAINER_ALT);

	OSSetRootGrid(dialog, layout1);
	OSAddGrid(layout1, 0, 0, layout2, OS_CELL_FILL);
	OSAddGrid(layout2, 2, 0, layout3, OS_CELL_FILL);
	OSAddGrid(layout1, 0, 1, layout4, OS_CELL_H_EXPAND);

	OSObject okButton = OSCreateButton(osDialogStandardOK, OS_BUTTON_STYLE_NORMAL);
	OSAddControl(layout4, 0, 0, okButton, OS_CELL_H_PUSH | OS_CELL_H_RIGHT);
	OSSetCommandNotificationCallback(dialog, osDialogStandardOK, OS_MAKE_CALLBACK(CommandDialogAlertOK, dialog));

	Control *label = (Control *) OSCreateLabel(message, messageBytes);
	label->textSize = 10;
	label->textColor = TEXT_COLOR_HEADING;
	OSAddControl(layout3, 0, 0, label, OS_CELL_H_EXPAND | OS_CELL_H_PUSH);

	OSAddControl(layout3, 0, 1, OSCreateLabel(description, descriptionBytes), OS_CELL_H_EXPAND | OS_CELL_H_PUSH);
	OSAddControl(layout2, 0, 0, OSCreateIconDisplay(iconID), OS_CELL_V_TOP);

	OSSetFocusedControl(okButton, false);

	return dialog;
}

OSObject OSCreateMenu(OSMenuSpecification *menuSpecification, OSObject _source, OSPoint position, unsigned flags) {
	Control *source = (Control *) _source;

	if (source) {
		RepaintControl(source);
	}

	size_t itemCount = menuSpecification->itemCount;
	bool menubar = flags & OS_CREATE_MENUBAR;

	Control *items[itemCount];
	int width = 0, height = 7;

	MenuItem *firstMenuItem = nullptr;

	for (uintptr_t i = 0; i < itemCount; i++) {
		switch (menuSpecification->items[i].type) {
			case OSMenuItem::SEPARATOR: {
				items[i] = (Control *) CreateMenuSeparator();
			} break;

			case OSMenuItem::SUBMENU:
			case OSMenuItem::COMMAND: {
				items[i] = (Control *) CreateMenuItem(menuSpecification->items[i], menubar);
				if (!firstMenuItem) firstMenuItem = (MenuItem *) items[i];
			} break;

			default: {} continue;
		}

		if (menubar) {
			if (items[i]->preferredHeight > height) {
				height = items[i]->preferredHeight;
			}

			width += items[i]->preferredWidth;
		} else {
			if (items[i]->preferredWidth > width) {
				width = items[i]->preferredWidth;
			}

			height += items[i]->preferredHeight;
		}
	}

	if (!menubar && !(flags & OS_CREATE_MENU_BLANK)) {
		if (width < 100) width = 100;
		width += 8;
	}

	if (width < menuSpecification->minimumWidth) {
		width = menuSpecification->minimumWidth;
	}

	if (height < menuSpecification->minimumHeight) {
		height = menuSpecification->minimumHeight;
	}

	OSObject grid = nullptr;

	if (!(flags & OS_CREATE_MENU_BLANK)) {
		grid = OSCreateGrid(menubar ? itemCount : 1, !menubar ? itemCount : 1, menubar ? OS_GRID_STYLE_MENUBAR : OS_GRID_STYLE_MENU);
		((Grid *) grid)->tabStop = false;
		((Grid *) grid)->isMenubar = menubar;
	}
	
	OSObject returnValue = grid;

	if (!menubar) {
		OSWindowSpecification specification = {};
		specification.width = width;
		specification.height = height;
		specification.flags = OS_CREATE_WINDOW_MENU;

		Window *window;
		int x = position.x;
		int y = position.y;

		if (x == -1 && y == -1 && source && x != -2) {
			if (flags & OS_CREATE_SUBMENU) {
				x = source->bounds.right;
				y = source->bounds.top;
			} else {
				x = source->bounds.left;
				y = source->bounds.bottom - 2;
			}

			OSRectangle bounds;
			OSSyscall(OS_SYSCALL_GET_WINDOW_BOUNDS, source->window->window, (uintptr_t) &bounds, 0, 0);

			x += bounds.left;
			y += bounds.top;
		}

		if ((x == -1 && y == -1) || x == -2) {
			OSPoint position;
			OSGetMousePosition(nullptr, &position);

			x = position.x;
			y = position.y;
		}

		window = CreateWindow(&specification, source ? source->window : nullptr, x, y);

		for (uintptr_t i = 0; i < openMenuCount; i++) {
			if (openMenus[i].source->window == source->window) {
				OSMessage message;
				message.type = OS_MESSAGE_DESTROY;
				OSSendMessage(openMenus[i].window, &message);
				break;
			}
		}

		openMenus[openMenuCount].source = source;
		openMenus[openMenuCount].window = window;
		openMenus[openMenuCount].fromMenubar = flags & OS_CREATE_MENU_FROM_MENUBAR;
		openMenuCount++;

		if (!(flags & OS_CREATE_MENU_BLANK)) {
			if (navigateMenuMode && navigateMenuUsedKey) {
				navigateMenuItem = firstMenuItem;
			} else {
				navigateMenuMode = true;
				navigateMenuItem = nullptr;
			}
		}

		navigateMenuFirstItem = firstMenuItem;

		if (grid) {
			OSSetRootGrid(window, grid);
		}

		returnValue = window;
	}

	for (uintptr_t i = 0; i < itemCount; i++) {
		OSAddControl(grid, menubar ? i : 0, !menubar ? i : 0, items[i], OS_CELL_H_EXPAND | OS_CELL_V_EXPAND);
	}

#if 0
	if (!menubar) {
		OSSetFocusedControl(items[0], true);
	}
#endif

	return returnValue;
}

void OSGetMousePosition(OSObject relativeWindow, OSPoint *position) {
	OSSyscall(OS_SYSCALL_GET_CURSOR_POSITION, (uintptr_t) position, 0, 0, 0);

	if (relativeWindow) {
		OSRectangle bounds;
		OSSyscall(OS_SYSCALL_GET_WINDOW_BOUNDS, ((Window *) relativeWindow)->window, (uintptr_t) &bounds, 0, 0);

		position->x -= bounds.left;
		position->y -= bounds.top;
	}
}

void OSSetProperty(OSObject object, uintptr_t index, void *value) {
	OSMessage message;
	message.type = OS_MESSAGE_SET_PROPERTY;
	message.setProperty.index = index;
	message.setProperty.value = value;
	OSSendMessage(object, &message);
}

void OSInitialiseGUI() {
	OSLinearBuffer buffer;
	OSGetLinearBuffer(OS_SURFACE_UI_SHEET, &buffer);

	uint32_t *skin = (uint32_t *) OSMapObject(buffer.handle, 0, buffer.width * buffer.height * 4, OS_FLAGS_DEFAULT);

	STANDARD_BACKGROUND_COLOR = skin[0 * 3 + 25 * buffer.width];
	
	LIST_VIEW_COLUMN_TEXT_COLOR = skin[1 * 3 + 25 * buffer.width];
	LIST_VIEW_PRIMARY_TEXT_COLOR = skin[2 * 3 + 25 * buffer.width];
	LIST_VIEW_SECONDARY_TEXT_COLOR = skin[3 * 3 + 25 * buffer.width];
	LIST_VIEW_BACKGROUND_COLOR = skin[4 * 3 + 25 * buffer.width];
	
	TEXT_COLOR_DEFAULT = skin[5 * 3 + 25 * buffer.width];
	// TEXT_COLOR_DISABLED = skin[6 * 3 + 25 * buffer.width];
	TEXT_COLOR_DISABLED_SHADOW = skin[7 * 3 + 25 * buffer.width];
	TEXT_COLOR_HEADING = skin[8 * 3 + 25 * buffer.width];
	TEXT_COLOR_TITLEBAR = skin[0 * 3 + 28 * buffer.width];
	TEXT_COLOR_TOOLBAR = skin[1 * 3 + 28 * buffer.width];
	
	TEXTBOX_SELECTED_COLOR_1 = skin[2 * 3 + 28 * buffer.width];
	TEXTBOX_SELECTED_COLOR_2 = skin[3 * 3 + 28 * buffer.width];

	DISABLE_TEXT_SHADOWS = skin[4 * 3 + 28 * buffer.width];

	OSFree(skin);
	OSCloseHandle(buffer.handle);
}

#include "list_view.cpp"
