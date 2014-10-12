//__________________________________________________________________________________//
//                               ______                                             //
//                              /  ___ /__  _ __ ___                                //
//                             /  / __/ _ \| '__/ _ \                               //
//                            |  | (_| (_) | | |  __/                               //
//                             \  \___\___/|_|  \___| 1.1                           //
//                              \______\                                            //
//                                                                                  //
//             Copyright (C) 2014 \ Job Talle (job@ccore.org)                       //
//                                 \ Thomas Versteeg (thomas@ccore.org)             //
//__________________________________________________________________________________//
//                                                                                  //
//      This program is free software: you can redistribute it and/or modify        //
//      it under the terms of the 3-clause BSD license.                             //
//                                                                                  //
//      You should have received a copy of the 3-clause BSD License along with      //
//      this program. If not, see <http://opensource.org/licenses/>.                //
//__________________________________________________________________________________//

// When building release under visual studio, disable the terminal
#if !defined _DEBUG && defined _WIN32 && defined _MSC_VER
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#endif

// Net needs to be included first, or else windows is going to cry when winsock.h is included before winsock2.h
#include <ccore/display.h>
#include <ccore/window.h>
#include <ccore/event.h>
#include <ccore/opengl.h>
#include <ccore/time.h>
#include <ccore/file.h>
#include <ccore/string.h>
#include <ccore/thread.h>
#include <ccore/print.h>
#include <ccore/gamepad.h>

#include <GL/glew.h>

#include "tga.h"
#include "icon.h"

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

#ifdef WINDOWS
// Check for leaks
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif

// These functions will be implemented later in this file
void ccNetworkSend(char *string);

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
float *squareAlpha = NULL;
int hsquares, vsquares;

// Threading variables and constants
#define THREAD_COUNT 8
#define THREAD_ITERATIONS 5
int threadVal;
ccMutex mutex;

