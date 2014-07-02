#include "key.h"

ccKeyData ccGetKeyPressed()
{
	ccKeyData key;

	key.keyCode = CC_KEY_UNDEFINED;

	return key;
}

ccKeyData ccGetKeyReleased()
{
	ccKeyData key;

	key.keyCode = CC_KEY_UNDEFINED;

	return key;
}
