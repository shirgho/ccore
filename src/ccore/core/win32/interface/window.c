#include "../../common/interface/window.h"

//note that static pointers are NULL by default
static ccWindow *_window;

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

static LRESULT CALLBACK wndProc(HWND winHandle, UINT message, WPARAM wParam, LPARAM lParam)
{
	if(_window == NULL) goto skipevent;

	_window->event.type = CC_EVENT_SKIP;

	switch(message) {
	case WM_CLOSE:
		_window->event.type = CC_EVENT_WINDOW_QUIT;
		break;
	case WM_SIZE:
		_window->rect.width = lParam & 0x0000FFFF;
		_window->rect.height = (lParam & 0xFFFF0000) >> 16;
		_window->aspect = (float)_window->rect.width / _window->rect.height;
		_window->sizeChanged = true;
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

ccWindow *ccGetWindow()
{
	return _window;
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
	ccAssert(_window == NULL);

	_window = malloc(sizeof(ccWindow));

	//Struct initialisation
	memcpy(&_window->rect, &rect, sizeof(ccRect));
	_window->sizeChanged = true;

	//Window creation
	HMODULE moduleHandle = GetModuleHandle(NULL);
	
	regHinstance(moduleHandle);
	_window->winHandle = CreateWindowEx(
		WS_EX_APPWINDOW,
		"ccWindow",
		title,
		WS_OVERLAPPEDWINDOW,
		rect.x, rect.y,
		rect.width, rect.height,
		NULL,
		NULL,
		moduleHandle,
		NULL);

	//apply flags
	if((flags & CC_WINDOW_FLAG_NORESIZE) == CC_WINDOW_FLAG_NORESIZE) {
		LONG lStyle = GetWindowLong(_window->winHandle, GWL_STYLE);
		lStyle &= ~(WS_THICKFRAME | WS_MAXIMIZEBOX);
		SetWindowLong(_window->winHandle, GWL_STYLE, lStyle);
	}
	if((flags & CC_WINDOW_FLAG_NOBUTTONS) == CC_WINDOW_FLAG_NOBUTTONS) {
		LONG lStyle = GetWindowLong(_window->winHandle, GWL_STYLE);
		lStyle &= ~WS_SYSMENU;
		SetWindowLong(_window->winHandle, GWL_STYLE, lStyle);
	}
	if((flags & CC_WINDOW_FLAG_ALWAYSONTOP) == CC_WINDOW_FLAG_ALWAYSONTOP) {
		RECT rect;
		GetWindowRect(_window->winHandle, &rect);
		SetWindowPos(_window->winHandle, HWND_TOPMOST, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, SWP_SHOWWINDOW);
	}

	ShowWindow(_window->winHandle, SW_SHOW);
	ccChangeWM(CC_WINDOW_MODE_WINDOW);
}

void ccFreeWindow()
{
	ccAssert(_window != NULL);

	wglDeleteContext(_window->renderContext);
	ReleaseDC(_window->winHandle, _window->hdc);

	DestroyWindow(_window->winHandle);
	free(_window);
}

ccError ccGLBindContext(int glVersionMajor, int glVersionMinor)
{
	int pixelFormatIndex;
	int glVerMajor, glVerMinor;

	ccAssert(_window != NULL);

	_window->hdc = GetDC(_window->winHandle);

	PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
		PFD_TYPE_RGBA,
		32,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		16,
		0, 0, 0, 0, 0, 0, 0
	};

	pixelFormatIndex = ChoosePixelFormat(_window->hdc, &pfd);
	SetPixelFormat(_window->hdc, pixelFormatIndex, &pfd);

	_window->renderContext = wglCreateContext(_window->hdc);
	if(_window->renderContext == NULL) return CC_ERROR_GLCONTEXT;

	//Make window the current context
	wglMakeCurrent(_window->hdc, _window->renderContext);

	//Version check
	glGetIntegerv(GL_MAJOR_VERSION, &glVerMajor);
	glGetIntegerv(GL_MINOR_VERSION, &glVerMinor);
	if(glVerMajor < glVersionMajor || (glVerMajor == glVersionMajor && glVerMinor < glVersionMinor)) return CC_ERROR_GLVERSION;

	//Fetch extentions after context creation
	if(glewInit() != GLEW_OK) return CC_ERROR_GLEWINIT;

	return CC_ERROR_NONE;
}

void ccGLSwapBuffers()
{
	ccAssert(_window != NULL);

	SwapBuffers(_window->hdc);
}

void ccChangeWM(ccWindowMode mode)
{
	ccAssert(_window != NULL);

	switch(mode)
	{
	case CC_WINDOW_MODE_MINIMIZED:
		ShowWindow(_window->winHandle, SW_MINIMIZE);
		break;
	case CC_WINDOW_MODE_WINDOW:
		ShowWindow(_window->winHandle, SW_SHOWDEFAULT);
		SetWindowLongPtr(_window->winHandle, GWL_STYLE, WS_OVERLAPPEDWINDOW | WS_VISIBLE);
		break;
	case CC_WINDOW_MODE_FULLSCREEN:
		ShowWindow(_window->winHandle, SW_SHOWDEFAULT);
		_window->rect.width = ccGetResolutionCurrent(_window->display)->width;
		_window->rect.height = ccGetResolutionCurrent(_window->display)->height;

		SetWindowLongPtr(_window->winHandle, GWL_STYLE, WS_SYSMENU | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE);
		ccResizeMoveWindow((ccRect){ _window->display->x, _window->display->y, _window->rect.width, _window->rect.height });
		break;
	case CC_WINDOW_MODE_MAXIMIZED:
		ShowWindow(_window->winHandle, SW_MAXIMIZE);
		break;
	}
}

void ccResizeMoveWindow(ccRect rect)
{
	ccAssert(_window != NULL);

	_window->rect = rect;
	MoveWindow(_window->winHandle, rect.x, rect.y, rect.width, rect.height, TRUE);
}

void ccCenterWindow()
{
	ccAssert(_window != NULL);

	ccResizeMoveWindow(
		(ccRect){_window->display->x + ((ccGetResolutionCurrent(_window->display)->width - _window->rect.width) >> 1),
				 _window->display->y + ((ccGetResolutionCurrent(_window->display)->height - _window->rect.height) >> 1),
				 _window->rect.width,
				 _window->rect.height
	});
}