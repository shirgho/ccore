#include <Windows.h>

#include "Popup.h"

void showPopup(char* message, char* caption, ccPopupType type)
{
	MessageBox(NULL, message, caption, MB_OK);
}

bool showDialogue(char* message, char* caption, ccDialogueType type)
{
	return MessageBox(NULL, message, caption, MB_YESNO) == 0 ? false : true;
}