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

#include <time.h>

#include <ccore/window.h> // Also includes event.h and display.h, these do not need to be included explicitly
#include <ccore/opengl.h>
#include <ccore/time.h>
#include <ccore/dirUtils.h>
#include <ccore/charUtils.h>
#include <ccore/thread.h>
#include <ccore/print.h>
#include <ccore/gamepad.h>

#include "tga.h"

// Some helper functions for colors
#define HEXTOR(x) (((x >> 16) & 0xff) / 255.0f)
#define HEXTOG(x) (((x >> 8) & 0xff) / 255.0f)
#define HEXTOB(x) ((x & 0xff) / 255.0f)

// Interface colors in hex format
#define COLOR_CLEAR 0x27586B
#define COLOR_LIGHT 0xD4A26A
#define COLOR_SQUARE 0x457385

// Dimensions
#define LOGO_WIDTH 574
#define LOGO_HEIGHT 159
#define COMMANDS_WIDTH 500
#define COMMANDS_HEIGHT 300
#define RES_WIDTH 800
#define RES_HEIGHT 600
#define SQUARE_SIZE 50

// These functions will be implemented later in this file
void initialize();
void setProjection();
void timestep();
void render();

void scrollSquaresUp();
void scrollSquaresDown();
void crossSquares();
void mouseTrail();

// This defines a threadable function
ccThreadFunction(counter);

// Custom ceil
int cceil(float n);

// Globals
GLuint logoTexture;
GLuint commandsTexture;

bool logoScreen = true;

int squareCount;
float *squareAlpha;
int hsquares, vsquares;

