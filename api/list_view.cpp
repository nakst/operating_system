#ifndef IMPLEMENTATION

#define LIST_VIEW_TEXT_MARGIN (6)
#define LIST_VIEW_DEFAULT_ROW_HEIGHT (21)
#define LIST_VIEW_HEADER_HEIGHT (25)

// TODO Keyboard controls.
// TODO Right-click.
// TODO Custom controls.
// TODO Dragging.
// TODO Tile view.
// TODO More efficient repositioning on scroll when delta > contentHeight.

uint32_t LIST_VIEW_COLUMN_TEXT_COLOR = 0x4D6278;
uint32_t LIST_VIEW_PRIMARY_TEXT_COLOR = 0x000000;
uint32_t LIST_VIEW_SECONDARY_TEXT_COLOR = 0x686868;
uint32_t LIST_VIEW_BACKGROUND_COLOR = 0xFFFFFFFF;

struct ListViewItem {
	bool repaint;
	int height;
	uintptr_t index;
};

struct ListView : Control {
	unsigned flags;

	ListViewItem *visibleItems;
	size_t visibleItemCount, visibleItemAllocated;
	uintptr_t firstVisibleItem;
	int offsetIntoFirstVisibleItem;
	size_t itemCount;

	OSListViewColumn *columns;
	size_t columnCount;
	bool repaintColumns;

	int totalX, totalY;
	int scrollX, scrollY;
	Scrollbar *scrollbarX, *scrollbarY;
	bool showScrollbarX, showScrollbarY;

	uintptr_t highlightItem, 
		  focusedItem,
		  shiftAnchorItem,
		  highlightColumn,
		  highlightSplitter;

	OSRectangle layoutClip;
	OSRectangle margin;
	OSRectangle oldBounds;

	OSPoint dragAnchor;
	OSRectangle selectionBox;

	bool draggingSelectionBox,
	     draggingSplitter,
	     ctrlHeldInLastLeftClick,
	     pressingColumnHeader;

	bool receivedLayout;

	int constantHeight;
};

static UIImage listViewHighlight           = {{228, 241, 59, 72}, {228 + 3, 241 - 3, 59 + 3, 72 - 3}, OS_DRAW_MODE_STRECH};
static UIImage listViewLastClickedHighlight= {{228, 241, 14 + 59, 14 + 72}, {228 + 3, 241 - 3, 14 + 59 + 3, 14 + 72 - 3}, OS_DRAW_MODE_STRECH};
static UIImage listViewSelected            = {{14 + 228, 14 + 241, 59, 72}, {14 + 228 + 3, 14 + 241 - 3, 59 + 3, 72 - 3}, OS_DRAW_MODE_STRECH};
static UIImage listViewSelected2           = {{14 + 228, 14 + 241, 28 + 59, 28 + 72}, {14 + 228 + 3, 14 + 241 - 3, 28 + 59 + 3, 28 + 72 - 3}, OS_DRAW_MODE_STRECH};
static UIImage listViewSelected3           = {{14 + 228, 14 + 241, 28 + 59 - 14, 28 + 72 - 14}, {14 + 228 + 3, 14 + 241 - 3, 28 + 59 + 3 - 14, 28 + 72 - 3 - 14}, OS_DRAW_MODE_STRECH};
static UIImage listViewSelected4           = {{14 + 228, 14 + 241, 14 + 28 + 59, 14 + 28 + 72}, {14 + 228 + 3, 14 + 241 - 3, 14 + 28 + 59 + 3, 14 + 28 + 72 - 3}, OS_DRAW_MODE_STRECH};
static UIImage listViewLastClicked         = {{14 + 228, 14 + 241, 59 - 14, 72 - 14}, {14 + 228 + 6, 14 + 228 + 7, 59 + 6 - 14, 59 + 7 - 14}};
static UIImage listViewSelectionBox        = {{14 + 228 - 14, 14 + 231 - 14, 42 + 59 - 14, 42 + 62 - 14}, {14 + 228 + 1 - 14, 14 + 228 + 2 - 14, 42 + 59 + 1 - 14, 42 + 59 + 2 - 14}};
static UIImage listViewColumnHeaderDivider = {{212, 213, 45, 70}, {212, 212, 45, 45}};
static UIImage listViewColumnHeader        = {{206, 212, 45, 70}, {206, 212, 45, 70}, OS_DRAW_MODE_STRECH};
static UIImage listViewColumnHeaderHover   = {{7 + 206, 7 + 212, 45, 70}, {1 + 7 + 206, 7 + 212 - 1, 45, 70}, OS_DRAW_MODE_STRECH};
static UIImage listViewColumnHeaderPressed = {{14 + 206, 14 + 212, 45, 70}, {1 + 14 + 206, 14 + 212 - 1, 45, 70}, OS_DRAW_MODE_STRECH};
static UIImage listViewBorder		   = {{237, 240, 87, 90}, {238, 239, 88, 89}};

#else

static OSRectangle ListViewGetContentBounds(ListView *list) {
	if (!list->receivedLayout) {
		return OS_MAKE_RECTANGLE_ALL(0);
	}

	OSRectangle contentBounds = list->bounds;

	if (list->flags & OS_CREATE_LIST_VIEW_BORDER) {
		contentBounds.left++;
		contentBounds.right--;
		contentBounds.top++;
		contentBounds.bottom--;
	}

	if (list->columns) {
		contentBounds.top += LIST_VIEW_HEADER_HEIGHT;
	}

	if (list->showScrollbarX) {
		contentBounds.bottom -= SCROLLBAR_SIZE;
	}

	if (list->showScrollbarY) {
		contentBounds.right -= SCROLLBAR_SIZE;
	}

	return contentBounds;
}

static OSRectangle ListViewGetHeaderBounds(ListView *list) {
	if (!list->receivedLayout) {
		return OS_MAKE_RECTANGLE_ALL(0);
	}

	OSRectangle headerBounds = list->bounds;

	if (list->flags & OS_CREATE_LIST_VIEW_BORDER) {
		headerBounds.left++;
		headerBounds.right--;
		headerBounds.top++;
		headerBounds.bottom--;
	}

	headerBounds.bottom = headerBounds.top + LIST_VIEW_HEADER_HEIGHT;

	return headerBounds;
}

static ListViewItem *ListViewInsertVisibleItem(ListView *list, uintptr_t index) {
	index -= list->firstVisibleItem;

	if (list->visibleItemCount + 1 > list->visibleItemAllocated) {
		list->visibleItemAllocated = list->visibleItemAllocated * 2 + 32;
		ListViewItem *old = list->visibleItems;
		list->visibleItems = (ListViewItem *) OSHeapAllocate(sizeof(ListViewItem) * list->visibleItemAllocated, false);
		OSCopyMemory(list->visibleItems, old, sizeof(ListViewItem) * list->visibleItemCount);
		OSHeapFree(old);
	}

	OSMoveMemory(list->visibleItems + index, list->visibleItems + list->visibleItemCount, sizeof(ListViewItem), true);
	list->visibleItemCount++;
	list->visibleItems[index].index = index + list->firstVisibleItem;
	return list->visibleItems + index;
}

