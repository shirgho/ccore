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

#include <stdint.h>

#include "core.h"
#include "error.h"
#include "types.h"

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef LINUX
#include <sys/time.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>

#define ccNetClose close

typedef int ccSocket;

#elif defined WINDOWS
#include <winsock.h>

#define ccNetClose closesocket

typedef SOCKET ccSocket;

#endif

#define ccNetFcntl				fcntl

#define ccNetFreeaddrinfo		freeaddrinfo
#define ccNetEndhostent 		endhostent
#define ccNetEndnetent 			endnetent
#define ccNetEndprotoent 		endprotoent
#define ccNetEndservent 		endservent
#define ccNetGethostbyaddr 		gethostbyaddr
#define ccNetGethostbyname 		gethostbyname
#define ccNetGethostent 		gethostent
#define ccNetGetnetbyaddr		getnetbyaddr
#define ccNetGetnetbyname		getnetbyname
#define ccNetGetprotobyname		getprotobyname
#define ccNetGetprotobynumber	getprotobynumber
#define ccNetGetprotoent		getprotoent
#define ccNetGetservbyname		getservbyname
#define ccNetGetservbyport		getservbyport
#define ccNetGetservent			getservent
#define ccNetSethostent			sethostent
#define ccNetSetnetent			setnetent
#define ccNetSetprotoent		setprotoent
#define ccNetSetservent			setservent

#define ccNetHtonl 				htonl
#define ccNetHtons 				htons
#define ccNetNtonl 				ntonl
#define ccNetNtons 				ntons

#define CC_NET_FD_CLR 	FD_CLR
#define CC_NET_FD_ISSET FD_ISSET
#define CC_NET_FD_SET 	FD_SET
#define CC_NET_FD_ZERO 	FD_ZERO

typedef struct sockaddr 	ccSockaddr;
typedef struct sockaddr_in 	ccSockaddr_in;
typedef uint32_t 			ccSocklen_t;
typedef struct addrinfo		ccAddrinfo;
typedef struct msghdr 		ccMsghdr;
typedef fd_set 				ccFd_set;
typedef struct hostent 		ccHostent;
typedef struct netent 		ccNetent;
typedef struct protoent 	ccProtoent;
typedef struct servent 		ccServent;

ccReturn ccNetInitialize();
ccReturn ccNetFree();

ccReturn ccNetSocket(ccSocket *sock, int family, int type, int protocol);
ccReturn ccNetBind(ccSocket sock, const ccSockaddr *addr, ccSocklen_t len);
ccReturn ccNetGetsockname(ccSocket sock, ccSockaddr *addr, ccSocklen_t *len);
ccReturn ccNetConnect(ccSocket sock, const ccSockaddr *addr, ccSocklen_t len);
ccReturn ccNetGetpeername(ccSocket sock, ccSockaddr *addr, ccSocklen_t *len);
ccReturn ccNetSend(ccSocket sock, intptr_t *bytesSend, const void *buf, size_t n, int flags);
ccReturn ccNetRecv(ccSocket sock, intptr_t *bytesReceived, void *buf, size_t n, int flags);
ccReturn ccNetSendto(ccSocket sock, intptr_t *bytesSend, const void *buf, size_t n, int flags, const ccSockaddr *addr, ccSocklen_t len);
ccReturn ccNetRecvfrom(ccSocket sock, intptr_t *bytesReceived, void *buf, size_t n, int flags, ccSockaddr *addr, ccSocklen_t *len);
ccReturn ccNetGetsockopt(ccSocket sock, int level, int optname, void *optval, ccSocklen_t *optlen);
ccReturn ccNetSetsockopt(ccSocket sock, int level, int optname, const void *optval, ccSocklen_t optlen);
ccReturn ccNetListen(ccSocket sock, int n);
ccReturn ccNetAccept(ccSocket sock, ccSocket *sockReceived, ccSockaddr *addr, ccSocklen_t *addr_len);
ccReturn ccNetShutdown(ccSocket sock, int how);

ccReturn ccNetSelect(ccSocket sock, ccFd_set *readfds, ccFd_set *writefds, ccFd_set *exceptfds, struct timeval *timeout);
ccReturn ccNetGethostname(char *name, size_t len);

#ifdef __cplusplus
}
#endif
