#ifndef WEBFB_H
#define WEBFB_H

#include <unistd.h>
#include <string>
#include <string.h>
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
#include <algorithm>
#include <fcntl.h>
#include <sys/stat.h>
#include <cstring>
#include <stdio.h>
#include <termios.h>
#include <stdlib.h>
#include <time.h>

#define SOCKET_PORT		10001
#define SOCKET_IPADDR	"172.31.1.1"

#define MAXPACKETSIZE   16384
#define TIMEOUT		    5

class WebFB {
    int			sockFD;
    uint32_t	sockPKT;
    uint32_t	sockError;
    sigset_t	sockSigMask;
    std::string IPAddr;
    std::string Port;

    
public:
    WebFB(std::string IPAddr, std::string Port);
    ~WebFB();

    int mkSock();
    int sockConnect();
    int SocketDataRd(uint16_t *pbuf, uint32_t bufsize);
    int SocketSetupPoll();
    int SocketPoll();
};


#endif