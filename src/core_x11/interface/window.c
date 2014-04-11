#include "../../core/interface/window.h"

ccWindow *ccNewWindow(unsigned short width, unsigned short height, const char *title)
{
	ccWindow *output;
	Window root;
	int screen;

	output = malloc(sizeof(ccWindow));

	output->display = XOpenDisplay(NULL);
	if(output->display == NULL){
		return NULL;
	}
	/* TODO handle NULL */

	root = DefaultRootWindow(output->display);

	screen = DefaultScreen(output->display);
	output->window = XCreateSimpleWindow(output->display, root, 10, 10, width, height, 1,
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

bool ccPollEventWindow(ccWindow *window)
{
	
	return false;
}
