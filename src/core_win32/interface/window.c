#include "../../core/interface/window.h"

static ccWindow *_activeWindow = NULL;

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
	winClass.style = 0;
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

bool ccPollEventWindow(ccWindow *window)
{
	if(PeekMessage(&_activeWindow->msg, _activeWindow->winHandle, 0, 0, PM_REMOVE))
	{
		DispatchMessage(&_activeWindow->msg);
		return ccEventSkip?false:true;
	}
	else{
		return false;
	}
}

ccWindow *ccNewWindow(unsigned short width, unsigned short height, const char* title)
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
	_activeWindow->winHandle = CreateWindowEx(
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

	ShowWindow(_activeWindow->winHandle, SW_SHOW);
	UpdateWindow(_activeWindow->winHandle);
	SetFocus(_activeWindow->winHandle);

	return _activeWindow;
}

void ccFreeWindow(ccWindow *window)
{
	free(_activeWindow);
	_activeWindow = NULL;
}
