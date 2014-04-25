#include "../../core/interface/window.h"

LRESULT CALLBACK wndProc(HWND winHandle, UINT message, WPARAM wParam, LPARAM lParam)
{
	ccWindow *activeWindow = (ccWindow*)GetWindowLong(winHandle, GWL_USERDATA);
	if(activeWindow == NULL) goto skipevent;

	activeWindow->event.type = CC_EVENT_SKIP;

	switch(message) {
	case WM_CLOSE:
		activeWindow->event.type = CC_EVENT_WINDOW_QUIT;
		break;
	case WM_SIZE:
		activeWindow->width = lParam & 0x0000FFFF;
		activeWindow->height = (lParam & 0xFFFF0000) >> 16;
		activeWindow->aspect = (float) activeWindow->width / activeWindow->height;
		activeWindow->sizeChanged = true;
		break;
	case WM_KEYDOWN:
		//TODO: save keycode
		activeWindow->event.type = CC_EVENT_KEY_DOWN;
		break;
	case WM_KEYUP:
		//TODO: save keycode
		activeWindow->event.type = CC_EVENT_KEY_UP;
		break;
	case WM_MOUSEMOVE:
		activeWindow->mouseX = (unsigned short)lParam & 0x0000FFFF;
		activeWindow->mouseY = (unsigned short)lParam & 0xFFFF0000;
		activeWindow->event.type = CC_EVENT_MOUSE_MOVE;
		break;
	case WM_LBUTTONDOWN:
		activeWindow->event.type = CC_EVENT_MOUSE_DOWN;
		activeWindow->event.mouseButton = CC_MOUSE_BUTTON_LEFT;
		break;
	case WM_MBUTTONDOWN:
		activeWindow->event.type = CC_EVENT_MOUSE_DOWN;
		activeWindow->event.mouseButton = CC_MOUSE_BUTTON_MIDDLE;
		break;
	case WM_RBUTTONDOWN:
		activeWindow->event.type = CC_EVENT_MOUSE_DOWN;
		activeWindow->event.mouseButton = CC_MOUSE_BUTTON_RIGHT;
		break;
	case WM_LBUTTONUP:
		activeWindow->event.type = CC_EVENT_MOUSE_UP;
		activeWindow->event.mouseButton = CC_MOUSE_BUTTON_LEFT;
		break;
	case WM_MBUTTONUP:
		activeWindow->event.type = CC_EVENT_MOUSE_UP;
		activeWindow->event.mouseButton = CC_MOUSE_BUTTON_MIDDLE;
		break;
	case WM_RBUTTONUP:
		activeWindow->event.type = CC_EVENT_MOUSE_UP;
		activeWindow->event.mouseButton = CC_MOUSE_BUTTON_RIGHT;
		break;
	case WM_MOUSEWHEEL:
		activeWindow->event.type = CC_EVENT_MOUSE_SCROLL_UP;
		activeWindow->event.scrollDelta = GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA;
		break;
	default:
	skipevent:
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
	winClass.hIcon = NULL;
	winClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	winClass.hbrBackground = NULL;
	winClass.lpszMenuName = NULL;
	winClass.lpszClassName = "ccWindow";
	winClass.hIconSm = NULL;

	RegisterClassEx(&winClass);
}

bool ccPollEvent(ccWindow *window)
{
	if(window->sizeChanged) {
		window->sizeChanged = false;
		window->event.type = CC_EVENT_WINDOW_RESIZE;
		return true;
	}

	if(PeekMessage(&window->msg, window->winHandle, 0, 0, PM_REMOVE)){
		DispatchMessage(&window->msg);
		return window->event.type==CC_EVENT_SKIP?false:true;
	}
	else{
		return false;
	}
}

ccWindow *ccNewWindow(unsigned short width, unsigned short height, const char* title, int flags)
{
	ccWindow *newWindow = malloc(sizeof(ccWindow));

	//Struct initialisation
	newWindow->width = width;
	newWindow->height = height;
	newWindow->sizeChanged = true;

	//Window creation
	HMODULE moduleHandle = GetModuleHandle(NULL);
	HWND desktopHandle = GetDesktopWindow();
	RECT desktopRect;

	GetWindowRect(desktopHandle, &desktopRect);
	
	regHinstance(moduleHandle);
	newWindow->winHandle = CreateWindowEx(
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

	SetWindowLong(newWindow->winHandle, GWL_USERDATA, (LONG)newWindow);
	
	//apply flags
	if((flags & CC_WINDOW_FLAG_NORESIZE) == CC_WINDOW_FLAG_NORESIZE) {
		LONG lStyle = GetWindowLong(newWindow->winHandle, GWL_STYLE);
		lStyle &= ~(WS_THICKFRAME | WS_MAXIMIZEBOX);
		SetWindowLong(newWindow->winHandle, GWL_STYLE, lStyle);
	}
	if((flags & CC_WINDOW_FLAG_NOBUTTONS) == CC_WINDOW_FLAG_NOBUTTONS) {
		LONG lStyle = GetWindowLong(newWindow->winHandle, GWL_STYLE);
		lStyle &= ~WS_SYSMENU;
		SetWindowLong(newWindow->winHandle, GWL_STYLE, lStyle);
	}
	if((flags & CC_WINDOW_FLAG_ALWAYSONTOP) == CC_WINDOW_FLAG_ALWAYSONTOP) {
		RECT rect;
		GetWindowRect(newWindow->winHandle, &rect);
		SetWindowPos(newWindow->winHandle, HWND_TOPMOST, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, SWP_SHOWWINDOW);
	}

	return newWindow;
}

void ccFreeWindow(ccWindow *window)
{
	wglDeleteContext(window->renderContext);
	ReleaseDC(window->winHandle, window->hdc);

	DestroyWindow(window->winHandle);
	free(window);
}

void ccGLMakeCurrent(ccWindow *window)
{
	wglMakeCurrent(window->hdc, window->renderContext);
}

void ccGLBindContext(ccWindow *window, int glVersionMajor, int glVersionMinor)
{
	int pixelFormatIndex;
	window->hdc = GetDC(window->winHandle);

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

	pixelFormatIndex = ChoosePixelFormat(window->hdc, &pfd);
	SetPixelFormat(window->hdc, pixelFormatIndex, &pfd);

	window->renderContext = wglCreateContext(window->hdc);
	if(window->renderContext == NULL) ccAbort("openGL could not be initialized.\nThis could happen because your openGL version is too old.");
	
	ccGLMakeCurrent(window);

	//Fetch extentions after context creation
	if(glewInit() != GLEW_OK) ccAbort("GLEW could not be initialized.");
}

void ccGLSwapBuffers(ccWindow *window)
{
	SwapBuffers(window->hdc);
}

void ccChangeWM(ccWindow *window, ccWindowMode mode)
{
	int sw;
	LONG lStyle = GetWindowLong(window->winHandle, GWL_STYLE);

	switch(mode)
	{
	case CC_WINDOW_MODE_MINIMIZED:
		sw = SW_SHOWMINIMIZED;
		break;
	case CC_WINDOW_MODE_WINDOW:
		sw = SW_SHOW;
		break;
	case CC_WINDOW_MODE_FULLSCREEN:
		lStyle &= ~WS_CAPTION;
	default:
		sw = SW_SHOWMAXIMIZED;
		break;
	}

	SetWindowLong(window->winHandle, GWL_STYLE, lStyle);
	ShowWindow(window->winHandle, sw);
}