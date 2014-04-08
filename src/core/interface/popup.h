#pragma once

#include "../core.h"

#ifdef X11
#include "../../core_x11/interface/popup.h"
#elif defined GTK
#include "../../core_gtk/interface/popup.h"
#elif defined WIN32
#include "../../core_win32/interface/popup.h"
#endif

void ccShowPopup(char* message, char* caption, ccPopupType type);
bool ccShowDialogue(char* message, char* caption, ccDialogueType type);
