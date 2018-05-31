#include <ft2build.h>
#include FT_FREETYPE_H

// TODO '...' when clipping text.

#define DEBUG_ADDITIONAL_KERNING (0)
#define FONT_SIZE (9)

typedef FT_Face Font;

static FT_Library freetypeLibrary;
static Font fontRegular, fontBold, fontMonospaced;

static bool fontRendererInitialised;

struct FontCacheEntry {
	uint8_t *data;
	int character, size;
	int width, height, xoff, yoff;
	int advanceWidth, glyphIndex;
	Font *font;
};

#define FONT_CACHE_SIZE 256
static FontCacheEntry fontCache[FONT_CACHE_SIZE];
static uintptr_t fontCachePosition;

static FontCacheEntry *LookupFontCacheEntry(int character, int size, Font &font) {
	// TODO Is there a faster way to do this?

	for (uintptr_t i = 0; i < FONT_CACHE_SIZE; i++) {
		FontCacheEntry *entry = fontCache + i;

		if (entry->character != character) continue;
		if (entry->size != size) continue;
		if (entry->font != &font) continue;

		return entry;
	}

	return nullptr;
}

static void OSFontRendererInitialise() {
	if (fontRendererInitialised) {
		return;
	}

	OSError error;

	OSNodeInformation nodeRegular;
	error = OSOpenNode(OSLiteral("/OS/Fonts/Noto Sans Regular.ttf"), OS_OPEN_NODE_RESIZE_BLOCK | OS_OPEN_NODE_READ_ACCESS, &nodeRegular);
	if (error != OS_SUCCESS) return;
	void *loadedFontRegular = OSMapObject(nodeRegular.handle, 0, OS_SHARED_MEMORY_MAP_ALL, OS_MAP_OBJECT_READ_ONLY);
	if (!loadedFontRegular) return;

	OSNodeInformation nodeBold;
	error = OSOpenNode(OSLiteral("/OS/Fonts/Noto Sans Bold.ttf"), OS_OPEN_NODE_RESIZE_BLOCK | OS_OPEN_NODE_READ_ACCESS, &nodeBold);
	if (error != OS_SUCCESS) return;
	void *loadedFontBold = OSMapObject(nodeBold.handle, 0, OS_SHARED_MEMORY_MAP_ALL, OS_MAP_OBJECT_READ_ONLY);
	if (!loadedFontBold) return;

	OSNodeInformation nodeMonospaced;
	error = OSOpenNode(OSLiteral("/OS/Fonts/Noto Sans Mono Regular.ttf"), OS_OPEN_NODE_RESIZE_BLOCK | OS_OPEN_NODE_READ_ACCESS, &nodeMonospaced);
	if (error != OS_SUCCESS) return;
	void *loadedFontMonospaced = OSMapObject(nodeMonospaced.handle, 0, OS_SHARED_MEMORY_MAP_ALL, OS_MAP_OBJECT_READ_ONLY);
	if (!loadedFontMonospaced) return;

	OSCloseHandle(nodeRegular.handle);
	OSCloseHandle(nodeBold.handle);
	OSCloseHandle(nodeMonospaced.handle);

	{
		FT_Error error;

		error = FT_Init_FreeType(&freetypeLibrary);

		if (error) {
			OSPrint("Could not initialise freetype.\n");
			return;
		}

		error = FT_New_Memory_Face(freetypeLibrary, (uint8_t *) loadedFontRegular, nodeRegular.fileSize, 0, &fontRegular);

		if (error) {
			OSPrint("Could not load the fonts into freetype.\n");
			return;
		}

		error = FT_New_Memory_Face(freetypeLibrary, (uint8_t *) loadedFontBold, nodeBold.fileSize, 0, &fontBold);

		if (error) {
			OSPrint("Could not load the fonts into freetype.\n");
			return;
		}

		error = FT_New_Memory_Face(freetypeLibrary, (uint8_t *) loadedFontMonospaced, nodeMonospaced.fileSize, 0, &fontMonospaced);

		if (error) {
			OSPrint("Could not load the fonts into freetype.\n");
			return;
		}
	}

	fontRendererInitialised = true;
}

