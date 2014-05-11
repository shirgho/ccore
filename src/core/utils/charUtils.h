#pragma once

#include "../types.h"

int ccStrLength(const char* str);
void ccStrCopy(const char* source, char* destination);
void ccStrTrimToChar(char* str, char ch, bool includeChar);