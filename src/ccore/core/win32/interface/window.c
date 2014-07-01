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
	//TODO: process mouse here too
	GetRawInputData(rawInput, RID_INPUT, NULL, &_window->dwSize, sizeof(RAWINPUTHEADER));

	if(_window->dwSize > _window->lpbSize) {
		_window->lpb = _window->lpbSize == 0?malloc(_window->dwSize):realloc(_window->lpb, _window->dwSize);
		_window->lpbSize = _window->dwSize;
	}

	GetRawInputData(rawInput, RID_INPUT, _window->lpb, &_window->dwSize, sizeof(RAWINPUTHEADER));

	//TODO: make lpb of type RAWINPUT*
	RAWINPUT* raw = (RAWINPUT*)_window->lpb;

	if(raw->header.dwType == RIM_TYPEMOUSE) {
		printf("mouse");
	}
	else if(raw->header.dwType == RIM_TYPEKEYBOARD)
	{
		ccKeyCode keyCode = CC_KEY_UNDEFINED;
		UINT vkCode = raw->data.keyboard.VKey;

		if(vkCode == 255) {
			return;
		}

		if((vkCode >= '0' && vkCode <= '9') || (vkCode >= 'A' && vkCode <= 'Z')) {
			keyCode = vkCode;
		}
		else if(vkCode >= VK_F1 && vkCode <= VK_F12) {
			keyCode = CC_KEY_F1 + vkCode - VK_F1;
		}
		else if(vkCode >= VK_NUMPAD0 && vkCode <= VK_NUMPAD9) {
			keyCode = CC_KEY_NUM0 + vkCode - VK_NUMPAD0;
		}
		else switch(vkCode)
		{
		case VK_LEFT:
			keyCode = CC_KEY_LEFT;
			break;
		case VK_UP:
			keyCode = CC_KEY_UP;
			break;
		case VK_RIGHT:
			keyCode = CC_KEY_RIGHT;
			break;
		case VK_DOWN:
			keyCode = CC_KEY_DOWN;
			break;
		case VK_BACK:
			keyCode = CC_KEY_BACKSPACE;
			break;
		case VK_TAB:
			keyCode = CC_KEY_TAB;
			break;
		case VK_RETURN:
			keyCode = CC_KEY_RETURN;
			break;
		case VK_ESCAPE:
			keyCode = CC_KEY_ESCAPE;
			break;
		case VK_SPACE:
			keyCode = CC_KEY_SPACE;
			break;
		case VK_CAPITAL:
			keyCode = CC_KEY_CAPSLOCK;
			break;
		case VK_INSERT:
			keyCode = CC_KEY_INSERT;
			break;
		case VK_DELETE:
			keyCode = CC_KEY_DELETE;
			break;
		case VK_HOME:
			keyCode = CC_KEY_HOME;
			break;
		case VK_END:
			keyCode = CC_KEY_END;
			break;
		case VK_PRIOR:
			keyCode = CC_KEY_PAGEUP;
			break;
		case VK_NEXT:
			keyCode = CC_KEY_PAGEDOWN;
			break;
		case VK_SNAPSHOT:
			keyCode = CC_KEY_PRINTSCREEN;
			break;
		case VK_SCROLL:
			keyCode = CC_KEY_SCROLLLOCK;
			break;
		case VK_NUMLOCK:
			keyCode = CC_KEY_NUMLOCK;
			break;
		case VK_PAUSE:
			keyCode = CC_KEY_PAUSEBREAK;
			break;
		case VK_CONTROL:
			keyCode = raw->data.keyboard.Flags & RI_KEY_E0?CC_KEY_RCONTROL:CC_KEY_LCONTROL;
			break;
		case VK_SHIFT:
			keyCode = MapVirtualKey(raw->data.keyboard.MakeCode, MAPVK_VSC_TO_VK_EX) == VK_LSHIFT?CC_KEY_LSHIFT:CC_KEY_RSHIFT;
			break;
		case VK_LSHIFT:
			keyCode = CC_KEY_LSHIFT;
			break;
		case VK_RSHIFT:
			keyCode = CC_KEY_RSHIFT;
			break;
		case VK_MENU:
			keyCode = raw->data.keyboard.Flags & RI_KEY_E0?CC_KEY_RMENU:CC_KEY_LMENU;
			break;
		}

		if(keyCode != CC_KEY_UNDEFINED) {
			_window->event.type = raw->data.keyboard.Message == WM_KEYDOWN?CC_EVENT_KEY_DOWN:CC_EVENT_KEY_UP;
			_window->event.key = keyCode;
		}
	}
}