static void ListViewInsertItemsIntoVisibleItemsList(ListView *list, uintptr_t index, size_t count) {
	int offsetIntoViewport = -list->offsetIntoFirstVisibleItem;
	OSRectangle contentBounds = ListViewGetContentBounds(list);

	for (uintptr_t i = list->firstVisibleItem; i < index; i++) {
		offsetIntoViewport += list->visibleItems[i - list->firstVisibleItem].height;

		if (offsetIntoViewport > contentBounds.bottom - contentBounds.top) {
			list->visibleItemCount = i - list->firstVisibleItem + 1;
			return;
		}
	}

	int remaining = contentBounds.bottom - contentBounds.top - offsetIntoViewport;

	if (!remaining) {
		return;
	}

	uintptr_t i;

	OSMessage m = {};
	m.type = OS_NOTIFICATION_GET_ITEM;
	m.listViewItem.mask = OS_LIST_VIEW_ITEM_HEIGHT;

	for (i = index; i < index + count && remaining > 0; i++) {
		ListViewItem *item = ListViewInsertVisibleItem(list, i);
		item->repaint = true;

		m.listViewItem.index = i;

		if (OSForwardMessage(list, list->notificationCallback, &m) != OS_CALLBACK_HANDLED) {
			OSCrashProcess(OS_FATAL_ERROR_MESSAGE_SHOULD_BE_HANDLED);
		}

		item->height = m.listViewItem.height != OS_LIST_VIEW_ITEM_HEIGHT_DEFAULT ? m.listViewItem.height : LIST_VIEW_DEFAULT_ROW_HEIGHT;
		remaining -= item->height;
	}

	list->visibleItemCount = i - list->firstVisibleItem;
}

static void ListViewUpdate(ListView *list) {
	bool hadScrollbarX = list->showScrollbarX;
	list->showScrollbarX = list->showScrollbarY = false;

	OSRectangle contentBounds = ListViewGetContentBounds(list);

	if (!contentBounds.right) {
		return;
	}

	bool dh = false, dv = false;

	if (list->totalX > contentBounds.right - contentBounds.left) {
		contentBounds.bottom -= SCROLLBAR_SIZE;
		dh = true;
	}

	if (list->totalY > contentBounds.bottom - contentBounds.top) {
		contentBounds.right -= SCROLLBAR_SIZE;
		dv = true;
	}

	if (list->totalX > contentBounds.right - contentBounds.left) {
		list->showScrollbarX = true;
		if (!dh) contentBounds.bottom -= SCROLLBAR_SIZE;
	}

	if (list->totalY > contentBounds.bottom - contentBounds.top) {
		list->showScrollbarY = true;
		if (!dv) contentBounds.right -= SCROLLBAR_SIZE;
	}

	OSMessage m;
	m.type = OS_MESSAGE_LAYOUT;
	m.layout.force = true;
	m.layout.clip = list->layoutClip;

	if (list->totalX > contentBounds.right - contentBounds.left) {
		m.layout.top = contentBounds.bottom;
		m.layout.bottom = contentBounds.bottom + SCROLLBAR_SIZE;
		m.layout.left = contentBounds.left;
		m.layout.right = contentBounds.right;

		OSSendMessage(list->scrollbarX, &m);
		OSSetScrollbarMeasurements(list->scrollbarX, list->totalX, contentBounds.right - contentBounds.left);
		OSSetScrollbarPosition(list->scrollbarX, list->scrollX, false);
	} else if (hadScrollbarX) {
		// If the horizontal scrollbar has been hidden, we need to add additional visible items at the end of the list.
		ListViewInsertItemsIntoVisibleItemsList(list, 
				list->firstVisibleItem + list->visibleItemCount, 
				list->itemCount - list->visibleItemCount - list->firstVisibleItem);
	}

	if (list->totalY > contentBounds.bottom - contentBounds.top) {
		m.layout.left = contentBounds.right;
		m.layout.right = contentBounds.right + SCROLLBAR_SIZE;
		m.layout.top = contentBounds.top;
		m.layout.bottom = contentBounds.bottom;

		OSSendMessage(list->scrollbarY, &m);
		OSSetScrollbarMeasurements(list->scrollbarY, list->totalY, contentBounds.bottom - contentBounds.top);
		OSSetScrollbarPosition(list->scrollbarY, list->scrollY, false);
	}
}

static void ListViewScrollVertically(ListView *list, int newScrollY) {
	RepaintControl(list);
	int oldScrollY = list->scrollY;
	int baseNewScrollY = newScrollY;

	if (oldScrollY < list->margin.top) {
		oldScrollY = 0;
	} else {
		oldScrollY -= list->margin.top;
	}

	if (newScrollY < list->margin.top) {
		newScrollY = 0;
	} else {
		newScrollY -= list->margin.top;
	}

	int delta = newScrollY - oldScrollY;
	if (delta < 0) delta = -delta;

	if (!delta) {
		list->scrollY = baseNewScrollY;
		return;
	}

	int constantHeight = list->constantHeight;

	OSMessage m = {};
	m.type = OS_NOTIFICATION_GET_ITEM;
	m.listViewItem.mask = OS_LIST_VIEW_ITEM_HEIGHT;

	OSRectangle contentBounds = ListViewGetContentBounds(list);

	if (delta > contentBounds.bottom - contentBounds.top) {
		// We're moving past at least an entire screen, so the contents of the list will completely change.

		list->visibleItemCount = 0;

		if (newScrollY) {
			if (list->flags & OS_CREATE_LIST_VIEW_CONSTANT_HEIGHT) {
				list->firstVisibleItem = newScrollY / constantHeight;
				list->offsetIntoFirstVisibleItem = newScrollY % constantHeight;
			} else {
				m.type = OS_NOTIFICATION_CONVERT_Y_TO_INDEX;
				m.convertYToIndex.y = newScrollY;

				if (OSForwardMessage(list, list->notificationCallback, &m) == OS_CALLBACK_NOT_HANDLED) {
					// Start from the beginning....

					int height = 0;
					m.type = OS_NOTIFICATION_GET_ITEM;
					m.listViewItem.mask = OS_LIST_VIEW_ITEM_HEIGHT;

					for (uintptr_t i = 0; i < list->itemCount; i++) {
						m.listViewItem.index = i;

						if (OSForwardMessage(list, list->notificationCallback, &m) == OS_CALLBACK_NOT_HANDLED) {
							OSCrashProcess(OS_FATAL_ERROR_MESSAGE_SHOULD_BE_HANDLED);
						}

						int startHeight = height;
						height += m.listViewItem.height != OS_LIST_VIEW_ITEM_HEIGHT_DEFAULT ? m.listViewItem.height : LIST_VIEW_DEFAULT_ROW_HEIGHT;

						if (height > newScrollY) {
							list->firstVisibleItem = i;
							list->offsetIntoFirstVisibleItem = newScrollY - startHeight;
							break;
						}
					}
				} else {
					list->firstVisibleItem = m.convertYToIndex.index;
					list->offsetIntoFirstVisibleItem = m.convertYToIndex.offset;
				}
			}
		} else {
			list->firstVisibleItem = 0;
			list->offsetIntoFirstVisibleItem = 0;
		}

		ListViewInsertItemsIntoVisibleItemsList(list, list->firstVisibleItem, list->itemCount - list->firstVisibleItem);
	} else if (oldScrollY < newScrollY) {
		// Move content up.

		delta += list->offsetIntoFirstVisibleItem;
		uintptr_t i;

		for (i = 0; i < list->visibleItemCount; i++) {
			if (list->visibleItems[i].height <= delta) {
				delta -= list->visibleItems[i].height;
			} else {
				break;
			}
		}

		list->offsetIntoFirstVisibleItem = delta;
		OSMoveMemory(list->visibleItems + i, list->visibleItems + list->visibleItemCount, -i * sizeof(ListViewItem), false);
		list->visibleItemCount -= i;
		list->firstVisibleItem += i;
		ListViewInsertItemsIntoVisibleItemsList(list, list->firstVisibleItem + list->visibleItemCount, list->itemCount - list->visibleItemCount - list->firstVisibleItem);
	} else if (oldScrollY > newScrollY) {
		// Move content down.

		if (delta < list->offsetIntoFirstVisibleItem) {
			list->offsetIntoFirstVisibleItem -= delta;
		} else {
			delta -= list->offsetIntoFirstVisibleItem;
			list->offsetIntoFirstVisibleItem = 0;

			while (delta > 0) {
				ListViewItem *item = ListViewInsertVisibleItem(list, list->firstVisibleItem);
				list->firstVisibleItem--;
				item->index--;

				int height = constantHeight;

				if (!height) {
					m.listViewItem.index = list->firstVisibleItem;
					OSForwardMessage(list, list->notificationCallback, &m);
					height = m.listViewItem.height;
					if (height == OS_LIST_VIEW_ITEM_HEIGHT_DEFAULT) height = LIST_VIEW_DEFAULT_ROW_HEIGHT;
				}

				item->repaint = true;
				item->height = height;

				if (delta < height) {
					list->offsetIntoFirstVisibleItem = height - delta;
					break;
				} else {
					delta -= height;
				}
			}

			int remaining = contentBounds.bottom - contentBounds.top;
			uintptr_t i = 0;

			while (remaining >= 0 && i < list->visibleItemCount) {
				remaining -= list->visibleItems[i].height;
				i++;
			}

			list->visibleItemCount = i;

			if (list->visibleItemCount + list->firstVisibleItem != list->itemCount) {
				list->visibleItemCount++;
			}
		}
	}

	list->scrollY = baseNewScrollY;
}

