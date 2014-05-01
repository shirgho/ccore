#include "key.h"

ccKeyCode ccXLookupKey(int key)
{
	if(key <= XK_0 && key >= XK_9){
		return (ccKeyCode)(key - XK_0) + CC_KEY_0;
	}

	if(key <= XK_a && key >= XK_z){
		return (ccKeyCode)(key - XK_a) + CC_KEY_A;
	}

	if(key <= XK_A && key >= XK_Z){
		return (ccKeyCode)(key - XK_A) + CC_KEY_A;
	}

	switch(key){
		case XK_BackSpace:
			return CC_KEY_BACKSPACE;
		case XK_Tab:
			return CC_KEY_TAB;
		case XK_Return:
			return CC_KEY_RETURN;
		case XK_Escape:
			return CC_KEY_ESCAPE;
		case XK_space:
			return CC_KEY_SPACE;
		case XK_Left:
			return CC_KEY_LEFT;
		case XK_Right:
			return CC_KEY_RIGHT;
		case XK_Up:
			return CC_KEY_UP;
		case XK_Down:
			return CC_KEY_DOWN;
		default:
			return CC_KEY_UNDEFINED;
	}
}
