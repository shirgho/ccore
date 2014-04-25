#include "../../core/interface/popup.h"

void ccShowPopup(ccWindow* parent, const char* message, const char* caption, ccPopupType type)
{
	UINT mbType;
	HWND hwnd = parent == NULL?NULL:parent->winHandle;

	switch(type) {
	case CC_POPUP_ERROR:
		mbType = MB_ICONERROR;
		break;
	default:
		mbType = MB_ICONINFORMATION;
		break;
	}
	MessageBox(hwnd, message, caption, mbType);
}

bool ccShowDialogue(ccWindow* parent, const char* message, const char* caption, ccDialogueType type)
{
	HWND hwnd = parent == NULL?NULL:parent->winHandle;
	return MessageBox(hwnd, message, caption, MB_YESNO) == IDYES?true:false;
}