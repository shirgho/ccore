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

static ccKeyCode translateKey(WPARAM wParam)
{
	if((wParam >= 'A' && wParam <= 'Z') || (wParam >= '0' && wParam <= '9')) {
		return wParam;
	}

	switch(wParam)
	{
	case VK_BACK:
		return CC_KEY_BACKSPACE;
		break;
	case VK_TAB:
		return CC_KEY_TAB;
		break;
	case VK_RETURN:
		return CC_KEY_RETURN;
		break;
	case VK_ESCAPE:
		return CC_KEY_ESCAPE;
		break;
	case VK_SPACE:
		return CC_KEY_SPACE;
		break;
	case VK_LSHIFT:
		return CC_KEY_LSHIFT;
		break;
	case VK_RSHIFT:
		return CC_KEY_RSHIFT;
		break;
	case VK_LCONTROL:
		return CC_KEY_LCONTROL;
		break;
	case VK_RCONTROL:
		return CC_KEY_RCONTROL;
		break;
	case VK_LEFT:
		return CC_KEY_LEFT;
		break;
	case VK_RIGHT:
		return CC_KEY_RIGHT;
		break;
	case VK_UP:
		return CC_KEY_UP;
		break;
	case VK_DOWN:
		return CC_KEY_DOWN;
		break;
	case VK_CAPITAL:
		return CC_KEY_CAPSLOCK;
		break;
	case VK_INSERT:
		return CC_KEY_INSERT;
		break;
	case VK_DELETE:
		return CC_KEY_DELETE;
		break;
	case VK_HOME:
		return CC_KEY_HOME;
		break;
	case VK_END:
		return CC_KEY_END;
		break;
	case VK_PRIOR:
		return CC_KEY_PAGEUP;
		break;
	case VK_NEXT:
		return CC_KEY_PAGEDOWN;
		break;
	case VK_SNAPSHOT:
		return CC_KEY_PRINTSCREEN;
		break;
	case VK_SCROLL:
		return CC_KEY_SCROLLLOCK;
		break;
	case VK_PAUSE:
		return CC_KEY_PAUSEBREAK;
		break;
	case VK_NUMLOCK:
		return CC_KEY_NUMLOCK;
		break;
	}

	if(wParam >= VK_F1 && wParam <= VK_F12) {
		return CC_KEY_F1 + wParam - VK_F1;
	}

	if(wParam >= VK_NUMPAD0 && wParam <= VK_NUMPAD9) {
		return CC_KEY_NUM0 + wParam - VK_NUMPAD0;
	}

	return CC_KEY_UNDEFINED;
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
	_window->rect.width = winRect.right - winRect.left;
	_window->rect.height = winRect.bottom - winRect.top;
	_window->aspect = (float)_window->rect.width / _window->rect.height;
	_window->sizeChanged = true;
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
	case WM_STYLECHANGED:
		updateWindowResolution();
		break;
	case WM_MOVE:
		updateWindowDisplay(_window);
		break;
	case WM_KEYDOWN:
		_window->event.type = CC_EVENT_KEY_DOWN;
		_window->event.key = translateKey(wParam);
		break;
	case WM_KEYUP:
		_window->event.type = CC_EVENT_KEY_UP;
		_window->event.key = translateKey(wParam);
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
	
	if(PeekMessage(&_window->msg, _window->winHandle, 0, 0, PM_REMOVE)){
		TranslateMessage(&_window->msg);
		DispatchMessage(&_window->msg);
		return _window->event.type == CC_EVENT_SKIP?false:true;
	}
	else{
		return false;
	}
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

	if((flags & CC_WINDOW_FLAG_ALWAYSONTOP) == CC_WINDOW_FLAG_ALWAYSONTOP) {
		RECT rect;
		GetWindowRect(_window->winHandle, &rect);
		SetWindowPos(_window->winHandle, HWND_TOPMOST, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, SWP_SHOWWINDOW);
	}
}

void ccFreeWindow()
{
	ccAssert(_window != NULL);

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
		ShowWindow(_window->winHandle, SW_SHOWDEFAULT);
		ccCenterWindow();
		break;
	case CC_WINDOW_MODE_FULLSCREEN:
		SetWindowLongPtr(_window->winHandle, GWL_STYLE, _window->style & ~(WS_CAPTION | WS_THICKFRAME));
		ShowWindow(_window->winHandle, SW_MAXIMIZE);
		break;
	case CC_WINDOW_MODE_MAXIMIZED:
		SetWindowLongPtr(_window->winHandle, GWL_STYLE, _window->style | WS_CAPTION);
		ShowWindow(_window->winHandle, SW_MAXIMIZE);
		break;
	}
}

void ccResizeMoveWindow(ccRect rect)
{
	RECT windowRect;

	ccAssert(_window != NULL);

	MoveWindow(_window->winHandle, rect.x, rect.y, rect.width, rect.height, FALSE);
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
	});
}