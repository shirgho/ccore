#if !defined _DEBUG && defined _WIN32
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#endif 

#include <stdio.h>
#include <stdlib.h>

#include "../src/core/core.h"

#include <GL/gl.h>
#include <GL/glu.h>

#include "../src/core/interface/window.h"
#include "../src/core/interface/popup.h"
#include "../src/core/utils/timing.h"
#include "../src/core/interface/event.h"

float rotQuad = 0.0f;

void resizeGL(unsigned int width, unsigned int height);
void renderGL();

int main(int argc, char** argv)
{
	ccWindow *window;
	bool quit;
	ccResolutions *resolutions;
	int i;

	//Find displays and print their stats
	ccFindDisplays();
	for(i = 0; i < ccGetDisplays()->amount; i++) {
		printf("\n%s\n%s\n%d\t%d\t%dx%d\t%dbpp\t%dHz\n", ccGetDisplays()->display[i].monitorName,
										 ccGetDisplays()->display[i].gpuName,
										 ccGetDisplays()->display[i].x,
									     ccGetDisplays()->display[i].y,
										 ccGetDisplays()->display[i].currentDisplayData.width,
										 ccGetDisplays()->display[i].currentDisplayData.height,
										 ccGetDisplays()->display[i].currentDisplayData.bitDepth,
										 ccGetDisplays()->display[i].currentDisplayData.refreshRate);
	}

	//Find all resolutions of one display and print them
	resolutions = ccGetResolutions(&ccGetDisplays()->display[1]);
	for(i = 0; i < resolutions->amount; i++) {
		printf("%dx%d\t%dbpp\t%dHz\n", resolutions->displayData[i].width, resolutions->displayData[i].height, resolutions->displayData[i].bitDepth, resolutions->displayData[i].refreshRate);
	}
	ccSetResolution(&ccGetDisplays()->display[1], &resolutions->displayData[8]);
	ccFreeResolutions(resolutions);

	window = ccNewWindow((ccRect){ 0, 0, 1024, 768 }, "CCore test application", 0);

	ccGLBindContext(window, 3, 2);
	ccChangeWM(window, CC_WINDOW_MODE_VISIBLE);
	ccChangeWM(window, CC_WINDOW_MODE_WINDOW);

	ccCenterWindow(window);

	glShadeModel(GL_SMOOTH);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);                  
	glEnable(GL_DEPTH_TEST);             
	glDepthFunc(GL_LEQUAL);              
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	rotQuad = 0;                                                
	glFlush();  

	quit = false;
	while(!quit){
		ccDelay(15);
		while(ccPollEvent(window)){
			switch(window->event.type){
				case CC_EVENT_WINDOW_QUIT:
					if(ccShowDialogue(window, "Really quit?", "quit", CC_DIALOGUE_YESNO) == true){
						quit = true;
					}
					break;
				case CC_EVENT_WINDOW_RESIZE:
					printf("Updated window size: x:%d y:%d %d x %d\n", window->rect.x, window->rect.y, window->rect.width, window->rect.height);
					resizeGL(window->rect.width, window->rect.height);
					break;
				case CC_EVENT_MOUSE_DOWN:
					if(window->event.mouseButton == CC_MOUSE_BUTTON_MIDDLE) {
						quit = true;
					}
					break;
				case CC_EVENT_MOUSE_SCROLL:
					rotQuad += window->event.scrollDelta << 2;
					break;
				case CC_EVENT_KEY_DOWN:
					switch(window->event.key){
						case CC_KEY_1:
							ccChangeWM(window, CC_WINDOW_MODE_FULLSCREEN);
							break;
						case CC_KEY_2:
							ccChangeWM(window, CC_WINDOW_MODE_WINDOW);
							break;
						case CC_KEY_3:
							ccChangeWM(window, CC_WINDOW_MODE_MINIMIZED);
							break;
						case CC_KEY_4:
							ccChangeWM(window, CC_WINDOW_MODE_MAXIMIZED);
							break;
						case CC_KEY_UNDEFINED:
							printf("Key is not supported!\n");
							break;
						default:
							printf("Key: %c\n", (char)window->event.key);
							break;
					}
					break;
				default:
					break;
			}
		}

		renderGL();
		ccGLSwapBuffers(window);
	}

	ccFreeWindow(window);
	ccFreeDisplays();

	return 0;
}

void resizeGL(unsigned int width, unsigned int height)
{                                                     
	if (height == 0){                                
		height = 1;  
	}		
	glViewport(0, 0, width, height);                  
	glMatrixMode(GL_PROJECTION);                      
	glLoadIdentity();                                 
	gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
	glMatrixMode(GL_MODELVIEW);                                           
}

void renderGL()
{              
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();                                  
	glTranslatef(0.0f, 0.0f, -7.0f);                   
	glRotatef(rotQuad, 1.0f, 0.5f, 0.25f);             
	glBegin(GL_QUADS);                                 
	glColor3f(0.0f, 1.0f, 0.0f);                   
	glVertex3f(1.0f, 1.0f, -1.0f);                 
	glVertex3f(-1.0f, 1.0f, -1.0f);                
	glVertex3f(-1.0f, 1.0f, 1.0f);                 
	glVertex3f(1.0f, 1.0f, 1.0f);                  
	glColor3f(1.0f, 0.5f, 0.0f);                   
	glVertex3f(1.0f, -1.0f, 1.0f);                 
	glVertex3f(-1.0f, -1.0f, 1.0f);                
	glVertex3f(-1.0f, -1.0f, -1.0f);               
	glVertex3f(1.0f, -1.0f, -1.0f);                
	glColor3f(1.0f, 0.0f, 0.0f);                   
	glVertex3f(1.0f, 1.0f, 1.0f);                  
	glVertex3f(-1.0f, 1.0f, 1.0f);                 
	glVertex3f(-1.0f, -1.0f, 1.0f);                
	glVertex3f(1.0f, -1.0f, 1.0f);                 
	glColor3f(1.0f, 1.0f, 0.0f);                   
	glVertex3f(-1.0f, 1.0f, -1.0f);                
	glVertex3f(1.0f, 1.0f, -1.0f);                 
	glVertex3f(1.0f, -1.0f, -1.0f);                
	glVertex3f(-1.0f, -1.0f, -1.0f);               
	glColor3f(1.0f, 0.0f, 1.0f);                   
	glVertex3f(1.0f, 1.0f, -1.0f);                 
	glVertex3f(1.0f, 1.0f, 1.0f);                  
	glVertex3f(1.0f, -1.0f, 1.0f);                 
	glVertex3f(1.0f, -1.0f, -1.0f);                
	glColor3f(0.0f, 1.0f, 1.0f);                   
	glVertex3f(-1.0f, 1.0f, 1.0f);                 
	glVertex3f(-1.0f, 1.0f, -1.0f);                
	glVertex3f(-1.0f, -1.0f, -1.0f);               
	glVertex3f(-1.0f, -1.0f, 1.0f);                
	glEnd();
}
