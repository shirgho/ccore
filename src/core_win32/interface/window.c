#include "../../core/interface/window.h"

static ccDisplays displays;

ccKeyCode translateKey(WPARAM wParam)
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
	}

	return CC_KEY_UNDEFINED;
}

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
		activeWindow->event.type = CC_EVENT_KEY_DOWN;
		activeWindow->event.key = translateKey(wParam);
		break;
	case WM_KEYUP:
		activeWindow->event.type = CC_EVENT_KEY_UP;
		activeWindow->event.key = translateKey(wParam);
		break;
	case WM_MOUSEMOVE:
		activeWindow->mouse.x = (unsigned short)lParam & 0x0000FFFF;
		activeWindow->mouse.y = (unsigned short)((lParam & 0xFFFF0000) >> 16);
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
		activeWindow->event.type = CC_EVENT_MOUSE_SCROLL;
		activeWindow->event.scrollDelta = GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA;
		break;
	case WM_SETFOCUS:
		activeWindow->event.type = CC_EVENT_FOCUS_GAINED;
		break;
	case WM_KILLFOCUS:
		activeWindow->event.type = CC_EVENT_FOCUS_LOST;
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
		TranslateMessage(&window->msg);
		DispatchMessage(&window->msg);
		return window->event.type==CC_EVENT_SKIP?false:true;
	}
	else{
		return false;
	}
}

ccWindow *ccNewWindow(ccDisplay *display, unsigned short width, unsigned short height, const char* title, int flags)
{
	ccWindow *newWindow = malloc(sizeof(ccWindow));

	//Struct initialisation
	newWindow->width = width;
	newWindow->height = height;
	newWindow->sizeChanged = true;
	newWindow->display = display;

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
	switch(mode)
	{
	case CC_WINDOW_MODE_VISIBLE:
		ShowWindow(window->winHandle, SW_SHOW);
		break;
	case CC_WINDOW_MODE_INVISIBLE:
		ShowWindow(window->winHandle, SW_HIDE);
		break;
	case CC_WINDOW_MODE_MINIMIZED:
		ShowWindow(window->winHandle, SW_MINIMIZE);
		break;
	case CC_WINDOW_MODE_WINDOW:
		SetWindowLongPtr(window->winHandle, GWL_STYLE, WS_OVERLAPPEDWINDOW | WS_VISIBLE);
		break;
	case CC_WINDOW_MODE_FULLSCREEN:
		window->width = GetSystemMetrics(SM_CXSCREEN);
		window->height = GetSystemMetrics(SM_CYSCREEN);

		SetWindowLongPtr(window->winHandle, GWL_STYLE, WS_SYSMENU | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE);
		MoveWindow(window->winHandle, 0, 0, window->width, window->height, TRUE);
		break;
	case CC_WINDOW_MODE_MAXIMIZED:
		ShowWindow(window->winHandle, SW_MAXIMIZE);
		break;
	}
}

BOOL CALLBACK addMonitor(__in HMONITOR hMonitor, __in HDC hdcMonitor, __in LPRECT lprcMonitor, __in LPARAM dwData)
{
	ccDisplay *currentDisplay;
	displays.amount++;
	if(displays.amount == 1) {
		displays.display = malloc(sizeof(ccDisplay));
	}
	else {
		displays.display = realloc(displays.display, sizeof(ccDisplay)*displays.amount);
	}

	currentDisplay = &displays.display[displays.amount - 1];
	currentDisplay->x = lprcMonitor->left;
	currentDisplay->y = lprcMonitor->top;
	currentDisplay->currentDisplayData.width = lprcMonitor->right - lprcMonitor->left;
	currentDisplay->currentDisplayData.height = lprcMonitor->bottom - lprcMonitor->top;

	return TRUE;
}

void ccFindDisplays()
{
	displays.amount = 0;
	EnumDisplayMonitors(NULL, NULL, addMonitor, 0);
}

ccDisplays *ccGetDisplays()
{
	return &displays;
}

ccDisplay *ccGetDefaultDisplay()
{
	return NULL;
}
/*
ccResolutions *ccGetResolutions(ccDisplay display) {
	DEVMODE dm;
	ccResolutions *resolutions = malloc(sizeof(ccResolutions));

	ZeroMemory(&dm, sizeof(dm));
	dm.dmSize = sizeof(dm);

	resolutions->amount = 0;
	resolutions->screenData = NULL;

	for(int i = 0; EnumDisplaySettings(NULL, i, &dm) != 0; i++) {
		if(resolutions->amount == 0 ||
			(resolutions->screenData[resolutions->amount - 1].width != dm.dmPelsWidth || resolutions->screenData[resolutions->amount - 1].height != dm.dmPelsHeight)) {
			
			if(resolutions->screenData == NULL) {
				resolutions->screenData = malloc(sizeof(ccScreenData));
			}
			else{
				resolutions->screenData = realloc(resolutions->screenData, (resolutions->amount + 1) * sizeof(ccScreenData));
			}

			resolutions->screenData[resolutions->amount].width = dm.dmPelsWidth;
			resolutions->screenData[resolutions->amount].height = dm.dmPelsHeight;
			resolutions->screenData[resolutions->amount].refreshRate = dm.dmDisplayFrequency;
			resolutions->screenData[resolutions->amount].bitDepth = dm.dmBitsPerPel;
			resolutions->amount++;
		}
	}

	return resolutions;
}
*/
void ccFreeResolutions(ccResolutions *resolutions) {
	if(resolutions->amount != 0) free(resolutions->displayData);
	free(resolutions);
}
/*
void ccSetResolution(ccDisplay display, ccDisplayData *screenData) {
	DEVMODE dm;
	ZeroMemory(&dm, sizeof(dm));
	dm.dmSize = sizeof(dm);
	EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &dm);

	dm.dmPelsWidth = screenData->width;
	dm.dmPelsHeight = screenData->height;
	dm.dmFields = (DM_PELSWIDTH | DM_PELSHEIGHT);

	if(ChangeDisplaySettings(&dm, CDS_TEST) != DISP_CHANGE_SUCCESSFUL) {
		//TODO: throw error
		printf("Couldn't change display mode");
	}
}
*/