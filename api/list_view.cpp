#ifndef IMPLEMENTATION

#define LIST_VIEW_TEXT_MARGIN (6)
#define LIST_VIEW_DEFAULT_ROW_HEIGHT (21)
#define LIST_VIEW_HEADER_HEIGHT (25)

// TODO Hover/select.
// TODO Keyboard controls.
// TODO Click/double-click/right-click.
// TODO Custom controls.
// TODO Dragging.
// TODO Tile view.
// TODO Clear scrollbar corner.
// TODO More efficient repositing on scroll when delta > contentHeight.

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

	int totalX, totalY;
	int scrollX, scrollY;
	int oldScrollX, oldScrollY;
	Scrollbar *scrollbarX, *scrollbarY;
	bool showScrollbarX, showScrollbarY;
	int oldContentWidth, oldContentHeight;

	OSRectangle layoutClip;
	OSRectangle margin;
	OSRectangle oldBounds;

	bool receivedLayout;
};

// static UIImage listViewHighlight           = {{228, 241, 59, 72}, {228 + 3, 241 - 3, 59 + 3, 72 - 3}};
// static UIImage listViewSelected            = {{14 + 228, 14 + 241, 59, 72}, {14 + 228 + 3, 14 + 241 - 3, 59 + 3, 72 - 3}};
// static UIImage listViewSelected2           = {{14 + 228, 14 + 241, 28 + 59, 28 + 72}, {14 + 228 + 3, 14 + 241 - 3, 28 + 59 + 3, 28 + 72 - 3}};
// static UIImage listViewLastClicked         = {{14 + 228, 14 + 241, 59 - 14, 72 - 14}, {14 + 228 + 6, 14 + 228 + 7, 59 + 6 - 14, 59 + 7 - 14}};
// static UIImage listViewSelectionBox        = {{14 + 228 - 14, 14 + 231 - 14, 42 + 59 - 14, 42 + 62 - 14}, {14 + 228 + 1 - 14, 14 + 228 + 2 - 14, 42 + 59 + 1 - 14, 42 + 59 + 2 - 14}};
static UIImage listViewColumnHeaderDivider = {{212, 213, 45, 70}, {212, 212, 45, 45}};
static UIImage listViewColumnHeader        = {{206, 212, 45, 70}, {206, 212, 45, 70}, OS_DRAW_MODE_STRECH};
static UIImage listViewBorder		   = {{237, 240, 87, 90}, {238, 239, 88, 89}};

#else

