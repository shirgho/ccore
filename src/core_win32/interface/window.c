#include "../../core/interface/window.h"

static ccWindow *_activeWindow = NULL;
static HDC _hdc;
static MSG _msg;
static HWND _winHandle;
static HGLRC _renderContext;

LRESULT CALLBACK wndProc(HWND winHandle, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message) {
	case WM_CLOSE:
		_activeWindow->event.type = ccEventWindowQuit;
		break;
	case WM_SIZE:
	case WM_SIZING:
		printf(".");
		_activeWindow->event.type = ccEventSkip;
		if(message != WM_EXITSIZEMOVE) {
			unsigned short newWidth = lParam & 0x0000FFFF;
			unsigned short newHeight = (lParam & 0xFFFF0000) >> 16;
			if(_activeWindow->width == newWidth && _activeWindow->height == newHeight) {
				_activeWindow->event.type = ccEventSkip;
			}
			else{
				_activeWindow->width = newWidth;
				_activeWindow->height = newHeight;
				_activeWindow->event.type = ccEventWindowResize;
			}
		}

		break;
	case WM_EXITSIZEMOVE:

		break;
	case WM_KEYDOWN:
		//TODO: save keycode
		_activeWindow->event.type = ccEventKeyDown;
		break;
	case WM_KEYUP:
		//TODO: save keycode
		_activeWindow->event.type = ccEventKeyUp;
		break;
	case WM_MOUSEMOVE:
		//TODO: save coordinates
		_activeWindow->event.type = ccEventMouseMove;
		break;
	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
		//TODO: save button index
		_activeWindow->event.type = ccEventMouseDown;
		break;
	case WM_LBUTTONUP:
	case WM_MBUTTONUP:
	case WM_RBUTTONUP:
		//TODO: save button index
		_activeWindow->event.type = ccEventMouseUp;
		break;
	case WM_MOUSEWHEEL:
		//TODO: save direction
		_activeWindow->event.type = ccEventMouseScrollUp;
		break;
	default:
		_activeWindow->event.type = ccEventSkip;
		return DefWindowProc(winHandle, message, wParam, lParam);
		break;
	}
	return 0;
}

void regHinstance(HINSTANCE instanceHandle)
{
	WNDCLASSEX winClass;

	winClass.cbSize = sizeof(WNDCLASSEX);
	winClass.style = CS_OWNDC;
	winClass.lpfnWndProc = wndProc;
	winClass.cbClsExtra = 0;
	winClass.cbWndExtra = 0;
	winClass.hInstance = instanceHandle;
	winClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	winClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	winClass.hbrBackground = (HBRUSH)COLOR_WINDOW;
	winClass.lpszMenuName = NULL;
	winClass.lpszClassName = "ccWindow";
	winClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	RegisterClassEx(&winClass);
}

bool ccPollEvent(ccWindow *window)
{
	if(PeekMessage(&_msg, _winHandle, 0, 0, PM_REMOVE)){
		DispatchMessage(&_msg);
		return window->event.type==ccEventSkip?false:true;
	}
	else{
		return false;
	}
}

ccWindow *ccNewWindow(unsigned short width, unsigned short height, const char* title, ccWindowMode mode, int flags)
{
	if(_activeWindow!=NULL) {
		ccAbort("Only one window can be created!");
		exit(0);
	}

	_activeWindow = malloc(sizeof(ccWindow));

	//Struct initialisation
	_activeWindow->width = width;
	_activeWindow->height = height;

	//Window creation
	HMODULE moduleHandle = GetModuleHandle(NULL);
	HWND desktopHandle = GetDesktopWindow();
	RECT desktopRect;

	GetWindowRect(desktopHandle, &desktopRect);
	
	regHinstance(moduleHandle);
	_winHandle = CreateWindowEx(
		WS_EX_APPWINDOW,
		"ccWindow",
		title,
		WS_OVERLAPPEDWINDOW,
		(desktopRect.right >> 1) - (width >> 1),
		(desktopRect.bottom >> 1) - (height >> 1),
		width, height,
		desktopHandle,
		NULL,
		moduleHandle,
		NULL);
	
	//apply flags
	if((flags & ccWFNoResize) == ccWFNoResize) {
		LONG lStyle = GetWindowLong(_winHandle, GWL_STYLE);
		lStyle &= ~(WS_THICKFRAME | WS_MAXIMIZEBOX);
		SetWindowLong(_winHandle, GWL_STYLE, lStyle);
	}
	if((flags & ccWFNoButtons) == ccWFNoButtons) {
		LONG lStyle = GetWindowLong(_winHandle, GWL_STYLE);
		lStyle &= ~WS_SYSMENU;
		SetWindowLong(_winHandle, GWL_STYLE, lStyle);
	}
	if((flags & ccWFAlwaysOnTop) == ccWFAlwaysOnTop) {
		RECT rect;
		GetWindowRect(_winHandle, &rect);
		SetWindowPos(_winHandle, HWND_TOPMOST, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, SWP_SHOWWINDOW);
	}

	ccChangeWM(_activeWindow, mode);
	return _activeWindow;
}

void ccFreeWindow(ccWindow *window)
{
	wglDeleteContext(_renderContext);
	ReleaseDC(_winHandle, _hdc);

	DestroyWindow(_winHandle);
	free(window);
	_activeWindow = NULL;
}

void ccGLBindContext(ccWindow *window, int glVersionMajor, int glVersionMinor)
{
	int pixelFormatIndex;
	_hdc = GetDC(_winHandle);

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

	pixelFormatIndex = ChoosePixelFormat(_hdc, &pfd);
	SetPixelFormat(_hdc, pixelFormatIndex, &pfd);

	_renderContext = wglCreateContext(_hdc);
	if(_renderContext == NULL) ccAbort("openGL could not be initialized.\nThis could happen because your openGL version is too old.");
	wglMakeCurrent(_hdc, _renderContext);

	//Fetch extentions after context creation
	if(glewInit() != GLEW_OK) ccAbort("GLEW could not be initialized.");
}

void ccGLSwapBuffers(ccWindow *window)
{
	SwapBuffers(_hdc);
}

void ccChangeWM(ccWindow *window, ccWindowMode mode)
{
	int sw;
	LONG lStyle = GetWindowLong(_winHandle, GWL_STYLE);

	switch(mode)
	{
	case ccWMMinimized:
		sw = SW_SHOWMINIMIZED;
		break;
	case ccWMWindow:
		sw = SW_SHOW;
		break;
	case ccWMFullScreen:
		lStyle &= ~WS_CAPTION;
	default:
		sw = SW_SHOWMAXIMIZED;
		break;
	}

	SetWindowLong(_winHandle, GWL_STYLE, lStyle);
	ShowWindow(_winHandle, sw);
}