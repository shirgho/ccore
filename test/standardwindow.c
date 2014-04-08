#include <stdio.h>
#include <stdlib.h>

#include "../src/core/interface/window.h"
#include "../src/core/interface/popup.h"
#include "../src/core/utils/timing.h"

int main(int argc, char** argv)
{
	ccWindow *window;
	printf("entry point\n");

	window = ccNewWindow(1024, 768, "CCore application");

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
