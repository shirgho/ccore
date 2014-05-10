#pragma once

#include "../types.h"

int ccStrLength(const char* str);
void ccStrCopy(const char* source, char* destination);
void ccStrTrimToSlash(char* str, bool includeSlash);