static void ListViewPaintCell(ListView *list, OSRectangle cellBounds, OSMessage *message, int row, int column, OSRectangle rowClip, OSListViewColumn *columnData) {
	OSRectangle textRegion = OS_MAKE_RECTANGLE(cellBounds.left + 4, cellBounds.right - 8, cellBounds.top + 2, cellBounds.bottom - 2);

	OSMessage m;

	m.type = OS_NOTIFICATION_PAINT_CELL;
	m.paintCell.index = row;
	m.paintCell.column = column;
	m.paintCell.surface = message->paint.surface;
	m.paintCell.clip = rowClip;
	m.paintCell.bounds = textRegion;

	if (OSForwardMessage(list, list->notificationCallback, &m) == OS_CALLBACK_HANDLED) {
		return;
	}

	bool hasIcon = columnData && (columnData->flags & OS_LIST_VIEW_COLUMN_ICON);

	m.type = OS_NOTIFICATION_GET_ITEM;
	m.listViewItem.mask = OS_LIST_VIEW_ITEM_TEXT | (hasIcon ? OS_LIST_VIEW_ITEM_ICON : 0);
	m.listViewItem.index = row;
	m.listViewItem.column = column;

	if (OSForwardMessage(list, list->notificationCallback, &m) != OS_CALLBACK_HANDLED) {
		OSCrashProcess(OS_FATAL_ERROR_MESSAGE_SHOULD_BE_HANDLED);
	}

	if (hasIcon) {
		OSRectangle region = textRegion;
		region.right = region.left + 16;
		textRegion.left += 20;

		uint16_t iconID = m.listViewItem.iconID;
		int h = (region.bottom - region.top) / 2 + region.top - 8;
		UIImage image = icons16[iconID];
		OSDrawSurfaceClipped(message->paint.surface, OS_SURFACE_UI_SHEET, 
				OS_MAKE_RECTANGLE(region.left, region.left + 16, h, h + 16),
				image.region, image.border, OS_DRAW_MODE_REPEAT_FIRST, 0xFF, rowClip);
	}

	OSString string;
	string.buffer = m.listViewItem.text;
	string.bytes = m.listViewItem.textBytes;

	DrawString(message->paint.surface, textRegion, &string, 
			((columnData ? (columnData->flags & OS_LIST_VIEW_COLUMN_RIGHT_ALIGNED) : column) 
				? OS_DRAW_STRING_HALIGN_RIGHT : OS_DRAW_STRING_HALIGN_LEFT) | OS_DRAW_STRING_VALIGN_CENTER,
			(columnData ? !(columnData->flags & OS_LIST_VIEW_COLUMN_PRIMARY) : column) 
				? LIST_VIEW_SECONDARY_TEXT_COLOR : LIST_VIEW_PRIMARY_TEXT_COLOR, -1, 0, 
			OS_MAKE_POINT(0, 0), nullptr, 0, 0, true, FONT_SIZE, fontRegular, rowClip, 0);
}

static bool ListViewGetBorderImage(ListView *list, uintptr_t i, UIImage *image) {
	if (i == list->itemCount) {
		return false;
	}

	bool selected = false;
	bool listFocus = list->window->focus == list;
	bool focus = list->focusedItem == i && listFocus;

	OSMessage m;
	m.type = OS_NOTIFICATION_GET_ITEM;
	m.listViewItem.index = i;
	m.listViewItem.mask = OS_LIST_VIEW_ITEM_SELECTED | OS_LIST_VIEW_ITEM_SELECTED_2;
	m.listViewItem.state = 0;

	if (OSForwardMessage(list, list->notificationCallback, &m) == OS_CALLBACK_HANDLED) {
		if (list->draggingSelectionBox) {
			if (list->ctrlHeldInLastLeftClick) {
				uint8_t s1 = (m.listViewItem.state & OS_LIST_VIEW_ITEM_SELECTED) ? 1 : 0;
				uint8_t s2 = (m.listViewItem.state & OS_LIST_VIEW_ITEM_SELECTED_2) ? 1 : 0;
				selected = s1 ^ s2;
			} else {
				selected = m.listViewItem.state & (OS_LIST_VIEW_ITEM_SELECTED | OS_LIST_VIEW_ITEM_SELECTED_2);
			}
		} else {
			selected = m.listViewItem.state & OS_LIST_VIEW_ITEM_SELECTED;
		}
	}

	if ((focus || list->highlightItem == i) && selected) {
		*image = !listFocus ? (list->highlightItem == i ? listViewSelected4 : listViewSelected2) : listViewSelected3;
	} else if (list->highlightItem == i) {
		*image = focus ? listViewLastClickedHighlight : listViewHighlight;
	} else if (selected) {
		*image = !listFocus ? listViewSelected2 : listViewSelected;
	} else if (focus) {
		*image = listViewLastClicked;
	} else {
		return false;
	}

	return true;
}

static void ListViewDrawSeparator(ListView *list, uintptr_t after, uintptr_t before, OSHandle surface, OSRectangle row, OSRectangle clip) {
	UIImage afterImage, beforeImage;
	bool afterFound = ListViewGetBorderImage(list, after, &afterImage);
	bool beforeFound = ListViewGetBorderImage(list, before, &beforeImage);

	if (afterFound) {
		afterImage.region.bottom = afterImage.region.top + 1;
		afterImage.border.top = afterImage.region.top;
		afterImage.border.bottom = afterImage.region.top;
	}

	if (beforeFound) {
		beforeImage.region.bottom = beforeImage.region.top + 1;
		beforeImage.border.top = beforeImage.region.top;
		beforeImage.border.bottom = beforeImage.region.top;
	}

	{
		OSRectangle r;
		ClipRectangle(row, clip, &r);
		OSFillRectangle(surface, r, OSColor(LIST_VIEW_BACKGROUND_COLOR));
	}

	if (afterFound && beforeFound) {
		OSDrawSurfaceClipped(surface, OS_SURFACE_UI_SHEET, row,
				beforeImage.region, beforeImage.border, beforeImage.drawMode, 0x80, clip);
		OSDrawSurfaceClipped(surface, OS_SURFACE_UI_SHEET, row,
				afterImage.region, afterImage.border, afterImage.drawMode, 0x80, clip);
	} else if (afterFound) {
		OSDrawSurfaceClipped(surface, OS_SURFACE_UI_SHEET, row,
				afterImage.region, afterImage.border, afterImage.drawMode, 0xFF, clip);
	} else if (beforeFound) {
		OSDrawSurfaceClipped(surface, OS_SURFACE_UI_SHEET, row,
				beforeImage.region, beforeImage.border, beforeImage.drawMode, 0xFF, clip);
	} 
}

