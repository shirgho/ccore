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

#include <string.h>

#include <ccore/net.h>
#include <ccore/error.h>
#include <ccore/print.h>

void printIPs(char *site)
{
	ccAddrinfo hints, *servinfo, *cur;
	char ip[INET6_ADDRSTRLEN];
	void *addr;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if(ccNetGetaddrinfo(site, NULL, &hints, &servinfo)){
		ccPrintf("ccNetGetaddrinfo error: %s\n", ccErrorString(ccErrorPop()));
	}

	ccPrintf("Retrieving IP's for \"%s\":\n", site);
	for(cur = servinfo; cur != NULL; cur = cur->ai_next){
		if(cur->ai_family == AF_INET){
			ccPrintf("\tIP4: ");
			addr = &((ccSockaddr_in*)cur->ai_addr)->sin_addr;
		}else{
			ccPrintf("\tIP6: ");
			addr = &((ccSockaddr_in6*)cur->ai_addr)->sin6_addr;
		}

		ccNetInet_ntop(cur->ai_family, addr, ip, sizeof ip);
		ccPrintf("%s\n", ip);
	}

	ccNetFreeaddrinfo(servinfo);
}

int main(int argc, char **argv)
{
	ccSocket socket;
	char site[128] = "www.example.net";

	if(argc == 2){
		strcpy(site, argv[1]);
	}

	ccNetInitialize();

	printIPs(site);

	socket = ccNetSocket(AF_INET, SOCK_STREAM, 0);

	ccNetFree();

	return 0;
}
