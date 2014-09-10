#include <ccore/error.h>

static ccError *ccErrorStack;
static int ccErrorStackIndex;
static int ccErrorStackSize;

void _ccErrorPush(ccError error, char *file, int line)
{
	if(ccErrorStackSize <= ccErrorStackIndex) {
		ccErrorStackSize++;
		ccErrorStack = realloc(ccErrorStack, sizeof(ccError)* ccErrorStackSize);
	}
	ccErrorStack[ccErrorStackIndex] = error;
	ccErrorStackIndex++;

	ccPrintf("File \"%s\", line %d:\n\tPushed error: \"%s\"\n", file, line, ccErrorString(error));
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
		case CC_ERROR_DISPLAY_NONE:
			return "Could not open display";
			break;
		case CC_ERROR_DISPLAY_RESOLUTIONCHANGE:
			return "Could not change display resolution";
			break;

			// Window related
		case CC_ERROR_WINDOW_CREATE:
			return "Can't create the window";
			break;
		case CC_ERROR_WINDOW_DESTROY:
			return "Can't destroy the window";
			break;
		case CC_ERROR_WINDOW_MODE:
			return "Couldn't change the window mode";
			break;

			// OpenGL related
		case CC_ERROR_GL_VERSION:
			return "The current OpenGL version is not supported";
			break;
		case CC_ERROR_GL_CONTEXT:
			return "The openGL context could not be created";
			break;
		case CC_ERROR_GLEWINIT:
			return "GLEW could not be initialized";
			break;
		case CC_ERROR_GL_BUFFERSWAP:
			return "Failed to swap the buffers";
			break;

			// Thread related
		case CC_ERROR_THREAD:
			return "A thread could not be started";
			break;
		case CC_ERROR_THREAD_MUTEXCREATE:
			return "A mutex object couldn't be created";
			break;
		case CC_ERROR_THREAD_MUTEX:
			return "Error working with a mutex";
			break;

			// Networking related
		case CC_ERROR_NET:
			return "A network function failed";
			break;

			// Gamepad related
		case CC_ERROR_GAMEPAD_NONE:
			return "No gamepad could be found";
			break;
		case CC_ERROR_GAMEPAD_DATA:
			return "The gamepad could not be read";
			break;
		case CC_ERROR_GAMEPAD_HAPTICNONE:
			return "This haptic motor is not accessible";
			break;

			// Mouse related
		case CC_ERROR_MOUSE_NONE:
			return "No mouse could be found";
			break;
		case CC_ERROR_MOUSE_DATA:
			return "The mouse could not be read";
			break;

			// Memory related
		case CC_ERROR_MEMORY_OVERFLOW:
			return "Out of memory";
			break;
		default:
			return "Error message not defined";
			break;
	}
}
