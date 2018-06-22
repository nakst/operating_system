#ifndef IMPLEMENTATION

enum VideoColorMode {
	// Little-endian.
	VIDEO_COLOR_24_RGB, // byte[0] = blue, byte[1] = green, byte[2] = red
	VIDEO_COLOR_32_XRGB,
	VIDEO_COLOR_VGA_PLANES,
};

struct ModifiedScanline {
	// These values describe a range that encompasses all modified pixels on the scanline.
	// There may be unmodified pixels described by this.
	
	uint16_t minimumX,
		 maximumX;
};

struct Surface {
	// All Surfaces are currently VIDEO_COLOR_32_XRGB.
	// This reduces the amount of graphics function I have to write.

	bool Initialise(size_t resX, size_t resY,
			bool createDepthBuffer = false);
	void Destroy();

	void Resize(size_t newResX, size_t newResY);

	// Copy the contents of the rectangular region sourceRegion from the source surface to this surface, 
	// where destinationPoint corresponds to where the top-left corner of the sourceRegion will be copied.
	// If avoidUnmodifiedRegions is set then regions that have not been marked as modified may not copied;
	// however, all of the modified regions will be definitely copied.
	// If depth is not SURFACE_COPY_WITHOUT_DEPTH_CHECKING, then for every pixel that is attempting to be copied,
	// it will check if the current value of depth is greater than the corresponding value in the depthBuffer;
	// if it is not, the pixel will not be drawn, otherwise the depth will be updated and the pixel will be drawn.
#define SURFACE_COPY_WITHOUT_DEPTH_CHECKING 0xFFFF
	void Copy(Surface &source, OSPoint destinationPoint, OSRectangle sourceRegion,
			bool avoidUnmodifiedRegions, uint16_t depth = SURFACE_COPY_WITHOUT_DEPTH_CHECKING, bool alreadyLocked = false); 

	// Alpha blend the contents of the source region from the source surface to the destination region on this surface.
	// The draw mode determines how the border is used.
	void Draw(Surface &source, OSRectangle destinationRegion, OSRectangle sourceRegion, 
			OSRectangle borderDimensions, OSDrawMode mode, uint8_t alpha, bool alreadyLocked);

	void BlendWindow(Surface &source, OSPoint destinationPoint, OSRectangle sourceRegion, uint16_t depth);

	// Fill a region of this surface with the specified color.
	void FillRectangle(OSRectangle region, OSColor color, bool alreadyLocked = false);

	// Mark a region of the surface as modified.
	void InvalidateScanline(uintptr_t y, uintptr_t from, uintptr_t to);
	void InvalidateRectangle(OSRectangle rectangle);

	// Mark the whole surface as unmodified.
	void ClearModifiedRegion();

	// The memory used by the surface.
	uint8_t *memory;
	SharedMemoryRegion *region;

	uint8_t *linearBuffer; // The linear buffer of the surface. 
			       // Currently each pixel is 32-bit [A/X]RGB, little-endian.
	uint16_t *depthBuffer; // The depth of each pixel in the surface.

	size_t resX, resY; // Currently both must be < 65536.
	uintptr_t stride;  // Number of bytes from the start of a scanline to the start of the next.

	// A bitset of every modified scanline on the surface.
	uint8_t *modifiedScanlineBitset;

	// Contains the lowest and highest column that was modified on each scanline.
	// Only valid if the corresponding bit is set in the modified scanline bitset.
	ModifiedScanline *modifiedScanlines;

	// Prevent invalidating the corners on the surface, so that they will never be copied (i.e. transparent).
	bool roundCorners;

	// 'memory' consists of 3 sections:
	// 	  Contents				Size					Offset					Optional?
	// 	- Pixel data				resX * resY * 4				0					No
	// 	- Depth buffer				resX * resY * 4				resX * resY * 4				Yes
	// 	- Modified scanline bitset		(resY + 7) / 8				resX * resY * 8				Not yet
	// 	- Modified scanline information		resY * sizeof(ModifiedScanline)		resX * resY * 8 + (resY + 7) / 8	Not yet

	// Prevent multiple threads modifing the surface at the same time.
	Mutex mutex;

	volatile unsigned handles;
};

struct Graphics {
	void Initialise();

	void UpdateScreen();
	void UpdateScreen_VIDEO_COLOR_24_RGB();
	void UpdateScreen_VIDEO_COLOR_32_XRGB();
	void VGAUpdateScreen();
	Mutex updateScreenMutex;

	uint8_t *linearBuffer; 
        size_t resX, resY; 
	uintptr_t strideY, strideX;
	VideoColorMode colorMode;

	Surface frameBuffer;
	Pool surfacePool;

#define CURSOR_SWAP_SIZE (20)
	Surface cursorSwap; // A surface for temporarily storing the pixels behind the cursor.
};

