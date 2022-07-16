#ifndef _BTISOCKET_H
#define _BTISOCKET_H

#include <stdint.h>
#include <strings.h>
#include <errno.h>
#define __USE_GNU
#include <sys/poll.h>
#include <sys/eventfd.h>
#include <sys/socket.h>
#include <syslog.h>
#include <arpa/inet.h>
#include <stddef.h>
#include <signal.h>

//	Data socket settings (from BTIAIDSvc)

#define SOCKET_PORT		10001
#define SOCKET_IPADDR	"172.31.1.1"

#define MAXPACKETSIZE   16384
#define TIMEOUT		    5

//	Global Variables

uint32_t	G_SOCKETERROR = 0;
uint32_t	G_SOCKETPKT = 0;
int			G_CLIENTSOCK = -1;
sigset_t	G_SOCKETSIGMASK;

//	Socket Functions

//Returns 0 when failure
int SocketCreate() {
	G_CLIENTSOCK = socket(AF_INET,SOCK_STREAM,0);
	return (int)!(G_CLIENTSOCK == -1);
}


void SocketClose() {
	close(G_CLIENTSOCK);
}

//Returns 0 when failure
int SocketConnect() {
	struct sockaddr_in addr;
	int result = 0;

	//Test socket
	if (-1 == G_CLIENTSOCK) {
		syslog(LOG_ERR,"SocketConnect - invalid socket handle");
		return 0;
	}

	bzero(&addr,sizeof(addr));

	addr.sin_family      = AF_INET;
	addr.sin_port        = htons(SOCKET_PORT);
	addr.sin_addr.s_addr = inet_addr(SOCKET_IPADDR);

	result = connect(G_CLIENTSOCK,(struct sockaddr *)&addr,sizeof(addr));
	if (result < 0) {
		syslog(LOG_ERR,"SocketConnect - socket connect failed (result=%d, errno=%d)",result, errno);
		return 0;
	}

	return 1;
}

//Returns:
//  -1 = Error
//   0 = Pulse Packet
//  >0 = Data Packet
int SocketDataRd(uint16_t *pbuf, uint32_t bufsize) {
	int result;
	uint32_t wordcount = 0;
	int j;

	if (!pbuf) {
		return -1; //Error
	}

	result = recv(G_CLIENTSOCK, (char*)&wordcount, sizeof(wordcount), MSG_WAITALL);

	//Socket error - disconnected?
	if (result <= 0) {
		syslog(LOG_ERR,"SocketDataRd - Socket read failed (Socket error: %i)", errno);
		return -1; //Error
	}

	//Unexpected packet
	if (result != sizeof(wordcount)) {
		syslog(LOG_ERR,"SocketDataRd - Unexpected Packet Size (bytes) exp %i, act %i", sizeof(wordcount), result);
		return -1; //Error
	}

	//Switch count from network to host order
	wordcount = ntohl(wordcount);

	//This was a pulse packet
	if (wordcount == 0) {
		return 0;
	}

	//Unexpected packet
	if (wordcount > bufsize) {
		syslog(LOG_ERR,"SocketDataRd - Unexpected Packet Size (bytes) %i, exp <= %i", wordcount * 2, bufsize);
		return -1; //Error
	}

	result = recv(G_CLIENTSOCK, (char*)pbuf, wordcount * 2, MSG_WAITALL);

	//Socket error - disconnected?
	if (result <= 0) {
		syslog(LOG_ERR,"SocketDataRd - Socket read failed (Socket error: %i)", errno);
		return -1; //Error
	}

	//Unexpected packet
	if (result != (wordcount * 2)) {
		syslog(LOG_ERR,"SocketDataRd - Unexpected Packet Size (bytes) exp %i, act %i", wordcount * 2, result);
		return -1; //Error
	}

	//Switch from network to host order
//	for (j = 0; j < wordcount; j++)
//	{
//		pbuf[j] = ntohs(pbuf[j]);
//	}

	G_SOCKETPKT++;

	return wordcount;
}

//  Setup sig mask for ppoll

//Returns 0 when failure
int SocketSetupPoll() {
	return !(sigprocmask(SIG_BLOCK, NULL, &G_SOCKETSIGMASK) < 0);
}

//	Poll for events (data ready on socket)

//Returns:
//	-1 = Error
//   0 = Poll Timeout
//   1 = Data Waiting
//   2 = No Data
int SocketPoll() {
	int result;
	int j;
	struct pollfd fds[2];
	nfds_t nfds;
	struct timespec timeout;

	timeout.tv_sec = TIMEOUT;
    timeout.tv_nsec = 0;

	//	Initialize the Poll (pollfd) structure

	bzero(&fds,sizeof(fds));
	nfds=0;

	fds[0].fd = G_CLIENTSOCK;
	fds[0].events = POLLIN;
	nfds++;

	//	Poll for signal

	result = ppoll(fds, nfds, &timeout, &G_SOCKETSIGMASK);
	if (result > 0) {
		for (j = 0; j < nfds; j++) {
			if (fds[j].revents & POLLIN) {
				if (fds[j].fd == G_CLIENTSOCK) { //Data was sent from server
					return 1; //Data Waiting
				}
			}
		}
		return 2;	//No Data
	} else if (result == 0) {
		return 0; //Poll Timeout
	}
	else if (errno == EINTR) {
		return 2; //No Data
	}
	return -1; //Error
}

#endif //_BTISOCKET_H