static void ListViewPaint(ListView *list, OSMessage *message) {
	OSRectangle contentBounds = ListViewGetContentBounds(list);
	OSRectangle contentClip;

	OSRectangle headerBounds = ListViewGetHeaderBounds(list);
	OSRectangle headerClip;

	if (!list->repaintCustomOnly && (list->flags & OS_CREATE_LIST_VIEW_BORDER)) {
		OSDrawSurfaceClipped(message->paint.surface, OS_SURFACE_UI_SHEET, list->bounds,
				listViewBorder.region, listViewBorder.border, listViewBorder.drawMode, 0xFF, message->paint.clip);
	} else if (!list->repaintCustomOnly && list->showScrollbarX && list->showScrollbarY) {
		OSFillRectangle(message->paint.surface, list->bounds, OSColor(STANDARD_BACKGROUND_COLOR));
	}

	if (ClipRectangle(headerBounds, message->paint.clip, &headerClip)
			&& list->columns && (!list->repaintCustomOnly || list->repaintColumns)) {
		list->repaintColumns = false;

		OSDrawSurfaceClipped(message->paint.surface, OS_SURFACE_UI_SHEET, headerBounds,
				listViewColumnHeader.region, listViewColumnHeader.border, listViewColumnHeader.drawMode, 0xFF, headerClip);

		int x = list->margin.left + headerBounds.left - list->scrollX;

		for (uintptr_t i = 0; i < list->columnCount; i++) {
			OSListViewColumn *column = list->columns + i;

			OSRectangle columnBounds = OS_MAKE_RECTANGLE(x, x + column->width, headerBounds.top, headerBounds.bottom);
			OSRectangle dividerPosition = OS_MAKE_RECTANGLE(x + column->width - 4, x + column->width - 3, headerBounds.top, headerBounds.bottom);

			OSRectangle textRegion = OS_MAKE_RECTANGLE(
					columnBounds.left + 4, columnBounds.right - 4, columnBounds.top + 2, columnBounds.bottom - 1);

			OSDrawSurfaceClipped(message->paint.surface, OS_SURFACE_UI_SHEET, dividerPosition,
					listViewColumnHeaderDivider.region, listViewColumnHeaderDivider.border, listViewColumnHeaderDivider.drawMode, 0xFF, headerClip);

			if (i == list->highlightColumn) {
				UIImage image = list->pressingColumnHeader ? listViewColumnHeaderPressed : listViewColumnHeaderHover;
				OSDrawSurfaceClipped(message->paint.surface, OS_SURFACE_UI_SHEET, 
						OS_MAKE_RECTANGLE(columnBounds.left - 4, columnBounds.right - 3, columnBounds.top, columnBounds.bottom),
						image.region, image.border, image.drawMode, 0xFF, headerClip);
			}

			OSString string; 
			string.buffer = column->title;
			string.bytes = column->titleBytes;

			DrawString(message->paint.surface, textRegion, &string, 
					OS_DRAW_STRING_HALIGN_LEFT | OS_DRAW_STRING_VALIGN_CENTER,
					LIST_VIEW_COLUMN_TEXT_COLOR, -1, 0, 
					OS_MAKE_POINT(0, 0), nullptr, 0, 0, true, FONT_SIZE, fontRegular, headerClip, 0);

			if (column->flags & OS_LIST_VIEW_COLUMN_SORT_ASCENDING) {
				OSDrawSurfaceClipped(message->paint.surface, OS_SURFACE_UI_SHEET, 
						OS_MAKE_RECTANGLE(columnBounds.right - 18, columnBounds.right - 9, columnBounds.top + 3, columnBounds.top + 12),
						smallArrowUpNormal.region, smallArrowUpNormal.border, smallArrowUpNormal.drawMode, 0x80, headerClip);
			} else if (column->flags & OS_LIST_VIEW_COLUMN_SORT_DESCENDING) {
				OSDrawSurfaceClipped(message->paint.surface, OS_SURFACE_UI_SHEET, 
						OS_MAKE_RECTANGLE(columnBounds.right - 18, columnBounds.right - 9, columnBounds.top + 3, columnBounds.top + 12),
						smallArrowDownNormal.region, smallArrowDownNormal.border, smallArrowDownNormal.drawMode, 0x80, headerClip);
			}

			x += column->width;
		}
	}

	if (ClipRectangle(contentBounds, message->paint.clip, &contentClip)) {
		if (!list->repaintCustomOnly && !(list->flags & OS_CREATE_LIST_VIEW_BORDER)) {
			OSFillRectangle(message->paint.surface, contentClip, OSColor(LIST_VIEW_BACKGROUND_COLOR));
		}

		int y = -list->offsetIntoFirstVisibleItem;
		int marginOffset = list->scrollY < list->margin.top ? (list->margin.top - list->scrollY) : 0;

		ListViewItem *item;

		OSRectangle row;

		if (list->columns) {
			row = OS_MAKE_RECTANGLE(
					contentBounds.left + list->margin.left - list->scrollX, 
					contentBounds.left + list->totalX - list->margin.right - list->scrollX,
					0, 0);
		} else {
			row = OS_MAKE_RECTANGLE(
					contentBounds.left + list->margin.left, 
					contentBounds.right - list->margin.right,
					0, 0);
		}

		for (uintptr_t i = list->firstVisibleItem; i < list->firstVisibleItem + list->visibleItemCount; i++, y += item->height) {
			item = list->visibleItems + (i - list->firstVisibleItem);

			if (!item->repaint && list->repaintCustomOnly
					&& (i == list->firstVisibleItem || !(item - 1)->repaint)) {
				continue;
			}

			OSMessage m = {};

			row.top = contentBounds.top + marginOffset + y;
			row.bottom = contentBounds.top + marginOffset + y + item->height;

			ListViewDrawSeparator(list, i == 0 ? list->itemCount : i - 1, i, message->paint.surface, 
					OS_MAKE_RECTANGLE(row.left - 3, row.right + 3, row.top, row.top + 1), contentClip);

			if (!item->repaint && list->repaintCustomOnly) {
				continue;
			}

			OSRectangle rowClip;
			ClipRectangle(contentClip, row, &rowClip);

			OSRectangle row2 = OS_MAKE_RECTANGLE(row.left - 3, row.right + 3, row.top + 1, row.bottom);
			OSRectangle row2Clip;
			ClipRectangle(contentClip, row2, &row2Clip);
			OSFillRectangle(message->paint.surface, row2Clip, OSColor(LIST_VIEW_BACKGROUND_COLOR));

			{
				UIImage image;
				bool found = ListViewGetBorderImage(list, i, &image);

				if (found) {
					image.region.top++;
					image.region.bottom--;
				
					OSDrawSurfaceClipped(message->paint.surface, OS_SURFACE_UI_SHEET, row2,
							image.region, image.border, image.drawMode, 0xFF, contentClip);
				} 
			}

			m.type = OS_NOTIFICATION_PAINT_ITEM;
			m.paintItem.index = i;
			m.paintItem.surface = message->paint.surface;
			m.paintItem.clip = rowClip;
			m.paintItem.bounds = row;

			OSCallbackResponse response = OSForwardMessage(list, list->notificationCallback, &m);
			
			if (response == OS_CALLBACK_NOT_HANDLED) {
				if (list->columns) {
					int x = row.left;

					for (uintptr_t j = 0; j < list->columnCount; j++) {
						OSListViewColumn *column = list->columns + j;

						OSRectangle cellBounds = OS_MAKE_RECTANGLE(x, x + column->width, row.top, row.bottom);
						ListViewPaintCell(list, cellBounds, message, i, j, rowClip, column);

						x += column->width;
					}
				} else {
					ListViewPaintCell(list, row, message, i, 0, rowClip, nullptr);
				}
			}

			if (i == list->itemCount - 1) {
				ListViewDrawSeparator(list, i, list->itemCount, message->paint.surface, 
						OS_MAKE_RECTANGLE(row2.left, row2.right, row.bottom, row.bottom + 1), contentClip);
			}
		}

		for (uintptr_t i = 0; i < list->visibleItemCount; i++) {
			list->visibleItems[i].repaint = false;
		}

		if (list->draggingSelectionBox) {
			OSDrawSurfaceClipped(message->paint.surface, OS_SURFACE_UI_SHEET, 
					OS_MAKE_RECTANGLE(list->selectionBox.left,
						list->selectionBox.right,
						list->selectionBox.top,
						list->selectionBox.bottom),
					listViewSelectionBox.region, listViewSelectionBox.border, listViewSelectionBox.drawMode, 0xFF, contentClip);
		}
	}
}