OSRectangle ListViewGetContentBounds(ListView *list) {
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

OSRectangle ListViewGetHeaderBounds(ListView *list) {
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

ListViewItem *ListViewInsertVisibleItem(ListView *list, uintptr_t index) {
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

void ListViewInsertItemsIntoVisibleItemsList(ListView *list, uintptr_t index, size_t count) {
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

void ListViewUpdate(ListView *list) {
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

	list->oldScrollX = list->scrollX;
	list->oldScrollY = list->scrollY;
	list->oldContentWidth = contentBounds.right - contentBounds.left;
	list->oldContentHeight = contentBounds.bottom - contentBounds.top;
}

void ListViewScrollVertically(ListView *list, int newScrollY) {
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

	int constantHeight = 0;

	OSMessage m = {};
	m.type = OS_NOTIFICATION_GET_ITEM;
	m.listViewItem.mask = OS_LIST_VIEW_ITEM_HEIGHT;

	if (list->flags & OS_CREATE_LIST_VIEW_CONSTANT_HEIGHT) {
		m.listViewItem.index = 0;
		OSForwardMessage(list, list->notificationCallback, &m);
		constantHeight = m.listViewItem.height;

		if (constantHeight == OS_LIST_VIEW_ITEM_HEIGHT_DEFAULT) {
			constantHeight = LIST_VIEW_DEFAULT_ROW_HEIGHT;
		}
	}

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

void ListViewPaintCell(ListView *list, OSRectangle cellBounds, OSMessage *message, int row, int column, OSRectangle rowClip) {
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

	m.type = OS_NOTIFICATION_GET_ITEM;
	m.listViewItem.mask = OS_LIST_VIEW_ITEM_TEXT;
	m.listViewItem.index = row;
	m.listViewItem.column = column;

	if (OSForwardMessage(list, list->notificationCallback, &m) != OS_CALLBACK_HANDLED) {
		OSCrashProcess(OS_FATAL_ERROR_MESSAGE_SHOULD_BE_HANDLED);
	}

	OSString string;
	string.buffer = m.listViewItem.text;
	string.bytes = m.listViewItem.textBytes;

	DrawString(message->paint.surface, textRegion, &string, 
			OS_DRAW_STRING_HALIGN_LEFT | OS_DRAW_STRING_VALIGN_CENTER,
			column ? LIST_VIEW_SECONDARY_TEXT_COLOR : LIST_VIEW_PRIMARY_TEXT_COLOR, -1, 0, 
			OS_MAKE_POINT(0, 0), nullptr, 0, 0, true, FONT_SIZE, fontRegular, rowClip, 0);
}

void ListViewPaint(ListView *list, OSMessage *message) {
	OSRectangle contentBounds = ListViewGetContentBounds(list);
	OSRectangle contentClip;

	OSRectangle headerBounds = ListViewGetHeaderBounds(list);
	OSRectangle headerClip;

	if (!list->repaintCustomOnly && (list->flags & OS_CREATE_LIST_VIEW_BORDER)) {
		OSDrawSurfaceClipped(message->paint.surface, OS_SURFACE_UI_SHEET, list->bounds,
				listViewBorder.region, listViewBorder.border, listViewBorder.drawMode, 0xFF, message->paint.clip);
	}

	if (ClipRectangle(headerBounds, message->paint.clip, &headerClip)
			&& list->columns && !list->repaintCustomOnly) {
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

			OSString string; 
			string.buffer = column->title;
			string.bytes = column->titleBytes;

			DrawString(message->paint.surface, textRegion, &string, 
					OS_DRAW_STRING_HALIGN_LEFT | OS_DRAW_STRING_VALIGN_CENTER,
					LIST_VIEW_COLUMN_TEXT_COLOR, -1, 0, 
					OS_MAKE_POINT(0, 0), nullptr, 0, 0, true, FONT_SIZE, fontRegular, headerClip, 0);

			x += column->width;
		}
	}

	if (ClipRectangle(contentBounds, message->paint.clip, &contentClip)) {
		if (!list->repaintCustomOnly && !(list->flags & OS_CREATE_LIST_VIEW_BORDER)) {
			OSFillRectangle(message->paint.surface, contentClip, OSColor(LIST_VIEW_BACKGROUND_COLOR));
		}

		int y = -list->offsetIntoFirstVisibleItem;
		int marginOffset = list->scrollY < list->margin.top ? (list->margin.top - list->scrollY) : 0;

		for (uintptr_t i = list->firstVisibleItem; i < list->firstVisibleItem + list->visibleItemCount; i++) {
			ListViewItem *item = list->visibleItems + (i - list->firstVisibleItem);

			if (!item->repaint && list->repaintCustomOnly) {
				return;
			} else {
				item->repaint = false;
			}

			OSMessage m = {};

			OSRectangle row = OS_MAKE_RECTANGLE(
					contentBounds.left + (list->columns ? 0 : list->margin.left), 
					contentBounds.right - (list->columns ? 0 : list->margin.right),
					contentBounds.top + marginOffset + y,
					contentBounds.top + marginOffset + y + item->height);

			OSRectangle rowClip;
			ClipRectangle(contentClip, row, &rowClip);
			OSFillRectangle(message->paint.surface, rowClip, OSColor(LIST_VIEW_BACKGROUND_COLOR));

			m.type = OS_NOTIFICATION_PAINT_ITEM;
			m.paintItem.index = i;
			m.paintItem.surface = message->paint.surface;
			m.paintItem.clip = rowClip;
			m.paintItem.bounds = row;

			OSCallbackResponse response = OSForwardMessage(list, list->notificationCallback, &m);
			
			if (response == OS_CALLBACK_NOT_HANDLED) {
				int x = row.left - list->scrollX + list->margin.left;

				if (list->columns) {
					for (uintptr_t j = 0; j < list->columnCount; j++) {
						OSListViewColumn *column = list->columns + j;

						OSRectangle cellBounds = OS_MAKE_RECTANGLE(x, x + column->width, row.top, row.bottom);
						ListViewPaintCell(list, cellBounds, message, i, j, rowClip);

						x += column->width;
					}
				} else {
					ListViewPaintCell(list, row, message, i, 0, rowClip);
				}

				y += item->height;
			}
		}
	}
}

OSCallbackResponse ProcessListViewMessage(OSObject listView, OSMessage *message) {
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
	} else if (message->type == OS_MESSAGE_MOUSE_MOVED) {
		if (list->showScrollbarX) OSSendMessage(list->scrollbarX, message);
		if (list->showScrollbarY) OSSendMessage(list->scrollbarY, message);

		response = OS_CALLBACK_HANDLED;
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

OSCallbackResponse ListViewScrollbarXMoved(OSObject scrollbar, OSMessage *message) {
	(void) scrollbar;
	ListView *list = (ListView *) message->context;

	if (message->type == OS_NOTIFICATION_VALUE_CHANGED) {
		RepaintControl(list);
		list->scrollX = message->valueChanged.newValue;
		return OS_CALLBACK_HANDLED;
	}

	return OS_CALLBACK_NOT_HANDLED;
}

OSCallbackResponse ListViewScrollbarYMoved(OSObject scrollbar, OSMessage *message) {
	(void) scrollbar;
	ListView *list = (ListView *) message->context;

	if (message->type == OS_NOTIFICATION_VALUE_CHANGED) {
		int newScrollY = message->valueChanged.newValue;
		ListViewScrollVertically(list, newScrollY);
		return OS_CALLBACK_HANDLED;
	}

	return OS_CALLBACK_NOT_HANDLED;
}

OSObject OSCreateListView(unsigned flags) {
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
		OSForwardMessage(list, list->notificationCallback, &m);

		itemsHeight = count * (m.listViewItem.height != OS_LIST_VIEW_ITEM_HEIGHT_DEFAULT ? m.listViewItem.height : LIST_VIEW_DEFAULT_ROW_HEIGHT);
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
