#include "../../common/interface/window.h"

ccEvent ccGetEvent()
{
	ccAssert(_window != NULL);

	return _window->event;
}

ccRect ccGetWindowRect()
{
	ccAssert(_window != NULL);

	return _window->rect;
}

bool ccWindowExists()
{
	return _window != NULL;
}

static void updateWindowDisplay()
{
	int i;
	int area, largestArea;
	ccRect displayRect;
	RECT winRect;
	
	GetWindowRect(_window->winHandle, &winRect);

	_window->rect.x = winRect.left;
	_window->rect.y = winRect.top;

	largestArea = 0;

	for(i = 0; i < ccGetDisplayAmount(); i++)
	{
		displayRect = ccGetDisplayRect(ccGetDisplay(i));
		area = ccRectIntersectionArea(&displayRect, &_window->rect);
		if(area > largestArea) {
			largestArea = area;
			_window->display = ccGetDisplay(i);
		}
	}
}

static void updateWindowResolution()
{
	RECT winRect;
	
	GetClientRect(_window->winHandle, &winRect);

	if(winRect.right - winRect.left == _window->rect.width && winRect.bottom - winRect.top == _window->rect.height) {
		return;
	}

	_window->rect.width = winRect.right - winRect.left;
	_window->rect.height = winRect.bottom - winRect.top;
	_window->aspect = (float)_window->rect.width / _window->rect.height;
	_window->sizeChanged = true;
}

static bool initializeRawInput()
{
	_window->rid[RAWINPUT_KEYBOARD].usUsagePage = 0x01;
	_window->rid[RAWINPUT_KEYBOARD].usUsage = 0x06;
	_window->rid[RAWINPUT_KEYBOARD].dwFlags = RIDEV_NOLEGACY;
	_window->rid[RAWINPUT_KEYBOARD].hwndTarget = _window->winHandle;

	_window->rid[RAWINPUT_MOUSE].usUsagePage = 0x01;
	_window->rid[RAWINPUT_MOUSE].usUsage = 0x02;
	_window->rid[RAWINPUT_MOUSE].dwFlags = RIDEV_NOLEGACY;
	_window->rid[RAWINPUT_MOUSE].hwndTarget = _window->winHandle;

	return RegisterRawInputDevices(_window->rid, NRAWINPUTDEVICES, sizeof(_window->rid[0]));
}

static void freeRawInput()
{
	_window->rid[RAWINPUT_KEYBOARD].dwFlags = RIDEV_REMOVE;
	_window->rid[RAWINPUT_KEYBOARD].hwndTarget = NULL;

	_window->rid[RAWINPUT_MOUSE].dwFlags = RIDEV_REMOVE;
	_window->rid[RAWINPUT_MOUSE].hwndTarget = NULL;

	RegisterRawInputDevices(_window->rid, NRAWINPUTDEVICES, sizeof(_window->rid[0]));
}

static void processRid(HRAWINPUT rawInput)
{
	GetRawInputData(rawInput, RID_INPUT, NULL, &_window->dwSize, sizeof(RAWINPUTHEADER));

	if(_window->dwSize > _window->lpbSize) {
		_window->lpb = _window->lpbSize == 0?malloc(_window->dwSize):realloc(_window->lpb, _window->dwSize);
		_window->lpbSize = _window->dwSize;
	}

	GetRawInputData(rawInput, RID_INPUT, _window->lpb, &_window->dwSize, sizeof(RAWINPUTHEADER));

	//TODO: make lpb of type RAWINPUT*
	RAWINPUT* raw = (RAWINPUT*)_window->lpb;

	if(raw->header.dwType == RIM_TYPEMOUSE) {
		
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
		_window->event.type = raw->data.keyboard.Message == WM_KEYDOWN?CC_EVENT_KEY_DOWN:CC_EVENT_KEY_UP;
		_window->event.keyCode = vkCode;
	}
}

