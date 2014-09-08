#include <ccore/error.h>

static ccError *ccErrorStack;
static int ccErrorStackIndex;
static int ccErrorStackSize;

void ccErrorPush(ccError error)
{
	if(ccErrorStackSize <= ccErrorStackIndex) {
		ccErrorStackSize++;
		ccErrorStack = realloc(ccErrorStack, sizeof(ccError)* ccErrorStackSize);
	}
	ccErrorStack[ccErrorStackIndex] = error;
	ccErrorStackIndex++;

	ccPrintf("Pushed error: %s\n", ccErrorString(error));
}

ccError ccErrorPop(void)
{
	if(ccErrorStackIndex == 0) {
		return CC_ERROR_NONE;
	}
	else {
		ccErrorStackIndex--;
		return ccErrorStack[ccErrorStackIndex];
	}
}

void _ccErrorFree(void)
{
	if(ccErrorStackSize != 0)
	{
		free(ccErrorStack);
		ccErrorStackSize = 0;
	}
}

const char *ccErrorString(ccError error)
{
	switch(error){
		case CC_ERROR_NONE:
			return "No errors";
			break;

			// Display related
		case CC_ERROR_NODISPLAY:
			return "Could not open display";
			break;
		case CC_ERROR_RESOLUTION_CHANGE:
			return "Could not change display resolution";
			break;

			// Window related
		case CC_ERROR_WINDOWCREATION:
			return "Can't create the window";
			break;
		case CC_ERROR_WINDOWDESTRUCTION:
			return "Can't destroy the window";
			break;
		case CC_ERROR_WINDOW_MODE:
			return "Couldn't change the window mode";
			break;

			// OpenGL related
		case CC_ERROR_GLVERSION:
			return "The current OpenGL version is not supported";
			break;
		case CC_ERROR_GLCONTEXT:
			return "The openGL context could not be created";
			break;
		case CC_ERROR_GLEWINIT:
			return "GLEW could not be initialized";
			break;
		case CC_ERROR_CANTSWAP:
			return "Failed to swap the buffers";
			break;

			// Thread related
		case CC_ERROR_THREAD:
			return "A thread could not be started";
			break;
		case CC_ERROR_MUTEX_CREATION:
			return "A mutex object couldn't be created";
			break;
		case CC_ERROR_MUTEX:
			return "Error working with a mutex";
			break;

			// Networking related

			// Gamepad related
		case CC_ERROR_NOGAMEPAD:
			return "No gamepad could be found";
			break;
		case CC_ERROR_GAMEPADDATA:
			return "The gamepad could not be read";
			break;
		case CC_ERROR_NOHAPTIC:
			return "This haptic motor is not accessible";
			break;

			// Mouse related
		case CC_ERROR_NOMOUSE:
			return "No mouse could be found";
			break;
		case CC_ERROR_MOUSEDATA:
			return "The mouse could not be read";
			break;

			// Memory related
		case CC_ERROR_OUT_OF_MEMORY:
			return "Out of memory";
			break;
		default:
			return "Error message not defined";
			break;
	}
}
