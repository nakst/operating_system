// TODO Heap panic with textboxes, to investigate.

// TODO Textboxes
//	- Clipboard and undo
//	- Scrolling (including drag selections)
//	- Actually use the OS_CALLBACK_GET_TEXT callback for everything.

#define BORDER_OFFSET_X (5)
#define BORDER_OFFSET_Y (29)

#define BORDER_SIZE_X (8)
#define BORDER_SIZE_Y (34)

static void SendCallback(OSControl *from, OSCallback &callback, OSCallbackData &data) {
	if (callback.callback) {
		callback.callback(from, callback.argument, &data);
	} else {
		switch (data.type) {
			case OS_CALLBACK_ACTION: {
				// We can't really do anything if the program doesn't want to handle the action.
			} break;

			case OS_CALLBACK_GET_TEXT: {
				// The program wants us to store the text.
				// Therefore, use the text we have stored in the control.
				*data.getText.string = from->text;
				data.getText.freeText = false;
			} break;

			case OS_CALLBACK_INSERT_TEXT: {
				OSString *string = &from->text;
				OSString *insert = data.insertText.string;
				OSCaret *caret = data.insertText.caret;

				if (string->allocated <= string->bytes + insert->bytes) {
					string->buffer = (char *) realloc(string->buffer, (string->allocated = string->bytes + insert->bytes + 16));
				}

				char *text = string->buffer;
				text += caret->byte;

				memmove(text + insert->bytes, text, string->bytes - (text - string->buffer));
				OSCopyMemory(text, insert->buffer, insert->bytes);

				string->bytes += insert->bytes;
				string->characters += insert->characters;
			} break;

			case OS_CALLBACK_REMOVE_TEXT: {
				char *text = from->text.buffer + data.removeText.caretStart->byte;
				char *text2 = from->text.buffer + data.removeText.caretEnd->byte;

				size_t bytes = text2 - text;
				size_t characters = data.removeText.caretEnd->character - data.removeText.caretStart->character;

				memmove(text, text2, from->text.bytes - (text2 - from->text.buffer));

				from->text.bytes -= bytes;
				from->text.characters -= characters;
			} break;

			default: {
				Panic();
			} break;
		}
	}
}

