#include <Windows.h>

#include "Popup.h"

void showPopup(char* message, char* caption, PopupType type)
{
	MessageBox(NULL, message, caption, MB_OK);
}

bool showDialogue(char* message, char* caption, DialogueType type)
{
	return MessageBox(NULL, message, caption, MB_YESNO) == 0 ? false : true;
}