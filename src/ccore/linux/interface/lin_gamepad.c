#include "lin_gamepad.h"

static int openGamepadDescriptor(char *locName)
{
	char dirName[30];
	int fd;

	snprintf(dirName, 30, "/dev/input/%s", locName);
	fd = open(dirName, O_RDONLY | O_NONBLOCK, 0);
	if(fd < 0){
		perror(dirName);
	}

	return fd;
}

static ccError createGamepad(char *locName, int i)
{
	char buf[64];
	int fd;

	fd = openGamepadDescriptor(locName);
	if(fd < 0){
		return CC_ERROR_GAMEPADDATA;
	}

	if(i == 0){
		ccMalloc(_gamepads->gamepad, sizeof(ccGamepad));
	}else{
		ccRealloc(_gamepads->gamepad, (i + 1) * sizeof(ccGamepad));
	}
	ccMalloc((_gamepads->gamepad + i)->data, sizeof(ccGamepad_x11));

	// Clear gamepad buffer
	while(read(fd, buf, 64) > 0);

	_gamepads->gamepad[i].plugged = true;
	_gamepads->gamepad[i].axisAmount = _gamepads->gamepad[i].buttonAmount = 0;
	ccMalloc(_gamepads->gamepad[i].name, 80);

	ioctl(fd, JSIOCGAXES, &_gamepads->gamepad[i].axisAmount);
	ioctl(fd, JSIOCGBUTTONS, &_gamepads->gamepad[i].buttonAmount);
	ioctl(fd, JSIOCGNAME(80), _gamepads->gamepad[i].name);

	ccCalloc(_gamepads->gamepad[i].axis, _gamepads->gamepad[i].axisAmount, sizeof(int));
	ccCalloc(_gamepads->gamepad[i].button, _gamepads->gamepad[i].buttonAmount, sizeof(char));

	GAMEPAD_DATA(_gamepads->gamepad + i)->fd = fd;
	GAMEPAD_DATA(_gamepads->gamepad + i)->id = atoi(locName + 2);

	return CC_ERROR_NONE;
}

static bool canReadINotify()
{
	fd_set set;
	struct timeval timeout;

	FD_ZERO(&set);
	FD_SET(GAMEPADS_DATA()->fd, &set);
	timeout.tv_sec = 0;
	timeout.tv_usec = 0;

	return select(GAMEPADS_DATA()->fd + 1, &set, NULL, NULL, &timeout) > 0 && 
		FD_ISSET(GAMEPADS_DATA()->fd, &set);
}

ccGamepadEvent ccGamepadEventPoll()
{
	struct js_event js;
	struct inotify_event ne;
	ccGamepadEvent event;
	int i, id;

	while(canReadINotify()){
		if(read(GAMEPADS_DATA()->fd, &ne, sizeof(struct inotify_event) + 16) >= 0){
			if(*ne.name != 'j' || *(ne.name + 1) != 's'){
				continue;
			}

			// Find the matching gamepad
			event.id = -1;
			id = atoi(ne.name + 2);	
			for(i = 0; i < _gamepads->amount; i++){
				if(GAMEPAD_DATA(_gamepads->gamepad + i)->id == id){
					event.id = i;
					break;
				}
			}

			if(ne.mask & IN_DELETE){
				if(event.id != -1){
					_gamepads->gamepad[event.id].plugged = false;
					close(GAMEPAD_DATA(_gamepads->gamepad + event.id)->fd);
				}

				event.type = CC_GAMEPAD_DISCONNECT;
			}else if(ne.mask & IN_CREATE){
				if(event.id != -1){
					_gamepads->gamepad[event.id].plugged = true;
					GAMEPAD_DATA(_gamepads + event.id)->fd = openGamepadDescriptor(ne.name);
					if(GAMEPAD_DATA(_gamepads + event.id)->fd < 0){
						_gamepads->gamepad[event.id].plugged = false;
						GAMEPAD_DATA(_gamepads + event.id)->fd = 0;
					}
				}else{
					createGamepad(ne.name, _gamepads->amount);
					_gamepads->amount++;
				}

				event.type = CC_GAMEPAD_CONNECT;
			}
			return event;
		}
	}

	event.type = CC_GAMEPAD_UNHANDLED;
	for(i = 0; i < _gamepads->amount; i++){
		if(!_gamepads->gamepad[i].plugged){
			continue;
		}
		if(read(GAMEPAD_DATA(_gamepads->gamepad + i)->fd, &js, sizeof(struct js_event)) > 0){
			event.id = i;
			event.type = CC_GAMEPAD_UNHANDLED;

			switch(js.type & ~JS_EVENT_INIT){
				case JS_EVENT_AXIS:
					if(_gamepads->gamepad[i].axis[js.number] != js.value){
						event.axisId = js.number;

						event.type = CC_GAMEPAD_AXIS_MOVE;

						_gamepads->gamepad[i].axis[js.number] = js.value;
						return event;
					}
				case JS_EVENT_BUTTON:
					if(_gamepads->gamepad[i].button[js.number] != (js.value != 0)){
						event.buttonId = js.number;

						if(_gamepads->gamepad[i].button[js.number] == 0){
							event.type = CC_GAMEPAD_BUTTON_DOWN;
						}else{
							event.type = CC_GAMEPAD_BUTTON_UP;
						}

						_gamepads->gamepad[i].button[js.number] = js.value != 0;
						return event;
					}
			}
		}
	}

	return event;
}

ccError ccGamepadInitialize()
{
	DIR *d;
	struct dirent *dir;
	int fd, watch;

	ccGamepadFree();

	// Attach notifications to check if a device connects/disconnects
	fd = inotify_init();
	if(fd < 0){
		goto error;
	}

	watch = inotify_add_watch(fd, "/dev/input", IN_CREATE | IN_DELETE);
	if(watch < 0){
		goto error;
	}

	if(_gamepads == NULL){
		ccMalloc(_gamepads, sizeof(ccGamepads));
		ccMalloc(_gamepads->data, sizeof(ccGamepads_x11));
	}
	_gamepads->amount = 0;

	GAMEPADS_DATA()->fd = fd;
	GAMEPADS_DATA()->watch = watch;

	d = opendir("/dev/input");
	while((dir = readdir(d)) != NULL){
		if(*dir->d_name == 'j' && *(dir->d_name + 1) == 's'){
			if(createGamepad(dir->d_name, _gamepads->amount) != CC_ERROR_NONE){
				goto error;
			}
			_gamepads->amount++;
		}
	}

	closedir(d);
	if(_gamepads->amount == 0){
		return CC_ERROR_NOGAMEPAD;
	}else{
		return CC_ERROR_NONE;
	}

error:
	free(_gamepads->data);
	free(_gamepads);
	closedir(d);
	close(fd);
	close(watch);
	return CC_ERROR_GAMEPADDATA;
}

void ccGamepadFree()
{
	int i;

	if(_gamepads == NULL){
		return;
	}

	inotify_rm_watch(GAMEPADS_DATA()->fd, GAMEPADS_DATA()->watch);
	close(GAMEPADS_DATA()->fd);

	for(i = 0; i < _gamepads->amount; i++){
		if(_gamepads->gamepad[i].plugged){
			close(GAMEPAD_DATA(_gamepads->gamepad + i)->fd);
		}

		free(_gamepads->gamepad[i].name);
		free(_gamepads->gamepad[i].data);
	}
	if(_gamepads->amount != 0){
		free(_gamepads->gamepad);
	}
	free(_gamepads->data);
	free(_gamepads);

	_gamepads->amount = 0;
}
