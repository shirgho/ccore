#if !defined _DEBUG && defined _WIN32
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#endif

#include <stdio.h>
#include <stdlib.h>

#include "../src/ccore/core/common/core.h"
#include "../src/ccore/core/common/interface/window.h"
#include "../src/ccore/core/common/utils/timing.h"
#include "../src/ccore/core/common/interface/event.h"
#include "../src/ccore/core/common/utils/dirUtils.h"
#include "../src/ccore/core/common/utils/charUtils.h"
#include "../src/ccore/core/common/debugutils/print.h"

#include "../src/ccore/modules/clipboard/common/clipboard.h"

float rotQuad = 0.0f;

GLuint loadShader(const char *location, GLenum type);
void resizeGL(unsigned int width, unsigned int height);
void renderGL();

int main(int argc, char** argv)
{
	ccWindow *window;
	char *dir;
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
	ccFindDisplays();
	for(i = 0; i < ccGetDisplayAmount(); i++) {
		printf("Display: %s, Device: %s\nX: %d, Y: %d\n", ccGetDisplay(i)->monitorName, ccGetDisplay(i)->gpuName, ccGetDisplay(i)->x, ccGetDisplay(i)->y);
	}
	
	//Find all resolutions of one display and print them
	for(i = 0; i < ccGetDefaultDisplay()->amount; i++) {
		printf("%dx%d\t%dbpp\t%dHz\n", ccGetDefaultDisplay()->resolution[i].width, ccGetDefaultDisplay()->resolution[i].height, ccGetDefaultDisplay()->resolution[i].bitDepth, ccGetDefaultDisplay()->resolution[i].refreshRate);
	}

	window = ccNewWindow((ccRect){ 0, 0, 1024, 768 }, "CCore test application", 0);

	//ccSetResolution(ccGetDisplay(1), ccGetResolution(ccGetDisplay(1), 4));
	//
	ccCenterWindow(window);
	ccGLBindContext(window, 3, 2);

	dir = ccStrConcatenate(2, ccGetDataDir(), "shaders/raytrace.vert");
	loadShader(dir, GL_VERTEX_SHADER);
	free(dir);

	dir = ccStrConcatenate(2, ccGetDataDir(), "shaders/raytrace.frag");
	loadShader(dir, GL_FRAGMENT_SHADER);
	free(dir);

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
					quit = true;
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
							printf("Going full screen on the first two windows\n");
							ccChangeWM(window, CC_WINDOW_MODE_FULLSCREEN);
							//Use 2 windows full screen
							ccResizeMoveWindow(window, ccRectConcatenate(2, ccGetDisplayRect(ccGetDisplay(1)), ccGetDisplayRect(ccGetDisplay(0))));
							break;
						case CC_KEY_2:
							printf("Going full screen\n");
							ccChangeWM(window, CC_WINDOW_MODE_FULLSCREEN);
							break;
						case CC_KEY_3:
							printf("windowed mode\n");
							ccChangeWM(window, CC_WINDOW_MODE_WINDOW);
							break;
						case CC_KEY_4:
							printf("minimizing\n");
							ccChangeWM(window, CC_WINDOW_MODE_MINIMIZED);
							break;
						case CC_KEY_5:
							printf("maximizing\n");
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

GLuint loadShader(const char *location, GLenum type)
{
	FILE *file;
	long fileSize;
	char *buffer;
	GLuint handle;

	file = fopen(location, "r");
	if(!file){
		printf("ERROR: Couldn't read file %s\n", location);
		return 0;
	}
	fseek(file, 0, SEEK_END);
	fileSize = ftell(file);
	rewind(file);

	buffer = malloc(fileSize);
	if(!buffer){
		printf("ERROR: Out of memory\n");
		return 0;
	}
	fread(buffer, fileSize, 1, file);

	handle = glCreateShader(type);
	if(!handle){
		printf("ERROR: Couldn't create shader handle\n");
		return 0;
	}

	free(buffer);
	fclose(file);

	return handle;
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
