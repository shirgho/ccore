#pragma once

inline int strLength(char* str)
{
	int i;
	for(i=0; str[i]!=0; i++);
	return i;
}

inline void strCopy(char* source, char* destination)
{
	for(int i=0; source[i]!=0; i++)
	{
		destination[i]=source[i];
	}
}

inline void strTrimToSlash(char* str)
{
	int i;
	for(i=strLength(str); str[i]!='\\'; i--);
	str[i+1]=0;
}