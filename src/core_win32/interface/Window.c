#include "Window.h"

//Private

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
	winClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	winClass.lpszMenuName = NULL;
	winClass.lpszClassName = "MainWindow";
	winClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	RegisterClassEx(&winClass);
}

//Public

ccWindow *ccNewWindow(unsigned short width, unsigned short height, const char* title)
{
	ccWindow *w = malloc(sizeof(ccWindow));

	//Window creation
	HMODULE hInstance = GetModuleHandle(NULL);

	HWND winHandle;

	regHinstance(hInstance);
	winHandle = CreateWindowEx(
		WS_EX_APPWINDOW,
		"MainWindow",
		title,
		WS_OVERLAPPEDWINDOW,
		0, 0, width, height,
		GetDesktopWindow(),
		NULL,
		hInstance,
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