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

#ifdef LINUX
#include <arpa/inet.h>

typedef int ccSocket;
#elif defined WINDOWS
typedef SOCKET ccSocket;
#endif

typedef struct sockaddr ccSockaddr;
typedef struct sockaddr_in ccSockaddr_in;
typedef socklen_t ccSocklen_t;
typedef struct msghdr ccMsghdr;

ccReturn ccNetInitialize();
ccReturn ccNetFree();

ccReturn ccNetSocket(ccSocket *sock, int family, int type, int protocol);
ccReturn ccNetSocketpair(ccSocket sock[2], int family, int type, int protocol);
ccReturn ccNetBind(ccSocket sock, const ccSockaddr *addr, ccSocklen_t len);
ccReturn ccNetGetsockname(ccSocket sock, ccSockaddr *addr, ccSocklen_t *len);
ccReturn ccNetConnect(ccSocket sock, const ccSockaddr *addr, ccSocklen_t len);
ccReturn ccNetGetpeername(ccSocket sock, ccSockaddr *addr, ccSocklen_t *len);
ccReturn ccNetSend(ccSocket sock, ssize_t *bytesSend, const void *buf, size_t n, int flags);
ccReturn ccNetRecv(ccSocket sock, ssize_t *bytesReceived, void *buf, size_t n, int flags);
ccReturn ccNetSendto(ccSocket sock, ssize_t *bytesSend, const void *buf, size_t n, int flags, const ccSockaddr *addr, ccSocklen_t len);
ccReturn ccNetRecvfrom(ccSocket sock, ssize_t *bytesReceived, void *buf, size_t n, int flags, ccSockaddr *addr, ccSocklen_t *len);
ccReturn ccNetSendmsg(ccSocket sock, ssize_t *bytesSend, const ccMsghdr *message, int flags);
ccReturn ccNetRecvmsg(ccSocket sock, ssize_t *bytesReceived, ccMsghdr *message, int flags);
ccReturn ccNetGetsockopt(ccSocket sock, int level, int optname, void *optval, ccSocklen_t *optlen);
ccReturn ccNetSetsockopt(ccSocket sock, int level, int optname, const void *optval, ccSocklen_t optlen);
ccReturn ccNetListen(ccSocket sock, int n);
ccReturn ccNetAccept(ccSocket sock, ccSocket *sockReceived, ccSockaddr *addr, ccSocklen_t *addr_len);
ccReturn ccNetShutdown(ccSocket sock, int how);

ccReturn ccNetWrite(ccSocket sock, ssize_t *bytesWritten, const void *buf, size_t count);

#define ccHtonl(x) htonl(x)
#define ccHtons(x) htons(x)
#define ccNtonl(x) ntonl(x)
#define ccNtons(x) ntons(x)

#ifdef __cplusplus
}
#endif
