#pragma once

#include "../../../core/core.h"

#ifdef LINUX
#include "../linux/clipboard.h"
#elif defined WIN32
#include "../win32/clipboard.h"
#endif

typedef enum {
	ccClipboardText,
	ccClipboardBinary
} ccClipboardDataType;

void ccClipboardSetData(ccClipboardDataType type, void *data);