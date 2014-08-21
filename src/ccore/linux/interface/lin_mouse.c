#include "lin_mouse.h"

static int openMouseDescriptor(char *locName)
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

static ccError createMouse(char *locName, int i)
{
	char buf[64];
	int fd;

	fd = openMouseDescriptor(locName);
	if(fd < 0){
		return CC_ERROR_MOUSEDATA;
	}

	if(i == 0){
		ccMalloc(_mice->mouse, sizeof(ccMouse));
	}else{
		ccRealloc(_mice->mouse, (i + 1) * sizeof(ccMouse));
	}
	ccMalloc((_mice->mouse + i)->data, sizeof(ccMouse_x11));

	// Clear mouse buffer
	while(read(fd, buf, 64) > 0);

	_mice->mouse[i].plugged = true;
	_mice->mouse[i].buttonAmount = 0;
	ccMalloc(_mice->mouse[i].name, 80);

	//ioctl(fd, JSIOCGBUTTONS, &_mice->mouse[i].buttonAmount);
	//ioctl(fd, EVIOCGNAME(80), _mice->mouse[i].name);

	ccCalloc(_mice->mouse[i].button, _mice->mouse[i].buttonAmount, sizeof(char));

	MOUSE_DATA(_mice->mouse + i)->fd = fd;
	MOUSE_DATA(_mice->mouse + i)->id = atoi(locName + 5);

	return CC_ERROR_NONE;
}

static bool canReadINotify()
{
	fd_set set;
	struct timeval timeout;

	FD_ZERO(&set);
	FD_SET(MICE_DATA()->fd, &set);
	timeout.tv_sec = 0;
	timeout.tv_usec = 0;

	return select(MICE_DATA()->fd + 1, &set, NULL, NULL, &timeout) > 0 && 
		FD_ISSET(MICE_DATA()->fd, &set);
}

ccMouseEvent ccMouseEventPoll()
{
	struct inotify_event ne;
	ccMouseEvent event;
	int i, id;

	while(canReadINotify()){
		if(read(MICE_DATA()->fd, &ne, sizeof(struct inotify_event) + 16) >= 0){
			if(strncmp(ne.name, "mouse", 5) != 0){
				continue;
			}

			// Find the matching mouse
			event.id = -1;
			id = atoi(ne.name + 5);	
			for(i = 0; i < _mice->amount; i++){
				if(MOUSE_DATA(_mice->mouse + i)->id == id){
					event.id = i;
					break;
				}
			}

			if(ne.mask & IN_DELETE){
				if(event.id != -1){
					_mice->mouse[event.id].plugged = false;
					close(MOUSE_DATA(_mice->mouse + event.id)->fd);
				}

				event.type = CC_MOUSE_DISCONNECT;
			}else if(ne.mask & IN_CREATE){
				if(event.id != -1){
					_mice->mouse[event.id].plugged = true;
					MOUSE_DATA(_mice + event.id)->fd = openMouseDescriptor(ne.name);
					if(MOUSE_DATA(_mice + event.id)->fd < 0){
						_mice->mouse[event.id].plugged = false;
						MOUSE_DATA(_mice + event.id)->fd = 0;
					}
				}else{
					createMouse(ne.name, _mice->amount);
					_mice->amount++;
				}

				event.type = CC_MOUSE_CONNECT;
			}
			return event;
		}
	}

	event.type = CC_MOUSE_UNHANDLED;
	for(i = 0; i < _mice->amount; i++){
		if(!_mice->mouse[i].plugged){
			continue;
		}
	}

	return event;
}

ccError ccMouseInitialize()
{
	DIR *d;
	struct dirent *dir;
	int fd, watch;

	ccMouseFree();

	// Attach notifications to check if a device connects/disconnects
	fd = inotify_init();
	if(fd < 0){
		goto error;
	}

	watch = inotify_add_watch(fd, "/dev/input", IN_CREATE | IN_DELETE);
	if(watch < 0){
		goto error;
	}

	if(_mice == NULL){
		ccMalloc(_mice, sizeof(ccMice));
		ccMalloc(_mice->data, sizeof(ccMice_x11));
	}
	_mice->amount = 0;

	MICE_DATA()->fd = fd;
	MICE_DATA()->watch = watch;

	d = opendir("/dev/input");
	while((dir = readdir(d)) != NULL){
		if(strncmp(dir->d_name, "mouse", 5) == 0){
			if(!createMouse(dir->d_name, _mice->amount) != CC_ERROR_NONE){
				_mice->amount++;
			}
		}
	}

	closedir(d);
	if(_mice->amount == 0){
		return CC_ERROR_NOMOUSE;
	}else{
		return CC_ERROR_NONE;
	}

error:
	free(_mice->data);
	free(_mice);
	closedir(d);
	close(fd);
	close(watch);
	return CC_ERROR_MOUSEDATA;
}

void ccMouseFree()
{
	int i;

	if(_mice == NULL){
		return;
	}

	inotify_rm_watch(MICE_DATA()->fd, MICE_DATA()->watch);
	close(MICE_DATA()->fd);

	for(i = 0; i < _mice->amount; i++){
		if(_mice->mouse[i].plugged){
			close(MOUSE_DATA(_mice->mouse + i)->fd);
		}

		free(_mice->mouse[i].name);
		free(_mice->mouse[i].data);
	}
	if(_mice->amount != 0){
		free(_mice->mouse);
	}
	free(_mice->data);
	free(_mice);

	_mice->amount = 0;
}
