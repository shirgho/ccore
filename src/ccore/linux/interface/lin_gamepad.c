#include "lin_gamepad.h"

ccError ccGamepadConnect()
{
	DIR *d;
	struct dirent *dir;
	char dirName[80];
	int i, j, amount;

	amount = ccGamepadCount();

	ccGamepadDisconnect();

	ccMalloc(_gamepads, sizeof(ccGamepads));
	ccMalloc(_gamepads->gamepad, sizeof(ccGamepad) * amount);
	_gamepads->amount = amount;

	d = opendir("/dev/input");

	for(i = 0; i < amount; i++){
		ccMalloc((_gamepads->gamepad + i)->data, sizeof(ccGamepad_x11));

		j = 0;
		while((dir = readdir(d)) != NULL){
			if(*dir->d_name == 'j' && *(dir->d_name + 1) == 's'){
				if(j == i){
					snprintf(dirName, 80, "/dev/input/js%c", *dir->d_name + 2);
					GAMEPAD_DATA(_gamepads->gamepad + i)->fd = open(dirName, O_RDONLY);
					break;
				}
				j++;
			}
		}

		if(GAMEPAD_DATA(_gamepads->gamepad + i)->fd == NULL){
			closedir(d);
			return CC_ERROR_GAMEPADDATA;
		}

		ioctl(GAMEPAD_DATA(_gamepads->gamepad + i)->fd, JSIOCGAXES, &_gamepads->gamepad[i].axisAmount);
		ioctl(GAMEPAD_DATA(_gamepads->gamepad + i)->fd, JSIOCGBUTTONS, &_gamepads->gamepad[i].buttonsAmount);
		ioctl(GAMEPAD_DATA(_gamepads->gamepad + i)->fd, JSIOCGNAME(80), &_gamepads->gamepad[i].name);
	}

	closedir(d);

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
	int i;

	if(_gamepads == NULL){
		return;
	}

	for(i = 0; i < _gamepads->amount; i++){
		free(_gamepads->gamepad[i].name);
		free(_gamepads->gamepad[i].data);
	}
	free(_gamepads->gamepad);
	free(_gamepads);
}
