#pragma once

#include <dirent.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/joystick.h>

#include <ccore/gamepad.h>
#include <ccore/assert.h>
#include <ccore/print.h>

typedef struct {
	int fd;
} ccGamepad_x11;

#define GAMEPAD_DATA(gamepad) ((ccGamepad_x11*)(gamepad)->data)
