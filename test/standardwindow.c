#if !defined _DEBUG && defined _WIN32
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#endif

#include <stdio.h>
#include <stdlib.h>

#include "../include/window.h"
#include "../include/opengl.h"
#include "../include/timing.h"
#include "../include/event.h"
#include "../include/dirUtils.h"
#include "../include/charUtils.h"
#include "../include/print.h"

float rotQuad = 0.0f;

void initGL();
void resizeGL(unsigned int width, unsigned int height);
void renderGL();

int main(int argc, char** argv)
{
	bool quit;
	int i;

/*
	//Create a .txt
	char *dir = ccStrConcatenate(2, ccGetDataDir(), "txtfile.txt");
	printf("Creating file %s\n", dir);
	FILE *newFile = fopen(dir, "w");
	if(!newFile) ccAbort("Can't create file!");

	for(i = 0; i < 100; i++) {
		fprintf(newFile, "%d\n", i);
	}
	fclose(newFile);
	free(dir);
*/
	
	//Find displays and print their stats
	printf("Finding displays\n");
	ccFindDisplays();
	for(i = 0; i < ccGetDisplayAmount(); i++) {
		ccAssert(ccGetDisplay(i) != NULL);

		printf("Display: %s, Device: %s\nX: %d, Y: %d\n", ccGetDisplay(i)->monitorName, ccGetDisplay(i)->gpuName, ccGetDisplay(i)->x, ccGetDisplay(i)->y);
	}
	
	printf("Printing resolutions from default display\n");
	//Find all resolutions of one display and print them
	for(i = 0; i < ccGetDefaultDisplay()->amount; i++) {
		ccAssert(ccGetDefaultDisplay()->resolution + i != NULL);

		printf("%dx%d\t%dbpp\t%dHz\n", ccGetDefaultDisplay()->resolution[i].width, ccGetDefaultDisplay()->resolution[i].height, ccGetDefaultDisplay()->resolution[i].bitDepth, ccGetDefaultDisplay()->resolution[i].refreshRate);
	}

	printf("Creating window\n");
	if(ccNewWindow((ccRect){ 0, 0, 1024, 768 }, "CCore test application", CC_WINDOW_FLAG_NORESIZE) != CC_ERROR_NONE) printf("Error creating window!\n");

	ccCenterWindow();
	ccGLBindContext(3, 2);

	initGL();
	rotQuad = 10.0f;

	quit = false;
	while(!quit){
		ccDelay(15);
		while(ccPollEvent()){
			switch(ccGetEvent().type){
				case CC_EVENT_WINDOW_QUIT:
					quit = true;
					break;
				case CC_EVENT_WINDOW_RESIZE:
					printf("Updated window size: x:%d y:%d %d x %d\n", ccGetWindowRect().x, ccGetWindowRect().y, ccGetWindowRect().width, ccGetWindowRect().height);
					resizeGL(ccGetWindowRect().width, ccGetWindowRect().height);
					break;
				case CC_EVENT_MOUSE_DOWN:
					if(ccGetEvent().mouseButton == CC_MOUSE_BUTTON_MIDDLE) {
						quit = true;
					}
					break;
				case CC_EVENT_MOUSE_SCROLL:
					rotQuad += ccGetEvent().scrollDelta * 2;
					break;
				case CC_EVENT_KEY_DOWN:
					switch(ccGetEvent().keyCode){
						case CC_KEY_1:
							printf("Going full screen on the first two windows\n");
							ccSetFullscreen(2, ccGetDisplay(0), ccGetDisplay(1));
							break;
						case CC_KEY_2:
							printf("Going full screen on current window\n");
							ccSetFullscreen(0);
							break;
						case CC_KEY_3:
							printf("Windowed mode\n");
							ccSetWindowed();
							break;
						case CC_KEY_4:
							printf("Set window resolution\n");
							ccResizeMoveWindow((ccRect){ 0, 0, 800, 600 }, true);
							ccCenterWindow();
							break;
						case CC_KEY_5:
							printf("Maximizing\n");
							ccSetMaximized();
							break;
						case CC_KEY_6:
							printf("Generating a new window\n");
							ccGLFreeContext();
							ccFreeWindow();
							ccNewWindow((ccRect){ 0, 0, 1024, 768 }, "CCore CSchmore", 0);
							ccGLBindContext(3, 2);
							initGL();
							ccCenterWindow();
							break;
						case CC_KEY_7:
							printf("Setting resolution\n");
							ccSetResolution(ccGetDisplay(0), 10);
							break;
						case CC_KEY_8:
							printf("Reverting resolution\n");
							ccSetResolution(ccGetDisplay(0), CC_DEFAULT_RESOLUTION);
							break;
						case CC_KEY_9:
							printf("Centering window\n");
							ccCenterWindow();
							break;
						case CC_KEY_C:
							printf("Centering window\n");
							ccCenterWindow();
							break;
						case CC_KEY_LCONTROL:
							printf("Left control\n");
							break;
						case CC_KEY_RCONTROL:
							printf("Right control\n");
							break;
						case CC_KEY_LSHIFT:
							printf("Left shift\n");
							break;
						case CC_KEY_RSHIFT:
							printf("Right shift\n");
							break;
						case CC_KEY_NUM2:
							printf("num2 key\n");
							break;
						default:
							printf("Key %c is not used\n", ccKeyToChar(ccGetEvent().keyCode));
							break;
					}
					break;
				default:
					break;
			}
		}

		if(ccWindowExists()) {
			renderGL();
			ccGLSwapBuffers();
		}
	}

	ccGLFreeContext();
	ccFreeWindow();
	ccRevertDisplays();
	ccFreeDisplays();

	return 0;
}

void initGL()
{
	glShadeModel(GL_SMOOTH);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);                  
	glEnable(GL_DEPTH_TEST);             
	glDepthFunc(GL_LEQUAL);              
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glFlush();  
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
