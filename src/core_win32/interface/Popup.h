#pragma once

#include "../Types.h"

typedef enum {
	PopupError,
	PopupNotification
} PopupType;

typedef enum {
	DialogueOkCancel,
	DialogueRetryCancel,
	DialogueYesNo
} DialogueType;

void showPopup(char* message, char* caption, PopupType type);
bool showDialogue(char* message, char* caption, DialogueType type);