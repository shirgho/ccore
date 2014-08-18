#include "lin_gamepad.h"

ccError ccGamepadConnect()
{
	DIR *d;
	struct dirent *dir;
	char dirName[80];
	int i, amount, fd, watch;

	amount = ccGamepadCount();

	if(amount == 0){
		return CC_ERROR_NONE;
	}

	ccGamepadDisconnect();

	ccMalloc(_gamepads, sizeof(ccGamepads));
	ccMalloc(_gamepads->data, sizeof(ccGamepads_x11));
	ccMalloc(_gamepads->gamepad, sizeof(ccGamepad) * amount);
	_gamepads->amount = amount;

	d = opendir("/dev/input");

	// Attach notifications to check if a device connects/disconnects
	fd = inotify_init();
	if(fd < 0){
		goto error;
	}

	watch = inotify_add_watch(fd, "/dev/input", IN_CREATE | IN_DELETE);
	if(fd < 0){
		goto error;
	}

	GAMEPADS_DATA()->fd = fd;
	GAMEPADS_DATA()->watch = watch;

	for(i = 0; i < amount; i++){
		ccMalloc((_gamepads->gamepad + i)->data, sizeof(ccGamepad_x11));

		while((dir = readdir(d)) != NULL){
			if(*dir->d_name == 'j' && *(dir->d_name + 1) == 's'){
				snprintf(dirName, 80, "/dev/input/%s", dir->d_name);
				break;
			}
		}

		fd = open(dirName, O_RDONLY | O_NONBLOCK);
		if(fd < 0){
			goto error;
		}

		_gamepads->gamepad[i].axisAmount = _gamepads->gamepad[i].buttonsAmount = 0;
		ccMalloc(_gamepads->gamepad[i].name, 80);

		ioctl(fd, JSIOCGAXES, &_gamepads->gamepad[i].axisAmount);
		ioctl(fd, JSIOCGBUTTONS, &_gamepads->gamepad[i].buttonsAmount);
		ioctl(fd, JSIOCGNAME(80), _gamepads->gamepad[i].name);

		ccPrintf("Gamepad opened device \"%s\" with %d axis and %d buttons\n", _gamepads->gamepad[i].name, _gamepads->gamepad[i].axisAmount,  _gamepads->gamepad[i].buttonsAmount);

		ccCalloc(_gamepads->gamepad[i].axis, _gamepads->gamepad[i].axisAmount, sizeof(int));
		ccCalloc(_gamepads->gamepad[i].buttons, _gamepads->gamepad[i].buttonsAmount, sizeof(char));

		GAMEPAD_DATA(_gamepads->gamepad + i)->fd = fd;
	}

	closedir(d);
	return CC_ERROR_NONE;

error:
	closedir(d);
	return CC_ERROR_GAMEPADDATA;
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

	inotify_rm_watch(GAMEPADS_DATA()->fd, GAMEPADS_DATA()->watch);
	close(GAMEPADS_DATA()->fd);

	for(i = 0; i < _gamepads->amount; i++){
		close(GAMEPAD_DATA(_gamepads->gamepad + i)->fd);

		free(_gamepads->gamepad[i].name);
		free(_gamepads->gamepad[i].buttons);
		free(_gamepads->gamepad[i].axis);
		free(_gamepads->gamepad[i].data);
	}
	free(_gamepads->gamepad);
	free(_gamepads->data);
	free(_gamepads);
}
