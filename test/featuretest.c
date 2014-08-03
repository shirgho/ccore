//__________________________________________________________________________________//
//                               ______                                             //
//                              /  ___ /__  _ __ ___                                //
//                             /  / __/ _ \| '__/ _ \                               //
//                            |  | (_| (_) | | |  __/                               //
//                             \  \___\___/|_|  \___| 1.0                           //
//                              \______\                                            //
//                                                                                  //
//             Copyright (C) 2014 \ Job Talle (jobtalle@hotmail.com)                //
//                                 \ Thomas Versteeg (thomasversteeg@gmx.com)       //
//__________________________________________________________________________________//
//                                                                                  //
//      This program is free software: you can redistribute it and/or modify        //
//      it under the terms of the GNU General Public License as published by        //
//      the Free Software Foundation.                                               //
//                                                                                  //
//      This program is distributed without any warranty; see the GNU               //
//      General Public License for more details.                                    //
//                                                                                  //
//      You should have received a copy of the GNU General Public License           //
//      along with this program. If not, see <http://www.gnu.org/licenses/>.        //
//__________________________________________________________________________________//

// When building release under visual studio, disable the terminal
#if !defined _DEBUG && defined _WIN32 && defined _MSC_VER
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#endif

#include <ccore/window.h> // Also includes event.h and display.h, these do not need to be included explicitly
#include <ccore/opengl.h>
#include <ccore/timing.h>

// Some helper functions for colors
#define HEXTOR(x) (((x >> 16) & 0xff) / 255.0)
#define HEXTOG(x) (((x >> 8) & 0xff) / 255.0)
#define HEXTOB(x) ((x & 0xff) / 255.0)

// Interface colors in hex format
#define COLOR_CLEAR 0x27586B
#define COLOR_BACK 0x457385
#define COLOR_LINES 0xFFFFFF

// Constants
#define RESOLUTION_DISPLAY_SCALE 0.001f

// These functions will be implemented later in this file
void initialize();
void projection(int width, int height);

// All data that will be demonstrated is stored in this struct
typedef struct {
	// General
	int current;
	GLfloat rotation;

	// Displays
	int displayCount;
	ccRect* rects;
	float* rectAlpha;

	// Input
} showcaseData;
void initializeShowcase(showcaseData* data);
void timestepShowcase(showcaseData* data, float timestep);
void renderShowcase(showcaseData* data);
void freeShowcase(showcaseData* data);

int main(int argc, char** argv)
{
	// All showcase data is contained within data
	showcaseData data = {0};

	// This variable tells the message loop when to quit
	bool quit = false;

	// Displays must be detected before creating the window
	ccFindDisplays();

	// Create a centered window
	ccNewWindow((ccRect){ 0, 0, 1400, 800 }, "CCORE feature showcase", CC_WINDOW_FLAG_NORESIZE);
	ccCenterWindow();
	
	// Prepare window for renderen with openGL 3.2
	ccGLBindContext(3, 2);
	
	// This function initializes openGL
	initialize();
	initializeShowcase(&data);

	// Event loop resides within this while statement
	while(!quit) {
		ccDelay(15); //TODO: don't lock framerate

		// Poll all events
		while(ccPollEvent()) {
			switch(ccGetEvent().type) {
			case CC_EVENT_WINDOW_QUIT:
				quit = true;
				break;
			case CC_EVENT_MOUSE_SCROLL:
				if(ccGetEvent().scrollDelta > 0) data.current --; else data.current++;
				if(data.current == -1) data.current = 0;
				if(data.current == 3) data.current = 2;
				break;
			}
		}

		timestepShowcase(&data, (float) 15 / 1000);
		renderShowcase(&data);
		ccGLSwapBuffers();
	}

	// Free memory before terminating
	ccFreeDisplays();
	ccGLFreeContext();
	ccFreeWindow();
}

