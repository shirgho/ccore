#include "charUtils.h"

int ccStrLength(char* str)
{
	int i;
	for(i = 0; str[i] != 0; i++);
	return i;
}

void ccStrCopy(char* source, char* destination)
{
	for(int i = 0; source[i] != 0; i++) {
		destination[i] = source[i];
	}
}

void ccStrTrimToSlash(char* str)
{
	int i;
	for(i = ccStrLength(str); str[i] != '\\'; i--);
	str[i + 1] = 0;
}