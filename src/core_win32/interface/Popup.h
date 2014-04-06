#pragma once

#include "../Types.h"

typedef enum {
	PopupError,
	PopupNotification
} ccPopupType;

typedef enum {
	DialogueOkCancel,
	DialogueRetryCancel,
	DialogueYesNo
} ccDialogueType;

void ccShowPopup(char* message, char* caption, ccPopupType type);
bool ccShowDialogue(char* message, char* caption, ccDialogueType type);