void initialize()
{
	// Configure openGL before drawing
	glShadeModel(GL_SMOOTH);
	glClearColor(HEXTOR(COLOR_CLEAR), HEXTOG(COLOR_CLEAR), HEXTOB(COLOR_CLEAR), 1);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glFlush();

	projection(ccGetWindowRect().width, ccGetWindowRect().height);
}

void projection(int width, int height)
{
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
	gluLookAt(0, 0, 8, 0, 0, 0, 0, 1, 0);
	glMatrixMode(GL_MODELVIEW);
}

void initializeShowcase(showcaseData* data)
{
	int i;

	data->current = 1;
	data->rotation = 90;

	data->displayCount = ccGetDisplayAmount();
	data->rects = malloc(sizeof(ccRect)* data->displayCount);

	for(i = 0; i < data->displayCount; i++) {
		data->rects[i].x = ccGetDisplay(i)->x;
		data->rects[i].y = ccGetDisplay(i)->y;
		data->rects[i].width = ccGetDisplay(i)->resolution[ccGetDisplay(i)->current].width;
		data->rects[i].height = ccGetDisplay(i)->resolution[ccGetDisplay(i)->current].height;
	}
}

void timestepShowcase(showcaseData* data, float timestep)
{

}

void renderShowcase(showcaseData* data)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	switch(data->current)
	{
	case 0:
		// ?

		break;
	case 1:
		// Resolutions
	{
		int i;

		for(i = 0; i < data->displayCount; i++) {
			glBegin(GL_TRIANGLES);
			glColor3f(HEXTOR(COLOR_BACK), HEXTOG(COLOR_BACK), HEXTOB(COLOR_BACK));
			glVertex2f(data->rects[i].x * RESOLUTION_DISPLAY_SCALE, -data->rects[i].y * RESOLUTION_DISPLAY_SCALE);
			glVertex2f(data->rects[i].x * RESOLUTION_DISPLAY_SCALE, -(data->rects[i].y + data->rects[i].height) * RESOLUTION_DISPLAY_SCALE);
			glVertex2f((data->rects[i].x + data->rects[i].width) * RESOLUTION_DISPLAY_SCALE, -(data->rects[i].y + data->rects[i].height) * RESOLUTION_DISPLAY_SCALE);
			glVertex2f((data->rects[i].x + data->rects[i].width) * RESOLUTION_DISPLAY_SCALE, -(data->rects[i].y + data->rects[i].height) * RESOLUTION_DISPLAY_SCALE);
			glVertex2f((data->rects[i].x + data->rects[i].width) * RESOLUTION_DISPLAY_SCALE, -data->rects[i].y * RESOLUTION_DISPLAY_SCALE);
			glVertex2f(data->rects[i].x * RESOLUTION_DISPLAY_SCALE, -data->rects[i].y * RESOLUTION_DISPLAY_SCALE);
			glEnd();

			glBegin(GL_LINE_LOOP);
			glColor3f(HEXTOR(COLOR_LINES), HEXTOG(COLOR_LINES), HEXTOB(COLOR_LINES));
			glVertex2f(data->rects[i].x * RESOLUTION_DISPLAY_SCALE, -data->rects[i].y * RESOLUTION_DISPLAY_SCALE);
			glVertex2f(data->rects[i].x * RESOLUTION_DISPLAY_SCALE, -(data->rects[i].y + data->rects[i].height) * RESOLUTION_DISPLAY_SCALE);
			glVertex2f((data->rects[i].x + data->rects[i].width) * RESOLUTION_DISPLAY_SCALE, -(data->rects[i].y + data->rects[i].height) * RESOLUTION_DISPLAY_SCALE);
			glVertex2f((data->rects[i].x + data->rects[i].width) * RESOLUTION_DISPLAY_SCALE, -data->rects[i].y * RESOLUTION_DISPLAY_SCALE);
			glEnd();
		}
	}
		break;
	case 2:
		// Input

		break;
	}
}

void freeShowcase(showcaseData* data)
{
	free(data->rects);
}