static void DrawControl(OSWindow *window, OSControl *control) {
	if (!window) return;

	int imageWidth = control->image.right - control->image.left;
	int imageHeight = control->image.bottom - control->image.top;

	bool isHoverControl = (control == window->hoverControl) || (control == window->focusedControl);
	bool isPressedControl = (control == window->pressedControl) || (control == window->focusedControl);

	if (control->manualImage) {
		isHoverControl = true;
		isPressedControl = false;
	}

	intptr_t styleX = (control->disabled ? 3
			: ((isPressedControl && isHoverControl) ? 2 
			: ((isPressedControl || isHoverControl) ? 0 : 1)));
	styleX = (imageWidth + 1) * styleX + control->image.left;

	OSCallbackData textEvent = {};
	OSString controlText;
	textEvent.type = OS_CALLBACK_GET_TEXT;
	textEvent.getText.string = &controlText;
	SendCallback(control, control->getText, textEvent);

	if (control->imageType == OS_CONTROL_IMAGE_FILL) {
		if (control->imageBorder.left) {
			OSDrawSurface(window->surface, OS_SURFACE_UI_SHEET, 
					control->bounds,
					control->image,
					control->imageBorder,
					OS_DRAW_MODE_REPEAT_FIRST);
		} else {
			OSDrawSurface(window->surface, OS_SURFACE_UI_SHEET, 
					control->bounds,
					OSRectangle(styleX, styleX + imageWidth, control->image.top, control->image.bottom),
					OSRectangle(styleX + 3, styleX + 5, control->image.top + 10, control->image.top + 11),
					OS_DRAW_MODE_REPEAT_FIRST);
		}

		if (control->textShadow) {
#define SHADOW_OFFSET (1)
			DrawString(window->surface, OSRectangle(control->textBounds.left + SHADOW_OFFSET, control->textBounds.right + SHADOW_OFFSET,
								control->textBounds.top + SHADOW_OFFSET, control->textBounds.bottom + SHADOW_OFFSET), 
					&controlText,
					control->textAlign,
					control->disabled ? 0x808080 : 0x000000, -1, 0xFFAFC6EA,
					OSPoint(0, 0), nullptr, -1, 0, false, control->fontSize);
		}
		
		DrawString(window->surface, control->textBounds, 
				&controlText,
				control->textAlign,
				control->disabled ? 0x808080 : (control->textShadow ? 0xFFFFFF : 0x000000), -1, 0xFFAFC6EA,
				OSPoint(0, 0), nullptr, control == window->focusedControl && !control->disabled ? control->caret.character : -1, 
				control->caret2.character, control->caretBlink == 2, control->fontSize);
	} else if (control->imageType == OS_CONTROL_IMAGE_CENTER_LEFT) {
		OSFillRectangle(window->surface, control->bounds, OSColor(0xF0, 0xF0, 0xF5));
		OSDrawSurface(window->surface, OS_SURFACE_UI_SHEET, 
				OSRectangle(control->bounds.left, control->bounds.left + control->fillWidth, 
					control->bounds.top, control->bounds.top + imageHeight),
				OSRectangle(styleX, styleX + imageWidth, control->image.top, control->image.bottom),
				OSRectangle(styleX + 3, styleX + 5, control->image.top + 10, control->image.top + 11),
				OS_DRAW_MODE_REPEAT_FIRST);
		OSDrawString(window->surface, 
				control->textBounds, 
				&controlText,
				control->textAlign,
				control->disabled ? 0x808080 : 0x000000, 0xF0F0F5);
	} else if (control->imageType == OS_CONTROL_IMAGE_NONE) {
		OSFillRectangle(window->surface, control->bounds, OSColor(0xF0, 0xF0, 0xF5));
		OSDrawString(window->surface, control->textBounds, 
				&controlText,
				control->textAlign,
				control->disabled ? 0x808080 : 0x000000, 0xF0F0F5);
	}

	if (control->checked) {
		int checkY = 92 + 13 * control->checked - 13; 
		OSDrawSurface(window->surface, OS_SURFACE_UI_SHEET, 
				OSRectangle(control->bounds.left, control->bounds.left + control->fillWidth, 
					control->bounds.top, control->bounds.top + imageHeight),
				OSRectangle(96, 96 + 13, checkY, checkY + 13),
				OSRectangle(96 + 1, 96 + 2, checkY + 1, checkY + 2),
				OS_DRAW_MODE_REPEAT_FIRST);
	}

	if (textEvent.getText.freeText) {
		OSHeapFree(controlText.buffer);
	}

	window->dirty = true;
}

OSError OSSetControlText(OSControl *control, char *text, size_t textLength) {
	if (control->text.buffer) {
		OSHeapFree(control->text.buffer);
	}

	if (control->getText.callback) {
		Panic();
	}

	char *temp = (char *) OSHeapAllocate(textLength, false);
	if (!temp) return OS_ERROR_COULD_NOT_ALLOCATE_MEMORY;
	OSCopyMemory(temp, text, textLength);
	text = temp;

	OSString *string = &control->text;
	string->buffer = text;
	string->bytes = textLength;
	string->allocated = textLength;
	string->characters = utf8_length(text, textLength);

	return OSInvalidateControl(control);
}

OSError OSInvalidateControl(OSControl *control) {
	DrawControl(control->parent, control);
	return OS_SUCCESS;
}

static bool ControlHitTest(OSControl *control, int x, int y) {
	if (control->parent->pressedControl == control) {
#define EXTRA_PRESSED_BORDER (5)
		if (x >= control->bounds.left - EXTRA_PRESSED_BORDER && x < control->bounds.right + EXTRA_PRESSED_BORDER 
				&& y >= control->bounds.top - EXTRA_PRESSED_BORDER && y < control->bounds.bottom + EXTRA_PRESSED_BORDER) {
			return true;
		} else {
			return false;
		}
	} else {
		if (x >= control->bounds.left && x < control->bounds.right
				&& y >= control->bounds.top && y < control->bounds.bottom) {
			return true;
		} else {
			return false;
		}
	}
}