static int _MeasureStringWidth(char *string, char *stringEnd, Font &font) {
	int totalWidth = 0;

	while (string < stringEnd) {
		int character = utf8_value(string);

		if (character == 0x11 || character == 0x12) {
			goto invisibleCharacter;
		}

		{
			int glyphIndex = FT_Get_Char_Index(font, character);
			FT_Load_Glyph(font, glyphIndex, FT_LOAD_DEFAULT);
			int advanceWidth = font->glyph->advance.x >> 6;

			totalWidth += advanceWidth + DEBUG_ADDITIONAL_KERNING;
		}

		invisibleCharacter:;
		string = utf8_advance(string);
	}

	return totalWidth;
}

static int GetLineHeight(Font &font, int size) {
	FT_Set_Char_Size(font, 0, size * 64, 100, 100);
	return font->size->metrics.height >> 6; 
}

int OSGetLineHeight(OSStandardFont font, int fontSize) {
	return GetLineHeight(font == OS_STANDARD_FONT_BOLD ? fontBold 
			: (font == OS_STANDARD_FONT_MONOSPACED ? fontMonospaced : fontRegular), 
			fontSize ? fontSize : FONT_SIZE);
}

static int MeasureString(char *string, size_t stringLength, int size, Font &font, int wrapX) {
	OSFontRendererInitialise();
	if (!fontRendererInitialised) return -1;

	char *stringEnd = string + stringLength;

	int lineHeight = GetLineHeight(font, size);

	int totalWidth = 0;
	int totalHeight = lineHeight;

	FT_Set_Char_Size(font, 0, size * 64, 100, 100);

	while (string < stringEnd) {
		int character = utf8_value(string);

		if (character == 0x11 || character == 0x12) {
			goto invisibleCharacter;
		}

		if (character == '\n') {
			totalWidth = 0;
			totalHeight += lineHeight;
			goto invisibleCharacter;
		}

		if (character == ' ') {
			// Can the next word fit on the line?
			char *wordStart = string;
			while (++string != stringEnd) if (*string == ' ') break;
			int width = _MeasureStringWidth(wordStart, string, font);
			string = wordStart;

			if (width + totalWidth > wrapX) {
				totalWidth = 0;
				totalHeight += lineHeight;
				goto invisibleCharacter;
			}
		}

		{
			int glyphIndex = FT_Get_Char_Index(font, character);
			FT_Load_Glyph(font, glyphIndex, FT_LOAD_DEFAULT);
			int advanceWidth = font->glyph->advance.x >> 6;

			totalWidth += advanceWidth + DEBUG_ADDITIONAL_KERNING;
		}

		invisibleCharacter:;
		string = utf8_advance(string);
	}

	return totalHeight;
}

static int MeasureStringWidth(const char *string, size_t stringLength, int size, Font &font) {
	OSFontRendererInitialise();
	if (!fontRendererInitialised) return -1;
	FT_Set_Char_Size(font, 0, size * 64, 100, 100);
	return _MeasureStringWidth((char *) string, (char *) string + stringLength, font);
}

int OSMeasureStringWidth(OSStandardFont font, int fontSize, const char *string, size_t stringBytes) {
	return MeasureStringWidth(string, stringBytes, fontSize ? fontSize : FONT_SIZE, font == OS_STANDARD_FONT_BOLD ? fontBold 
			: (font == OS_STANDARD_FONT_MONOSPACED ? fontMonospaced : fontRegular));
}

static void DrawCaret(OSPoint &outputPosition, OSRectangle &region, OSRectangle &invalidatedRegion, OSLinearBuffer &linearBuffer, int lineHeight, void *bitmap, int descent) {
	for (int y = 1; y < lineHeight; y++) {
		int oY = outputPosition.y - lineHeight + y + descent;

		if (oY < region.top - 1) continue;
		if (oY >= region.bottom) break;

		if (oY < invalidatedRegion.top) invalidatedRegion.top = oY;
		if (oY > invalidatedRegion.bottom) invalidatedRegion.bottom = oY;

		int oX = outputPosition.x;

		if (oX < region.left) continue;
		if (oX >= region.right) break;

		if (oX < invalidatedRegion.left) invalidatedRegion.left = oX;
		if (oX > invalidatedRegion.right) invalidatedRegion.right = oX;

		uint32_t *destination = (uint32_t *) ((uint8_t *) bitmap + 
				(oX) * 4 + 
				(oY) * linearBuffer.stride);
		*destination = 0xFF000000;
	}
}

