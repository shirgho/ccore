#include "charUtils.h"

void ccStrCopy(const char* source, char* destination)
{
	int i;
	for(i = 0; source[i] != '\0'; i++) {
		destination[i] = source[i];
	}
	destination[i] = 0;
}

void ccStrTrimToChar(char* str, char ch, bool includeChar)
{
	int i;
	for(i = strlen(str); str[i] != ch; i--);
	str[i + includeChar] = 0;
}

void ccStrReplaceChar(char *str, char ch, char newCh)
{
	int i;
	for(i = strlen(str); i >= 0; i--)
	{
		if(str[i] == ch) str[i] = newCh;
	}
}

char *ccStrCombine(int amount, ...)
{
	va_list strings;
	int i;
	int l = 0;
	int *lengths;
	char **elements;
	char *newStr;

	lengths = malloc(amount*sizeof(int));
	elements = malloc(amount*sizeof(char*));
	
	va_start(strings, amount);
	for(i = 0; i < amount; i++) {
		elements[i] = va_arg(strings, char*);
		lengths[i] = strlen(elements[i]);
		l += lengths[i];
	}

	newStr = malloc(l+1);
	newStr[l] = '\0';
	l = 0;

	for(i = 0; i < amount; i++) {
		ccStrCopy(elements[i], newStr + l);
		l += lengths[i];
	}

	va_end(strings);

	free(lengths);
	free(elements);
	return newStr;
}