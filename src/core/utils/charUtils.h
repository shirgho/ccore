#pragma once

#include "../types.h"

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

void ccStrCopy(const char *source, char *destination);
void ccStrTrimToChar(char *str, char ch, bool includeChar);
void ccStrReplaceChar(char *str, char ch, char newCh);
char *ccStrCombine(int amount, ...);