void OSDisableControl(OSControl *control, bool disabled) {
	control->disabled = disabled;

	if (disabled) {
		if (control == control->parent->hoverControl)   control->parent->hoverControl = nullptr;
		if (control == control->parent->pressedControl) control->parent->pressedControl = nullptr;
	}

	OSInvalidateControl(control);
}

void OSCheckControl(OSControl *control, int checked) {
	control->checked = checked;

	OSInvalidateControl(control);
}

OSError OSAddControl(OSWindow *window, OSControl *control, int x, int y) {
	control->bounds.left   += x + BORDER_OFFSET_X;
	control->bounds.top    += y + BORDER_OFFSET_Y;
	control->bounds.right  += x + BORDER_OFFSET_X;
	control->bounds.bottom += y + BORDER_OFFSET_Y;

	control->textBounds.left   += x + BORDER_OFFSET_X;
	control->textBounds.top    += y + BORDER_OFFSET_Y;
	control->textBounds.right  += x + BORDER_OFFSET_X;
	control->textBounds.bottom += y + BORDER_OFFSET_Y;

	control->parent = window;
	window->dirty = true;

	window->controls[window->controlsCount++] = control;
	DrawControl(window, control);

	return OS_SUCCESS;
}

OSControl *OSCreateControl(OSControlType type, char *text, size_t textLength) {
	OSControl *control = (OSControl *) OSHeapAllocate(sizeof(OSControl), true);
	if (!control) return nullptr;

	control->type = type;
	control->textAlign = OS_DRAW_STRING_HALIGN_CENTER | OS_DRAW_STRING_VALIGN_CENTER;
	control->cursorStyle = OS_CURSOR_NORMAL;
	control->fontSize = FONT_SIZE;

	switch (type) {
		case OS_CONTROL_BUTTON: {
			control->bounds.right = 80;
			control->bounds.bottom = 21;
			control->textBounds = control->bounds;
			control->imageType = OS_CONTROL_IMAGE_FILL;
			control->image = OSRectangle(42, 42 + 8, 88, 88 + 21);
		} break;

		case OS_CONTROL_GROUP: {
			control->bounds.right = 100;
			control->bounds.bottom = 100;
			control->textBounds = control->bounds;
			control->imageType = OS_CONTROL_IMAGE_FILL;
			control->image = OSRectangle(20, 20 + 6, 85, 85 + 6);
			control->imageBorder = OSRectangle(22, 23, 87, 88);
		} break;

		case OS_CONTROL_CHECKBOX: {
			control->bounds.right = 21 + MeasureStringWidth(text, textLength, GetGUIFontScale(control->fontSize));
			control->bounds.bottom = 13;
			control->textBounds = control->bounds;
			control->textBounds.left = 13 + 4;
			control->imageType = OS_CONTROL_IMAGE_CENTER_LEFT;
			control->fillWidth = 13;
			control->image = OSRectangle(42, 42 + 8, 110, 110 + 13);
			control->textAlign = OS_DRAW_STRING_HALIGN_LEFT | OS_DRAW_STRING_VALIGN_CENTER;
		} break;

		case OS_CONTROL_RADIOBOX: {
			control->bounds.right = 21 + MeasureStringWidth(text, textLength, GetGUIFontScale(control->fontSize));
			control->bounds.bottom = 14;
			control->textBounds = control->bounds;
			control->textBounds.left = 13 + 4;
			control->imageType = OS_CONTROL_IMAGE_CENTER_LEFT;
			control->fillWidth = 14;
			control->image = OSRectangle(116, 116 + 14, 42, 42 + 14);
			control->textAlign = OS_DRAW_STRING_HALIGN_LEFT | OS_DRAW_STRING_VALIGN_CENTER;
		} break;

		case OS_CONTROL_STATIC: {
			control->bounds.right = 4 + MeasureStringWidth(text, textLength, GetGUIFontScale(control->fontSize));
			control->bounds.bottom = 14;
			control->textBounds = control->bounds;
			control->imageType = OS_CONTROL_IMAGE_NONE;
		} break;

		case OS_CONTROL_TEXTBOX: {
			control->bounds.right = 160;
			control->bounds.bottom = 21;
			control->textBounds = control->bounds;
			control->textBounds.left += 4;
			control->textBounds.right -= 4;
			control->imageType = OS_CONTROL_IMAGE_FILL;
			control->image = OSRectangle(1, 1 + 7, 122, 122 + 21);
			control->canHaveFocus = true;
			control->textAlign = OS_DRAW_STRING_HALIGN_LEFT | OS_DRAW_STRING_VALIGN_CENTER;
			control->cursorStyle = OS_CURSOR_TEXT;
		} break;

		case OS_CONTROL_TITLEBAR: {
			control->bounds.bottom = 28;
			control->textBounds = control->bounds;
			control->imageType = OS_CONTROL_IMAGE_FILL;
			control->image = OSRectangle(97, 97 + 7, 43, 43 + 28);
			control->manualImage = true;
			control->textShadow = true;
			control->fontSize = 20;
		} break;
	}

	OSSetControlText(control, text, textLength);

	return control;
}

