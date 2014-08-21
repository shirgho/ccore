#pragma once

#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <sys/ioctl.h>
#include <sys/inotify.h>

#include <ccore/mouse.h>
#include <ccore/assert.h>
#include <ccore/print.h>

typedef struct {
	int id, fd;
} ccMouse_x11;

typedef struct {
	int fd, watch;
} ccMice_x11;

#define MOUSE_DATA(mouse) ((ccMouse_x11*)(mouse)->data)
#define MICE_DATA() ((ccMice_x11*)(_mice)->data)

ccMouseEvent ccMouseEventPoll();