struct VESAVideoModeInformation {
	// Useful fields marked with /**/
	uint16_t attributes;
	uint8_t windowA, windowB;
	uint16_t granularity;
	uint16_t windowSize;
	uint16_t segmentA, segmentB;
	uint32_t farWindowMove;
	uint16_t bytesPerScanline;
/**/	uint16_t widthPixels;
/**/	uint16_t heightPixels;
	uint8_t characterWidth;
	uint8_t characterHeight;
	uint8_t memoryPlanes;
/**/	uint8_t bitsPerPixel;
	uint8_t bankCount;
	uint8_t memoryModel;
	uint8_t bankSizeKB;
	uint8_t imagePages;
	uint8_t reserved;
	uint8_t redMaskSize;
	uint8_t redMaskPosition;
	uint8_t greenMaskSize;
	uint8_t greenMaskPosition;
	uint8_t blueMaskSize;
	uint8_t blueMaskPosition;
	uint8_t reservedMaskSize;
	uint8_t reservedMaskPosition;
	uint8_t directColorModeInfo;
/**/	uint32_t bufferPhysical;
	uint32_t offscreenMemoryPhysical;
	uint16_t offscreenMemorySizeKB;
/**/	uint16_t bytesPerScanlineLinear;
	uint8_t imagesBanked;
	uint8_t imagesLinear;
/**/	uint8_t linearRedMaskSize;
/**/	uint8_t linearRedMaskPosition;
/**/	uint8_t linearGreenMaskSize;
/**/	uint8_t linearGreenMaskPosition;
/**/	uint8_t linearBlueMaskSize;
/**/	uint8_t linearBlueMaskPosition;
	uint8_t linearReservedMaskSize;
	uint8_t linearReservedMaskPosition;
	uint32_t maximumPixelClock;
};

VESAVideoModeInformation *vesaMode = (VESAVideoModeInformation *) (LOW_MEMORY_MAP_START + 0x7000);
Graphics graphics;

#else

void Graphics::UpdateScreen_VIDEO_COLOR_32_XRGB() {
	volatile uint8_t *pixel = (volatile uint8_t *) linearBuffer;
	volatile uint8_t *source = (volatile uint8_t *) frameBuffer.linearBuffer;

	for (uintptr_t y_ = 0; y_ < resY / 8; y_++) {
		if (frameBuffer.modifiedScanlineBitset[y_] == 0) {
			pixel += strideY * 8;
			source += frameBuffer.resX * 32;
			continue;
		}

		for (uintptr_t y = 0; y < 8; y++) {
			volatile uint8_t *scanlineStart = pixel;
			volatile uint8_t *sourceStart = source;

			if ((frameBuffer.modifiedScanlineBitset[y_] & (1 << y)) == 0) {
				pixel = scanlineStart + strideY;
				source += frameBuffer.resX * 4;
				continue;
			}

			ModifiedScanline *scanline = frameBuffer.modifiedScanlines + y + (y_ << 3);

			uintptr_t x = scanline->minimumX;
			pixel += 4 * x;
			source += 4 * x;

			// uintptr_t total = scanline->maximumX - scanline->minimumX;

			// TODO SIMD

			while (x < scanline->maximumX) {
				pixel[0] = source[0];
				pixel[1] = source[1];
				pixel[2] = source[2];

				pixel += 4;
				source += 4;
				x++;
			}

			pixel = scanlineStart + strideY;
			source = sourceStart + frameBuffer.resX * 4;
		}
	}

	frameBuffer.ClearModifiedRegion();
}

void Graphics::UpdateScreen_VIDEO_COLOR_24_RGB() {
	volatile uint8_t *pixel = (volatile uint8_t *) linearBuffer;
	volatile uint8_t *source = (volatile uint8_t *) frameBuffer.linearBuffer;

	for (uintptr_t y_ = 0; y_ < resY / 8; y_++) {
		if (frameBuffer.modifiedScanlineBitset[y_] == 0) {
			pixel += strideY * 8;
			source += frameBuffer.resX * 32;
			continue;
		}

		for (uintptr_t y = 0; y < 8; y++) {
			volatile uint8_t *scanlineStart = pixel;
			volatile uint8_t *sourceStart = source;

			if ((frameBuffer.modifiedScanlineBitset[y_] & (1 << y)) == 0) {
				pixel = scanlineStart + strideY;
				source += frameBuffer.resX * 4;
				continue;
			}

			ModifiedScanline *scanline = frameBuffer.modifiedScanlines + y + (y_ << 3);

			uintptr_t x = scanline->minimumX;
			pixel += 3 * x;
			source += 4 * x;

			uintptr_t total = scanline->maximumX - scanline->minimumX;

			if (simdSSSE3Support && total > 8) {
				size_t pixelGroups = total / 4 - 1;
				size_t pixels = pixelGroups * 4;
				SSSE3Framebuffer32To24Copy(pixel, source, pixelGroups);

				x += pixels;
				pixel += 3 * pixels;
				source += 4 * pixels;
			}

			while (x < scanline->maximumX) {
				pixel[0] = source[0];
				pixel[1] = source[1];
				pixel[2] = source[2];

				pixel += 3;
				source += 4;
				x++;
			}

			pixel = scanlineStart + strideY;
			source = sourceStart + frameBuffer.resX * 4;
		}
	}

	frameBuffer.ClearModifiedRegion();
}

