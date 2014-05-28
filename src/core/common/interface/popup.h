#pragma once

#include "../core.h"

typedef struct _ccWindow ccWindow;
#include "window.h"

#ifdef LINUX
#include "../../x11/interface/popup.h"
#elif defined WIN32
#include "../../win32/interface/popup.h"
#endif

typedef enum {
	CC_POPUP_ERROR,
	CC_POPUP_NOTIFY
} ccPopupType;

typedef enum {
	CC_DIALOGUE_YESNO
} ccDialogueType;

void ccShowPopup(ccWindow* parent, const char* message, const char* caption, ccPopupType type);
bool ccShowDialogue(ccWindow* parent, const char* message, const char* caption, ccDialogueType type);