static void ListViewSetRange(ListView *list, int from, int to, uint16_t mask, uint16_t state) {
	if (from >= to) return;

	if (from < 0) from = 0; 
	if (from >= (int) list->itemCount) from = list->itemCount;
	if (to < 0) to = 0; 
	if (to >= (int) list->itemCount) to = list->itemCount;

	OSMessage m;

	m.type = OS_NOTIFICATION_SET_ITEM_RANGE;
	m.listViewItemRange.indexFrom = from;
	m.listViewItemRange.indexTo = to;
	m.listViewItemRange.mask = mask;
	m.listViewItemRange.state = state;

	if (OSForwardMessage(list, list->notificationCallback, &m) != OS_CALLBACK_NOT_HANDLED) {
		return;
	}

	m.type = OS_NOTIFICATION_SET_ITEM;
	m.listViewItem.mask = mask;
	m.listViewItem.state = state;

	for (int i = from; i < to; i++) {
		m.listViewItem.index = i;

		if (OSForwardMessage(list, list->notificationCallback, &m) == OS_CALLBACK_REJECTED) {
			return;
		}
	}
}

static void ListViewSelectionBoxChanged(ListView *list, OSRectangle oldBox, OSRectangle newBox, bool moveS2ToS) {
	OSRectangle contentBounds = ListViewGetContentBounds(list);

	int rowLeft = list->columns ? contentBounds.left + list->margin.left - list->scrollX : contentBounds.left + list->margin.left;
	int rowRight = list->columns ? contentBounds.left + list->totalX - list->margin.right - list->scrollX : contentBounds.right - list->margin.right;

	int constantHeight = list->constantHeight;

	int y = contentBounds.top - (list->firstVisibleItem * constantHeight + list->offsetIntoFirstVisibleItem);
	y += list->scrollY < list->margin.top ? (list->margin.top - list->scrollY) : 0;

	int fromNew = 0;
	int toNew = -1;
	int fromOld = 0;
	int toOld = -1;

	if (ClipRectangle(OS_MAKE_RECTANGLE(rowLeft, rowRight, oldBox.top, oldBox.bottom), oldBox, nullptr)) {
		fromOld = (oldBox.top - y) / constantHeight + list->firstVisibleItem;
		toOld = (oldBox.bottom - y) / constantHeight + list->firstVisibleItem;
	}

	if (ClipRectangle(OS_MAKE_RECTANGLE(rowLeft, rowRight, newBox.top, newBox.bottom), newBox, nullptr)) {
		fromNew = (newBox.top - y) / constantHeight + list->firstVisibleItem;
		toNew = (newBox.bottom - y) / constantHeight + list->firstVisibleItem;
	}

	OSMessage m = {};

	if (moveS2ToS) {
		if (list->ctrlHeldInLastLeftClick) {
			if (fromNew < 0) fromNew = 0; 
			if (fromNew >= (int) list->itemCount) fromNew = list->itemCount - 1;
			if (toNew < 0) toNew = 0; 
			if (toNew >= (int) list->itemCount) toNew = list->itemCount - 1;

			m.listViewItem.mask = OS_LIST_VIEW_ITEM_SELECTED | OS_LIST_VIEW_ITEM_SELECTED_2;

			for (int i = fromNew; i <= toNew; i++) {
				if (i < 0) {
					continue;
				}

				if (i >= (int) list->itemCount) {
					break;
				}

				m.type = OS_NOTIFICATION_GET_ITEM;
				m.listViewItem.state = 0;
				m.listViewItem.index = i;
				if (OSForwardMessage(list, list->notificationCallback, &m) == OS_CALLBACK_REJECTED) break;

				uint8_t s1 = (m.listViewItem.state & OS_LIST_VIEW_ITEM_SELECTED) ? 1 : 0;
				uint8_t s2 = (m.listViewItem.state & OS_LIST_VIEW_ITEM_SELECTED_2) ? 1 : 0;
				uint8_t s3 = s1 ^ s2;

				m.type = OS_NOTIFICATION_SET_ITEM;
				m.listViewItem.state = s3 ? OS_LIST_VIEW_ITEM_SELECTED : 0;
				m.listViewItem.index = i;
				if (OSForwardMessage(list, list->notificationCallback, &m) == OS_CALLBACK_REJECTED) break;
			}
		} else {
			ListViewSetRange(list, fromNew, toNew + 1, OS_LIST_VIEW_ITEM_SELECTED | OS_LIST_VIEW_ITEM_SELECTED_2, OS_LIST_VIEW_ITEM_SELECTED);
		}

		return;
	}

	if (toNew == -1 && toOld == -1) {
	} else if (toNew == -1) {
		ListViewSetRange(list, fromOld, toOld + 1, OS_LIST_VIEW_ITEM_SELECTED_2, 0);
	} else if (toOld == -1) {
		ListViewSetRange(list, fromNew, toNew + 1, OS_LIST_VIEW_ITEM_SELECTED_2, OS_LIST_VIEW_ITEM_SELECTED_2);
	} else {
		if (fromNew < fromOld) {
			ListViewSetRange(list, fromNew, fromOld, OS_LIST_VIEW_ITEM_SELECTED_2, OS_LIST_VIEW_ITEM_SELECTED_2);
		} else {
			ListViewSetRange(list, fromOld, fromNew, OS_LIST_VIEW_ITEM_SELECTED_2, 0);
		}

		if (toNew > toOld) {
			ListViewSetRange(list, toOld + 1, toNew + 1, OS_LIST_VIEW_ITEM_SELECTED_2, OS_LIST_VIEW_ITEM_SELECTED_2);
		} else {
			ListViewSetRange(list, toNew + 1, toOld + 1, OS_LIST_VIEW_ITEM_SELECTED_2, 0);
		}
	}
}

