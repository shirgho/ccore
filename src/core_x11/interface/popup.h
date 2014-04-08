#pragma once

#include "../../core/types.h"

typedef enum {
	ccPopupError,
	ccPopupNotification
} ccPopupType;

typedef enum {
	ccDialogueYesNo
} ccDialogueType;

void ccShowPopup(char* message, char* caption, ccPopupType type);
bool ccShowDialogue(char* message, char* caption, ccDialogueType type);
