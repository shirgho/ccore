#include "win_window.h"

void _ccEventStackPush(ccEvent event)
{
	WINDOW_DATA->eventStackPos++;
	
	if(WINDOW_DATA->eventStackPos >= WINDOW_DATA->eventStackSize) {
		WINDOW_DATA->eventStackSize++;
		WINDOW_DATA->eventStack = realloc(WINDOW_DATA->eventStack, sizeof(ccEvent)*WINDOW_DATA->eventStackSize);
	}
	WINDOW_DATA->eventStack[WINDOW_DATA->eventStackPos] = event;
}

static void updateWindowDisplay(void)
{
	RECT winRect;
	GetWindowRect(WINDOW_DATA->winHandle, &winRect);

	_ccWindow->rect.x = winRect.left;
	_ccWindow->rect.y = winRect.top;

	ccWindowUpdateDisplay();
}

static void updateWindowResolution(void)
{
	RECT winRect;
	ccEvent resizeEvent;
	
	GetClientRect(WINDOW_DATA->winHandle, &winRect);

	if(winRect.right - winRect.left == _ccWindow->rect.width && winRect.bottom - winRect.top == _ccWindow->rect.height) {
		return;
	}

	_ccWindow->rect.width = winRect.right - winRect.left;
	_ccWindow->rect.height = winRect.bottom - winRect.top;
	_ccWindow->aspect = (float)_ccWindow->rect.width / _ccWindow->rect.height;

	resizeEvent.type = CC_EVENT_WINDOW_RESIZE;
	_ccEventStackPush(resizeEvent);
}

static bool initializeRawInput(void)
{
	WINDOW_DATA->rid[RAWINPUT_KEYBOARD].usUsagePage = 1;
	WINDOW_DATA->rid[RAWINPUT_KEYBOARD].usUsage = 6;
	WINDOW_DATA->rid[RAWINPUT_KEYBOARD].dwFlags = RIDEV_NOLEGACY;
	WINDOW_DATA->rid[RAWINPUT_KEYBOARD].hwndTarget = WINDOW_DATA->winHandle;

	WINDOW_DATA->rid[RAWINPUT_MOUSE].usUsagePage = 1;
	WINDOW_DATA->rid[RAWINPUT_MOUSE].usUsage = 2;
	WINDOW_DATA->rid[RAWINPUT_MOUSE].dwFlags = 0;
	WINDOW_DATA->rid[RAWINPUT_MOUSE].hwndTarget = WINDOW_DATA->winHandle;

	return RegisterRawInputDevices(WINDOW_DATA->rid, NRAWINPUTDEVICES - 1, sizeof(RAWINPUTDEVICE));
}

static void freeRawInput(void)
{
	WINDOW_DATA->rid[RAWINPUT_KEYBOARD].dwFlags = RIDEV_REMOVE;
	WINDOW_DATA->rid[RAWINPUT_KEYBOARD].hwndTarget = NULL;

	WINDOW_DATA->rid[RAWINPUT_MOUSE].dwFlags = RIDEV_REMOVE;
	WINDOW_DATA->rid[RAWINPUT_MOUSE].hwndTarget = NULL;

	RegisterRawInputDevices(WINDOW_DATA->rid, NRAWINPUTDEVICES - 1, sizeof(RAWINPUTDEVICE));
}