static void ListViewMouseUpdated(ListView *list, OSMessage *message) {
	if (message->type == OS_MESSAGE_MOUSE_MOVED) {
		list->highlightItem = -1;
		if (!list->pressingColumnHeader) list->highlightColumn = -1;
		list->highlightSplitter = -1;
		list->window->cursor = OS_CURSOR_NORMAL;

		if (!IsPointInRectangle(list->inputBounds, message->mouseMoved.newPositionX, message->mouseMoved.newPositionY)) {
			return;
		}

		if (!list->window->hover) {
			list->window->hover = list;
			list->window->cursor = OS_CURSOR_NORMAL;
		}

		if (list->columns) {
			OSRectangle headerBounds = ListViewGetHeaderBounds(list);

			if (IsPointInRectangle(headerBounds, message->mouseMoved.newPositionX, message->mouseMoved.newPositionY)) {
				int x = list->margin.left + headerBounds.left - list->scrollX;

				for (uintptr_t i = 0; i < list->columnCount; i++) {
					OSListViewColumn *column = list->columns + i;

					OSRectangle columnBounds = OS_MAKE_RECTANGLE(x, x + column->width - 8, headerBounds.top, headerBounds.bottom);
					OSRectangle splitterBounds = OS_MAKE_RECTANGLE(x + column->width - 8, x + column->width, headerBounds.top, headerBounds.bottom);

					if (IsPointInRectangle(columnBounds, message->mouseMoved.newPositionX, message->mouseMoved.newPositionY)
							&& (list->flags & OS_CREATE_LIST_VIEW_SORTABLE)) {
						if (!list->pressingColumnHeader) list->highlightColumn = i;
						return;
					}

					if (IsPointInRectangle(splitterBounds, message->mouseMoved.newPositionX, message->mouseMoved.newPositionY)) {
						list->highlightSplitter = i;
						list->window->cursor = OS_CURSOR_SPLIT_HORIZONTAL;
						return;
					}

					x += column->width;
				}
			}
		}

		OSRectangle contentBounds = ListViewGetContentBounds(list);

		if (!IsPointInRectangle(contentBounds, message->mouseMoved.newPositionX, message->mouseMoved.newPositionY)) {
			return;
		}

		if (!(list->flags & OS_CREATE_LIST_VIEW_ANY_SELECTIONS)) {
			return;
		}

		int y = -list->offsetIntoFirstVisibleItem;
		int marginOffset = list->scrollY < list->margin.top ? (list->margin.top - list->scrollY) : 0;

		for (uintptr_t i = 0; i < list->visibleItemCount; i++)  {
			ListViewItem *item = list->visibleItems + i;

			OSRectangle row;

			if (list->columns) {
				row = OS_MAKE_RECTANGLE(
						contentBounds.left + list->margin.left - list->scrollX - 3, 
						contentBounds.left + list->totalX - list->margin.right - list->scrollX + 3,
						contentBounds.top + marginOffset + y,
						contentBounds.top + marginOffset + y + item->height);
			} else {
				row = OS_MAKE_RECTANGLE(
						contentBounds.left + list->margin.left - 3, 
						contentBounds.right - list->margin.right + 3,
						contentBounds.top + marginOffset + y,
						contentBounds.top + marginOffset + y + item->height);
			}

			if (IsPointInRectangle(row, message->mouseMoved.newPositionX, message->mouseMoved.newPositionY)) {
				list->highlightItem = i + list->firstVisibleItem;
				return;
			}

			y += item->height;
		}
	} else if (message->type == OS_MESSAGE_START_DRAG) {
		if (!(list->flags & OS_CREATE_LIST_VIEW_MULTI_SELECT)) {
			return;
		}

		OSRectangle contentBounds = ListViewGetContentBounds(list);

		list->dragAnchor = OS_MAKE_POINT(
				message->mouseDragged.originalPositionX, 
				message->mouseDragged.originalPositionY);

		if (!IsPointInRectangle(contentBounds, message->mouseMoved.newPositionX, message->mouseMoved.newPositionY)) {
			if (list->highlightSplitter != (uintptr_t) -1) {
				list->draggingSplitter = true;
				list->dragAnchor.x -= list->columns[list->highlightSplitter].width;
			} 
		} else {
			list->selectionBox = OS_MAKE_RECTANGLE_ALL(0);
			list->draggingSelectionBox = true;
			RepaintControl(list);
		}
	} else if (message->type == OS_MESSAGE_MOUSE_LEFT_RELEASED) {
		if (list->draggingSelectionBox) {
			ListViewSelectionBoxChanged(list, OS_MAKE_RECTANGLE_ALL(0), list->selectionBox, true);
		}

		if (list->pressingColumnHeader) {
			OSMessage m;
			m.type = OS_NOTIFICATION_SORT_COLUMN;
			m.listViewColumn.descending = false;

			for (uintptr_t i = 0; i < list->columnCount; i++) {
				if (i == list->highlightColumn) {
					if (list->columns[i].flags & OS_LIST_VIEW_COLUMN_SORT_ASCENDING) {
						list->columns[i].flags &= ~(OS_LIST_VIEW_COLUMN_SORT_ASCENDING | OS_LIST_VIEW_COLUMN_SORT_DESCENDING);
						list->columns[i].flags |= OS_LIST_VIEW_COLUMN_SORT_DESCENDING;
						m.listViewColumn.descending = true;
					} else {
						list->columns[i].flags &= ~(OS_LIST_VIEW_COLUMN_SORT_ASCENDING | OS_LIST_VIEW_COLUMN_SORT_DESCENDING);
						list->columns[i].flags |= OS_LIST_VIEW_COLUMN_SORT_ASCENDING;
					}
				} else {
					list->columns[i].flags &= ~(OS_LIST_VIEW_COLUMN_SORT_ASCENDING | OS_LIST_VIEW_COLUMN_SORT_DESCENDING);
				}
			}

			m.listViewColumn.index = list->highlightColumn;
			OSForwardMessage(list, list->notificationCallback, &m);
		}

		list->draggingSelectionBox = false;
		list->draggingSplitter = false;
		list->pressingColumnHeader = false;
		list->highlightColumn = -1;
		RepaintControl(list);
	} else if (message->type == OS_MESSAGE_MOUSE_DRAGGED) {
		if (!(list->flags & OS_CREATE_LIST_VIEW_MULTI_SELECT)) {
			return;
		}

		if (list->draggingSelectionBox) {
			OSPoint currentPoint = OS_MAKE_POINT(
					message->mouseDragged.newPositionX, 
					message->mouseDragged.newPositionY);

			OSRectangle previousSelectionBox = list->selectionBox;

			if (currentPoint.x < list->dragAnchor.x) {
				list->selectionBox.left = currentPoint.x;
				list->selectionBox.right = list->dragAnchor.x;
			} else {
				list->selectionBox.left = list->dragAnchor.x;
				list->selectionBox.right = currentPoint.x;
			}

			if (currentPoint.y < list->dragAnchor.y) {
				list->selectionBox.top = currentPoint.y;
				list->selectionBox.bottom = list->dragAnchor.y;
			} else {
				list->selectionBox.top = list->dragAnchor.y;
				list->selectionBox.bottom = currentPoint.y;
			}

			ListViewSelectionBoxChanged(list, previousSelectionBox, list->selectionBox, false);
		} else if (list->draggingSplitter) {
			OSListViewColumn *column = list->columns + list->highlightSplitter;
			column->width = message->mouseDragged.newPositionX - list->dragAnchor.x;

			if (column->width < column->minimumWidth) {
				column->width = column->minimumWidth;
			}
		}

		RepaintControl(list);
	} else if (message->type == OS_MESSAGE_MOUSE_LEFT_PRESSED) {
		list->ctrlHeldInLastLeftClick = message->mousePressed.ctrl;

		if (!(list->flags & OS_CREATE_LIST_VIEW_ANY_SELECTIONS)) {
			return;
		}

		OSMessage m;

		if (!message->mousePressed.ctrl || !(list->flags & OS_CREATE_LIST_VIEW_MULTI_SELECT)) {
			ListViewSetRange(list, 0, list->itemCount, OS_LIST_VIEW_ITEM_SELECTED, 0);
		}

		if (list->highlightItem != (uintptr_t) -1) {
			int from = (message->mousePressed.shift && (list->flags & OS_CREATE_LIST_VIEW_MULTI_SELECT)) ? list->shiftAnchorItem : list->highlightItem;
			int to = list->highlightItem;

			for (int i = from; from < to ? i <= to : i >= to; i += from < to ? 1 : -1) {
				if (i < 0) continue;
				if (i >= (int) list->itemCount) break;

				bool select = true;

				if (message->mousePressed.ctrl && !message->mousePressed.shift) {
					m.type = OS_NOTIFICATION_GET_ITEM;
					m.listViewItem.index = i;
					m.listViewItem.mask = OS_LIST_VIEW_ITEM_SELECTED;
					m.listViewItem.state = 0;

					if (OSForwardMessage(list, list->notificationCallback, &m) == OS_CALLBACK_HANDLED) {
						select = (m.listViewItem.state & OS_LIST_VIEW_ITEM_SELECTED) ? false : true;
					}
				}

				m.type = OS_NOTIFICATION_SET_ITEM;
				m.listViewItem.index = i;
				m.listViewItem.mask = OS_LIST_VIEW_ITEM_SELECTED;
				m.listViewItem.state = select ? OS_LIST_VIEW_ITEM_SELECTED : 0;
				OSForwardMessage(list, list->notificationCallback, &m);
			}

			list->focusedItem = to;

			if (!message->mousePressed.shift) {
				list->shiftAnchorItem = to;
			}

			if (message->mousePressed.clickChainCount == 2) { 
				m.type = OS_NOTIFICATION_CHOOSE_ITEM;
				OSForwardMessage(list, list->notificationCallback, &m);
			}
		} else if (list->highlightColumn != (uintptr_t) -1) {
			list->pressingColumnHeader = true;
			RepaintControl(list);
		}
	}
}