OSWindow *OSCreateWindow(char *title, size_t titleLengthBytes, size_t width, size_t height, bool decorate) {
	OSWindow *window = (OSWindow *) OSHeapAllocate(sizeof(OSWindow), true);

	// Add the size of the border.
	width += BORDER_SIZE_X;
	height += BORDER_SIZE_Y;

	OSError result = OSSyscall(OS_SYSCALL_CREATE_WINDOW, (uintptr_t) window, width, height, 0);

	if (result != OS_SUCCESS) {
		OSHeapFree(window);
		return nullptr;
	}

	if (decorate) {
		// Draw the window background and border.
		OSDrawSurface(window->surface, OS_SURFACE_UI_SHEET, OSRectangle(0, width, 0, height), 
				OSRectangle(96, 105, 42, 77), OSRectangle(96 + 3, 96 + 5, 42 + 29, 42 + 31), OS_DRAW_MODE_REPEAT_FIRST);

		OSControl *titlebar = OSCreateControl(OS_CONTROL_TITLEBAR, title, titleLengthBytes);
		titlebar->bounds.right = width - 2;
		titlebar->textBounds = titlebar->bounds;
		OSAddControl(window, titlebar, 1 - BORDER_OFFSET_X, 1 - BORDER_OFFSET_Y);
	}

	OSUpdateWindow(window);

	return window;
}

void RemoveSelectedText(OSControl *control) {
	OSCallbackData callback = {};
	callback.type = OS_CALLBACK_REMOVE_TEXT;

	if (control->caret.byte < control->caret2.byte) {
		callback.removeText.caretStart = &control->caret;
		callback.removeText.caretEnd = &control->caret2;
	} else {
		callback.removeText.caretStart = &control->caret2;
		callback.removeText.caretEnd = &control->caret;
	}

	SendCallback(control, control->removeText, callback);

	if (control->caret.byte < control->caret2.byte) {
		control->caret2 = control->caret;
	} else {
		control->caret = control->caret2;
	}
}

enum CharacterType {
	CHARACTER_INVALID,
	CHARACTER_IDENTIFIER, // A-Z, a-z, 0-9, _, >= 0x7F
	CHARACTER_WHITESPACE, // space, tab, newline
	CHARACTER_OTHER,
};

CharacterType GetCharacterType(int character) {
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

	return CHARACTER_OTHER;
}