static void processRid(HRAWINPUT rawInput)
{
	GetRawInputData(rawInput, RID_INPUT, NULL, &WINDOW_DATA->dwSize, sizeof(RAWINPUTHEADER));

	if(WINDOW_DATA->dwSize > WINDOW_DATA->lpbSize) {
		WINDOW_DATA->lpb = realloc(WINDOW_DATA->lpb, WINDOW_DATA->dwSize);
		WINDOW_DATA->lpbSize = WINDOW_DATA->dwSize;
	}

	GetRawInputData(rawInput, RID_INPUT, WINDOW_DATA->lpb, &WINDOW_DATA->dwSize, sizeof(RAWINPUTHEADER));

	RAWINPUT* raw = (RAWINPUT*)WINDOW_DATA->lpb;

	if(raw->header.dwType == RIM_TYPEMOUSE) {
		USHORT buttonFlags = raw->data.mouse.usButtonFlags;
		
		if(buttonFlags == 0) {
			_ccWindow->event.type = CC_EVENT_MOUSE_MOVE;
			_ccWindow->event.mouseDelta.x = raw->data.mouse.lLastX;
			_ccWindow->event.mouseDelta.y = raw->data.mouse.lLastY;
		}
		else if(buttonFlags & RI_MOUSE_WHEEL) {
			_ccWindow->event.type = CC_EVENT_MOUSE_SCROLL;
			_ccWindow->event.scrollDelta = (float)((short)raw->data.mouse.usButtonData) / WHEEL_DELTA;
		}
		else if(buttonFlags & RI_MOUSE_LEFT_BUTTON_DOWN) {
			_ccWindow->event.type = CC_EVENT_MOUSE_DOWN;
			_ccWindow->event.mouseButton = CC_MOUSE_BUTTON_LEFT;
		}
		else if(buttonFlags & RI_MOUSE_LEFT_BUTTON_UP) {
			_ccWindow->event.type = CC_EVENT_MOUSE_UP;
			_ccWindow->event.mouseButton = CC_MOUSE_BUTTON_LEFT;
		}
		else if(buttonFlags & RI_MOUSE_RIGHT_BUTTON_DOWN) {
			_ccWindow->event.type = CC_EVENT_MOUSE_DOWN;
			_ccWindow->event.mouseButton = CC_MOUSE_BUTTON_RIGHT;
		}
		else if(buttonFlags & RI_MOUSE_RIGHT_BUTTON_UP) {
			_ccWindow->event.type = CC_EVENT_MOUSE_UP;
			_ccWindow->event.mouseButton = CC_MOUSE_BUTTON_RIGHT;
		}
		else if(buttonFlags & RI_MOUSE_MIDDLE_BUTTON_DOWN) {
			_ccWindow->event.type = CC_EVENT_MOUSE_DOWN;
			_ccWindow->event.mouseButton = CC_MOUSE_BUTTON_MIDDLE;
		}
		else if(buttonFlags & RI_MOUSE_MIDDLE_BUTTON_UP) {
			_ccWindow->event.type = CC_EVENT_MOUSE_UP;
			_ccWindow->event.mouseButton = CC_MOUSE_BUTTON_MIDDLE;
		}
	}
	else if(raw->header.dwType == RIM_TYPEKEYBOARD)
	{
		UINT vkCode = raw->data.keyboard.VKey;

		//Parse raw keycodes

		if(vkCode == 255) {
			return;
		}
		else if(vkCode == VK_CONTROL) {
			vkCode = raw->data.keyboard.Flags & RI_KEY_E0?VK_RCONTROL:VK_LCONTROL;
		}
		else if(vkCode == VK_SHIFT) {
			vkCode = MapVirtualKey(raw->data.keyboard.MakeCode, MAPVK_VSC_TO_VK_EX);
		}

		//fill event with data
		_ccWindow->event.type = raw->data.keyboard.Message == WM_KEYDOWN?CC_EVENT_KEY_DOWN:CC_EVENT_KEY_UP;
		_ccWindow->event.keyCode = vkCode;
	}
	else if(raw->header.dwType == RIM_TYPEHID)
	{
		_generateGamepadEvents(raw);
	}
}

static LRESULT CALLBACK wndProc(HWND winHandle, UINT message, WPARAM wParam, LPARAM lParam)
{
	_ccWindow->event.type = CC_EVENT_SKIP;
	switch(message) {
	case WM_INPUT:
		processRid((HRAWINPUT)lParam);
		break;
	case WM_CLOSE:
		_ccWindow->event.type = CC_EVENT_WINDOW_QUIT;
		break;
	case WM_SIZE:
		updateWindowResolution();
		break;
	case WM_MOVE:
		updateWindowDisplay();
		break;
	case WM_MOUSEMOVE:
		_ccWindow->mouse.x = (unsigned short)lParam & 0x0000FFFF;
		_ccWindow->mouse.y = (unsigned short)((lParam & 0xFFFF0000) >> 16);
		break;
	case WM_SETFOCUS:
	{
		ccEvent event;
		event.type = CC_EVENT_FOCUS_GAINED;
		_ccEventStackPush(event);
	}
		break;
	case WM_KILLFOCUS:
	{
		ccEvent event;
		event.type = CC_EVENT_FOCUS_LOST;
		_ccEventStackPush(event);
	}
		break;
	default:
		return DefWindowProc(winHandle, message, wParam, lParam);
		break;
	}
	return 0;
}

static void regHinstance(HINSTANCE instanceHandle)
{
	WNDCLASSEX winClass;

	winClass.cbSize = sizeof(WNDCLASSEX);
	winClass.style = CS_OWNDC;
	winClass.lpfnWndProc = wndProc;
	winClass.cbClsExtra = 0;
	winClass.cbWndExtra = 0;
	winClass.hInstance = instanceHandle;
	winClass.hIcon = NULL;
	winClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	winClass.hbrBackground = NULL;
	winClass.lpszMenuName = NULL;
	winClass.lpszClassName = "ccWindow";
	winClass.hIconSm = NULL;

	RegisterClassEx(&winClass);
}