void Graphics::UpdateScreen() {
	windowManager.mutex.Acquire();
	int cursorX = windowManager.cursorX + windowManager.cursorImageOffsetX, cursorY = windowManager.cursorY + windowManager.cursorImageOffsetY;
	int cursorImageX = windowManager.cursorImageX, cursorImageY = windowManager.cursorImageY;
	int cursorImageWidth = windowManager.cursorImageWidth, cursorImageHeight = windowManager.cursorImageHeight;
	windowManager.mutex.Release();

	updateScreenMutex.Acquire();
	Defer(updateScreenMutex.Release());

	frameBuffer.mutex.Acquire();
	Defer(frameBuffer.mutex.Release());

	OSRectangle sourceRectangle = OS_MAKE_RECTANGLE(cursorX, cursorX + CURSOR_SWAP_SIZE, cursorY, cursorY + CURSOR_SWAP_SIZE);

	if (sourceRectangle.left < 0) sourceRectangle.left = 0;
	if (sourceRectangle.top < 0) sourceRectangle.top = 0;
	if (sourceRectangle.right > (int) frameBuffer.resX) sourceRectangle.right = frameBuffer.resX;
	if (sourceRectangle.bottom > (int) frameBuffer.resY) sourceRectangle.bottom = frameBuffer.resY;

	cursorSwap.mutex.Acquire();
	cursorSwap.Copy(frameBuffer, OS_MAKE_POINT(0, 0), sourceRectangle, false, SURFACE_COPY_WITHOUT_DEPTH_CHECKING, true);
	cursorSwap.mutex.Release();

	frameBuffer.Draw(uiSheetSurface, OS_MAKE_RECTANGLE(cursorX, cursorX + cursorImageWidth,
						     cursorY, cursorY + cursorImageHeight),
					 OS_MAKE_RECTANGLE(cursorImageX, cursorImageX + cursorImageWidth, cursorImageY, cursorImageY + cursorImageHeight),
					 OS_MAKE_RECTANGLE(cursorImageX + 2, cursorImageX + 3, cursorImageY + 2, cursorImageY + 3), OS_DRAW_MODE_REPEAT_FIRST, 0xFF, true);
		
	switch (colorMode) {
		case VIDEO_COLOR_24_RGB: {
			UpdateScreen_VIDEO_COLOR_24_RGB();
		} break;

		case VIDEO_COLOR_32_XRGB: {
			UpdateScreen_VIDEO_COLOR_32_XRGB();
		} break;

		case VIDEO_COLOR_VGA_PLANES: {
			VGAUpdateScreen();
		} break;
					 
		default: {
			KernelPanic("Graphics::UpdateScreen - Unsupported color mode.\n");
		} break;
	}

	if (cursorX < 0) cursorX = 0;
	if (cursorY < 0) cursorY = 0;
	frameBuffer.Copy(cursorSwap, OS_MAKE_POINT(cursorX, cursorY), 
			OS_MAKE_RECTANGLE(0, sourceRectangle.right - sourceRectangle.left, 
				0, sourceRectangle.bottom - sourceRectangle.top),
			false, SURFACE_COPY_WITHOUT_DEPTH_CHECKING, true);
}

void Graphics::Initialise() {
	if (vesaMode->widthPixels) {
		linearBuffer = (uint8_t *) kernelVMM.Allocate("ScreenBuffer", vesaMode->bytesPerScanlineLinear * vesaMode->heightPixels, VMM_MAP_ALL, VMM_REGION_PHYSICAL, vesaMode->bufferPhysical, 0);
		resX = vesaMode->widthPixels;
		resY = vesaMode->heightPixels;
		strideX = vesaMode->bitsPerPixel >> 3;
		strideY = vesaMode->bytesPerScanlineLinear;

		// TODO Other color modes.
		colorMode = vesaMode->bitsPerPixel == 32 ? VIDEO_COLOR_32_XRGB : VIDEO_COLOR_24_RGB; 
	} else {
		VGASetMode();
		resX = 640;
		resY = 480;
		colorMode = VIDEO_COLOR_VGA_PLANES;
	}

	surfacePool.Initialise(sizeof(Surface));

	cursorSwap.Initialise(CURSOR_SWAP_SIZE, CURSOR_SWAP_SIZE, false);
	frameBuffer.Initialise(resX, resY, true /*Create depth buffer for window manager*/);
}