void MoveCaret(OSString *string, OSCaret *caret, bool right, bool word, bool strongWhitespace = false) {
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

static void FindCaret(OSControl *control, int positionX, int positionY, bool secondCaret, unsigned clickChainCount) {
	if (!clickChainCount) {
		Panic();
	}

	if (control->type == OS_CONTROL_TEXTBOX && !control->disabled) {
		OSCallbackData textEvent = {};
		OSString string;
		textEvent.getText.string = &string;
		textEvent.type = OS_CALLBACK_GET_TEXT;
		SendCallback(control, control->getText, textEvent);

		if (clickChainCount >= 3) {
			control->caret.byte = 0;
			control->caret.character = 0;
			control->caret2.byte = string.bytes;
			control->caret2.character = string.characters;
		} else {
			OSFindCharacterAtCoordinate(control->textBounds, OSPoint(positionX, positionY), &string, control->textAlign, &control->caret2);

			if (!secondCaret) {
				control->caret = control->caret2;

				if (clickChainCount == 2) {
					MoveCaret(&string, &control->caret, false, true, true);
					MoveCaret(&string, &control->caret2, true, true, true);
					control->wordSelectionAnchor  = control->caret;
					control->wordSelectionAnchor2 = control->caret2;
				}
			} else {
				if (clickChainCount == 2) {
					if (control->caret2.byte < control->caret.byte) {
						MoveCaret(&string, &control->caret2, false, true);
						control->caret = control->wordSelectionAnchor2;
					} else {
						MoveCaret(&string, &control->caret2, true, true);
						control->caret = control->wordSelectionAnchor;
					}
				}
			}
		}

		control->caretBlink = 0;

		if (textEvent.getText.freeText) {
			OSHeapFree(string.buffer);
		}
	}
}

static unsigned lastClickChainCount;

static void UpdateMousePosition(OSWindow *window, int x, int y) {
	OSControl *previousHoverControl = window->hoverControl;
	window->previousHoverControl = previousHoverControl;

	if (previousHoverControl) {
		if (!ControlHitTest(previousHoverControl, x, y)) {
			window->hoverControl = nullptr;
			DrawControl(window, previousHoverControl);
		}
	}

	for (uintptr_t i = 0; !window->hoverControl && i < window->controlsCount; i++) {
		OSControl *control = window->controls[i];

		if (ControlHitTest(control, x, y)) {
			window->hoverControl = control;
			DrawControl(window, window->hoverControl);

			if (!window->pressedControl) {
				OSSetCursorStyle(window->handle, control->cursorStyle);
			}
		}
	}

	if (!window->hoverControl && !window->pressedControl) {
		OSSetCursorStyle(window->handle, OS_CURSOR_NORMAL);
	}

	if (window->pressedControl) {
		if (!lastClickChainCount) return;
		FindCaret(window->pressedControl, x, y, true, lastClickChainCount);
		DrawControl(window, window->pressedControl);
	}
}

static void ProcessTextboxInput(OSMessage *message, OSControl *control) {
	int ic = -1,
	    isc = -1;

	if (control && !control->disabled && control->type == OS_CONTROL_TEXTBOX) {
		control->caretBlink = 0;

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
			case OS_SCANCODE_SPACE:		ic = ' ';  isc = ' '; break;

			case OS_SCANCODE_BACKSPACE: {
				if (control->caret.byte == control->caret2.byte && control->caret.byte) {
					MoveCaret(&control->text, &control->caret2, false, message->keyboard.ctrl);

					OSCallbackData callback = {};
					callback.type = OS_CALLBACK_REMOVE_TEXT;
					callback.removeText.caretStart = &control->caret2;
					callback.removeText.caretEnd = &control->caret;

					SendCallback(control, control->removeText, callback);

					control->caret = control->caret2;
				} else {
					RemoveSelectedText(control);
				}
			} break;

			case OS_SCANCODE_DELETE: {
				if (control->caret.byte == control->caret2.byte && control->caret.byte != control->text.bytes) {
					MoveCaret(&control->text, &control->caret2, true, message->keyboard.ctrl);

					OSCallbackData callback = {};
					callback.type = OS_CALLBACK_REMOVE_TEXT;
					callback.removeText.caretStart = &control->caret;
					callback.removeText.caretEnd = &control->caret2;

					SendCallback(control, control->removeText, callback);

					control->caret2 = control->caret;
				} else {
					RemoveSelectedText(control);
				}
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
					}

					control->caret = control->caret2;
				}
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
					}

					control->caret = control->caret2;
				}
			} break;

			case OS_SCANCODE_HOME: {
				control->caret2.byte = 0;
				control->caret2.character = 0;
				if (!message->keyboard.shift) control->caret = control->caret2;
			} break;

			case OS_SCANCODE_END: {
				control->caret2.byte = control->text.bytes;
				control->caret2.character = control->text.characters;
				if (!message->keyboard.shift) control->caret = control->caret2;
			} break;
		}

		if (message->keyboard.scancode == OS_SCANCODE_A && message->keyboard.ctrl && !message->keyboard.alt) {
			control->caret.byte = 0;
			control->caret.character = 0;

			control->caret2.byte = control->text.bytes;
			control->caret2.character = control->text.characters;
		}

		if (ic != -1 && !message->keyboard.alt && !message->keyboard.ctrl) {
			RemoveSelectedText(control);

			{
				char data[4];
				int bytes = utf8_encode(message->keyboard.shift ? isc : ic, data);

				OSString insert = {};
				insert.buffer = data;
				insert.bytes = bytes;
				insert.characters = 1;

				// Insert the pressed character.
				OSCallbackData callback = {};
				callback.type = OS_CALLBACK_INSERT_TEXT;
				callback.insertText.string = &insert;
				callback.insertText.caret = &control->caret;
				SendCallback(control, control->insertText, callback);
			}

			{
				// Update the caret and redraw the control.
				control->caret.character++;
				control->caret.byte = utf8_advance(control->text.buffer + control->caret.byte) - control->text.buffer;
				control->caret2 = control->caret;
			}
		}

		DrawControl(control->parent, control);
	}
}

