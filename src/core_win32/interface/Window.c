#include "Window.h"

LRESULT CALLBACK wndProc(HWND Hw, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	switch(Msg) {
	default:
		return DefWindowProc(Hw, Msg, wParam, lParam);
		break;
	}
}

void regHinstance(HINSTANCE HIn)
{
	WNDCLASSEX winClass;

	winClass.cbSize = sizeof(WNDCLASSEX);
	winClass.style = 0;
	winClass.lpfnWndProc = wndProc;
	winClass.cbClsExtra = 0;
	winClass.cbWndExtra = 0;
	winClass.hInstance = HIn;
	winClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	winClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	winClass.hbrBackground = (HBRUSH)COLOR_WINDOW;
	winClass.lpszMenuName = NULL;
	winClass.lpszClassName = "CCoreWindow";
	winClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	RegisterClassEx(&winClass);
}

ccWindow *ccNewWindow(unsigned short width, unsigned short height, const char* title)
{
	ccWindow *w = malloc(sizeof(ccWindow));

	//Struct initialisation

	//Window creation
	HMODULE instanceHandle = GetModuleHandle(NULL);
	HWND winHandle;
	HWND desktopHandle = GetDesktopWindow();
	RECT desktopRect;

	GetWindowRect(desktopHandle, &desktopRect);

	regHinstance(instanceHandle);
	winHandle = CreateWindowEx(
		WS_EX_APPWINDOW,
		"CCoreWindow",
		title,
		WS_OVERLAPPEDWINDOW,
		(desktopRect.right >> 1) - (width >> 1),
		(desktopRect.bottom >> 1) - (height >> 1),
		width, height,
		desktopHandle,
		NULL,
		instanceHandle,
		NULL);

	ShowWindow(winHandle, SW_SHOW);
	UpdateWindow(winHandle);
	SetFocus(winHandle);

	return w;
}

void ccFreeWindow(ccWindow *w)
{
	free(w);
}