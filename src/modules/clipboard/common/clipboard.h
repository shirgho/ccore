#pragma once

#include "../../core/core.h"

#ifdef LINUX
#include "../../modules_/modules_linux/clipboard/clipboard.h"
#elif defined WIN32
#include "../../modules_/modules_win32/clipboard/clipboard.h"
#endif

typedef enum {
	ccClipboardText,
	ccClipboardBinary
} ccClipboardDataType;

void ccClipboardSetData(ccClipboardDataType type, void *data);