OSError OSProcessGUIMessage(OSMessage *message) {
	// TODO Message security. 
	// 	How should we tell who sent the message?
	// 	(and that they gave us a valid window?)

	OSWindow *window = message->targetWindow;

	switch (message->type) {
		case OS_MESSAGE_MOUSE_MOVED: {
			UpdateMousePosition(window, message->mouseMoved.newPositionX, 
					message->mouseMoved.newPositionY);
		} break;

		case OS_MESSAGE_MOUSE_LEFT_PRESSED: {
			if (window->hoverControl) {
				OSControl *control = window->hoverControl;

				if (control->canHaveFocus) {
					window->focusedControl = control; // TODO Lose when the window is deactivated.
					FindCaret(control, message->mousePressed.positionX, message->mousePressed.positionY, false, (lastClickChainCount = message->mousePressed.clickChainCount));
				}

				window->pressedControl = control;
				DrawControl(window, control);
			}
		} break;

		case OS_MESSAGE_MOUSE_LEFT_RELEASED: {
			if (window->pressedControl) {
				OSControl *previousPressedControl = window->pressedControl;
				window->pressedControl = nullptr;

				if (previousPressedControl->type == OS_CONTROL_CHECKBOX) {
					// If this is a checkbox, then toggle the check.
					previousPressedControl->checked = previousPressedControl->checked ? OS_CONTROL_NO_CHECK : OS_CONTROL_CHECKED;
				} else if (previousPressedControl->type == OS_CONTROL_RADIOBOX) {
					// TODO If this is a radiobox,
					// 	clear the check on all other controls in the container,

					previousPressedControl->checked = OS_CONTROL_RADIO_CHECK;
				}

				DrawControl(window, previousPressedControl);

				if (window->hoverControl == previousPressedControl
						/* || (!window->hoverControl && window->previousHoverControl == previousPressedControl) */) { 
					OSCallbackData data = {};
					data.type = OS_CALLBACK_ACTION;
					SendCallback(previousPressedControl, previousPressedControl->action, data);
				}

				UpdateMousePosition(window, message->mousePressed.positionX, 
						message->mousePressed.positionY);
			}
		} break;

		case OS_MESSAGE_WINDOW_CREATED: {
			window->dirty = true;
		} break;

		case OS_MESSAGE_WINDOW_BLINK_TIMER: {
			if (window->focusedControl) {
				window->focusedControl->caretBlink++;

				if (window->focusedControl->caretBlink == 3) {
					window->focusedControl->caretBlink = 1;
				}

				DrawControl(window, window->focusedControl);
			}
		} break;

		case OS_MESSAGE_KEY_PRESSED: {
			OSControl *control = window->focusedControl;
			ProcessTextboxInput(message, control);
		} break;

		case OS_MESSAGE_KEY_RELEASED: {
		} break;

		default: {
			return OS_ERROR_MESSAGE_NOT_HANDLED_BY_GUI;
		}
	}

	if (window->dirty) {
		OSUpdateWindow(window);
		window->dirty = false;
	}

	return OS_SUCCESS;
}