bool ccWindowPollEvent(void)
{
	static bool canPollInput = true;

	ccAssert(_ccWindow != NULL);
	
	if(canPollInput) {
		canPollInput = false;
		if(WINDOW_DATA->queryXinput) _queryXinput();
	}

	if(WINDOW_DATA->eventStackPos != -1) {

		_ccWindow->event = WINDOW_DATA->eventStack[WINDOW_DATA->eventStackIndex];

		WINDOW_DATA->eventStackIndex++;
		WINDOW_DATA->eventStackPos--;

		if(WINDOW_DATA->eventStackPos == -1) WINDOW_DATA->eventStackIndex = 0;

		return true;
	}
	
	if(PeekMessage(&WINDOW_DATA->msg, NULL, 0, 0, PM_REMOVE)){
		TranslateMessage(&WINDOW_DATA->msg);
		DispatchMessage(&WINDOW_DATA->msg);
		return true;
	}

	canPollInput = true;
	return false;
}

ccReturn ccWindowCreate(ccRect rect, const char* title, int flags)
{
	HMODULE moduleHandle = GetModuleHandle(NULL);
	RECT windowRect;

	ccAssert(_ccWindow == NULL);

	//initialize struct
	ccMalloc(_ccWindow, sizeof(ccWindow));

	_ccWindow->supportsRawInput = true;
	_ccWindow->rect = rect;
	ccMalloc(_ccWindow->data, sizeof(ccWindow_win));

	WINDOW_DATA->eventStackSize = 0;
	WINDOW_DATA->eventStackPos = -1;
	WINDOW_DATA->eventStackIndex = 0;
	WINDOW_DATA->eventStack = NULL;
	WINDOW_DATA->queryXinput = false;
	WINDOW_DATA->renderContext = NULL;
	WINDOW_DATA->lpbSize = 0;
	WINDOW_DATA->lpb = NULL;
	
	//apply flags
	WINDOW_DATA->style = WS_OVERLAPPEDWINDOW;
	if((flags & CC_WINDOW_FLAG_NORESIZE) == CC_WINDOW_FLAG_NORESIZE) WINDOW_DATA->style &= ~(WS_THICKFRAME | WS_MAXIMIZEBOX);
	if((flags & CC_WINDOW_FLAG_NOBUTTONS) == CC_WINDOW_FLAG_NOBUTTONS)WINDOW_DATA->style &= ~WS_SYSMENU;

	windowRect.left = rect.x;
	windowRect.top = rect.y;
	windowRect.right = rect.x + rect.width;
	windowRect.bottom = rect.y + rect.height;
	if(AdjustWindowRectEx(&windowRect, WINDOW_DATA->style, FALSE, WS_EX_APPWINDOW) == FALSE) {
		ccErrorPush(CC_ERROR_WINDOWCREATION);
		return CC_FAIL;
	}
	
	regHinstance(moduleHandle);
	WINDOW_DATA->winHandle = CreateWindowEx(
		WS_EX_APPWINDOW,
		"ccWindow",
		title,
		WINDOW_DATA->style,
		windowRect.left, windowRect.top,
		windowRect.right - windowRect.left, windowRect.bottom - windowRect.top,
		NULL,
		NULL,
		moduleHandle,
		NULL);

	WINDOW_DATA->style |= WS_VISIBLE;
	
	ShowWindow(WINDOW_DATA->winHandle, SW_SHOW);
	
	initializeRawInput();
	
	if((flags & CC_WINDOW_FLAG_ALWAYSONTOP) == CC_WINDOW_FLAG_ALWAYSONTOP) {
		RECT rect;
		if(GetWindowRect(WINDOW_DATA->winHandle, &rect) == FALSE) {
			ccErrorPush(CC_ERROR_WINDOWCREATION);
			return CC_FAIL;
		}
		if(SetWindowPos(WINDOW_DATA->winHandle, HWND_TOPMOST, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, SWP_SHOWWINDOW) == FALSE) return CC_ERROR_WINDOWCREATION;
	}
	
	return CC_SUCCESS;
}

ccReturn ccWindowFree(void)
{
	ccAssert(_ccWindow != NULL);

	freeRawInput();

	if(WINDOW_DATA->lpbSize != 0) free(WINDOW_DATA->lpb);

	ReleaseDC(WINDOW_DATA->winHandle, WINDOW_DATA->hdc);
	if(DestroyWindow(WINDOW_DATA->winHandle) == FALSE) {
		ccErrorPush(CC_ERROR_WINDOWDESTRUCTION);
		return CC_FAIL;
	}
	if(WINDOW_DATA->eventStackSize != 0) free(WINDOW_DATA->eventStack);

	free(_ccWindow->data);
	free(_ccWindow);

	_ccWindow = NULL;

	return CC_SUCCESS;
}