void Surface::Resize(size_t newResX, size_t newResY) {
	// WARNING:
	// 	Don't expose this through the system call API!
	// 	We don't lock all the surfaces properly at the moment in draw/copy operations.

	mutex.Acquire();
	Defer(mutex.Release());

	if (depthBuffer) {
		KernelPanic("Surface::Resize - Attempt to resize a surface with a depth buffer.\n");
	}

	Surface oldState;
	CopyMemory(&oldState, this, sizeof(Surface));
	ZeroMemory(&oldState.mutex, sizeof(Mutex));

	{
		resX = newResX;
		resY = newResY;

		size_t memoryNeeded = resY * sizeof(ModifiedScanline) + resX * resY * 4 + (resY + 7) / 8;
		CloseHandleToObject(region, KERNEL_OBJECT_SHMEM, 0);
		region = sharedMemoryManager.CreateSharedMemory(memoryNeeded);
		memory = (uint8_t *) kernelVMM.Allocate("Surface", memoryNeeded, VMM_MAP_LAZY, VMM_REGION_SHARED, 0, VMM_REGION_FLAG_CACHABLE, region);
		linearBuffer = memory;
		depthBuffer = nullptr;
		modifiedScanlines = (ModifiedScanline *) (memory + resX * resY * 4);
		modifiedScanlineBitset = (uint8_t *) (modifiedScanlines + resY);
		stride = resX * 4;
	}

	Copy(oldState, OS_MAKE_POINT(0, 0), OS_MAKE_RECTANGLE(0, oldState.resX < newResX ? oldState.resX : newResX, 0, oldState.resY < newResY ? oldState.resY : newResY), false, SURFACE_COPY_WITHOUT_DEPTH_CHECKING, true);
	kernelVMM.Free(oldState.memory); 
}

bool Surface::Initialise(size_t _resX, size_t _resY, bool createDepthBuffer) {
	resX = _resX;
	resY = _resY;

	handles = 1;

	stride = resX * 4;

	// Check the surface is within our working size limits.
	if (!resX || !resY || resX >= 65536 || resY >= 65536) {
		return false;
	}

	// Allocate the memory needed by the surface.
	size_t memoryNeeded;
	if (createDepthBuffer) {
		memoryNeeded = resY * sizeof(ModifiedScanline) + resX * resY * 6 + (resY + 7) / 8;
		region = sharedMemoryManager.CreateSharedMemory(memoryNeeded);
		memory = (uint8_t *) kernelVMM.Allocate("Surface", memoryNeeded, VMM_MAP_LAZY, VMM_REGION_SHARED, 0, VMM_REGION_FLAG_CACHABLE, region);
		linearBuffer = memory;
		depthBuffer = (uint16_t *) (memory + resX * resY * 4);
		modifiedScanlines = (ModifiedScanline *) (depthBuffer + resX * resY);
		modifiedScanlineBitset = (uint8_t *) (modifiedScanlines + resY);
	} else {
		memoryNeeded = resY * sizeof(ModifiedScanline) + resX * resY * 4 + (resY + 7) / 8;
		region = sharedMemoryManager.CreateSharedMemory(memoryNeeded);
		memory = (uint8_t *) kernelVMM.Allocate("Surface", memoryNeeded, VMM_MAP_LAZY, VMM_REGION_SHARED, 0, VMM_REGION_FLAG_CACHABLE, region);
		linearBuffer = memory;
		depthBuffer = nullptr;
		modifiedScanlines = (ModifiedScanline *) (memory + resX * resY * 4);
		modifiedScanlineBitset = (uint8_t *) (modifiedScanlines + resY);
	}

	if (!memory) {
		return false;
	}

	// KernelLog(LOG_VERBOSE, "Created surface %x using %dKB.\n", this, memoryNeeded / 1024);

	// We probably want to invalidate the whole surface when it is created.
	InvalidateRectangle(OS_MAKE_RECTANGLE(0, resX, 0, resY));

#if 0
	// Set the background.
	FillRectangle(OS_MAKE_RECTANGLE(0, resX, 0, resY), OS_MAKE_COLOR(0xFF0000));
#endif

	// We created the surface successfully!
	return true;
}

void Surface::ClearModifiedRegion() {
	mutex.AssertLocked();

	ZeroMemory(modifiedScanlineBitset, (resY + 7) / 8);
}

void Surface::InvalidateScanline(uintptr_t y, uintptr_t from, uintptr_t to) {
	mutex.AssertLocked();

	if (roundCorners) {
		if (y == 0 || y == resY - 1) {
			if (from < 5) from = 5;
			if (to >= resX - 5) to = resX - 5;
		} else if (y == 1 || y == resY - 2) {
			if (from < 3) from = 3;
			if (to >= resX - 3) to = resX - 3;
		} else if (y == 2 || y == resY - 3) {
			if (from < 2) from = 2;
			if (to >= resX - 2) to = resX - 2;
		} else if (y == 3 || y == 4 || y == resY - 4 || y == resY - 5) {
			if (from < 1) from = 1;
			if (to >= resX - 1) to = resX - 1;
		}
	}

	if ((modifiedScanlineBitset[y >> 3] & 1 << (y & 7))) {
		if (modifiedScanlines[y].minimumX > from)
			modifiedScanlines[y].minimumX = from;
		if (modifiedScanlines[y].maximumX < to)
			modifiedScanlines[y].maximumX = to;
	} else {
		modifiedScanlineBitset[y >> 3] |= 1 << (y & 7);
		modifiedScanlines[y].minimumX = from;
		modifiedScanlines[y].maximumX = to;
	}
}

