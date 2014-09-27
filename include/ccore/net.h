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
//      it under the terms of the GNU Lesser General Public License as published by //
//      the Free Software Foundation.                                               //
//                                                                                  //
//      This program is distributed without any warranty; see the GNU               //
//      General Public License for more details.                                    //
//                                                                                  //
//      You should have received a copy of the GNU Lesser General Public License    //
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


#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#include <unistd.h>

typedef int						ccSocket;
typedef socklen_t 				ccSocklen_t;

#define ccNetClose 				close
#define ccNetInet_lnaof			inet_lnaof
#define ccNetInet_makeaddr		inet_makeaddr
#define ccNetInet_netof			inet_netof
#define ccNetInet_network		inet_network
#define ccNetInet_ntoa			inet_ntoa
#define ccNetInet_pton			inet_pton
#define ccNetInet_ntop			inet_ntop

#elif defined WINDOWS

#pragma comment(lib, "Ws2_32.lib")

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>

typedef SOCKET					ccSocket;
typedef int						ccSocklen_t;

#define ccNetClose 				closesocket
#define ccNetInet_lnaof			InetLnaof
#define ccNetInet_makeaddr		InetMakeaddr
#define ccNetInet_netof			InetNetof
#define ccNetInet_network		InetNetwork
#define ccNetInet_ntoa			InetNtoa
#define ccNetInet_pton			InetPton
#define ccNetInet_ntop			InetNtop

#define s_addr					S_un.S_addr

#endif

#define ccNetFcntl				fcntl
#define ccNetRead				read
#define ccNetWrite				write
#define ccNetFreeaddrinfo		freeaddrinfo
#define ccNetEndhostent 		endhostent
#define ccNetEndnetent 			endnetent
#define ccNetEndprotoent 		endprotoent
#define ccNetEndservent 		endservent
#define ccNetGethostbyaddr 		gethostbyaddr
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

#define ccNetSocket				socket
#define ccNetBind				bind
#define ccNetGetsockname		getsockname
#define ccNetConnect			connect
#define ccNetGetpeername		getpeername
#define ccNetSend				send
#define ccNetRecv				recv
#define ccNetSendto				sendto
#define ccNetRecvfrom			recvfrom
#define ccNetGetsockopt			getsockopt
#define ccNetSetsockopt			setsockopt
#define ccNetListen				listen
#define ccNetAccept				accept
#define ccNetShutdown			shutdown
#define ccNetSelect				select
#define ccNetGethostname		gethostname
#define ccNetGetaddrinfo		getaddrinfo
#define ccNetGetnameinfo		getnameinfo

#define CC_NET_FD_CLR 			FD_CLR
#define CC_NET_FD_ISSET			FD_ISSET
#define CC_NET_FD_SET 			FD_SET
#define CC_NET_FD_ZERO			FD_ZERO

typedef struct sockaddr 		ccSockaddr;
typedef struct sockaddr6 		ccSockaddr6;
typedef struct sockaddr_in 		ccSockaddr_in;
typedef struct sockaddr_in6		ccSockaddr_in6;
typedef struct addrinfo			ccAddrinfo;
typedef struct msghdr 			ccMsghdr;
typedef fd_set 					ccFd_set;
typedef struct hostent 			ccHostent;
typedef struct netent 			ccNetent;
typedef struct protoent 		ccProtoent;
typedef struct servent 			ccServent;

ccReturn ccNetInitialize();
ccReturn ccNetFree();

#ifdef __cplusplus
}
#endif