inline static void DrawStringPixel(int oX, int oY, void *bitmap, size_t stride, uint32_t textColor, uint32_t selectionColor, int32_t backgroundColor, uint32_t pixel, bool selected) {
	uint32_t *destination = (uint32_t *) ((uint8_t *) bitmap + 
			(oX) * 4 + 
			(oY) * stride);

	uint8_t alpha = (textColor & 0xFF000000) >> 24;
	if (!alpha) alpha = 0xFF;

	if (pixel == 0xFFFFFF && alpha == 0xFF) {
		*destination = 0xFF000000 | textColor;
	} else if (pixel) {
		uint32_t original;

		if (selected) {
			original = selectionColor;
		} else if (backgroundColor < 0) {
			original = *destination;
		} else {
			original = backgroundColor;
		}

		uint32_t ra = (((pixel & 0x000000FF) >> 0) * alpha) >> 8;
		uint32_t ga = (((pixel & 0x0000FF00) >> 8) * alpha) >> 8;
		uint32_t ba = (((pixel & 0x00FF0000) >> 16) * alpha) >> 8;
		uint32_t r2 = (255 - ra) * ((original & 0x000000FF) >> 0);
		uint32_t g2 = (255 - ga) * ((original & 0x0000FF00) >> 8);
		uint32_t b2 = (255 - ba) * ((original & 0x00FF0000) >> 16);
		uint32_t r1 = ra * ((textColor & 0x000000FF) >> 0);
		uint32_t g1 = ga * ((textColor & 0x0000FF00) >> 8);
		uint32_t b1 = ba * ((textColor & 0x00FF0000) >> 16);

		uint32_t result = 0xFF000000 | (0x00FF0000 & ((b1 + b2) << 8)) 
			| (0x0000FF00 & ((g1 + g2) << 0)) 
			| (0x000000FF & ((r1 + r2) >> 8));

		*destination = result;
	}
}

