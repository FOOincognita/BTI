#include "WebFB.h"

//* Constructor
WebFB::WebFB() 
    : IPAddr(SOCK_IP) , Port(SOCK_PORT), sockFD(-1), sockPKT(0), sockError(0) {
}

//* Constructor
WebFB::WebFB(std::string _IP, uint16_t _Port) 
    : IPAddr(_IP) , Port(_Port), sockFD(-1), sockPKT(0), sockError(0) {
}

//* Destructor
//? Closes Socket
WebFB::~WebFB() { close(this->sockFD); }

//* Creates Socket
//? Returns 0 when failure
int WebFB::mkSock() {
	this->sockFD = socket(AF_INET,SOCK_STREAM,0);
	return (int)!(this->sockFD == -1);
}

//* Connects to WebFB
//Returns 0 when failure
int WebFB::sockConnect() {
	struct sockaddr_in(addr);
	int result(0);

	if (this->sockFD == -1) {
		//! Add error handling
		return 0;
	}

	std::memset(&addr, 0, sizeof(addr));
    //bzero(&addr, sizeof(addr));

	addr.sin_family      = AF_INET;
	addr.sin_port        = htons(this->Port);
	addr.sin_addr.s_addr = inet_addr((this->IPAddr.c_str()));

	result = connect(this->sockFD, (struct sockaddr*)&addr, sizeof(addr));
	if (result < 0) {
		//! Add error checking
		return 0;
	}

	return 1;
}

//* Reads Data From Socket
//?  -1 = Error
//?   0 = Pulse Packet
//?  >0 = Data Packet
int WebFB::rdSockData(uint16_t *pbuf, uint32_t bufsize) {
	int result(0);
	uint32_t wordcount(0);
	int j(0);

	if (!pbuf) { return -1; } //? Error

	result = recv(this->sockFD, (char*)&wordcount, sizeof(wordcount), MSG_WAITALL);

	//Socket error - disconnected?
	if (result <= 0) {
		syslog(LOG_ERR,"SocketDataRd - Socket read failed (Socket error: %i)", errno);
		return -1; //? Error
	}

	//Unexpected packet
	if (result != sizeof(wordcount)) {
		syslog(LOG_ERR,"SocketDataRd - Unexpected Packet Size (bytes) exp %i, act %i", sizeof(wordcount), result);
		return -1; //? Error
	}

	//Switch count from network to host order
	wordcount = ntohl(wordcount);

	//? Pulse packet
	if (!wordcount) {
		return 0;
	}

	//? Error: Unexpected packet
	if (wordcount > bufsize) {
		syslog(LOG_ERR,"SocketDataRd - Unexpected Packet Size (bytes) %i, exp <= %i", wordcount * 2, bufsize);
		return -1; 
	}

	result = recv(this->sockFD, (char*)pbuf, wordcount * 2, MSG_WAITALL);

	//? Error: Socket disconnected?
	if (result <= 0) {
		syslog(LOG_ERR,"SocketDataRd - Socket read failed (Socket error: %i)", errno);
		return -1; 
	}

	//? Error: Unexpected packet
	if (result != (wordcount * 2)) {
		syslog(LOG_ERR,"SocketDataRd - Unexpected Packet Size (bytes) exp %i, act %i", wordcount * 2, result);
		return -1;
	} 

	this->sockFD++;

	return wordcount;
}

//* Setup sig mask for ppoll
//? Returns 0 when failure
int WebFB::initSockPoll() {
	return !(sigprocmask(SIG_BLOCK, NULL, &(this->sockSigMask)) < 0);
}

//* Poll for events (data ready on socket)
//?	-1 = Error
//?   0 = Poll Timeout
//?   1 = Data Waiting
//?   2 = No Data
int WebFB::sockPoll() {
	int result(0), j(0);
	struct pollfd fds[2];
	nfds_t nfds(0);
	struct timespec timeout;

	timeout.tv_sec = TIMEOUT;
    timeout.tv_nsec = 0;

	//*	Initialize the Poll (pollfd) structure
    std::memset(&fds, 0, sizeof(fds));
	//bzero(&fds, sizeof(fds));

	fds[0].fd = this->sockFD;
	fds[0].events = POLLIN;
	nfds++;

	//	Poll for signal
	result = ppoll(fds, nfds, &timeout, &(this->sockSigMask));
	if (result > 0) {
		for (j = 0; j < nfds; j++) {
			if (fds[j].revents & POLLIN) {
				if (fds[j].fd == this->sockFD) { //? Data was sent from server
					return 1; //? Data Waiting
				}
			}
		}
		return 2;	//? No Data
	} else if (!result) {
		return 0; //? Poll Timeout
	}
	else if (errno == EINTR) {
		return 2; //? No Data
	}
	return -1; //? Error
}

//	Parse data packets read from the socket
int WebFB::ParsePKTS(LPUINT16 buf, uint32_t wordcount) {
	ERRVAL           errval;
	SEQFINDINFO      sfinfo;
	UINT16           seqtype;
	LPUINT16         pRec;

	LPSEQRECORD717SF pRec717;
	LPSEQRECORD429   pRec429;

	std::stringstream ss;
	std::string hexStr(""), bit1428Str(""), laloStr(""), LAT("c8"), LON("c9"), w32("");
	bool valid(true);
	LaLo LatLon;
	Parity par29, par32;
	double dec(0);

	errval = BTICard_SeqFindInit(buf, wordcount, &sfinfo);

	if (errval) {
		syslog(LOG_ERR,"ParsePackets - SeqFindInit Failed (%i)", errval);
		return -1;
	}

	//	Walk the record stream using our modified find-next method
	while(!BTIUTIL_SeqFindNext(&pRec,&seqtype,&sfinfo)) {
		switch(seqtype) {
			default: break;
			case SEQTYPE_429:	
				pRec429 = (LPSEQRECORD429)pRec;

				w32 = "";
				hexStr = "";
				ss.clear();
	
				// Grab data (HEX)
				ss << std::hex << ntohl(pRec429->data);
				hexStr = ss.str();

				if (hexStr.size() == 8) {
					//! Check if lat or lon
					laloStr = hexStr.substr(hexStr.length()-2);
					if(laloStr == LAT) {
						LatLon = (LaLo)0;
					} else if (laloStr == LON) {
						LatLon = (LaLo)1;
					} else {
						valid = false;
					}
				
					if (valid) {
						printf("%sHEX: 0x%s %s\n", C, hexStr.c_str(), RST);

						//! Convert hex to 32-bit word
						for (auto& i : hexStr) { w32 += hexMap[i]; }
						printf("%s32-BIT WORD: %s %s\n", C, w32.c_str(), RST);

						//! grab bits 14-28
						bit1428Str = w32.substr(4, 20); 
						printf("%sBITS 14-28: %s %s\n", C, bit1428Str.c_str(), RST);

						//! Convert 14-28 to decimal & multiply
						dec = std::stol(bit1428Str.c_str(), nullptr, 2)*0.00017166154;
						printf("%sBITS 14-28 [DECIMAL]: %.6f %s \n", C, dec, RST);

						//! Check for Parity
						//? Change based on order
						par29 = (Parity)(w32.at(3) - '0'); //* ASCII magic
						par32 = (Parity)(w32.at(0) - '0');

					}
				}
		}
	}

	return 0;
}