void Surface::InvalidateRectangle(OSRectangle rectangle) {
	mutex.Acquire();
	Defer(mutex.Release());

	if (rectangle.top < 0) {
		rectangle.top = 0;
	}

	if (rectangle.bottom > (intptr_t) resY) {
		rectangle.bottom = resY;
	}

	if (rectangle.left < 0) {
		rectangle.left = 0;
	}

	if (rectangle.right > (intptr_t) resX) {
		rectangle.right = resX;
	}

	for (intptr_t y = rectangle.top; y < rectangle.bottom; y++) {
		InvalidateScanline(y, rectangle.left, rectangle.right);
	}
}

void Surface::Copy(Surface &source, OSPoint destinationPoint, OSRectangle sourceRegion, bool avoidUnmodifiedRegions, uint16_t depth,
		bool alreadyLocked) {
	if (depth != SURFACE_COPY_WITHOUT_DEPTH_CHECKING) {
		if (!depthBuffer) {
			KernelPanic("Surface::Copy - Attempt to use depth checking on surface without depth buffer.\n");
		}
	}

	if (destinationPoint.x < 0) {
		intptr_t shiftRight = -destinationPoint.x;
		destinationPoint.x = 0;
		sourceRegion.left += shiftRight;
	}

	if (destinationPoint.y < 0) {
		intptr_t shiftDown = -destinationPoint.y;
		destinationPoint.y = 0;
		sourceRegion.top += shiftDown;
	}

	if (destinationPoint.x + sourceRegion.right - sourceRegion.left > (intptr_t) resX) {
		intptr_t shiftLeft = destinationPoint.x + sourceRegion.right - sourceRegion.left - resX;
		sourceRegion.right -= shiftLeft;
	}

	if (destinationPoint.y + sourceRegion.bottom - sourceRegion.top > (intptr_t) resY) {
		intptr_t shiftUp = destinationPoint.y + sourceRegion.bottom - sourceRegion.top - resY;
		sourceRegion.bottom -= shiftUp;
	}

	if (sourceRegion.left < 0 || sourceRegion.top < 0
			|| sourceRegion.right > (intptr_t) source.resX || sourceRegion.bottom > (intptr_t) source.resY
			|| sourceRegion.left >= sourceRegion.right || sourceRegion.top >= sourceRegion.bottom) {
		// If the source region was invalid, don't perform the operation.
		return;
	}

	OSRectangle destinationRegion = OS_MAKE_RECTANGLE(destinationPoint.x,
						    destinationPoint.x + sourceRegion.right - sourceRegion.left,
						    destinationPoint.y,
						    destinationPoint.y + sourceRegion.bottom - sourceRegion.top);

	if (!alreadyLocked) AcquireMutexPair(&mutex, &source.mutex);
	Defer(if (!alreadyLocked) ReleaseMutexPair(&mutex, &source.mutex););

	uint8_t *destinationPixel = linearBuffer + destinationRegion.top * stride + destinationRegion.left * 4;
	uint8_t *sourcePixel = source.linearBuffer + sourceRegion.top * source.stride + sourceRegion.left * 4;
	uint16_t *depthPixel = depthBuffer + destinationRegion.top * resX + destinationRegion.left;

	intptr_t destinationY = destinationRegion.top;
	intptr_t sourceY = sourceRegion.top;

	__m128i thisDepth = _mm_set1_epi16(depth);

	while (sourceY < sourceRegion.bottom) {
		intptr_t offsetX = 0;
		size_t countX = sourceRegion.right - sourceRegion.left;

		uint8_t *destinationStart = destinationPixel;
		uint8_t *sourceStart = sourcePixel;
		uint16_t *depthStart = depthPixel;

		if (sourceY < 0 || sourceY >= (intptr_t) source.resY) {
			goto nextScanline;
		}

		if (avoidUnmodifiedRegions) {
			if (source.modifiedScanlineBitset[sourceY / 8] & (1 << (sourceY & 7))) {
				ModifiedScanline *scanline = source.modifiedScanlines + sourceY;

				uint16_t minimumX = scanline->minimumX;
				uint16_t maximumX = scanline->maximumX;

				if (maximumX < sourceRegion.left) {
					goto nextScanline;
				} else if (minimumX > sourceRegion.right) {
					goto nextScanline;
				}

				if (minimumX < sourceRegion.left) {
					minimumX = sourceRegion.left;
				}
				
				if (maximumX > sourceRegion.right) {
					maximumX = sourceRegion.right;
				}

				offsetX = minimumX - sourceRegion.left;
				countX = maximumX - minimumX;
			} else {
				goto nextScanline;
			}
		}

		InvalidateScanline(destinationY, destinationRegion.left + offsetX, destinationRegion.left + offsetX + countX);

		destinationPixel += offsetX * 4;
		sourcePixel += offsetX * 4;
		depthPixel += offsetX;

		if (depth == SURFACE_COPY_WITHOUT_DEPTH_CHECKING) {
			while (countX >= 4) {
				_mm_storeu_si128((__m128i *) destinationPixel, 
						_mm_loadu_si128((__m128i *) sourcePixel));
				destinationPixel += 16;
				sourcePixel += 16;
				countX -= 4;
			}

			while (countX >= 1) {
				*destinationPixel++ = *sourcePixel++;
				*destinationPixel++ = *sourcePixel++;
				*destinationPixel++ = *sourcePixel++;
				*destinationPixel++ = *sourcePixel++;

				countX -= 1;
			}
		} else {
#if 1
			while (countX >= 4) {
				__m128i oldDepth = _mm_loadl_epi64((__m128i *) depthPixel);
				__m128i maskDepth = _mm_cmplt_epi16(thisDepth, oldDepth);
				__m128i maskDepth128 = _mm_unpacklo_epi16(maskDepth, maskDepth);
				__m128i destinationValue = _mm_loadu_si128((__m128i *) destinationPixel);
				__m128i sourceValue = _mm_loadu_si128((__m128i *) sourcePixel);
				__m128i blendedValue = _mm_or_si128(_mm_and_si128(maskDepth128, destinationValue), _mm_andnot_si128(maskDepth128, sourceValue));
				__m128i blendedDepth = _mm_or_si128(_mm_and_si128(maskDepth, oldDepth), _mm_andnot_si128(maskDepth, thisDepth));
				_mm_storeu_si128((__m128i *) destinationPixel, blendedValue);
				_mm_storel_epi64((__m128i *) depthPixel, blendedDepth);

				destinationPixel += 16;
				sourcePixel += 16;
				depthPixel += 4;
				countX -= 4;
			}
#endif

			while (countX >= 1) {
				if (*depthPixel <= depth) {
					*depthPixel = depth;
					destinationPixel[0] = sourcePixel[0];
					destinationPixel[1] = sourcePixel[1];
					destinationPixel[2] = sourcePixel[2];
					destinationPixel[3] = sourcePixel[3];
				}

				destinationPixel += 4;
				sourcePixel += 4;
				depthPixel += 1;
				countX -= 1;
			}
		}

		nextScanline:
		destinationPixel = destinationStart + stride;
		sourcePixel = sourceStart + source.stride;
		depthPixel = depthStart + resX;
		destinationY++;
		sourceY++;
	}
}

