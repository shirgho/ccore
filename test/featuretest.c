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
#include <ccore/dirUtils.h>
#include <ccore/charUtils.h>

#include "tga.h"

// Some helper functions for colors
#define HEXTOR(x) (((x >> 16) & 0xff) / 255.0)
#define HEXTOG(x) (((x >> 8) & 0xff) / 255.0)
#define HEXTOB(x) ((x & 0xff) / 255.0)

// Interface colors in hex format
#define COLOR_CLEAR 0x27586B
#define COLOR_BACK 0x457385
#define COLOR_LINES 0xFFFFFF
#define COLOR_LIGHT 0xD4A26A

// Constants
#define RESOLUTION_DISPLAY_SCALE 0.001f

// These functions will be implemented later in this file
void initialize();
void projectionOrtho(int width, int height);
void render();

void renderLogo();

// Globals
GLuint logoTexture;

int main(int argc, char** argv)
{
	// This variable tells the message loop when to quit
	bool quit = false;
	char *imageFileName;
	ccRect newRect;

	// Displays must be detected before creating the window
	ccFindDisplays();

	// Create a centered window
	ccNewWindow((ccRect){ 0, 0, 574, 159 }, "CCORE feature showcase", CC_WINDOW_FLAG_NORESIZE);
	ccCenterWindow();
	
	// Prepare window for renderen with openGL 3.2
	ccGLBindContext(3, 2);
	
	// This function initializes openGL
	initialize();

	// Load a texture using tga.c
	imageFileName = ccStrConcatenate(2, ccGetDataDir(), "image.tga");
	logoTexture = loadTGATexture(imageFileName);
	free(imageFileName);

	// Display logo for 1 second
	projectionOrtho(ccGetWindowRect().width, ccGetWindowRect().height);
	renderLogo();
	ccGLSwapBuffers();
	ccDelay(1000);

	// Resize the window
	newRect = ccGetWindowRect();
	newRect.width = 800;
	newRect.height = 600;
	ccResizeMoveWindow(newRect, true);
	ccCenterWindow();

	// Event loop resides within this while statement
	while(!quit) {
		ccDelay(15); //TODO: don't lock framerate

		// Poll all events
		while(ccPollEvent()) {
			switch(ccGetEvent().type) {
			case CC_EVENT_WINDOW_QUIT:
				quit = true;
				break;
			case CC_EVENT_KEY_DOWN:
				switch(ccGetEvent().keyCode) {
				case CC_KEY_ESCAPE:
					quit = true;
					break;
				}
				break;
			}
		}

		render();
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
	glClearColor(HEXTOR(COLOR_CLEAR), HEXTOG(COLOR_CLEAR), HEXTOB(COLOR_CLEAR), 1);
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);
	glFlush();
}

void projectionOrtho(int width, int height)
{
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, width, 0, height, -1, 1);
	glMatrixMode(GL_MODELVIEW);
}

void renderLogo()
{
	glClear(GL_COLOR_BUFFER_BIT);

	glBindTexture(GL_TEXTURE_2D, logoTexture);
	
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, -1.0f);
	glVertex2f(0.0f, 0.0f);
	glTexCoord2f(0.0f, 0.0f);
	glVertex2f(0.0f, 159.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex2f(574.0f, 159.0f);
	glTexCoord2f(1.0f, -1.0f);
	glVertex2f(574.0f, 0.0f);
	glEnd();
}

void render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}