static OSError DrawString(OSHandle surface, OSRectangle region, 
		OSString *string,
		unsigned alignment, uint32_t color, int32_t backgroundColor, uint32_t selectionColor,
		OSPoint coordinate, OSCaret *caret, uintptr_t caretIndex, uintptr_t caretIndex2, bool caretBlink,
		int size, Font &font, OSRectangle clipRegion, int blur, int scrollX) {
	bool actuallyDraw = caret == nullptr;

	OSFontRendererInitialise();
	if (!fontRendererInitialised) OSCrashProcess(OS_FATAL_ERROR_COULD_NOT_LOAD_FONT);

	FT_Set_Char_Size(font, 0, size * 64, 100, 100);

	int lineHeight = font->size->metrics.height >> 6; 
	int ascent = font->size->metrics.ascender >> 6; 

	char *stringEnd = string->buffer + string->bytes;

	OSLinearBuffer linearBuffer = {};
	void *bitmap = nullptr;
	if (surface != OS_INVALID_HANDLE) {
		OSGetLinearBuffer(surface, &linearBuffer);
		bitmap = OSMapObject(linearBuffer.handle, 0, linearBuffer.stride * linearBuffer.height, OS_MAP_OBJECT_READ_WRITE);
	}

	int totalWidth = MeasureStringWidth(string->buffer, string->bytes, size, font);

	OSPoint outputPosition;

	if (alignment & OS_DRAW_STRING_HALIGN_LEFT) {
		if (alignment & OS_DRAW_STRING_HALIGN_RIGHT) {
			// Centered text.
			outputPosition.x = (region.right - region.left) / 2 + region.left - totalWidth / 2;
		} else {
			// Left-justified text.
			outputPosition.x = region.left;
		}
	} else if (alignment & OS_DRAW_STRING_HALIGN_RIGHT) {
		// Right-justified text.
		outputPosition.x = region.right - totalWidth;
	} else	outputPosition.x = region.left;

	if (alignment & OS_DRAW_STRING_VALIGN_TOP) {
		if (alignment & OS_DRAW_STRING_VALIGN_BOTTOM) {
			// Centered text.
			outputPosition.y = (region.bottom - region.top) / 2 + region.top - lineHeight / 2 - 1;
		} else {
			// Top-justified text.
			outputPosition.y = region.top;
		}
	} else if (alignment & OS_DRAW_STRING_VALIGN_BOTTOM) {
		// Bottom-justified text.
		outputPosition.y = region.right - lineHeight;
	} else	outputPosition.y = region.top;

	outputPosition.y += ascent;
	outputPosition.x -= scrollX;

	// Now that we've decided where to draw in the region, clip the region.
	if (clipRegion.left != -1) {
		if (region.left < clipRegion.left) region.left = clipRegion.left;
		if (region.right > clipRegion.right) region.right = clipRegion.right;
		if (region.top < clipRegion.top) region.top = clipRegion.top;
		if (region.bottom > clipRegion.bottom) region.bottom = clipRegion.bottom;
	}

	OSRectangle invalidatedRegion = OS_MAKE_RECTANGLE(outputPosition.x, outputPosition.x,
			outputPosition.y, outputPosition.y);

	uintptr_t characterIndex = 0;

	if (coordinate.x < outputPosition.x && !actuallyDraw) {
		caret->byte = caret->character = 0;
		return OS_SUCCESS;
	}

	char *buffer = string->buffer;

	while (buffer < stringEnd) {
		int character = utf8_value(buffer);

		int width, height, xoff, yoff;
		uint8_t *output = nullptr;
		int glyphIndex, advanceWidth;
		bool selected = false;
		FontCacheEntry *cacheEntry;

		if (character == 0x11 || character == 0x12) {
			goto invisibleCharacter;
		}

		if (character == ' ' && (alignment & OS_DRAW_STRING_WORD_WRAP)) {
			// Can the next word fit on the line?
			char *wordStart = buffer;
			while (++buffer != stringEnd) if (*buffer == ' ') break;
			int width = _MeasureStringWidth(wordStart, buffer, font);
			buffer = wordStart;

			if (width + outputPosition.x > region.right) {
				outputPosition.x = region.left;
				outputPosition.y += lineHeight;
				goto invisibleCharacter;
			}
		}

		if (character == '\n') {
			outputPosition.x = region.left;
			outputPosition.y += lineHeight;
			goto invisibleCharacter;
		}

		cacheEntry = LookupFontCacheEntry(character, size, font);

		if (cacheEntry) {
			output = cacheEntry->data;
			width = cacheEntry->width;
			height = cacheEntry->height;
			xoff = cacheEntry->xoff;
			yoff = cacheEntry->yoff;
			glyphIndex = cacheEntry->glyphIndex;
			advanceWidth = cacheEntry->advanceWidth;
		} else {
			glyphIndex = FT_Get_Char_Index(font, character);
			FT_Load_Glyph(font, glyphIndex, FT_LOAD_DEFAULT);
			advanceWidth = font->glyph->advance.x >> 6;
		}

		advanceWidth += DEBUG_ADDITIONAL_KERNING;

		if (outputPosition.x + advanceWidth < region.left) {
			goto skipCharacter;
		} else if (outputPosition.x >= region.right && !(alignment & OS_DRAW_STRING_WORD_WRAP)) {
			break;
		}

		if (coordinate.x >= outputPosition.x && coordinate.x < outputPosition.x + advanceWidth / 2 && !actuallyDraw) {
			caret->character = characterIndex;
			caret->byte = buffer - string->buffer;
			return OS_SUCCESS;
		}

		if (coordinate.x >= outputPosition.x + advanceWidth / 2 && coordinate.x < outputPosition.x + advanceWidth && !actuallyDraw) {
			caret->character = characterIndex + 1;
			caret->byte = utf8_advance(buffer) - string->buffer;
			return OS_SUCCESS;
		}

		if (!actuallyDraw) {
			goto skipCharacter;
		}

		if (caretIndex != (uintptr_t) -1) {
			if (caretIndex < caretIndex2) {
				if (characterIndex >= caretIndex && characterIndex < caretIndex2) {
					selected = true;
				}
			} else {
				if (characterIndex >= caretIndex2 && characterIndex < caretIndex) {
					selected = true;
				}
			}
		}

		if (selected) {
			for (int y = 1; y < lineHeight; y++) {
				int oY = outputPosition.y - lineHeight + y + (lineHeight - ascent);

				if (oY < region.top - 1) continue;
				if (oY >= region.bottom) break;

				if (oY < invalidatedRegion.top) invalidatedRegion.top = oY;
				if (oY > invalidatedRegion.bottom) invalidatedRegion.bottom = oY;

				for (int x = 0; x < advanceWidth + 1; x++) {
					int oX = outputPosition.x + x;

					if (oX < region.left) continue;
					if (oX >= region.right) break;

					if (oX < invalidatedRegion.left) invalidatedRegion.left = oX;
					if (oX > invalidatedRegion.right) invalidatedRegion.right = oX;

					uint32_t *destination = (uint32_t *) ((uint8_t *) bitmap + 
							(oX) * 4 + 
							(oY) * linearBuffer.stride);

					*destination = selectionColor;
				}
			}
		}

		if (!output) {
			FT_Render_Glyph(font->glyph, FT_RENDER_MODE_LCD);

			FT_Bitmap *bitmap = &font->glyph->bitmap;
			width = bitmap->width / 3;
			height = bitmap->rows;
			xoff = font->glyph->bitmap_left;
			yoff = -font->glyph->bitmap_top;

			output = (uint8_t *) OSHeapAllocate(width * height * 4, false);

			for (int y = 0; y < height; y++) {
				for (int x = 0; x < width; x++) {
					 int32_t r = (int32_t) ((uint8_t *) bitmap->buffer)[x * 3 + y * bitmap->pitch + 0];
					 int32_t g = (int32_t) ((uint8_t *) bitmap->buffer)[x * 3 + y * bitmap->pitch + 1];
					 int32_t b = (int32_t) ((uint8_t *) bitmap->buffer)[x * 3 + y * bitmap->pitch + 2];

					 // Reduce how noticible the colour fringes are.
					 int32_t average = (r + g + b) / 3;
					 r -= (r - average) / 3;
					 g -= (g - average) / 3;
					 b -= (b - average) / 3;

					 output[(x + y * width) * 4 + 0] = (uint8_t) r;
					 output[(x + y * width) * 4 + 1] = (uint8_t) g;
					 output[(x + y * width) * 4 + 2] = (uint8_t) b;
				}
			}

			if (output) {
				FontCacheEntry *entry = fontCache + fontCachePosition;
				if (entry->data) OSHeapFree(entry->data);
				entry->data = output;
				entry->character = character;
				entry->width = width;
				entry->height = height;
				entry->xoff = xoff;
				entry->yoff = yoff;
				entry->advanceWidth = advanceWidth;
				entry->glyphIndex = glyphIndex;
				entry->size = size;
				entry->font = &font;
				fontCachePosition = (fontCachePosition + 1) % FONT_CACHE_SIZE;
			} else {
				goto skipCharacter;
			}
		}

		for (int y = 0; y < height; y++) {
			int oY = outputPosition.y + yoff + y;

			if (oY < region.top) continue;
			if (oY >= region.bottom) break;

			if (oY < 0) continue;
			if (oY >= (int) linearBuffer.height) break;

			if (oY < invalidatedRegion.top) invalidatedRegion.top = oY;
			if (oY > invalidatedRegion.bottom) invalidatedRegion.bottom = oY;

			for (int x = 0; x < width; x++) {
				int oX = outputPosition.x + xoff + x;

				if (oX < region.left) continue;
				if (oX >= region.right) break;

				if (oX < 0) continue;
				if (oX >= (int) linearBuffer.width) break;

				if (oX < invalidatedRegion.left) invalidatedRegion.left = oX;
				if (oX > invalidatedRegion.right) invalidatedRegion.right = oX;

				if (blur) {
					uint32_t pixel = *((uint32_t *) (output + (x * 4 + y * width * 4)));

					for (int i = -blur; i <= blur; i++) {
						int oY = outputPosition.y + yoff + y + i;

						if (oY < region.top) continue;
						if (oY >= region.bottom) break;

						if (oY < 0) continue;
						if (oY >= (int) linearBuffer.height) break;

						if (oY < invalidatedRegion.top) invalidatedRegion.top = oY;
						if (oY > invalidatedRegion.bottom) invalidatedRegion.bottom = oY;

						for (int j = -blur; j <= blur; j++) {
							int oX = outputPosition.x + xoff + x + j;

							if (oX < region.left) continue;
							if (oX >= region.right) break;

							if (oX < 0) continue;
							if (oX >= (int) linearBuffer.width) break;

							if (oX < invalidatedRegion.left) invalidatedRegion.left = oX;
							if (oX > invalidatedRegion.right) invalidatedRegion.right = oX;

							uint32_t divisor = (6 * (i * i + j * j + 1));
							uint32_t r = ((pixel & 0xFF) >> 0) / divisor;

							DrawStringPixel(oX, oY, bitmap, linearBuffer.stride, color, selectionColor, backgroundColor, r | (r << 8) | (r << 16), selected);
						}
					}
				} else {
					uint32_t pixel = *((uint32_t *) (output + (x * 4 + y * width * 4)));
					DrawStringPixel(oX, oY, bitmap, linearBuffer.stride, color, selectionColor, backgroundColor, pixel, selected);
				}
			}
		}

		skipCharacter:
		if (characterIndex == caretIndex2 && caretIndex != (uintptr_t) -1 && !caretBlink) {
			DrawCaret(outputPosition, region, invalidatedRegion, linearBuffer, lineHeight, bitmap, lineHeight - ascent);
		}

		outputPosition.x += advanceWidth;

		invisibleCharacter:
		buffer = utf8_advance(buffer);
		characterIndex++;
	}

	if (characterIndex == caretIndex2 && caretIndex != (uintptr_t) -1 && !caretBlink) {
		DrawCaret(outputPosition, region, invalidatedRegion, linearBuffer, lineHeight, bitmap, lineHeight - ascent);
	}

	if (coordinate.x >= outputPosition.x && !actuallyDraw && characterIndex) {
		caret->character = characterIndex;
		caret->byte = buffer - string->buffer;
		return OS_SUCCESS;
	}

	if (surface != OS_INVALID_HANDLE) {
		OSInvalidateRectangle(surface, invalidatedRegion);
	}

	if (bitmap) {
		OSUnmapObject(bitmap);
		OSCloseHandle(linearBuffer.handle);
	}

	return actuallyDraw ? OS_SUCCESS : OS_ERROR_NO_CHARACTER_AT_COORDINATE;
}

