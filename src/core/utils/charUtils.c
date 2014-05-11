#include "charUtils.h"

int ccStrLength(const char* str)
{
	int i;
	for(i = 0; str[i] != 0; i++);
	return i;
}

void ccStrCopy(const char* source, char* destination)
{
	for(int i = 0; source[i] != 0; i++) {
		destination[i] = source[i];
	}
}

void ccStrTrimToChar(char* str, char ch, bool includeChar)
{
	int i;
	for(i = ccStrLength(str); str[i] != ch; i--);
	str[i + includeChar] = 0;
}