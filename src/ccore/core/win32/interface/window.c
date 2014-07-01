#include "../../common/interface/window.h"

static ccEvent* keyEventStack;
static int keyEventStackSize;
static int keyEventStackPos;

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

static LRESULT CALLBACK llKeyProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if(nCode == HC_ACTION) {
		DWORD wParamKey = ((PKBDLLHOOKSTRUCT)lParam)->vkCode;
		ccKeyCode ccKey = CC_KEY_UNDEFINED;

		if((wParamKey >= CC_KEY_A && wParamKey <= CC_KEY_Z) || (wParamKey >= CC_KEY_0 && wParamKey <= CC_KEY_9)) {
			ccKey = wParamKey;
		}
		else if(wParamKey >= VK_F1 && wParamKey <= VK_F12) {
			ccKey = CC_KEY_F1 + wParamKey - VK_F1;
		}
		else if(wParamKey >= VK_NUMPAD0 && wParamKey <= VK_NUMPAD9) {
			ccKey = CC_KEY_NUM0 + wParamKey - VK_NUMPAD0;
		}
		else{
			switch(wParamKey) {
			case VK_LCONTROL:
				ccKey = CC_KEY_LCONTROL;
				break;
			case VK_RCONTROL:
				ccKey = CC_KEY_RCONTROL;
				break;
			case VK_LSHIFT:
				ccKey = CC_KEY_LSHIFT;
				break;
			case VK_RSHIFT:
				ccKey = CC_KEY_RSHIFT;
				break;
			}
		}

		if(ccKey != CC_KEY_UNDEFINED && (wParam == WM_KEYUP || wParam == WM_KEYDOWN)) {

			if(keyEventStackPos == keyEventStackSize) {
				keyEventStackSize++;
				printf("new stack size: %d\n", keyEventStackSize);
				keyEventStack = realloc(keyEventStack, keyEventStackSize*sizeof(ccEvent));
			}

			keyEventStack[keyEventStackPos].type = wParam == WM_KEYUP? CC_EVENT_KEY_UP : CC_EVENT_KEY_DOWN;
			keyEventStack[keyEventStackPos].key = ccKey;

			keyEventStackPos++;
		}
	}

	return CallNextHookEx(NULL, nCode, wParam, lParam);
}

static LRESULT CALLBACK wndProc(HWND winHandle, UINT message, WPARAM wParam, LPARAM lParam)
{
	if(_window == NULL) goto skipevent;

	_window->event.type = CC_EVENT_SKIP;

	switch(message) {
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
	
	if(keyEventStackPos != 0) {
		keyEventStackPos--;
		_window->event = keyEventStack[keyEventStackPos];
		return true;
	}

	if(_window->sizeChanged) {
		_window->sizeChanged = false;
		_window->event.type = CC_EVENT_WINDOW_RESIZE;
		return true;
	}
	
	if(PeekMessage(&_window->msg, _window->winHandle, 0, 0, PM_REMOVE)){
		TranslateMessage(&_window->msg);
		DispatchMessage(&_window->msg);
		return _window->event.type == CC_EVENT_SKIP?false:true;
	}

	return false;
}

void ccNewWindow(ccRect rect, const char* title, int flags)
{
	HMODULE moduleHandle = GetModuleHandle(NULL);
	RECT windowRect;

	ccAssert(_window == NULL);

	keyEventStackSize = 1;
	keyEventStackPos = 0;
	keyEventStack = malloc(sizeof(ccEvent));

	//initialize struct
	_window = malloc(sizeof(ccWindow));
	memcpy(&_window->rect, &rect, sizeof(ccRect));
	_window->sizeChanged = true;

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

	_window->llKeyHook = SetWindowsHookEx(WH_KEYBOARD_LL, llKeyProc, NULL, 0);

	_window->style |= WS_VISIBLE;

	ShowWindow(_window->winHandle, SW_SHOW);

	if((flags & CC_WINDOW_FLAG_ALWAYSONTOP) == CC_WINDOW_FLAG_ALWAYSONTOP) {
		RECT rect;
		GetWindowRect(_window->winHandle, &rect);
		SetWindowPos(_window->winHandle, HWND_TOPMOST, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, SWP_SHOWWINDOW);
	}
}

void ccFreeWindow()
{
	ccAssert(_window != NULL);

	free(keyEventStack);

	UnhookWindowsHookEx(_window->llKeyHook);
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