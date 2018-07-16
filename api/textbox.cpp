// TODO:
// 	- Undo/redo.
// 	- Multi-line.
// 		- FindCaret
// 		- EnsureCaretVisible
// 		- Vertical scrolling
// 		- Word wrap
// 	- Limit the length to 1024 characters?

struct TextboxLine {
	uintptr_t offsetBytes, offsetCharacters;
	size_t lengthBytes, lengthCharacters;
};

struct Textbox : Control {
	OSCaret caret, caret2;
	OSCaret wordSelectionAnchor, wordSelectionAnchor2;
	uint8_t caretBlink : 1,
		sentEditResultNotification : 1;
	int scrollX;
	OSString previousString;
	OSTextboxStyle style;
	TextboxLine *lines;
	size_t lineCount;
};

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
				if (utf8_value(string->buffer + caret->byte) == '\n') caret->line--;
			} else {
				return; // We cannot move any further left.
			}
		} else {
			if (caret->character != string->characters) {
				caret->character++;
				if (utf8_value(string->buffer + caret->byte) == '\n') caret->line++;
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
	// TODO Assumes single line.

	if (positionX < control->textBounds.left) {
		positionX = control->textBounds.left;
	} else if (positionX > control->textBounds.right) {
		positionX = control->textBounds.right;
	}

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

static void TextboxRemoveSelectedText(Textbox *control) {
	// You can't delete nothing!
	if (control->caret.byte == control->caret2.byte) return;

	// Make sure that caret2 comes first.
	if (control->caret.byte < control->caret2.byte) {
		OSCaret temp = control->caret2;
		control->caret2 = control->caret;
		control->caret = temp;
	}

	// Calculate the deletion metrics.
	int bytes = control->caret.byte - control->caret2.byte;
	int characters = control->caret.character - control->caret2.character;
	int lines = control->caret.line - control->caret2.line;

	// Remove the text.
	OSCopyMemory(control->text.buffer + control->caret2.byte, control->text.buffer + control->caret.byte, control->text.bytes - control->caret.byte);
	control->text.characters -= characters;
	control->text.bytes -= bytes;

	// Remove the lines.
	if (control->caret.line == control->caret2.line) {
		control->lines[control->caret2.line].lengthBytes -= bytes;
		control->lines[control->caret2.line].lengthCharacters -= characters;
	} else {
		OSMoveMemory(control->lines + control->caret.line + 1, control->lines + control->lineCount, -(lines * sizeof(TextboxLine)), false);
		control->lineCount -= lines;

		TextboxLine *line = control->lines + control->caret2.line;
		char *c = control->text.buffer + line->offsetBytes;
		char *d = c;
		int characters = 0;

		while (c < control->text.buffer + control->text.bytes) {
			if (*c == '\n') {
				break;
			}

			characters++;
			c = utf8_advance(c);
		}

		line->lengthCharacters = characters;
		line->lengthBytes = c - d;
	}

	// Merge the carets.
	control->caret = control->caret2;

	// Update the line cache.
	for (uintptr_t i = control->caret.line + 1; i < control->lineCount; i++) {
		control->lines[i].offsetBytes -= bytes;
		control->lines[i].offsetCharacters -= characters;
	}
}

static void TextboxEnsureCaretVisible(Textbox *control) {
	if (control->style == OS_TEXTBOX_STYLE_MULTILINE) {
		// TODO
		return;
	}

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

template <typename F>
static void TextboxInsertText(Textbox *control, size_t bytes, F callback) {
	size_t characters = 0;
	int lines = 0;

	// Make space in the buffer.
	char *old = control->text.buffer;
	control->text.buffer = (char *) GUIAllocate(control->text.bytes + bytes, false);
	OSCopyMemory(control->text.buffer, old, control->caret.byte);
	OSCopyMemory(control->text.buffer + control->caret.byte + bytes, old + control->caret.byte, control->text.bytes - control->caret.byte);
	GUIFree(old);

	// Insert the text.
	char *c = control->text.buffer + control->caret.byte;
	char *d = c;
	callback(c);

	// Count the number of characters and lines.
	{
		while (c < d + bytes) { 
			if (utf8_value(c) == '\n') {
				lines++;
			}

			characters++; 
			c = utf8_advance(c); 
		} 
	}

	// Insert the new lines.
	if (lines) {
		TextboxLine *newLines = (TextboxLine *) GUIAllocate(sizeof(TextboxLine) * (control->lineCount + lines), false);
		OSCopyMemory(newLines, control->lines, control->lineCount * sizeof(TextboxLine));
		OSMoveMemory(newLines + control->caret.line + 1, newLines + control->lineCount, sizeof(TextboxLine) * lines, true);
		control->lineCount += lines;
		GUIFree(control->lines);
		control->lines = newLines;
	}

	// Process newlines.
	{
		c = d;
		int line = control->caret.line;
		size_t character = control->caret.character;

		while (c < d + bytes) { 
			TextboxLine *previous = control->lines + line;

			if (utf8_value(c) == '\n') {
				TextboxLine *next = control->lines + line + 1;

				size_t previousLengthBytes = previous->lengthBytes;
				previous->lengthBytes = c - (previous->offsetBytes + control->text.buffer);
				next->lengthBytes = previousLengthBytes - previous->lengthBytes;
				next->offsetBytes = previous->offsetBytes + previous->lengthBytes + 1;

				size_t previousLengthCharacters = previous->lengthCharacters;
				previous->lengthCharacters = character - previous->offsetCharacters;
				next->lengthCharacters = previousLengthCharacters - previous->lengthCharacters;
				next->offsetCharacters = previous->offsetCharacters + previous->lengthCharacters + 1;

				line++;
			} else {
				previous->lengthBytes += utf8_length_char(c);
				previous->lengthCharacters += 1;
			}

			c = utf8_advance(c); 
			character++;
		} 
	}

	// Update the buffer and carets.
	control->text.characters += characters;
	control->caret.character += characters;
	control->caret.byte += bytes;
	control->caret.line += lines;
	control->text.bytes += bytes;
	control->caret2 = control->caret;

	// Update the line cache.
	for (uintptr_t i = control->caret.line + 1; i < control->lineCount; i++) {
		control->lines[i].offsetBytes += bytes;
		control->lines[i].offsetCharacters += characters;
	}
}

static void TextboxInsertCharacter(Textbox *control, int character) {
	char data[4];
	int bytes = utf8_encode(character, data);

	TextboxInsertText(control, bytes, [bytes, data] (char *c) {
		OSCopyMemory(c, data, bytes);
	});
}

static OSCallbackResponse ProcessTextboxNotification(OSNotification *notification) {
	Textbox *control = (Textbox *) notification->context;

	if (notification->type == OS_NOTIFICATION_COMMAND) {
		if (notification->command.command == osCommandSelectAll) {
			control->caret.byte = 0;
			control->caret.character = 0;
			control->caret.line = 0;

			control->caret2.byte = control->text.bytes;
			control->caret2.character = control->text.characters;
			control->caret2.line = control->lineCount;
		}

		if (notification->command.command == osCommandCopy || notification->command.command == osCommandCut) {
			OSString string;
			int length = control->caret.byte - control->caret2.byte;
			if (length < 0) length = -length;
			string.bytes = length;
			if (control->caret.byte > control->caret2.byte) string.buffer = control->text.buffer + control->caret2.byte;
			else string.buffer = control->text.buffer + control->caret.byte;
			CopyText(string);
		}

		if (notification->command.command == osCommandDelete || notification->command.command == osCommandCut || notification->command.command == osCommandPaste) {
			TextboxRemoveSelectedText(control);
		}

		if (notification->command.command == osCommandPaste) {
			// TODO Remove newlines on single line textboxes.

			TextboxInsertText(control, ClipboardTextBytes(), [] (char *c) {
				OSSyscall(OS_SYSCALL_PASTE_TEXT, ClipboardTextBytes(), (uintptr_t) c, 0, 0);
			});
		}

		RepaintControl(control);
		TextboxEnsureCaretVisible(control);
		return OS_CALLBACK_HANDLED;
	}

	return OS_CALLBACK_NOT_HANDLED;
}

OSCallbackResponse ProcessTextboxMessage(OSObject object, OSMessage *message) {
	Textbox *control = (Textbox *) message->context;

	OSCallbackResponse result = OS_CALLBACK_NOT_HANDLED;
	static int lastClickChainCount = 1;
	bool ensureCaretVisible = false;

	if (message->type == OS_MESSAGE_CUSTOM_PAINT) {
#define TEXTBOX_MARGIN (4)

		int lineHeight = GetLineHeight(fontRegular, control->textSize);

		for (uintptr_t i = 0; i < control->lineCount; i++) {
			TextboxLine *line = control->lines + i;

			OSRectangle lineBounds = OS_MAKE_RECTANGLE(control->textBounds.left - TEXTBOX_MARGIN, 
						control->textBounds.right + TEXTBOX_MARGIN, control->textBounds.top, control->textBounds.bottom);

			if (control->style == OS_TEXTBOX_STYLE_MULTILINE) {
				lineBounds.top += i * lineHeight + TEXTBOX_MARGIN;
				lineBounds.bottom = lineBounds.top + lineHeight;
			}

			OSString lineString = { line->offsetBytes + control->text.buffer, line->lengthBytes, line->lengthCharacters };
			bool showSelection = (control->window->lastFocus == control && (control->style != OS_TEXTBOX_STYLE_COMMAND || control->window->focus == control)) && !control->disabled;
			bool hideCaret = control->window->focus != control || control->caretBlink || control->disabled;
			intptr_t caretIndex = control->caret.character - line->offsetCharacters;
			intptr_t caretIndex2 = control->caret2.character - line->offsetCharacters;
			if (!showSelection) caretIndex2 = caretIndex;
			uint32_t selectionColor = control->window->focus == control ? TEXTBOX_SELECTED_COLOR_1 : TEXTBOX_SELECTED_COLOR_2;
			if (control->caret.line < i && control->caret2.line >= i) caretIndex = 0;
			if (control->caret2.line < i && control->caret.line >= i) caretIndex2 = 0;

			DrawString(message->paint.surface, 		// Surface
				   lineBounds, 				// Bounds
				   &lineString, 			// String
				   control->textAlign, 			// Alignment
				   control->textColor, 			// Text color
				   -1, 					// Background color
				   selectionColor,			// Selection color
				   {0, 0}, nullptr, 			// Unused
				   caretIndex,				// Caret index/selection start
				   caretIndex2, 			// Selection end
				   hideCaret,				// Hide caret
				   control->textSize, 			// Text size
				   fontRegular, 			// Font
				   message->paint.clip, 		// Clip region
				   0, 					// Blur
				   control->scrollX - TEXTBOX_MARGIN);	// Horizontal scroll
		}

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

		OSNotification n;

		if (!control->sentEditResultNotification) {
			n.type = OS_NOTIFICATION_CANCEL_EDIT;
			OSSendNotification(control, control->notificationCallback, &n, control->window->instance);

			if (control->style == OS_TEXTBOX_STYLE_COMMAND) {
				CreateString(control->previousString.buffer, control->previousString.bytes, &control->text, control->previousString.characters);
			}
		}

		n.type = OS_NOTIFICATION_END_EDIT;
		OSSendNotification(control, control->notificationCallback, &n, control->window->instance);
		control->cursor = control->style == OS_TEXTBOX_STYLE_COMMAND ? OS_CURSOR_NORMAL : OS_CURSOR_TEXT;
		control->window->cursor = (OSCursorStyle) control->cursor;

		GUIFree(control->previousString.buffer);
		control->previousString.buffer = nullptr;
	} else if (message->type == OS_MESSAGE_START_FOCUS) {
		ensureCaretVisible = true;

		control->caretBlink = false;
		control->window->caretBlinkPause = CARET_BLINK_PAUSE;

		control->sentEditResultNotification = false;

		if (control->style == OS_TEXTBOX_STYLE_COMMAND) {
			control->caret.byte = 0;
			control->caret.character = 0;
			control->caret2.byte = control->text.bytes;
			control->caret2.character = control->text.characters;

			OSSyscall(OS_SYSCALL_RESET_CLICK_CHAIN, 0, 0, 0, 0);
		}

		OSEnableCommand(control->window->instance, osCommandPaste, ClipboardTextBytes());
		OSEnableCommand(control->window->instance, osCommandSelectAll, true);

		OSSetCommandNotificationCallback(control->window->instance, osCommandPaste, OS_MAKE_NOTIFICATION_CALLBACK(ProcessTextboxNotification, control));
		OSSetCommandNotificationCallback(control->window->instance, osCommandSelectAll, OS_MAKE_NOTIFICATION_CALLBACK(ProcessTextboxNotification, control));
		OSSetCommandNotificationCallback(control->window->instance, osCommandCopy, OS_MAKE_NOTIFICATION_CALLBACK(ProcessTextboxNotification, control));
		OSSetCommandNotificationCallback(control->window->instance, osCommandCut, OS_MAKE_NOTIFICATION_CALLBACK(ProcessTextboxNotification, control));
		OSSetCommandNotificationCallback(control->window->instance, osCommandDelete, OS_MAKE_NOTIFICATION_CALLBACK(ProcessTextboxNotification, control));

		{
			OSNotification n;
			n.type = OS_NOTIFICATION_START_EDIT;
			OSSendNotification(control, control->notificationCallback, &n, OSGetInstance(control));
			control->cursor = OS_CURSOR_TEXT;
			control->window->cursor = (OSCursorStyle) control->cursor;

			if (control->style == OS_TEXTBOX_STYLE_COMMAND) {
				CreateString(control->text.buffer, control->text.bytes, &control->previousString, control->text.characters);
			}
		}
	} else if (message->type == OS_MESSAGE_CLIPBOARD_UPDATED) {
		OSEnableCommand(control->window->instance, osCommandPaste, ClipboardTextBytes());
	} else if (message->type == OS_MESSAGE_END_LAST_FOCUS) {
		ensureCaretVisible = true;

		OSDisableCommand(control->window->instance, osCommandPaste, true);
		OSDisableCommand(control->window->instance, osCommandSelectAll, true);
		OSDisableCommand(control->window->instance, osCommandCopy, true);
		OSDisableCommand(control->window->instance, osCommandCut, true);
		OSDisableCommand(control->window->instance, osCommandDelete, true);
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
		FindCaret(control, message->mouseDragged.newPositionX, message->mouseDragged.newPositionY, true, lastClickChainCount);
		RepaintControl(control);
	} else if (message->type == OS_MESSAGE_CLICK_REPEAT) {
		OSPoint mousePosition;
		OSGetMousePosition(control->window, &mousePosition);

		if (mousePosition.x < control->textBounds.left) {
			int difference = control->textBounds.left - mousePosition.x;
			control->scrollX -= difference / 10;
			FindCaret(control, control->textBounds.left, mousePosition.y, true, lastClickChainCount);
		} else if (mousePosition.x > control->textBounds.right) {
			int difference = mousePosition.x - control->textBounds.right;
			control->scrollX += difference / 10;
			FindCaret(control, control->textBounds.right, mousePosition.y, true, lastClickChainCount);
		}

		int fullWidth = MeasureStringWidth(control->text.buffer, control->text.bytes, control->textSize, fontRegular);
		int controlWidth = control->textBounds.right - control->textBounds.left;

		if (fullWidth < controlWidth) {
			control->scrollX = 0;
		} else if (control->scrollX > fullWidth - controlWidth) {
			control->scrollX = fullWidth - controlWidth;
		} else if (control->scrollX < 0) {
			control->scrollX = 0;
		}

		RepaintControl(control);
	} else if (message->type == OS_MESSAGE_TEXT_UPDATED) {
		if (control->caret2.byte > control->text.bytes) {
			control->caret2.byte = control->text.bytes;
		}

		control->caret = control->caret2;
		ensureCaretVisible = true;

		if (control->style == OS_TEXTBOX_STYLE_MULTILINE) {
			GUIFree(control->lines);
			control->lineCount = 1;

			for (uintptr_t i = 0; i < control->text.bytes; i = i + utf8_length_char(control->text.buffer + i)) {
				if (control->text.buffer[i] == '\n') {
					control->lineCount++;
				}
			}

			control->lines = (TextboxLine *) GUIAllocate(sizeof(TextboxLine) * control->lineCount, true);

			uintptr_t i = 0, characters = 0;

			for (uintptr_t line = 0; line < control->lineCount; line++) {
				control->lines[line].offsetBytes = i;
				control->lines[line].offsetCharacters = characters;

				while (i < control->text.bytes && control->text.buffer[i] != '\n') {
					i = i + utf8_length_char(control->text.buffer + i);
					characters++;
				}

				control->lines[line].lengthBytes = i - control->lines[line].offsetBytes;
				control->lines[line].lengthCharacters = characters - control->lines[line].offsetCharacters;

				if (i < control->text.bytes) {
					// Skip newline character.
					i++;
					characters++;
				}
			}
		}
	} else if (message->type == OS_MESSAGE_DESTROY) {
		GUIFree(control->previousString.buffer);
		GUIFree(control->lines);
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
				if (message->keyboard.ctrl) {
					control->caret2.byte = 0;
					control->caret2.character = 0;
					control->caret2.line = 0;
				} else {
					TextboxLine *current = control->lines + control->caret2.line;
					control->caret2.byte = current->offsetBytes;
					control->caret2.character = current->offsetCharacters;
				}

				if (!message->keyboard.shift) {
					control->caret = control->caret2;
				}

				withAltOrCtrl = true;
			} break;

			case OS_SCANCODE_END: {
				if (message->keyboard.ctrl) {
					control->caret2.byte = control->text.bytes;
					control->caret2.character = control->text.characters;
					control->caret2.line = control->lineCount - 1;
				} else {
					TextboxLine *current = control->lines + control->caret2.line;
					control->caret2.byte = current->offsetBytes + current->lengthBytes;
					control->caret2.character = current->offsetCharacters + current->lengthCharacters;
				}

				if (!message->keyboard.shift) {
					control->caret = control->caret2;
				}

				withAltOrCtrl = true;
			} break;

			case OS_SCANCODE_BACKSPACE: {
				if (control->caret.byte == control->caret2.byte && control->caret.byte) {
					MoveCaret(&control->text, &control->caret2, false, message->keyboard.ctrl);
				}

				TextboxRemoveSelectedText(control);
				withAltOrCtrl = true;
			} break;

			case OS_SCANCODE_DELETE: {
				if (control->caret.byte == control->caret2.byte && control->caret.byte != control->text.bytes) {
					MoveCaret(&control->text, &control->caret, true, message->keyboard.ctrl);
				}

				TextboxRemoveSelectedText(control);
				withAltOrCtrl = true;
			} break;

			case OS_SCANCODE_ENTER: {
				if (control->style == OS_TEXTBOX_STYLE_COMMAND) {
					OSNotification n;
					n.type = OS_NOTIFICATION_CONFIRM_EDIT;
					OSSendNotification(control, control->notificationCallback, &n, OSGetInstance(control));
					control->sentEditResultNotification = true;

					n.type = OS_NOTIFICATION_COMMAND;
					n.command.command = control->command;

					OSCallbackResponse response = OSSendNotification(control, control->notificationCallback, &n, OSGetInstance(control));

					if (response == OS_CALLBACK_REJECTED) {
						CreateString(control->previousString.buffer, control->previousString.bytes, &control->text, control->previousString.characters);

						OSMessage m;
						m.type = OS_MESSAGE_TEXT_UPDATED;
						OSSendMessage(control, &m);
					}

					OSRemoveFocusedControl(control->window, true);

					withAltOrCtrl = true;
				} else if (control->style == OS_TEXTBOX_STYLE_MULTILINE) {
					ic = '\n';
				} else {
					result = OS_CALLBACK_NOT_HANDLED;
				}
			} break;

			default: {
				result = OS_CALLBACK_NOT_HANDLED;
			} break;
		}

		if ((message->keyboard.ctrl || message->keyboard.alt) && !withAltOrCtrl) {
			return OS_CALLBACK_NOT_HANDLED;
		}

		if (ic != -1 && !message->keyboard.alt && !message->keyboard.ctrl) {
			TextboxRemoveSelectedText(control);
			TextboxInsertCharacter(control, message->keyboard.shift ? isc : ic);
		}

		RepaintControl(control);
	}

	if (control->window && control->window->lastFocus == control) {
		bool noSelection = control->caret.byte == control->caret2.byte;
		OSDisableCommand(control->window->instance, osCommandCopy, noSelection);
		OSDisableCommand(control->window->instance, osCommandCut, noSelection);
		OSDisableCommand(control->window->instance, osCommandDelete, noSelection);
	}

	if (ensureCaretVisible) {
		TextboxEnsureCaretVisible(control);
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
	control->textAlign = (style == OS_TEXTBOX_STYLE_MULTILINE ? OS_DRAW_STRING_VALIGN_TOP : OS_DRAW_STRING_VALIGN_CENTER) | OS_DRAW_STRING_HALIGN_LEFT;
	control->textSize = style == OS_TEXTBOX_STYLE_LARGE ? FONT_SIZE * 2 : FONT_SIZE;
	control->textColor = TEXT_COLOR_DEFAULT;
	control->preferredHeight = GetLineHeight(fontRegular, control->textSize) + 5;
	control->style = style;
	control->rightClickMenu = osMenuTextboxContext;
	control->lineCount = 1;
	control->lines = (TextboxLine *) GUIAllocate(sizeof(TextboxLine), true);

	OSSetMessageCallback(control, OS_MAKE_MESSAGE_CALLBACK(ProcessTextboxMessage, control));

	return control;
}