OS_EXTERN_C OSError OSFindCharacterAtCoordinate(OSRectangle region, OSPoint coordinate, OSString *string, unsigned flags, OSCaret *position, int fontSize, int scrollX) {
	return DrawString(OS_INVALID_HANDLE, region, string,
			flags, 0, 0, 0,
			coordinate, position, -1, -1, false,
			fontSize ? fontSize : FONT_SIZE, fontRegular, OS_MAKE_RECTANGLE(-1, -1, -1, -1), 0, scrollX);
}

OSError OSDrawString(OSHandle surface, OSRectangle region, 
		OSString *string, int fontSize,
		unsigned alignment, uint32_t color, int32_t backgroundColor, OSStandardFont font, OSRectangle clipRegion, int blur) {
	return DrawString(surface, region, string,
			alignment ? alignment : OS_DRAW_STRING_HALIGN_CENTER | OS_DRAW_STRING_VALIGN_CENTER, color, backgroundColor, 0,
			OS_MAKE_POINT(0, 0), nullptr, -1, -1, false, fontSize ? fontSize : FONT_SIZE, font == OS_STANDARD_FONT_BOLD ? fontBold 
			: (font == OS_STANDARD_FONT_MONOSPACED ? fontMonospaced : fontRegular), clipRegion, blur, 0);
}
