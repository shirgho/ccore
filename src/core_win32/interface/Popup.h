#pragma once

#include <Windows.h>

#include "../Types.h"

typedef enum {
	PopupError = MB_ICONERROR,
	PopupNotification = MB_ICONINFORMATION
} ccPopupType;

typedef enum {
	DialogueOkCancel = MB_OKCANCEL,
	DialogueRetryCancel = MB_RETRYCANCEL,
	DialogueYesNo = MB_YESNO
} ccDialogueType;

void ccShowPopup(char* message, char* caption, ccPopupType type);
bool ccShowDialogue(char* message, char* caption, ccDialogueType type);