#include <stdio.h>
#include <stdlib.h>

#include "../src/core/interface/window.h"
#include "../src/core/interface/popup.h"
#include "../src/core/utils/timing.h"

int main(int argc, char** argv)
{
	ccWindow *window;
	bool quit;

	window = ccNewWindow(1024, 768, "CCore test application");
	ccGLBindContextWindow(window, 3, 2);
	quit = false;

	while(!quit) {
		ccPollEventWindow(window);
		switch(window->event.type){
		case ccEventWindowQuit:
			if(ccShowDialogue("Really quit?", "quit", ccDialogueYesNo) == true){
				quit = true;
			}
			break;
		case ccEventMouseDown:
			quit = true;
		default:
			break;
		}
	}

	ccFreeWindow(window);

	return 0;
}
