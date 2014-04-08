#include "../../core/interface/window.h"

ccWindow *ccNewWindow(unsigned short width, unsigned short height, const char *title)
{
	ccWindow *output;
	int screen;

	output = malloc(sizeof(output));

	output->display = XOpenDisplay(NULL);
	/* TODO handle NULL */

	screen = DefaultScreen(output->display);
	output->window = XCreateSimpleWindow(output->display, RootWindow(output->display, screen), 10, 10, width, height, 1,
		   	BlackPixel(output->display, screen), WhitePixel(output->display, screen));
	XSelectInput(output->display, output->window, ExposureMask | KeyPressMask);
	XMapWindow(output->display, output->window);

	return output;
}

void ccFreeWindow(ccWindow *window)
{
	XCloseDisplay(window->display);
	free(window);
}
