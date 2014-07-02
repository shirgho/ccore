#pragma once

#include "../../common/interface/event.h"

#define CC_KEY_FUNCTION(NUMBER) NUMBER 

#define CC_KEY_NUM(NUMBER) NUMBER 

#define CC_KEY_BACKSPACE 
#define CC_KEY_TAB 
#define CC_KEY_RETURN 
#define CC_KEY_ESCAPE 
#define CC_KEY_SPACE 
#define CC_KEY_CAPSLOCK 
#define CC_KEY_INSERT 
#define CC_KEY_DELETE 
#define CC_KEY_HOME 
#define CC_KEY_END 
#define CC_KEY_PAGEUP 
#define CC_KEY_PAGEDOWN 
#define CC_KEY_PRINTSCREEN 
#define CC_KEY_SCROLLLOCK 
#define CC_KEY_NUMLOCK
#define CC_KEY_PAUSEBREAK

#define CC_KEY_LSHIFT 0
#define CC_KEY_RSHIFT 1
#define CC_KEY_LCONTROL 2
#define CC_KEY_RCONTROL 3

#define CC_KEY_LEFT
#define CC_KEY_UP 
#define CC_KEY_RIGHT
#define CC_KEY_DOWN

char ccGetKeyPressed();
char ccGetKeyReleased();
