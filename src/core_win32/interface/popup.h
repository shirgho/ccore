#pragma once

#include <Windows.h>

#include "../types.h"

typedef enum {
	ccPopupError = MB_ICONERROR,
	ccPopupNotification = MB_ICONINFORMATION
} ccPopupType;

typedef enum {
	ccDialogueYesNo = MB_YESNO
} ccDialogueType;

void ccShowPopup(char* message, char* caption, ccPopupType type);
bool ccShowDialogue(char* message, char* caption, ccDialogueType type);