int main(int argc, char** argv)
{
	// This variable tells the message loop when to quit
	bool quit = false;
	char *imageFileName;

	// Demonstrate threading
	/*
	   ccThread thread;
	   int threadData = 42;

	   ccPrintf("Creating thread:\t%s\n", ccErrorString(ccThreadCreate(&thread, &counter)));
	   ccPrintf("Starting thread:\t%s\n", ccErrorString(ccThreadStart(thread, &threadData)));

	   ccPrintf("Waiting for thread");

	   while(!ccThreadFinished(thread)) {
	   ccPrintf(".");
	   ccTimeDelay(1);
	   }
	   ccPrintf("Thread finished!\n");
	   */

	// Displays must be detected before creating the window and using display functions
	ccDisplayInitialize();

	// Create a centered window that cannot be resized
	ccWindowCreate((ccRect){ 0, 0, LOGO_WIDTH, LOGO_HEIGHT }, "CCORE feature showcase", CC_WINDOW_FLAG_NORESIZE | CC_WINDOW_FLAG_ALWAYSONTOP);
	ccWindowCenter();

	// Prepare gamepad capturing
	ccPrintf("Connecting gamepads: %s, found %d gamepad(s)\n", ccErrorString(ccGamepadConnect()), ccGamepadCount());

	// Prepare window for rendering with openGL 3.2 or higher
	ccPrintf("GL Context status: %s\n", ccErrorString(ccGLBindContext(3, 2)));

	// This function initializes openGL in this example
	initialize();

	// Load textures using tga.c
	imageFileName = ccStrConcatenate(2, ccGetDirDataDir(), "logo.tga");
	logoTexture = loadTGATexture(imageFileName);
	if(logoTexture == 0){
		ccPrintf("Could't load TGA texture: %s\n", imageFileName);
	}
	free(imageFileName);

	imageFileName = ccStrConcatenate(2, ccGetDirDataDir(), "commands.tga");
	commandsTexture = loadTGATexture(imageFileName);
	if(commandsTexture == 0){
		ccPrintf("Could't load TGA texture: %s\n", imageFileName);
	}
	free(imageFileName);

	// Set the projection
	setProjection();

	// Event loop resides within this while statement
	while(!quit) {
		ccTimeDelay(15); //Limit the frame rate

		// Poll all events (ccPollEvent returns true until there are no more events waiting to be polled)
		while(ccWindowPollEvent()) {
			switch(ccWindowGetEvent().type) {
				case CC_EVENT_WINDOW_QUIT:
					// Quit when the close button is pressed
					quit = true;
					break;
				case CC_EVENT_WINDOW_RESIZE:
					// Adapt projection and contents to new size
					setProjection();
					break;
				case CC_EVENT_KEY_DOWN:
					if(logoScreen) break;

					switch(ccWindowGetEvent().keyCode) {
						case CC_KEY_ESCAPE:
							// Quit when the escape key is pressed
							quit = true;
							break;
						case CC_KEY_F:
							// Go full screen on the current display
							ccWindowSetFullscreen(CC_FULLSCREEN_CURRENT_DISPLAY);
							break;
						case CC_KEY_W:
							// Go to windowed
							ccWindowSetWindowed();
							ccWindowResizeMove((ccRect){ ccWindowGetRect().x, ccWindowGetRect().y, RES_WIDTH, RES_HEIGHT });
							ccWindowCenter();
							break;
						case CC_KEY_M:
							// Maximize the window
							ccWindowSetMaximized();
							break;
						case CC_KEY_C:
							// Center the window
							ccWindowCenter();
							break;
						case CC_KEY_X:
							// Go full screen on the first two displays if possible
							if(ccDisplayGetAmount() >= 2) {
								ccWindowSetFullscreen(2, ccDisplayGet(0), ccDisplayGet(1));
							}
							break;
						case CC_KEY_R:
							// Change the resolution to a random one from the list of possible resolutions
							ccDisplaySetResolution(ccWindowGetDisplay(), rand() % ccDisplayGetResolutionAmount(ccWindowGetDisplay()));
							break;
						case CC_KEY_N:
							// Revert all resolutions
							ccDisplayRevertModes();
							break;
					}

					if(!logoScreen) {
						squareAlpha[ccWindowGetEvent().keyCode % squareCount] = 1.0f;
					}
					break;
				case CC_EVENT_MOUSE_UP:
					if(logoScreen && ccWindowGetEvent().mouseButton == CC_MOUSE_BUTTON_LEFT) {
						// Proceed to the demo screen
						ccRect windowRect = ccWindowGetRect();

						logoScreen = false;

						// Enlarge the window a bit
						windowRect.width = RES_WIDTH;
						windowRect.height = RES_HEIGHT;
						ccWindowResizeMove(windowRect);
						ccWindowCenter();

						setProjection();
					}
					break;
				case CC_EVENT_MOUSE_DOWN:
					// Create a cross where the mouse was pressed
					if(!logoScreen) {
						crossSquares();
					}
					break;
				case CC_EVENT_MOUSE_SCROLL:
					// Scroll all squares for a nice effect
					if(ccWindowGetEvent().scrollDelta > 0) {
						scrollSquaresUp();
					}
					else{
						scrollSquaresDown();
					}
					break;
				case CC_EVENT_MOUSE_MOVE:
					// Highlight squares the mouse is hovering over
					if(!logoScreen) {
						mouseTrail();
					}
					break;
				case CC_EVENT_GAMEPAD:
					if(ccWindowGetEvent().gamepadEvent.type == CC_GAMEPAD_AXIS_MOVE){
						ccPrintf("Gamepad %d axis %d moved %d\n", ccWindowGetEvent().gamepadEvent.gamepadId, ccWindowGetEvent().gamepadEvent.axisId, ccWindowGetEvent().gamepadEvent.value);
					}else if(ccWindowGetEvent().gamepadEvent.type == CC_GAMEPAD_BUTTON_DOWN){
						ccPrintf("Gamepad %d button %d down\n", ccWindowGetEvent().gamepadEvent.gamepadId, ccWindowGetEvent().gamepadEvent.buttonId);
					}else if(ccWindowGetEvent().gamepadEvent.type == CC_GAMEPAD_BUTTON_UP){
						ccPrintf("Gamepad %d button %d up\n", ccWindowGetEvent().gamepadEvent.gamepadId, ccWindowGetEvent().gamepadEvent.buttonId);
					}else if(ccWindowGetEvent().gamepadEvent.type == CC_GAMEPAD_CONNECT){
						ccPrintf("Gamepad connected\n");
					}else if(ccWindowGetEvent().gamepadEvent.type == CC_GAMEPAD_DISCONNECT){
						ccPrintf("Gamepad disconnected\n");
					}
					break;
				default:
					break;
			}
		}

		// Process logic
		timestep();

		// Render using openGL
		render();

		// Swap the buffers
		ccGLSwapBuffers();
	}

	// Free memory before terminating
	ccGamepadDisconnect();
	ccGLFreeContext();
	ccDisplayFree();
	ccWindowFree();

	return 0;
}

// A thread procedure is implemented here

ccThreadFunction(counter)
{
	int count;
	for(count = 0;count<=500;count++) {
		ccTimeDelay(1);
	}
	ccPrintf("\nPassed integer: %d\n", *(int*)ccThreadData);
}

// All code below this point is not CCORE related

void initialize()
{
	glClearColor(HEXTOR(COLOR_CLEAR), HEXTOG(COLOR_CLEAR), HEXTOB(COLOR_CLEAR), 1.0f);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glFlush();
}

