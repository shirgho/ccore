#include <ccore/event.h>

char ccEventKeyToChar(int key)
{
	if(key >= CC_KEY_0 && key <= CC_KEY_9){
		return '0' + (key - CC_KEY_0);
	}

	if(key >= CC_KEY_NUM0 && key <= CC_KEY_NUM9){
		return '0' + (key - CC_KEY_NUM0);
	}

	if(key >= CC_KEY_A && key <= CC_KEY_Z){
		return 'a' + (key - CC_KEY_A);
	}

	switch(key){
		case CC_KEY_TAB:
			return '\t';
		case CC_KEY_RETURN:
			return '\n';
		case CC_KEY_SPACE:
			return ' ';
		default:
			return '\0';
	}
}
