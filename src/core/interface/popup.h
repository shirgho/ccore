#pragma once

#include "../core.h"

#ifdef X11
#include "../../core_x11/interface/popup.h"
#elif defined GTK
#include "../../core_gtk/interface/popup.h"
#elif defined WIN32
#include "../../core_win32/interface/popup.h"
#endif

typedef enum {
	ccPopupError,
	ccPopupNotification
} ccPopupType;

typedef enum {
	ccDialogueYesNo
} ccDialogueType;

void ccShowPopup(const char* message, const char* caption, ccPopupType type);
bool ccShowDialogue(const char* message, const char* caption, ccDialogueType type);
