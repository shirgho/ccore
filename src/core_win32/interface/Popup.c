#include "Popup.h"

void ccShowPopup(char* message, char* caption, ccPopupType type)
{
	MessageBox(NULL, message, caption, type);
}

bool ccShowDialogue(char* message, char* caption, ccDialogueType type)
{
	return MessageBox(NULL, message, caption, type) == IDYES ? true : false;
}