void Surface::BlendWindow(Surface &source, OSPoint destinationPoint, OSRectangle sourceRegion, uint16_t depth) {
	mutex.Acquire();
	Defer(mutex.Release());

	intptr_t y = sourceRegion.top;

	uint8_t *destinationPixel = linearBuffer + destinationPoint.y * stride + destinationPoint.x * 4;
	uint8_t *sourcePixel = source.linearBuffer + sourceRegion.top * source.stride + sourceRegion.left * 4;
	uint16_t *depthPixel = depthBuffer + destinationPoint.y * resX + destinationPoint.x;

	__m128i thisDepth = _mm_set1_epi16(depth);

	__m128i constant255 = _mm_set1_epi32(0xFF);

	while (y < sourceRegion.bottom) {
		size_t countX = sourceRegion.right - sourceRegion.left;
		InvalidateScanline(y - sourceRegion.top + destinationPoint.y, destinationPoint.x, destinationPoint.x + sourceRegion.right - sourceRegion.left);

		uint8_t *a = destinationPixel, *b = sourcePixel;
		uint16_t *c = depthPixel;

		while (countX >= 4) {
			__m128i destinationValue = _mm_loadu_si128((__m128i *) destinationPixel);
			__m128i sourceValue 	 = _mm_loadu_si128((__m128i *) sourcePixel);

			__m128i alpha = _mm_srli_epi32(sourceValue, 24);
			alpha = _mm_and_si128(alpha, constant255);

			__m128i red 	= _mm_mullo_epi16(_mm_and_si128(_mm_srli_epi32(sourceValue, 0),  constant255), alpha);
			__m128i green 	= _mm_mullo_epi16(_mm_and_si128(_mm_srli_epi32(sourceValue, 8),  constant255), alpha);
			__m128i blue 	= _mm_mullo_epi16(_mm_and_si128(_mm_srli_epi32(sourceValue, 16), constant255), alpha);

			alpha = _mm_sub_epi32(constant255, alpha);

			red 	= _mm_srli_epi32(_mm_add_epi32(red,   _mm_mullo_epi16(_mm_and_si128(_mm_srli_epi32(destinationValue, 0),  constant255), alpha)), 8);
			green 	= _mm_srli_epi32(_mm_add_epi32(green, _mm_mullo_epi16(_mm_and_si128(_mm_srli_epi32(destinationValue, 8),  constant255), alpha)), 8);
			blue 	= _mm_srli_epi32(_mm_add_epi32(blue,  _mm_mullo_epi16(_mm_and_si128(_mm_srli_epi32(destinationValue, 16), constant255), alpha)), 8);

			sourceValue = _mm_or_si128(_mm_slli_epi32(red, 0), _mm_or_si128(_mm_slli_epi32(green, 8), _mm_slli_epi32(blue, 16)));

			_mm_storeu_si128((__m128i *) destinationPixel, sourceValue);
			_mm_storel_epi64((__m128i *) depthPixel, thisDepth);

			destinationPixel += 16;
			sourcePixel += 16;
			depthPixel += 4;
			countX -= 4;
		}

		while (countX >= 1) {
			uint32_t modified = *sourcePixel;
			uint32_t original = *destinationPixel;
			uint32_t alpha1 = (modified & 0xFF000000) >> 24;
			uint32_t alpha2 = 255 - alpha1;
			uint32_t r2 = alpha2 * ((original & 0x000000FF) >> 0);
			uint32_t g2 = alpha2 * ((original & 0x0000FF00) >> 8);
			uint32_t b2 = alpha2 * ((original & 0x00FF0000) >> 16);
			uint32_t r1 = alpha1 * ((modified & 0x000000FF) >> 0);
			uint32_t g1 = alpha1 * ((modified & 0x0000FF00) >> 8);
			uint32_t b1 = alpha1 * ((modified & 0x00FF0000) >> 16);
			uint32_t result = 0xFF000000 | (0x00FF0000 & ((b1 + b2) << 8)) 
				| (0x0000FF00 & ((g1 + g2) << 0)) 
				| (0x000000FF & ((r1 + r2) >> 8));

			*destinationPixel = result;
			*depthPixel = depth;

			destinationPixel += 4;
			sourcePixel += 4;
			depthPixel += 1;
			countX -= 1;
		}

		y++;
		destinationPixel = a + stride;
		sourcePixel = b + source.stride;
		depthPixel = c + resX;
	}
}