ccReturn ccWindowSetWindowed(void)
{
	ccAssert(_ccWindow != NULL);

	SetWindowLongPtr(WINDOW_DATA->winHandle, GWL_STYLE, WINDOW_DATA->style | WS_CAPTION);
	if(ShowWindow(WINDOW_DATA->winHandle, SW_SHOW) == FALSE) {
		ccErrorPush(CC_ERROR_WINDOW_MODE);
		return CC_FAIL;
	}
	ccWindowResizeMove(ccDisplayGetRect(_ccWindow->display));

	return CC_SUCCESS;
}

ccReturn ccWindowSetMaximized(void)
{
	ccAssert(_ccWindow != NULL);

	SetWindowLongPtr(WINDOW_DATA->winHandle, GWL_STYLE, WINDOW_DATA->style | WS_CAPTION);
	if(ShowWindow(WINDOW_DATA->winHandle, SW_MAXIMIZE) == FALSE) {
		ccErrorPush(CC_ERROR_WINDOW_MODE);
		return CC_FAIL;
	}

	return CC_SUCCESS;
}

ccReturn _ccWindowResizeMove(ccRect rect, bool addBorder)
{
	ccAssert(_ccWindow != NULL);

	if(addBorder) {
		RECT windowRect;
		windowRect.left = rect.x;
		windowRect.top = rect.y;
		windowRect.right = rect.x + rect.width;
		windowRect.bottom = rect.y + rect.height;
		if(AdjustWindowRectEx(&windowRect, WINDOW_DATA->style, FALSE, WS_EX_APPWINDOW) == FALSE) {
			ccErrorPush(CC_ERROR_WINDOW_MODE);
			return CC_FAIL;
		}

		if(MoveWindow(WINDOW_DATA->winHandle, windowRect.left, windowRect.top, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top, FALSE) == FALSE) return CC_ERROR_WINDOW_MODE;
	}
	else{
		if(MoveWindow(WINDOW_DATA->winHandle, rect.x, rect.y, rect.width, rect.height, FALSE) == FALSE) {
			ccErrorPush(CC_ERROR_WINDOW_MODE);
			return CC_FAIL;
		}
	}

	return CC_SUCCESS;
}

ccReturn ccWindowSetFullscreen(int displayCount, ...)
{
	ccAssert(_ccWindow);

	SetWindowLongPtr(WINDOW_DATA->winHandle, GWL_STYLE, WINDOW_DATA->style & ~(WS_CAPTION | WS_THICKFRAME));
	if(ShowWindow(WINDOW_DATA->winHandle, SW_SHOW) == FALSE) {
		ccErrorPush(CC_ERROR_WINDOW_MODE);
		return CC_FAIL;
	}

	if(displayCount == CC_FULLSCREEN_CURRENT_DISPLAY) {
		return _ccWindowResizeMove(ccDisplayGetRect(_ccWindow->display), false);
	}
	else{
		va_list displays;
		ccRect* rectList;
		ccRect spanRect;
		int i;

		ccMalloc(rectList, displayCount * sizeof(ccRect));

		va_start(displays, displayCount);

		for(i = 0; i < displayCount; i++) {
			rectList[i] = ccDisplayGetRect(va_arg(displays, ccDisplay*));
		}

		spanRect = ccRectConcatenate(displayCount, rectList);
		
		free(rectList);

		va_end(displays);

		return _ccWindowResizeMove(spanRect, false);
	}
}

ccReturn ccWindowResizeMove(ccRect rect)
{
	return _ccWindowResizeMove(rect, true);
}

ccReturn ccWindowCenter(void)
{
	RECT windowRect;

	ccAssert(_ccWindow != NULL);

	if(GetWindowRect(WINDOW_DATA->winHandle, &windowRect) == FALSE) {
		ccErrorPush(CC_ERROR_WINDOW_MODE);
		return CC_FAIL;
	}

	return _ccWindowResizeMove(
		(ccRect){_ccWindow->display->x + ((ccDisplayGetResolutionCurrent(_ccWindow->display)->width - (windowRect.right - windowRect.left)) >> 1),
				 _ccWindow->display->y + ((ccDisplayGetResolutionCurrent(_ccWindow->display)->height - (windowRect.bottom - windowRect.top)) >> 1),
				 windowRect.right - windowRect.left,
				 windowRect.bottom - windowRect.top
	}, false);
}