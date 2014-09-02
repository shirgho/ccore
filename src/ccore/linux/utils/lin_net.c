#include "lin_net.h"

ccReturn ccNetInitialize(void)
{
	return CC_SUCCESS;
}

ccReturn ccNetFree(void)
{
	return CC_SUCCESS;
}

ccReturn ccNetSocket(ccSocket *sock, int family, int type, int protocol)
{
	*sock = socket(family, type, protocol);
	CC_RETURN_ON_ERROR(*sock);
}

ccReturn ccNetSocketpair(ccSocket sock[2], int family, int type, int protocol)
{
	int err;

	err = socketpair(family, type, protocol, sock);
	CC_RETURN_ON_ERROR(err);
}

ccReturn ccNetBind(ccSocket sock, const ccSockaddr *addr, ccSocklen_t len)
{
	int err;
	
	err = bind(sock, addr, len);
	CC_RETURN_ON_ERROR(err);
}

ccReturn ccNetGetsockname(ccSocket sock, ccSockaddr *addr, ccSocklen_t *len)
{
	int err;
	
	err = getsockname(sock, addr, len);
	CC_RETURN_ON_ERROR(err);
}

ccReturn ccNetConnect(ccSocket sock, const ccSockaddr *addr, ccSocklen_t len)
{
	int err;
	
	err = connect(sock, addr, len);
	CC_RETURN_ON_ERROR(err);
}

ccReturn ccNetGetpeername(ccSocket sock, ccSockaddr *addr, ccSocklen_t *len)
{
	int err;
	
	err = getpeername(sock, addr, len);
	CC_RETURN_ON_ERROR(err);
}

ccReturn ccNetSend(ccSocket sock, ssize_t *bytesSend, const void *buf, size_t n, int flags)
{
	*bytesSend = send(sock, buf, n, flags);
	CC_RETURN_ON_ERROR(*bytesSend);
}

ccReturn ccNetRecv(ccSocket sock, ssize_t *bytesReceived, void *buf, size_t n, int flags)
{
	*bytesReceived = recv(sock, buf, n, flags);
	CC_RETURN_ON_ERROR(*bytesReceived);
}

ccReturn ccNetSendto(ccSocket sock, ssize_t *bytesSend, const void *buf, size_t n, int flags, const ccSockaddr *addr, ccSocklen_t len)
{
	*bytesSend = sendto(sock, buf, n, flags, addr, len);
	CC_RETURN_ON_ERROR(*bytesSend);
}

ccReturn ccNetRecvfrom(ccSocket sock, ssize_t *bytesReceived, void *buf, size_t n, int flags, ccSockaddr *addr, ccSocklen_t *len)
{
	*bytesReceived = recvfrom(sock, buf, n, flags, addr, len);
	CC_RETURN_ON_ERROR(*bytesReceived);
}

ccReturn ccNetGetsockopt(ccSocket sock, int level, int optname, void *optval, ccSocklen_t *optlen)
{
	int err;
	
	err = getsockopt(sock, level, optname, optval, optlen);
	CC_RETURN_ON_ERROR(err);
}

ccReturn ccNetSetsockopt(ccSocket sock, int level, int optname, const void *optval, ccSocklen_t optlen)
{
	int err;
	
	err = setsockopt(sock, level, optname, optval, optlen);
	CC_RETURN_ON_ERROR(err);
}

ccReturn ccNetListen(ccSocket sock, int n)
{
	int err;
	
	err = listen(sock, n);
	CC_RETURN_ON_ERROR(err);
}

ccReturn ccNetAccept(ccSocket sock, ccSocket *sockReceived, ccSockaddr *addr, ccSocklen_t *addr_len)
{
	*sockReceived = accept(sock, addr, addr_len);
	CC_RETURN_ON_ERROR(*sockReceived);
}

ccReturn ccNetShutdown(ccSocket sock, int how)
{
	int err;
	
	err = shutdown(sock, how);
	CC_RETURN_ON_ERROR(err);
}

ccReturn ccNetSelect(ccSocket sock, ccFd_set *readfds, ccFd_set *writefds, ccFd_set *exceptfds, struct timeval *timeout)
{
	int err;
	
	err = select(sock, readfds, writefds, exceptfds, timeout);
	CC_RETURN_ON_ERROR(err);
}

ccReturn ccNetGethostname(char *name, size_t len)
{
	int err;

	err = gethostname(name, len);
	CC_RETURN_ON_ERROR(err);
}