int main(int argc, char** argv)
{
	int i;
	char *imageFileName;
	unsigned long *iconData;

	// This variable tells the message loop when to quit
	bool quit = false;

	ccInitialize();

	// Demonstrate threading
	ccThread thread[THREAD_COUNT];
	int threadData = 42;

	threadVal = 0;
	ccThreadMutexCreate(&mutex);

	ccPrintf("Waiting for %d threads counting to %d\n", THREAD_COUNT, THREAD_ITERATIONS * THREAD_COUNT);

	for(i = 0; i < THREAD_COUNT; i++) {
		ccThreadStart(&thread[i], &counter, &threadData);
	}

	for(i = 0; i < THREAD_COUNT; i++) {
		ccThreadJoin(&thread[i]);
	}

	ccThreadMutexFree(&mutex);

	ccPrintf("%d threads finished counting to %d\n", THREAD_COUNT, THREAD_ITERATIONS * THREAD_COUNT);

	// Create a centered window that cannot be resized
	ccDisplayInitialize();
	ccWindowCreate((ccRect){ 0, 0, LOGO_WIDTH, LOGO_HEIGHT }, "CCORE feature showcase", CC_WINDOW_FLAG_NORESIZE);
	ccWindowSetCentered();

	// Prepare gamepad capturing
	ccGamepadInitialize();

	// Prepare window for rendering with openGL 3.2 or higher
	ccGLContextBind();

	// This test uses glew for openGL extension loading
	glewInit();

	// This function initializes openGL in this example
	initialize();

	// Load textures using tga.c
	imageFileName = ccStringConcatenate(2, ccFileDataDirGet(), "logo.tga");
	logoTexture = loadTGATexture(imageFileName);
	if(logoTexture == 0){
		ccPrintf("Could't load TGA texture: %s\n", imageFileName);
	}
	free(imageFileName);

	imageFileName = ccStringConcatenate(2, ccFileDataDirGet(), "commands.tga");
	commandsTexture = loadTGATexture(imageFileName);
	if(commandsTexture == 0){
		ccPrintf("Could't load TGA texture: %s\n", imageFileName);
	}
	free(imageFileName);
	
	iconData = iconGetData();
	ccWindowIconSet(iconGetSize(), iconData);
	free(iconData);

	// Set the projection
	setProjection();

	// Event loop resides within this while statement
	while(!quit) {
		ccTimeDelay(15); //Limit the frame rate

		// Poll all events (ccPollEvent returns true until there are no more events waiting to be polled)
		while(ccWindowEventPoll()) {
			switch(ccWindowEventGet().type) {
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

					switch(ccWindowEventGet().keyCode) {
						case CC_KEY_V:
							ccPrintf("Clipboard string: %s\n", ccWindowClipboardGet());
							break;
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
							ccWindowSetCentered();
							break;
						case CC_KEY_M:
							// Maximize the window
							ccWindowSetMaximized();
							break;
						case CC_KEY_C:
							// Center the window
							ccWindowSetCentered();
							break;
						case CC_KEY_X:
							// Go full screen on the first two displays if possible
							if(ccDisplayGetAmount() >= 2) {
								ccWindowSetFullscreen(2, ccDisplayGet(0), ccDisplayGet(1));
							}
							break;
						case CC_KEY_R:
							// Change the resolution to a random one from the list of possible resolutions
							ccDisplayResolutionSet(ccWindowGetDisplay(), rand() % ccDisplayResolutionGetAmount(ccWindowGetDisplay()));
							break;
						case CC_KEY_N:
							// Revert all resolutions
							ccDisplayRevertModes();
							break;
						case CC_KEY_T:
							ccWindowMouseSetPosition((ccPoint){.x = ccWindowGetRect().width / 2, .y = ccWindowGetRect().height / 2});
							ccWindowMouseSetCursor(CC_CURSOR_NONE);
							break;	
						case CC_KEY_RETURN:
							ccPrintf("Setting clipboard... %s\n", ccErrorString(ccWindowClipboardSet("ccore clipboard test")));
							break;
						case CC_KEY_P:
							ccPrintf("Current display \"%s\" has %d resolutions\n", ccWindowGetDisplay()->monitorName, ccDisplayResolutionGetAmount(ccWindowGetDisplay()));
							ccPrintf("\tThe current resolution is: %dx%d\n", ccDisplayResolutionGetCurrent(ccWindowGetDisplay())->width, ccDisplayResolutionGetCurrent(ccWindowGetDisplay())->height);
							break;
						default:
							ccPrintf("Key \"%s\" pressed\n", ccEventKeyToStr(ccWindowEventGet().keyCode));
							break;
					}

					if(!logoScreen) {
						squareAlpha[ccWindowEventGet().keyCode % squareCount] = 1.0f;
					}
					break;
				case CC_EVENT_FOCUS_LOST:
					ccWindowSetBlink();
					break;
				case CC_EVENT_MOUSE_UP:
					if(logoScreen && ccWindowEventGet().mouseButton == CC_MOUSE_BUTTON_LEFT) {
						// Proceed to the demo screen
						ccRect windowRect = ccWindowGetRect();

						logoScreen = false;

						// Enlarge the window a bit
						windowRect.width = RES_WIDTH;
						windowRect.height = RES_HEIGHT;
						ccWindowResizeMove(windowRect);
						ccWindowSetCentered();

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
					if(ccWindowEventGet().scrollDelta > 0) {
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
					if(ccWindowEventGet().gamepadEvent.type == CC_GAMEPAD_AXIS_MOVE){
						ccPrintf("Gamepad %d axis %d moved %d\n", ccWindowEventGet().gamepadEvent.id, ccWindowEventGet().gamepadEvent.axisId, ccGamepadGet(ccWindowEventGet().gamepadEvent.id)->axis[ccWindowEventGet().gamepadEvent.axisId]);
					}else if(ccWindowEventGet().gamepadEvent.type == CC_GAMEPAD_BUTTON_DOWN){
						ccPrintf("Gamepad %d button %d down\n", ccWindowEventGet().gamepadEvent.id, ccWindowEventGet().gamepadEvent.buttonId);
						if(ccWindowEventGet().gamepadEvent.buttonId == 0) {
							ccGamepadOutputSet(ccGamepadGet(ccWindowEventGet().gamepadEvent.id), 0, GAMEPAD_OUTPUT_VALUE_MAX);
							ccGamepadOutputSet(ccGamepadGet(ccWindowEventGet().gamepadEvent.id), 1, GAMEPAD_OUTPUT_VALUE_MAX);
						}
						else if(ccWindowEventGet().gamepadEvent.buttonId == 10) {
							ccGamepadOutputSet(ccGamepadGet(ccWindowEventGet().gamepadEvent.id), 0, GAMEPAD_OUTPUT_VALUE_MAX);
						}
						else if(ccWindowEventGet().gamepadEvent.buttonId == 11) {
							ccGamepadOutputSet(ccGamepadGet(ccWindowEventGet().gamepadEvent.id), 1, GAMEPAD_OUTPUT_VALUE_MAX);
						}
					}else if(ccWindowEventGet().gamepadEvent.type == CC_GAMEPAD_BUTTON_UP){
						ccPrintf("Gamepad %d button %d up\n", ccWindowEventGet().gamepadEvent.id, ccWindowEventGet().gamepadEvent.buttonId);
						if(ccWindowEventGet().gamepadEvent.buttonId == 0) {
							ccGamepadOutputSet(ccGamepadGet(ccWindowEventGet().gamepadEvent.id), 0, GAMEPAD_OUTPUT_VALUE_MIN);
							ccGamepadOutputSet(ccGamepadGet(ccWindowEventGet().gamepadEvent.id), 1, GAMEPAD_OUTPUT_VALUE_MIN);
						}
						else if(ccWindowEventGet().gamepadEvent.buttonId == 10) {
							ccGamepadOutputSet(ccGamepadGet(ccWindowEventGet().gamepadEvent.id), 0, GAMEPAD_OUTPUT_VALUE_MIN);
						}
						else if(ccWindowEventGet().gamepadEvent.buttonId == 11) {
							ccGamepadOutputSet(ccGamepadGet(ccWindowEventGet().gamepadEvent.id), 1, GAMEPAD_OUTPUT_VALUE_MIN);
						}
					}else if(ccWindowEventGet().gamepadEvent.type == CC_GAMEPAD_CONNECT){
						ccPrintf("Gamepad %d \"%s\" connected\n",  ccWindowEventGet().gamepadEvent.id, ccGamepadGet(ccWindowEventGet().gamepadEvent.id)->name);
					}else if(ccWindowEventGet().gamepadEvent.type == CC_GAMEPAD_DISCONNECT){
						ccPrintf("Gamepad %d \"%s\" disconnected\n", ccWindowEventGet().gamepadEvent.id, ccGamepadGet(ccWindowEventGet().gamepadEvent.id)->name);
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
		ccGLBuffersSwap();

	}

	ccDisplayRevertModes();
	
	// Free memory before terminating
	ccFree();

	free(squareAlpha);

#ifdef WINDOWS
	// Check for leaks
	_CrtDumpMemoryLeaks();
#endif

	return 0;
}

// A thread procedure is implemented here

ccThreadFunction(counter)
{
	int count;

	ccPrintf("\nPassed integer: %d\n", *(int*)ccThreadData);

	for(count = 0; count<THREAD_ITERATIONS; count++) {
		ccThreadMutexJoin(&mutex);
		threadVal++;
		printf("thread counter: %d\n", threadVal);
		ccThreadMutexRelease(&mutex);
		ccTimeDelay(10);
	}

	ccThreadReturn();
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