static LRESULT CALLBACK wndProc(HWND winHandle, UINT message, WPARAM wParam, LPARAM lParam)
{
	if(_window == NULL) goto skipevent;

	_window->event.type = CC_EVENT_SKIP;

	switch(message) {
	case WM_KEYDOWN:
		printf("KD");
		break;
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

void ccNewWindow(ccRect rect, const char* title, int flags)
{
	HMODULE moduleHandle = GetModuleHandle(NULL);
	RECT windowRect;

	ccAssert(_window == NULL);

	//initialize struct
	_window = malloc(sizeof(ccWindow));
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
	AdjustWindowRectEx(&windowRect, _window->style, FALSE, WS_EX_APPWINDOW);
	
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

	ShowWindow(_window->winHandle, SW_SHOW);

	initializeRawInput();

	if((flags & CC_WINDOW_FLAG_ALWAYSONTOP) == CC_WINDOW_FLAG_ALWAYSONTOP) {
		RECT rect;
		GetWindowRect(_window->winHandle, &rect);
		SetWindowPos(_window->winHandle, HWND_TOPMOST, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, SWP_SHOWWINDOW);
	}
}

void ccFreeWindow()
{
	ccAssert(_window != NULL);

	freeRawInput();

	if(_window->lpbSize != 0) free(_window->lpb);

	ReleaseDC(_window->winHandle, _window->hdc);
	//TODO: release context?
	DestroyWindow(_window->winHandle);
	free(_window);
	_window = NULL;
}

void ccChangeWM(ccWindowMode mode)
{
	ccAssert(_window != NULL);

	switch(mode)
	{
	case CC_WINDOW_MODE_WINDOW:
		SetWindowLongPtr(_window->winHandle, GWL_STYLE, _window->style | WS_CAPTION);
		ShowWindow(_window->winHandle, SW_SHOW);
		ccResizeMoveWindow(ccGetDisplayRect(_window->display), true);
		break;
	case CC_WINDOW_MODE_FULLSCREEN:
		SetWindowLongPtr(_window->winHandle, GWL_STYLE, _window->style & ~(WS_CAPTION | WS_THICKFRAME));
		ShowWindow(_window->winHandle, SW_SHOW);
		ccResizeMoveWindow(ccGetDisplayRect(_window->display), false);
		break;
	case CC_WINDOW_MODE_MAXIMIZED:
		SetWindowLongPtr(_window->winHandle, GWL_STYLE, _window->style | WS_CAPTION);
		ShowWindow(_window->winHandle, SW_MAXIMIZE);
		break;
	}
}

void ccResizeMoveWindow(ccRect rect, bool addBorder)
{
	ccAssert(_window != NULL);

	if(addBorder) {
		RECT windowRect;
		windowRect.left = rect.x;
		windowRect.top = rect.y;
		windowRect.right = rect.x + rect.width;
		windowRect.bottom = rect.y + rect.height;
		AdjustWindowRectEx(&windowRect, _window->style, FALSE, WS_EX_APPWINDOW);

		MoveWindow(_window->winHandle, windowRect.left, windowRect.top, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top, FALSE);
	}
	else{
		MoveWindow(_window->winHandle, rect.x, rect.y, rect.width, rect.height, FALSE);
	}
}

void ccCenterWindow()
{
	RECT windowRect;

	ccAssert(_window != NULL);

	GetWindowRect(_window->winHandle, &windowRect);
	ccResizeMoveWindow(
		(ccRect){_window->display->x + ((ccGetResolutionCurrent(_window->display)->width - (windowRect.right - windowRect.left)) >> 1),
				 _window->display->y + ((ccGetResolutionCurrent(_window->display)->height - (windowRect.bottom - windowRect.top)) >> 1),
				 windowRect.right - windowRect.left,
				 windowRect.bottom - windowRect.top
	}, false);
}