static OSCallbackResponse ProcessListViewMessage(OSObject listView, OSMessage *message) {
	ListView *list = (ListView *) listView;
	OSCallbackResponse response = OS_CALLBACK_NOT_HANDLED;
	bool redrawScrollbars = list->repaint;

	if (message->type == OS_MESSAGE_CHILD_UPDATED) {
		SetParentDescendentInvalidationFlags(list, DESCENDENT_RELAYOUT);
	} else if (message->type == OS_MESSAGE_PARENT_UPDATED) {
		OSSendMessage(list->scrollbarX, message);
		OSSendMessage(list->scrollbarY, message);
	} else if (message->type == OS_MESSAGE_DESTROY) {
		OSSendMessage(list->scrollbarX, message);
		OSSendMessage(list->scrollbarY, message);
		OSHeapFree(list->visibleItems);
	} else if (message->type == OS_MESSAGE_END_HOVER) {
		if (!list->pressingColumnHeader) list->highlightColumn = -1;
		list->highlightItem = -1;
	} else if (message->type == OS_MESSAGE_MOUSE_LEFT_PRESSED 
			|| message->type == OS_MESSAGE_MOUSE_DRAGGED
			|| message->type == OS_MESSAGE_MOUSE_LEFT_RELEASED
			|| message->type == OS_MESSAGE_START_DRAG) {
		ListViewMouseUpdated(list, message);
	} else if (message->type == OS_MESSAGE_MOUSE_MOVED) {
		if (list->showScrollbarX) OSSendMessage(list->scrollbarX, message);
		if (list->showScrollbarY) OSSendMessage(list->scrollbarY, message);

		uintptr_t oldHighlightColumn = list->highlightColumn;
		uintptr_t oldHighlightItem = list->highlightItem;

		response = OS_CALLBACK_HANDLED;
		ListViewMouseUpdated(list, message);

		bool repaint = false;

		if (oldHighlightItem != list->highlightItem) {
			if (oldHighlightItem >= list->firstVisibleItem && oldHighlightItem < list->firstVisibleItem + list->visibleItemCount) {
				ListViewItem *item = list->visibleItems + oldHighlightItem - list->firstVisibleItem;
				item->repaint = true;
				repaint = true;
			}

			if (list->highlightItem >= list->firstVisibleItem && list->highlightItem < list->firstVisibleItem + list->visibleItemCount) {
				ListViewItem *item = list->visibleItems + list->highlightItem - list->firstVisibleItem;
				item->repaint = true;
				repaint = true;
			}
		}

		if (oldHighlightColumn != list->highlightColumn) {
			list->repaintColumns = true;
			repaint = true;
		}

		if (repaint) {
			RepaintControl(list, true);
		}
	} else if (message->type == OS_MESSAGE_CUSTOM_PAINT) {
		ListViewPaint(list, message);
		response = OS_CALLBACK_HANDLED;
	}

	if (response == OS_CALLBACK_NOT_HANDLED) {
		response = ProcessControlMessage(listView, message);
	}

	if (message->type == OS_MESSAGE_LAYOUT) {
		list->layoutClip = message->layout.clip;
		list->receivedLayout = true;

		if (list->oldBounds.left == list->bounds.left && 
				list->oldBounds.right == list->bounds.right && 
				list->oldBounds.top == list->bounds.top && 
				list->oldBounds.bottom == list->bounds.bottom) {
			goto done;
		} else {
			list->oldBounds = list->bounds;
		}

		ListViewInsertItemsIntoVisibleItemsList(list, list->firstVisibleItem + list->visibleItemCount, list->itemCount - list->visibleItemCount - list->firstVisibleItem);

		{
			int height = -list->offsetIntoFirstVisibleItem;

			for (uintptr_t i = 0; i < list->visibleItemCount; i++) {
				height += list->visibleItems[i].height;
			}

			OSRectangle contentBounds = ListViewGetContentBounds(list);

			if (height < contentBounds.bottom - contentBounds.top) {
				int newScrollY = list->scrollY - (contentBounds.bottom - contentBounds.top - height) + list->margin.bottom;

				if (newScrollY >= 0) {
					ListViewScrollVertically(list, newScrollY);
				} else {
					ListViewScrollVertically(list, 0);
				}
			}

			if (list->totalX - list->scrollX < contentBounds.right - contentBounds.left) {
				list->scrollX = list->totalX - contentBounds.right + contentBounds.left;

				if (list->scrollX < 0) {
					list->scrollX = 0;
				}
			}
		}

		done:;
		ListViewUpdate(list);
	} else if (message->type == OS_MESSAGE_PAINT) {
		if ((list->descendentInvalidationFlags & DESCENDENT_REPAINT) || message->paint.force || redrawScrollbars) {
			list->descendentInvalidationFlags &= ~DESCENDENT_REPAINT;

			OSMessage m = *message;
			m.paint.force = message->paint.force || redrawScrollbars;
			if (list->showScrollbarX) OSSendMessage(list->scrollbarX, &m);
			if (list->showScrollbarY) OSSendMessage(list->scrollbarY, &m);
		}
	} 

#if 0
	for (uintptr_t i = 0; i < list->visibleItemCount; i++) {
		if (list->visibleItems[i].index != list->firstVisibleItem + i) {
			OSCrashProcess(OS_FATAL_ERROR_UNKNOWN);
		}
	}
#endif

	return response;
}

