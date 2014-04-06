#include "Popup.h"

void showPopup(char* message, char* caption, ccPopupType type)
{
	MessageBox(NULL, message, caption, type);
}

bool showDialogue(char* message, char* caption, ccDialogueType type)
{
	return MessageBox(NULL, message, caption, type) == 0 ? false : true;
}