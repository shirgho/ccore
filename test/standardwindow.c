#ifndef _DEBUG
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

	window = ccNewWindow(1024, 768, "CCore test application", 0); 
	ccGLBindContext(window, 3, 2);
	ccChangeWM(window, CC_WINDOW_MODE_WINDOW);

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
					printf("%d %d\n", window->width, window->height);
					resizeGL(window->width, window->height);
					break;
				case CC_EVENT_MOUSE_DOWN:
					if(window->event.mouseState.button == CC_MOUSE_BUTTON_LEFT || window->event.mouseState.button == CC_MOUSE_BUTTON_SPECIAL_1){
						quit = true;
					}
					break;
				case CC_EVENT_MOUSE_SCROLL_UP:
					rotQuad += 5;
					break;
				case CC_EVENT_KEY_DOWN:
					ccChangeWM(window, CC_WINDOW_MODE_MAXIMIZED);
					break;
				default:
					break;
			}
		}

		renderGL();
		ccGLSwapBuffers(window);
	}

	ccFreeWindow(window);

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
	rotQuad += 0.5f;                                   
}
