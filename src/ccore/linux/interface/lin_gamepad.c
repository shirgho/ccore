#include "lin_gamepad.h"

static ccError refreshGamepad(int i)
{
	char dirName[30], buf[64];
	int fd;

	if(GAMEPAD_DATA(_gamepads->gamepad + i)->fd <= 0){
		snprintf(dirName, 30, "/dev/input/js%d", GAMEPAD_DATA(_gamepads->gamepad + i)->id);
		fd = open(dirName, O_RDONLY | O_NONBLOCK);
		if(fd < 0){
			perror(dirName);
			return CC_ERROR_GAMEPADDATA;
		}

		// Clear gamepad buffer
		while(read(fd, buf, 64) > 0);

		_gamepads->pluggedAmount++;
		_gamepads->gamepad[i].plugged = true;
		_gamepads->gamepad[i].axisAmount = _gamepads->gamepad[i].buttonsAmount = 0;

		GAMEPAD_DATA(_gamepads->gamepad + i)->fd = fd;
	}else{
		fd = GAMEPAD_DATA(_gamepads->gamepad + i)->fd;
	}	

	ioctl(fd, JSIOCGAXES, &_gamepads->gamepad[i].axisAmount);
	ioctl(fd, JSIOCGBUTTONS, &_gamepads->gamepad[i].buttonsAmount);
	ioctl(fd, JSIOCGNAME(80), _gamepads->gamepad[i].name);

	ccCalloc(_gamepads->gamepad[i].axis, _gamepads->gamepad[i].axisAmount, sizeof(int));
	ccCalloc(_gamepads->gamepad[i].buttons, _gamepads->gamepad[i].buttonsAmount, sizeof(char));

	return CC_ERROR_NONE;
}

static ccError createGamepad(char *locName, int i)
{
	char dirName[30], buf[64];
	int fd;

	ccMalloc((_gamepads->gamepad + i)->data, sizeof(ccGamepad_x11));
	snprintf(dirName, 30, "/dev/input/%s", locName);

	fd = open(dirName, O_RDONLY | O_NONBLOCK, 0);
	if(fd < 0){
		perror(dirName);
		return CC_ERROR_GAMEPADDATA;
	}

	// Clear gamepad buffer
	while(read(fd, buf, 64) > 0);

	_gamepads->pluggedAmount++;
	_gamepads->gamepad[i].plugged = true;
	_gamepads->gamepad[i].axisAmount = _gamepads->gamepad[i].buttonsAmount = 0;
	ccMalloc(_gamepads->gamepad[i].name, 80);

	ioctl(fd, JSIOCGAXES, &_gamepads->gamepad[i].axisAmount);
	ioctl(fd, JSIOCGBUTTONS, &_gamepads->gamepad[i].buttonsAmount);
	ioctl(fd, JSIOCGNAME(80), _gamepads->gamepad[i].name);

	ccCalloc(_gamepads->gamepad[i].axis, _gamepads->gamepad[i].axisAmount, sizeof(int));
	ccCalloc(_gamepads->gamepad[i].buttons, _gamepads->gamepad[i].buttonsAmount, sizeof(char));

	GAMEPAD_DATA(_gamepads->gamepad + i)->fd = fd;
	GAMEPAD_DATA(_gamepads->gamepad + i)->id = atoi(locName + 2);

	return CC_ERROR_NONE;
}

ccError ccGamepadConnect()
{
	DIR *d;
	struct dirent *dir;
	int i, amount, fd, watch;

	ccGamepadDisconnect();

	amount = ccGamepadCount();

	if(_gamepads == NULL){
		ccMalloc(_gamepads, sizeof(ccGamepads));
	}
	_gamepads->totalAmount = amount;
	_gamepads->pluggedAmount = 0;

	ccMalloc(_gamepads->data, sizeof(ccGamepads_x11));

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

	if(amount == 0){
		return CC_ERROR_NONE;
	}

	ccMalloc(_gamepads->gamepad, sizeof(ccGamepad) * amount);

	d = opendir("/dev/input");
	for(i = 0; i < amount; i++){
		while((dir = readdir(d)) != NULL){
			if(*dir->d_name == 'j' && *(dir->d_name + 1) == 's'){
				if(createGamepad(dir->d_name, i) != CC_ERROR_NONE){
					goto error;
				}
				break;
			}
		}
	}

	closedir(d);
	return CC_ERROR_NONE;

error:
	closedir(d);
	return CC_ERROR_GAMEPADDATA;
}

ccError ccGamepadRefresh()
{
	struct dirent *dir;
	DIR *d;
	int i, id, amount;
	bool found;

	amount = ccGamepadCount();

	if(amount == _gamepads->pluggedAmount){
		return CC_ERROR_NONE;
	}

	d = opendir("/dev/input");

	// A new item needs to be added to the list
	if(amount > _gamepads->totalAmount){
		ccRealloc(_gamepads->gamepad, sizeof(ccGamepad) * amount);

		// Scan the directories for the newest device and add it
		while((dir = readdir(d)) != NULL && _gamepads->totalAmount <= amount){
			if(*dir->d_name == 'j' && *(dir->d_name + 1) == 's'){
				id = atoi(dir->d_name + 2);
				found = false;
				for(i = 0; i < _gamepads->totalAmount; i++){
					if(GAMEPAD_DATA(_gamepads->gamepad + i)->id == id){
						found = true;
						break;
					}
				}
				if(!found){
					if(createGamepad(dir->d_name, _gamepads->totalAmount) != CC_ERROR_NONE){
						goto error;							
					}
					_gamepads->totalAmount++;
				}
			}
		}
	}

	if(amount > _gamepads->pluggedAmount){
		for(i = 0; i < _gamepads->totalAmount; i++){
			if(!_gamepads->gamepad[i].plugged){
				refreshGamepad(i);
			}
		}
	}else if(amount < _gamepads->pluggedAmount){
		// Set the plugged status for all the gamepads
		for(i = 0; i < _gamepads->totalAmount; i++){
			_gamepads->gamepad[i].plugged = false;
		}
		_gamepads->pluggedAmount = 0;

		while((dir = readdir(d)) != NULL){
			if(*dir->d_name == 'j' && *(dir->d_name + 1) == 's'){
				id = atoi(dir->d_name + 2);
				for(i = 0; i < _gamepads->totalAmount; i++){
					if(GAMEPAD_DATA(_gamepads->gamepad + i)->id == id){
						_gamepads->gamepad[i].plugged = true;
						_gamepads->pluggedAmount++;
						break;
					}
				}
			}
		}
		// Close unplugged gamepads
		for(i = 0; i < _gamepads->totalAmount; i++){
			if(!_gamepads->gamepad[i].plugged && GAMEPAD_DATA(_gamepads->gamepad + i)->fd >= 0){
				free(_gamepads->gamepad[i].axis);
				free(_gamepads->gamepad[i].buttons);
				close(GAMEPAD_DATA(_gamepads->gamepad + i)->fd);
				GAMEPAD_DATA(_gamepads->gamepad + i)->fd = -1;
			}
		}
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

	for(i = 0; i < _gamepads->totalAmount; i++){
		if(_gamepads->gamepad[i].plugged){
			close(GAMEPAD_DATA(_gamepads->gamepad + i)->fd);
		}

		free(_gamepads->gamepad[i].name);
		free(_gamepads->gamepad[i].data);
	}
	free(_gamepads->gamepad);
	free(_gamepads->data);

	_gamepads->totalAmount = _gamepads->pluggedAmount = 0;
}