void setProjection()
{
	int width = ccWindowGetRect().width;
	int height = ccWindowGetRect().height;

	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, width, 0, height, -1, 1);
	glMatrixMode(GL_MODELVIEW);

	if(!logoScreen) {
		int i;
		hsquares = cceil((float)ccWindowGetRect().width / SQUARE_SIZE);
		vsquares = cceil((float)ccWindowGetRect().height / SQUARE_SIZE);
		squareCount = hsquares * vsquares;

		squareAlpha = realloc(squareAlpha ,sizeof(float)*squareCount);

		for(i = 0; i < squareCount; i++) {
			squareAlpha[i] = 0.0f;
		}
	}
}

void renderLogo()
{
	glEnable(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, logoTexture);

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, -1.0f);
	glVertex2f(0.0f, 0.0f);
	glTexCoord2f(0.0f, 0.0f);
	glVertex2f(0.0f, (float)LOGO_HEIGHT);
	glTexCoord2f(1.0f, 0.0f);
	glVertex2f((float)LOGO_WIDTH, (float)LOGO_HEIGHT);
	glTexCoord2f(1.0f, -1.0f);
	glVertex2f((float)LOGO_WIDTH, 0.0f);
	glEnd();

	glDisable(GL_TEXTURE_2D);
}

void renderCommands()
{
	glEnable(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, commandsTexture);

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, -1.0f);
	glVertex2f(0.0f, (float)ccWindowGetRect().height - COMMANDS_HEIGHT);
	glTexCoord2f(0.0f, 0.0f);
	glVertex2f(0.0f, (float)ccWindowGetRect().height);
	glTexCoord2f(1.0f, 0.0f);
	glVertex2f((float)COMMANDS_WIDTH, (float)ccWindowGetRect().height);
	glTexCoord2f(1.0f, -1.0f);
	glVertex2f((float)COMMANDS_WIDTH, (float)ccWindowGetRect().height - COMMANDS_HEIGHT);
	glEnd();

	glDisable(GL_TEXTURE_2D);
}

void timestep()
{
	int i;
	for(i = 0; i < squareCount; i++) {
		if(squareAlpha[i] != 0) {
			squareAlpha[i] *= .98f;
			if(squareAlpha[i] < .01f) squareAlpha[i] = 0;
		}
	}
}

void render()
{
	glClear(GL_COLOR_BUFFER_BIT);

	if(logoScreen) {
		renderLogo();
	}
	else{
		int i, x, y;
		x = 0;
		y = 0;
		for(i = 0; i < squareCount; i++) {
			if(squareAlpha[i] != 0) {
				glBegin(GL_QUADS);
				glColor4f(HEXTOR(COLOR_SQUARE), HEXTOG(COLOR_SQUARE), HEXTOB(COLOR_SQUARE), squareAlpha[i]);
				glVertex2f((float)x, (float)y);
				glVertex2f((float)x + SQUARE_SIZE, (float)y);
				glVertex2f((float)x + SQUARE_SIZE, (float)y + SQUARE_SIZE);
				glVertex2f((float)x, (float)y + SQUARE_SIZE);
				glEnd();
			}

			x += SQUARE_SIZE;
			if(x == hsquares * SQUARE_SIZE) {
				x = 0;
				y += SQUARE_SIZE;
			}
		}
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

		renderCommands();
	}
}

void scrollSquaresUp()
{
	int i;

	for(i = squareCount - 1; i >= 0; i--) {
		if(i - hsquares >= 0) {
			squareAlpha[i] = squareAlpha[i - hsquares];
		}
	}

	mouseTrail();
}

void scrollSquaresDown()
{
	int i;

	for(i = 0; i < squareCount; i++) {
		if(i + hsquares < squareCount) {
			squareAlpha[i] = squareAlpha[i + hsquares];
		}
	}

	mouseTrail();
}

int mouseToIndex()
{
	int index = (ccWindowGetMouse().x / SQUARE_SIZE) + ((ccWindowGetRect().height - ccWindowGetMouse().y) / SQUARE_SIZE) * hsquares;
	if(index >= squareCount || index < 0) index = -1;
	return index;
}

void crossSquares()
{
	int i;
	int index = mouseToIndex();
	int iStart = index - (index % hsquares);
	int iEnd = iStart + hsquares;

	for(i = iStart; i < iEnd; i++) {
		squareAlpha[i] = 1.0f;
	}

	iStart = index % hsquares;
	for(i = iStart; i < squareCount; i += hsquares) {
		squareAlpha[i] = 1.0f;
	}
}

void mouseTrail()
{
	int index = mouseToIndex();
	if(index != -1) squareAlpha[index] = 1;
}

int cceil(float n)
{
	int i = (int)n;
	if(n - i == 0) return i;
	else return i + 1;
}
