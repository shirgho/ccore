#include <stdio.h>
#include <stdlib.h>

#include "core/interface/window.h"
#include "core/interface/popup.h"
#include "core/utils/timing.h"

int main(int argc, char** argv)
{
	printf("entry point\n");

	ccWindow *window = ccNewWindow(1024, 768, "CCore application");

	while(true) {
		ccDelay(10);
		while(window->pollEvent()) {
			switch(window->event.type) {
			case ccEventWindowQuit:
				if(ccShowDialogue("Really quit?", "quit", ccDialogueYesNo)) return 0;
				break;
			}
		}
	}

	ccFreeWindow(window);

	return 0;
}