void Surface::Draw(Surface &source, OSRectangle destinationRegion, OSRectangle sourceRegion, OSRectangle sourceBorderRegion,
		OSDrawMode mode, uint8_t alpha, bool alreadyLocked) {
	// Restriction: The source region must be within the bounds of the source.

	if (sourceRegion.left < 0 || sourceRegion.top < 0
			|| sourceRegion.right > (intptr_t) source.resX || sourceRegion.bottom > (intptr_t) source.resY
			|| sourceRegion.left >= sourceRegion.right || sourceRegion.top >= sourceRegion.bottom) {
		return;
	}

	// Restriction: The sourceBorderRegion region must be within the bounds of the source.

	if (sourceBorderRegion.left < 0 || sourceBorderRegion.top < 0
			|| sourceBorderRegion.right > (intptr_t) source.resX || sourceBorderRegion.bottom > (intptr_t) source.resY) {
		return;
	}

	// Acquire the lock (if we haven't already).

	if (!alreadyLocked) AcquireMutexPair(&mutex, &source.mutex);
	Defer(if (!alreadyLocked) ReleaseMutexPair(&mutex, &source.mutex));

	// Clip the destination region to the bounds of the destination.

	OSRectangle clipRegion = OS_MAKE_RECTANGLE(0, resX, 0, resY);
	OSClipDrawSourceRegion(&sourceRegion, &sourceBorderRegion, &destinationRegion, &clipRegion, mode);
	destinationRegion = clipRegion;

	// Work out the border region on the destination.

	OSRectangle destinationBorderRegion = OS_MAKE_RECTANGLE(destinationRegion.left + sourceBorderRegion.left - sourceRegion.left,
			destinationRegion.right + sourceBorderRegion.right - sourceRegion.right,
			destinationRegion.top + sourceBorderRegion.top - sourceRegion.top,
			destinationRegion.bottom + sourceBorderRegion.bottom - sourceRegion.bottom);

	// Can we replace OS_DRAW_MODE_REPEAT_FIRST with FillRectangle?

	bool drawCenter = true;
	if (mode == OS_DRAW_MODE_REPEAT_FIRST && alpha == 0xFF) {
		uint32_t color = *((uint32_t *) (source.linearBuffer + sourceBorderRegion.left * 4 + sourceBorderRegion.top * source.stride));

		if ((color & 0xFF000000) == 0xFF000000) {
			drawCenter = false;
			FillRectangle(destinationBorderRegion, OSColor(color), true);
		}
	}

	// For every pixel...

	for (intptr_t y = destinationRegion.top; y < destinationRegion.bottom; y++) {
		intptr_t sy = y - destinationRegion.top + sourceRegion.top;
		bool verticalEdge = false;

		if (y >= destinationBorderRegion.bottom) {
			sy = sourceBorderRegion.bottom + y - destinationBorderRegion.bottom;
		} else if (y >= destinationBorderRegion.top) {
			verticalEdge = true;

			switch (mode) {
				case OS_DRAW_MODE_REPEAT: {
					sy = sourceBorderRegion.top + ((y - destinationBorderRegion.top) % (sourceBorderRegion.bottom - sourceBorderRegion.top));
				} break;

				case OS_DRAW_MODE_STRECH: {
					intptr_t sourceBorderSize = sourceBorderRegion.bottom - sourceBorderRegion.top;
					intptr_t destinationBorderSize = destinationBorderRegion.bottom - destinationBorderRegion.top;
					sy = (y - destinationBorderRegion.top) * sourceBorderSize / destinationBorderSize + sourceBorderRegion.top;
				} break;

				default: {
					sy = sourceBorderRegion.top;
				} break;
			}
		}

		InvalidateScanline(y, destinationRegion.left, destinationRegion.right);

		for (intptr_t x = destinationRegion.left; x < destinationRegion.right; x++) {
			intptr_t sx = x - destinationRegion.left + sourceRegion.left;

			if (x >= destinationBorderRegion.right) {
				sx = sourceBorderRegion.right + x - destinationBorderRegion.right;
			} else if (x >= destinationBorderRegion.left) {
				switch (mode) {
					case OS_DRAW_MODE_REPEAT: {
						sx = sourceBorderRegion.left + ((x - destinationBorderRegion.left) % (sourceBorderRegion.right - sourceBorderRegion.left));
					} break;

					case OS_DRAW_MODE_STRECH: {
						intptr_t sourceBorderSize = sourceBorderRegion.right - sourceBorderRegion.left;
						intptr_t destinationBorderSize = destinationBorderRegion.right - destinationBorderRegion.left;
						sx = (x - destinationBorderRegion.left) * sourceBorderSize / destinationBorderSize + sourceBorderRegion.left;
					} break;

					default: {
						sx = sourceBorderRegion.left;
					} break;
				}

				if (verticalEdge && !drawCenter) {
					x = destinationBorderRegion.right - 1;
					continue;
				}
			}

			// Alpha blend the destination and source.

			uint32_t *destinationPixel = (uint32_t *) (linearBuffer + x * 4 + y * stride);
			uint32_t *sourcePixel = (uint32_t *) (source.linearBuffer + sx * 4 + sy * source.stride);
			uint32_t modified = *sourcePixel;

			if (alpha != 0xFF) {
				modified = (modified & 0xFFFFFF) | (((((modified & 0xFF000000) >> 24) * alpha) << 16) & 0xFF000000);
			}

			if ((modified & 0xFF000000) == 0xFF000000) {
				*destinationPixel = modified;
			} else {
				uint32_t original = *destinationPixel;
				uint32_t alpha1 = (modified & 0xFF000000) >> 24;
				uint32_t alpha2 = 255 - alpha1;
				uint32_t r2 = alpha2 * ((original & 0x000000FF) >> 0);
				uint32_t g2 = alpha2 * ((original & 0x0000FF00) >> 8);
				uint32_t b2 = alpha2 * ((original & 0x00FF0000) >> 16);
				uint32_t r1 = alpha1 * ((modified & 0x000000FF) >> 0);
				uint32_t g1 = alpha1 * ((modified & 0x0000FF00) >> 8);
				uint32_t b1 = alpha1 * ((modified & 0x00FF0000) >> 16);
				uint32_t result = 0xFF000000 | (0x00FF0000 & ((b1 + b2) << 8)) 
							     | (0x0000FF00 & ((g1 + g2) << 0)) 
							     | (0x000000FF & ((r1 + r2) >> 8));
				*destinationPixel = result;
			}
		}
	}
}

