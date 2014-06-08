#pragma once

#include "../../../core/common/core.h"

#ifdef X11
#include "../x11/clipboard.h"
#elif defined WIN32
#include "../win32/clipboard.h"
#endif

typedef enum {
	ccClipboardText,
	ccClipboardBinary
} ccClipboardDataType;

void ccClipboardSetData(ccClipboardDataType type, void *data);