static LRESULT CALLBACK wndProc(HWND winHandle, UINT message, WPARAM wParam, LPARAM lParam)
{
	if(_window == NULL) goto skipevent;

	_window->event.type = CC_EVENT_SKIP;

	switch(message) {
	case WM_INPUT:
		processRid((HRAWINPUT)lParam);
		break;
	case WM_CLOSE:
		_window->event.type = CC_EVENT_WINDOW_QUIT;
		break;
	case WM_SIZE:
		updateWindowResolution();
		break;
	case WM_MOVE:
		updateWindowDisplay(_window);
		break;
	case WM_MOUSEMOVE:
		_window->mouse.x = (unsigned short)lParam & 0x0000FFFF;
		_window->mouse.y = (unsigned short)((lParam & 0xFFFF0000) >> 16);
		_window->event.type = CC_EVENT_MOUSE_MOVE;
		break;
	case WM_LBUTTONDOWN:
		_window->event.type = CC_EVENT_MOUSE_DOWN;
		_window->event.mouseButton = CC_MOUSE_BUTTON_LEFT;
		break;
	case WM_MBUTTONDOWN:
		_window->event.type = CC_EVENT_MOUSE_DOWN;
		_window->event.mouseButton = CC_MOUSE_BUTTON_MIDDLE;
		break;
	case WM_RBUTTONDOWN:
		_window->event.type = CC_EVENT_MOUSE_DOWN;
		_window->event.mouseButton = CC_MOUSE_BUTTON_RIGHT;
		break;
	case WM_LBUTTONUP:
		_window->event.type = CC_EVENT_MOUSE_UP;
		_window->event.mouseButton = CC_MOUSE_BUTTON_LEFT;
		break;
	case WM_MBUTTONUP:
		_window->event.type = CC_EVENT_MOUSE_UP;
		_window->event.mouseButton = CC_MOUSE_BUTTON_MIDDLE;
		break;
	case WM_RBUTTONUP:
		_window->event.type = CC_EVENT_MOUSE_UP;
		_window->event.mouseButton = CC_MOUSE_BUTTON_RIGHT;
		break;
	case WM_MOUSEWHEEL:
		_window->event.type = CC_EVENT_MOUSE_SCROLL;
		_window->event.scrollDelta = GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA;
		break;
	case WM_SETFOCUS:
		_window->event.type = CC_EVENT_FOCUS_GAINED;
		break;
	case WM_KILLFOCUS:
		_window->event.type = CC_EVENT_FOCUS_LOST;
		break;
	default:
	skipevent:
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

bool ccPollEvent()
{
	ccAssert(_window != NULL);

	if(_window->sizeChanged) {
		_window->sizeChanged = false;
		_window->event.type = CC_EVENT_WINDOW_RESIZE;
		return true;
	}
	
	if(PeekMessage(&_window->msg, NULL, 0, 0, PM_REMOVE)){
		TranslateMessage(&_window->msg);
		DispatchMessage(&_window->msg);
		return true;
	}

	return false;
}

ccError ccNewWindow(ccRect rect, const char* title, int flags)
{
	HMODULE moduleHandle = GetModuleHandle(NULL);
	RECT windowRect;

	ccAssert(_window == NULL);

	//initialize struct
	ccMalloc(_window, sizeof(ccWindow));
	memcpy(&_window->rect, &rect, sizeof(ccRect));
	_window->sizeChanged = true;

	_window->lpbSize = 0;

	//apply flags
	_window->style = WS_OVERLAPPEDWINDOW;
	if((flags & CC_WINDOW_FLAG_NORESIZE) == CC_WINDOW_FLAG_NORESIZE) _window->style &= ~(WS_THICKFRAME | WS_MAXIMIZEBOX);
	if((flags & CC_WINDOW_FLAG_NOBUTTONS) == CC_WINDOW_FLAG_NOBUTTONS) _window->style &= ~WS_SYSMENU;

	windowRect.left = rect.x;
	windowRect.top = rect.y;
	windowRect.right = rect.x + rect.width;
	windowRect.bottom = rect.y + rect.height;
	if(AdjustWindowRectEx(&windowRect, _window->style, FALSE, WS_EX_APPWINDOW) == FALSE) return CC_ERROR_WINDOWCREATION;
	
	regHinstance(moduleHandle);
	_window->winHandle = CreateWindowEx(
		WS_EX_APPWINDOW,
		"ccWindow",
		title,
		_window->style,
		windowRect.left, windowRect.top,
		windowRect.right - windowRect.left, windowRect.bottom - windowRect.top,
		NULL,
		NULL,
		moduleHandle,
		NULL);

	_window->style |= WS_VISIBLE;

	if(ShowWindow(_window->winHandle, SW_SHOW) == FALSE) return CC_ERROR_WINDOWCREATION;

	initializeRawInput();

	if((flags & CC_WINDOW_FLAG_ALWAYSONTOP) == CC_WINDOW_FLAG_ALWAYSONTOP) {
		RECT rect;
		if(GetWindowRect(_window->winHandle, &rect) == FALSE) return CC_ERROR_WINDOWCREATION;
		if(SetWindowPos(_window->winHandle, HWND_TOPMOST, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, SWP_SHOWWINDOW) == FALSE) return CC_ERROR_WINDOWCREATION;
	}

	return CC_ERROR_NONE;
}

ccError ccFreeWindow()
{
	ccAssert(_window != NULL);

	freeRawInput();

	if(_window->lpbSize != 0) free(_window->lpb);

	ReleaseDC(_window->winHandle, _window->hdc);
	if(DestroyWindow(_window->winHandle) == FALSE) return CC_ERROR_WINDOWDESTRUCTION;
	free(_window);
	_window = NULL;

	return CC_ERROR_NONE;
}

ccError ccChangeWM(ccWindowMode mode)
{
	ccAssert(_window != NULL);

	switch(mode)
	{
	case CC_WINDOW_MODE_WINDOW:
		SetWindowLongPtr(_window->winHandle, GWL_STYLE, _window->style | WS_CAPTION);
		if(ShowWindow(_window->winHandle, SW_SHOW) == FALSE) return CC_ERROR_WINDOW_MODE;
		ccResizeMoveWindow(ccGetDisplayRect(_window->display), true);
		break;
	case CC_WINDOW_MODE_FULLSCREEN:
		SetWindowLongPtr(_window->winHandle, GWL_STYLE, _window->style & ~(WS_CAPTION | WS_THICKFRAME));
		if(ShowWindow(_window->winHandle, SW_SHOW) == FALSE) return CC_ERROR_WINDOW_MODE;
		ccResizeMoveWindow(ccGetDisplayRect(_window->display), false);
		break;
	case CC_WINDOW_MODE_MAXIMIZED:
		SetWindowLongPtr(_window->winHandle, GWL_STYLE, _window->style | WS_CAPTION);
		if(ShowWindow(_window->winHandle, SW_MAXIMIZE) == FALSE) return CC_ERROR_WINDOW_MODE;
		break;
	}
	
	return CC_ERROR_NONE;
}

ccError ccResizeMoveWindow(ccRect rect, bool addBorder)
{
	ccAssert(_window != NULL);

	if(addBorder) {
		RECT windowRect;
		windowRect.left = rect.x;
		windowRect.top = rect.y;
		windowRect.right = rect.x + rect.width;
		windowRect.bottom = rect.y + rect.height;
		if(AdjustWindowRectEx(&windowRect, _window->style, FALSE, WS_EX_APPWINDOW) == FALSE) return CC_ERROR_WINDOW_MODE;

		if(MoveWindow(_window->winHandle, windowRect.left, windowRect.top, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top, FALSE) == FALSE) return CC_ERROR_WINDOW_MODE;
	}
	else{
		if(MoveWindow(_window->winHandle, rect.x, rect.y, rect.width, rect.height, FALSE) == FALSE) return CC_ERROR_WINDOW_MODE;
	}

	return CC_ERROR_NONE;
}

ccError ccCenterWindow()
{
	RECT windowRect;

	ccAssert(_window != NULL);

	if(GetWindowRect(_window->winHandle, &windowRect) == FALSE) return CC_ERROR_WINDOW_MODE;
	return ccResizeMoveWindow(
		(ccRect){_window->display->x + ((ccGetResolutionCurrent(_window->display)->width - (windowRect.right - windowRect.left)) >> 1),
				 _window->display->y + ((ccGetResolutionCurrent(_window->display)->height - (windowRect.bottom - windowRect.top)) >> 1),
				 windowRect.right - windowRect.left,
				 windowRect.bottom - windowRect.top
	}, false);
}