void Surface::FillRectangle(OSRectangle region, OSColor color, bool alreadyLocked) {
	if (region.left < 0 || region.top < 0
			|| region.right > (intptr_t) resX || region.bottom > (intptr_t) resY
			|| region.left >= region.right || region.top >= region.bottom) {
		// If the destination region was invalid, don't perform the operation.
		return;
	}

	if (!alreadyLocked) mutex.Acquire();
	Defer(if (!alreadyLocked) mutex.Release());

	uint8_t *destinationPixel = linearBuffer;

	destinationPixel += region.left * 4;
	destinationPixel += region.top * stride;

	uint8_t red = color.red;
	uint8_t green = color.green;
	uint8_t blue = color.blue;

	__m128i w = _mm_set_epi8(0xFF, red, green, blue,
	 			 0xFF, red, green, blue,
				 0xFF, red, green, blue,
				 0xFF, red, green, blue);

	for (intptr_t y = region.top; y < region.bottom; y++) {
		uintptr_t remainingX = region.right - region.left;
		uint8_t *scanlineStart = destinationPixel;

		InvalidateScanline(y, region.left, region.right);

		while (remainingX >= 4) {
			_mm_storeu_si128((__m128i *) destinationPixel, w);
			destinationPixel += 16;
			remainingX -= 4;
		}

		while (remainingX) {
			destinationPixel[0] = color.blue;
			destinationPixel[1] = color.green;
			destinationPixel[2] = color.red;
			destinationPixel[3] = 0xFF;
			destinationPixel += 4;
			remainingX--;
		}

		destinationPixel = scanlineStart + 4 * resX;
	}
}

void Surface::Destroy() {
	CloseHandleToObject(region, KERNEL_OBJECT_SHMEM, 0);
	kernelVMM.Free(memory);
	OSHeapFree(this);
}

#endif
