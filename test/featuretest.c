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

// These functions will be implemented later in this file
void initialize();
void projection(int width, int height);
void render();

int main(int argc, char** argv)
{
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

	// Event loop resides within this while statement
	while(!quit) {
		ccDelay(15); //TODO: don't lock framerate

		// Poll all events
		while(ccPollEvent()) {
			switch(ccGetEvent().type) {
			case CC_EVENT_WINDOW_QUIT:
				quit = true;
				break;
			}
		}

		render();
		ccGLSwapBuffers();
	}

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
	glMatrixMode(GL_MODELVIEW);
}

void render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}