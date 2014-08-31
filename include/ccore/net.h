//__________________________________________________________________________________//
//                               ______                                             //
//                              /  ___ /__  _ __ ___                                //
//                             /  / __/ _ \| '__/ _ \                               //
//                            |  | (_| (_) | | |  __/                               //
//                             \  \___\___/|_|  \___| 1.0                           //
//                              \______\                                            //
//                                                                                  //
//             Copyright (C) 2014 \ Job Talle (jobtalle@hotmail.com)                //
//                                 \ Thomas Versteeg (thomasversteeg@gmx.com)       //
//__________________________________________________________________________________//
//                                                                                  //
//      This program is free software: you can redistribute it and/or modify        //
//      it under the terms of the GNU General Public License as published by        //
//      the Free Software Foundation.                                               //
//                                                                                  //
//      This program is distributed without any warranty; see the GNU               //
//      General Public License for more details.                                    //
//                                                                                  //
//      You should have received a copy of the GNU General Public License           //
//      along with this program. If not, see <http://www.gnu.org/licenses/>.        //
//__________________________________________________________________________________//

#pragma once

#include "core.h"
#include "error.h"
#include "types.h"

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef X11
#include <errno.h>

#define CC_NET_ERROR_SOCKET_INVALID -1
#define CC_NET_ERROR_SOCKET_ERROR -1
#define CC_NET_ERROR_SOCKET_WOULDBLOCK EWOULDBLOCK

typedef int ccSocket;

#define _close close
#define _ioctl ioctl
#define _errno errno

#elif defined WINDOWS
#include <winsock.h>
#define CC_NET_ERROR_SOCKET_INVALID INVALID_SOCKET
#define CC_NET_ERROR_SOCKET_ERROR SOCKET_ERROR
#define CC_NET_ERROR_SOCKET_WOULDBLOCK WSAEWOULDBLOCK

#define _close closesocket
#define _ioctl ioctlsocket
#define _errno WSAGetLastError()

typedef SOCKET ccSocket;
#endif

ccReturn ccNetInitialize();
ccReturn ccNetFree();
#define ccNetClose(x) _close(x)
#define ccNetIoctl(x, y, z) _ioctl(x, y, z)

#define ccNetLastError() _errno

#ifdef __cplusplus
}
#endif
