#ifndef _DEBUG
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#endif

#include <stdio.h>
#include <stdlib.h>
#include "../src/core/interface/window.h"
#include "../src/core/interface/popup.h"
#include "../src/core/utils/timing.h"
#include "../src/core/interface/event.h"

int main(int argc, char** argv)
{
	printf("entry point\n");
	
	ccWindow *window = ccNewWindow(1024, 768, "CCore application", CC_WINDOW_MODE_WINDOW, 0);
	
	ccGLBindContext(window, 2, 1);
	
	glClearColor(1, 50, .2f, 1);

	bool running = true;
	while(running) {
		ccDelay(15);

		while(ccPollEvent(window)) {
			switch(window->event.type) {
			case CC_EVENT_WINDOW_QUIT:
				if(ccShowDialogue("Really quit?", "quit", CC_DIALOGUE_YESNO)) running = false;
				break;
			case CC_EVENT_WINDOW_RESIZE:
				printf("%d %d\n", window->width, window->height);
				glViewport(0, 0, window->width, window->height); 
				break;
			case CC_EVENT_KEY_DOWN:
				ccChangeWM(window, CC_WINDOW_MODE_MAXIMIZED);
				break;
			}
		}
		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(45.0f, 4.0f / 3.0f, 0.2f, 255.0f);
		glLoadIdentity();
		
		glBegin(GL_TRIANGLES);
		glVertex3f(0, 0, 0);
		glColor3f(1, 0, 0);
		glVertex3f(1, 0, 0);
		glColor3f(0, 1, 0);
		glVertex3f(0, -1, 0);
		glColor3f(0, 0, 1);
		glEnd();

		ccGLSwapBuffers(window);
	}

	ccFreeWindow(window);
	
	return 0;
}