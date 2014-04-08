#pragma once

#include <Windows.h>

#include "../../core/types.h"

typedef enum {
	ccPopupError = MB_ICONERROR,
	ccPopupNotification = MB_ICONINFORMATION
} ccPopupType;

typedef enum {
	ccDialogueYesNo = MB_YESNO
} ccDialogueType;
