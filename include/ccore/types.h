//__________________________________________________________________________________\\
//                                                                                  \\
//                               ______                                             \\
//                              /  ___ /__  _ __ ___                                \\
//                             /  / __/ _ \| '__/ _ \                               \\
//                            |  | (_| (_) | | |  __/                               \\
//                             \  \___\___/|_|  \___| 1.0                           \\
//                              \______\                                            \\
//                                                                                  \\
//             Copyright (C) 2014 \ Job Talle (jobtalle@hotmail.com)                \\
//                                 \ Thomas Versteeg (thomasversteeg@gmx.com)       \\
//__________________________________________________________________________________\\
//                                                                                  \\
//      This program is free software: you can redistribute it and/or modify        \\
//      it under the terms of the GNU General Public License as published by        \\
//      the Free Software Foundation.                                               \\
//                                                                                  \\
//      This program is distributed without any warranty; see the GNU               \\
//      General Public License for more details.                                    \\
//                                                                                  \\
//      You should have received a copy of the GNU General Public License           \\
//      along with this program. If not, see <http://www.gnu.org/licenses/>.        \\
//__________________________________________________________________________________\\

#pragma once

#include <stdlib.h>
#include <stdarg.h>

#define true 1
#define false 0

typedef char bool;

typedef struct {
	int x, y;
} ccPoint;

typedef struct {
	int x, y;
	int width, height;
} ccRect;

#ifdef __GNUC__
#define max(a,b) \
	({ __typeof__ (a) _a = (a); \
	__typeof__ (b) _b = (b); \
	_a > _b ? _a : _b; })

#define min(a,b) \
({ __typeof__ (a) _a = (a); \
    __typeof__ (b) _b = (b); \
    _a < _b ? _a : _b; })
#endif

int ccRectIntersectionArea(ccRect *rectA, ccRect *rectB);
ccRect ccRectConcatenate(int amount, ccRect* rectArray);