static OSCallbackResponse ListViewScrollbarXMoved(OSObject scrollbar, OSMessage *message) {
	(void) scrollbar;
	ListView *list = (ListView *) message->context;

	if (message->type == OS_NOTIFICATION_VALUE_CHANGED) {
		RepaintControl(list);
		list->scrollX = message->valueChanged.newValue;
		return OS_CALLBACK_HANDLED;
	}

	return OS_CALLBACK_NOT_HANDLED;
}

static OSCallbackResponse ListViewScrollbarYMoved(OSObject scrollbar, OSMessage *message) {
	(void) scrollbar;
	ListView *list = (ListView *) message->context;

	if (message->type == OS_NOTIFICATION_VALUE_CHANGED) {
		int newScrollY = message->valueChanged.newValue;
		ListViewScrollVertically(list, newScrollY);
		return OS_CALLBACK_HANDLED;
	}

	return OS_CALLBACK_NOT_HANDLED;
}

OSObject OSCreateListView(unsigned flags, int constantHeight) {
	ListView *list = (ListView *) GUIAllocate(sizeof(ListView), true);

	list->type = API_OBJECT_CONTROL;
	list->flags = flags;

	list->tabStop = true;
	list->ignoreActivationClicks = true;
	list->noAnimations = true;
	list->focusable = true;

	list->preferredWidth = 80;
	list->preferredHeight = 80;

	OSSetCallback(list, OS_MAKE_CALLBACK(ProcessListViewMessage, nullptr));

	list->scrollbarX = (Scrollbar *) OSCreateScrollbar(OS_ORIENTATION_HORIZONTAL, false);
	list->scrollbarY = (Scrollbar *) OSCreateScrollbar(OS_ORIENTATION_VERTICAL, false);

	list->scrollbarX->parent = list;
	list->scrollbarX->layout = OS_CELL_FILL;
	list->scrollbarY->parent = list;
	list->scrollbarY->layout = OS_CELL_FILL;

	bool smallMargins = flags & OS_CREATE_LIST_VIEW_BORDER;
	list->margin.left = smallMargins ? 7 : 15;
	list->margin.right = smallMargins ? 7 : 15;
	list->margin.top = smallMargins ? 4 : 7;
	list->margin.bottom = smallMargins ? 4 : 7;

	list->totalX = list->margin.left + list->margin.right;
	list->totalY = list->margin.top + list->margin.bottom;

	list->highlightItem = -1;
	list->highlightColumn = -1;
	list->focusedItem = -1;

	list->constantHeight = constantHeight == OS_LIST_VIEW_ITEM_HEIGHT_DEFAULT ? LIST_VIEW_DEFAULT_ROW_HEIGHT : constantHeight;

	OSSetObjectNotificationCallback(list->scrollbarX, OS_MAKE_CALLBACK(ListViewScrollbarXMoved, list));
	OSSetObjectNotificationCallback(list->scrollbarY, OS_MAKE_CALLBACK(ListViewScrollbarYMoved, list));

	return list;
}

void OSListViewInsert(OSObject listView, uintptr_t index, size_t count) {
	ListView *list = (ListView *) listView;
	list->itemCount += count;

	for (uintptr_t i = 0; i < list->visibleItemCount; i++) {
		if (list->visibleItems[i].index >= index) {
			list->visibleItems[i].index += count;
		}
	}

	int itemsHeight = 0;

	OSMessage m = {};
	m.type = OS_NOTIFICATION_GET_ITEM;
	m.listViewItem.mask = OS_LIST_VIEW_ITEM_HEIGHT;

	if (list->flags & OS_CREATE_LIST_VIEW_CONSTANT_HEIGHT) {
		itemsHeight = count * list->constantHeight;
	} else {
		m.type = OS_NOTIFICATION_MEASURE_HEIGHT;
		m.measureHeight.fromIndex = index;
		m.measureHeight.toIndex = index + count;

		if (OSForwardMessage(list, list->notificationCallback, &m) == OS_CALLBACK_HANDLED) {
			itemsHeight = m.measureHeight.height;
		} else {
			m.type = OS_NOTIFICATION_GET_ITEM;
			m.listViewItem.mask = OS_LIST_VIEW_ITEM_HEIGHT;

			for (uintptr_t i = 0; i < count; i++) {
				m.listViewItem.index = index + i;

				if (OSForwardMessage(list, list->notificationCallback, &m) != OS_CALLBACK_HANDLED) {
					OSCrashProcess(OS_FATAL_ERROR_MESSAGE_SHOULD_BE_HANDLED);
				}

				itemsHeight += (m.listViewItem.height != OS_LIST_VIEW_ITEM_HEIGHT_DEFAULT ? m.listViewItem.height : LIST_VIEW_DEFAULT_ROW_HEIGHT);
			}
		}
	}

	list->totalY += itemsHeight;

	if (index <= list->firstVisibleItem && list->itemCount != count) {
		list->firstVisibleItem += index;
		list->scrollY += itemsHeight;
	} else if (index < list->firstVisibleItem + list->visibleItemCount || list->itemCount == count) {
		ListViewInsertItemsIntoVisibleItemsList(list, index, count);
	}

	ListViewInsertItemsIntoVisibleItemsList(list, list->firstVisibleItem + list->visibleItemCount, list->itemCount - list->visibleItemCount - list->firstVisibleItem);

	RepaintControl(list, true);
	ListViewUpdate(list);
}

void OSListViewRemove(OSObject listView, uintptr_t index, size_t count) {
	ListView *list = (ListView *) listView;
	(void) list;
	(void) index;
	(void) count;
	// TODO
}

void OSListViewReset(OSObject listView) {
	ListView *list = (ListView *) listView;
	list->visibleItemCount = 0;
	list->firstVisibleItem = 0;
	list->offsetIntoFirstVisibleItem = 0;
	list->itemCount = 0;
	list->scrollX = 0;
	list->scrollY = 0;
	list->totalY = list->margin.top + list->margin.bottom;
	list->highlightItem = -1;
	list->highlightColumn = -1;
	list->focusedItem = -1;
	RepaintControl(list);
}

void OSListViewInvalidate(OSObject listView, uintptr_t index, size_t count) {
	ListView *list = (ListView *) listView;

	if (index + count > list->itemCount) {
		OSCrashProcess(OS_FATAL_ERROR_INDEX_OUT_OF_BOUNDS);
	}

	if (index < list->firstVisibleItem) {
		count -= list->firstVisibleItem - index;
		index = list->firstVisibleItem;
	}

	if (index > list->firstVisibleItem + list->visibleItemCount) {
		// None of the invalidated items are visible.
		return;
	}

	for (uintptr_t i = index; i < index + count && i < list->firstVisibleItem + list->visibleItemCount; i++) {
		list->visibleItems[i].repaint = true;
	}

	RepaintControl(list, true);
}

void OSListViewSetColumns(OSObject listView, OSListViewColumn *columns, size_t count) {
	ListView *list = (ListView *) listView;
	list->columns = columns;
	list->columnCount = count;
	list->totalX = list->margin.left + list->margin.right;

	for (uintptr_t i = 0; i < count; i++) {
		list->totalX += columns[i].width;
	}

	RepaintControl(list);
	ListViewUpdate(list);
}

#endif
