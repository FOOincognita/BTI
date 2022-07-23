#ifndef WEBFB_H
#define WEBFB_H

#include "global.h"
class WebFB {
    int			    sockFD;
    uint32_t	    sockPKT;
    uint32_t	    sockError;
    sigset_t        sockSigMask;
    uint16_t        sockPort;
    std::string     sockIP;
public:
    WebFB();
    WebFB(std::string IP, uint16_t Port);
    ~WebFB();

    int mkSock();
    int sockConnect();
    int rdSockData(uint16_t *pbuf, uint32_t bufsize);
    int initSockPoll();
    int sockPoll();
    int ParsePKTS(LPUINT16 buf, uint32_t wordcount);
};


#endif