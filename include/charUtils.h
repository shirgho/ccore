#pragma once

#include "types.h"

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

void ccStrTrimToChar(char *str, char ch, bool includeChar);
void ccStrReplaceChar(char *str, char ch, char newCh);
void ccStrTrim(char *str);
char *ccStrConcatenate(int amount, ...);