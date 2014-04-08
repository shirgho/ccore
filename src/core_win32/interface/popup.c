#include "../../core/interface/popup.h"

void ccShowPopup(const char* message, const char* caption, ccPopupType type)
{
	UINT mbType;
	switch(type) {
	case ccPopupError:
		mbType = MB_ICONERROR;
		break;
	default:
		mbType = MB_ICONINFORMATION;
		break;
	}
	MessageBox(NULL, message, caption, mbType);
}

bool ccShowDialogue(const char* message, const char* caption, ccDialogueType type)
{
	return MessageBox(NULL, message, caption, MB_YESNO) == IDYES ? true : false;
}