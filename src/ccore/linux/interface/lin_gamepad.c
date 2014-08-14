#include "lin_gamepad.h"

ccError ccGamepadConnect()
{
	int amount;

	amount = ccGamepadCount();

	return CC_ERROR_NONE;
}

int ccGamepadCount()
{
	DIR *d;
	struct dirent *dir;
	int amount;

	d = opendir("/dev/input");
		
	if(d == NULL){
		closedir(d);
		return -1;
	}

	amount = 0;
	while((dir = readdir(d)) != NULL){
		if(*dir->d_name == 'j' && *(dir->d_name + 1) == 's'){
			amount++;
		}
	}

	closedir(d);

	return amount;
}

void ccGamepadDisconnect()
{

}
