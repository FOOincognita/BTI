#ifndef _GLOBAL_H
#define _GLOBAL_H

#include "BTICard.h"
#include "BTI429.h"

#include <iostream>
#include <string>
#include <string.h>
#include <strings.h>
#include <sstream>
#include <stdexcept>
#include <bitset>
#include <iomanip>
#include <cstdint>
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <unordered_map>
#include <termios.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
//! #define __USE_GNU
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

//*	Definitions
//? WebFB Default Values
#define SOCK_IP	    "172.31.1.1"
#define SOCK_PORT   10001
#define MAXPKT      16384
#define MAX_WPS     8192
#define TIMEOUT		5

//? Console Text Color
#define C           "\033[1;36m"	// Cyan
#define R           "\033[1;31m" 	// Red
#define G           "\033[1;32m" 	// Green
#define M           "\033[1;35m"	// Magenta 
#define RST         "\033[0m" 	    // Reset

//? Status Print Macros
#define STAT(x) std::cout << std::endl << M << "[STATUS]: " << x << RST << std::endl
#define FAIL(x) std::cout << std::endl << R << "[ERROR]: " << x << RST << std::endl; exit(1) 
#define PASS(x) std::cout << std::endl << G << "[SUCCESS]: " << x << RST << std::endl   

//* Constants, Typedefs, Structures, & Enumerations
enum class LaLo {LATITUDE, LONGITUDE};
enum class Parity {POS, NEG};

// Hex to Bin Lookup
const std::unordered_map<char,std::string> hexMap = {
    {'0',"0000"},
    {'1',"0001"},
    {'2',"0010"},
    {'3',"0011"},
    {'4',"0100"},
    {'5',"0101"},
    {'6',"0110"},
    {'7',"0111"},
    {'8',"1000"},
    {'9',"1001"},
    {'a',"1010"},
    {'b',"1011"},
    {'c',"1100"},
    {'d',"1101"},
    {'e',"1110"},
    {'f',"1111"},
    {'A',"1010"},
    {'B',"1011"},
    {'C',"1100"},
    {'D',"1101"},
    {'E',"1110"},
    {'F',"1111"},
};

typedef std::string ipaddr_t;

typedef struct manage_t {
    struct termios  oldattr;        // Console control (init & restore)
    uint16_t        buf[MAXPKT];    // Data Buffer
} manage_t;

typedef struct datafile_t {
	uint16_t streamactivity;		// see ACTSTREAM_ below
	uint16_t channel;
	uint16_t wps;
	uint16_t updatedSubfrm;
	uint32_t timestamp;				// last time file was updated
	uint32_t timestamph;			// last time file was updated
	uint16_t sfdata[4][MAX_WPS];	// allocate max.
} datafile_t;

typedef struct chan717Info_t {
		int        valid;
		int        channel;
		int        wps;
		uint32_t   lastupdate;		// Time of last update
		datafile_t datafile;		// Most recent SF data info
		uint32_t   recordcnt;		// Records for this channel
		uint32_t   errorcnt;		// Total errors for this channel
} chan717Info_t;

typedef struct chan429Info_t {
	int        valid;
	int        channel;
	int        btx;
	uint32_t   timestamp;			//
	uint32_t   timestamph;			//
	uint32_t   recordcnt;			// Records for this channel
	uint32_t   errorcnt;			// Total errors for this channel
	uint32_t   msg;
	uint32_t   lastupdate;			// Time of last update
} chan429Info_t;

typedef struct {
	uint16_t type;					//
	uint16_t count;					//
	uint32_t timestamp;				//
	uint32_t timestamph;			//
	uint16_t activity;				//
	uint16_t subframe;				// Valid in all versions
	uint16_t resv;					// (future: superframe) Valid in all versions
	uint16_t datacount;				//
	uint16_t data[8192];			// Variable length (don't exceed data[datacount-1])
} SEQRECORD717SF;

#define SEQTYPE_717SF 0x0009		// Sequential record type is ARINC 717 Subframe

#ifndef LPSEQRECORD717SF
typedef SEQRECORD717SF * LPSEQRECORD717SF;
#endif

enum {
	ACTSTREAM_SERVERFAULT=0x1,
	ACTSTREAM_SUBF1VALID=0x10,
	ACTSTREAM_SUBF2VALID=0x20,
	ACTSTREAM_SUBF3VALID=0x40,
	ACTSTREAM_SUBF4VALID=0x80,
};

ERRVAL BTIUTIL_SeqFindCheckValidType(UINT16 seqtype) {
	seqtype &= SEQTYPE_MASK;
	if (seqtype == SEQTYPE_429)  return(ERR_NONE);
	return(ERR_SEQTYPE);
}

ERRVAL BTIUTIL_SeqFindNext(LPUINT16 *pRecord,LPUINT16 seqtype,LPSEQFINDINFO sfinfo) {
	ERRVAL errval;
	LPUINT16 pSeqBuf;

	if (!sfinfo) return(ERR_SEQFINDINFO);

	for (pSeqBuf = sfinfo->pRecNext; pSeqBuf < sfinfo->pRecLast;) {
		// Check for a known record type
		errval = BTIUTIL_SeqFindCheckValidType(pSeqBuf[0]);
		if (errval) return(errval);

		//Advance the pointer in the sfinfo struct
		sfinfo->pRecNext += pSeqBuf[1];

		if (pRecord) *pRecord = pSeqBuf;
		if (seqtype) *seqtype = pSeqBuf[0] & SEQTYPE_MASK;

		return(ERR_NONE);
	}
	return(ERR_SEQNEXT);
}

//	Save console settings in oldattr and setup console with new settings
void init_console(struct termios *oldattr) {
	struct termios newattr;

	tcgetattr(0, oldattr);
	newattr = *oldattr;
	newattr.c_lflag &= ~ICANON;
	newattr.c_lflag &= ~ISIG;
	newattr.c_lflag &= ~(ECHO | ECHOE | ECHOK | ECHONL);
	newattr.c_cc[VMIN] = 0;
	newattr.c_cc[VTIME] = 0;
	tcsetattr(STDIN_FILENO, TCSANOW, &newattr);
	printf("\033[2J");    // clear screen
	printf("\033[0;0H");  // set position to 0,0
}

//* Restore console setting contained in oldattr
void restore_console(struct termios oldattr) {
	tcsetattr(STDIN_FILENO, TCSANOW, &oldattr);
}

//! Conversion functions
/// Converts hex to decimal
int htod(std::string h) {
    int x(0);   
    std::stringstream ss;
    ss << std::hex << h;
    ss >> x;
    return x;
}

// Converts binary to decimal
int btoc(std::string s) {
    std::bitset<32> bits(s);
    int number = bits.to_ulong();
    return number;
}

// Converts hex to binary
std::string htob(std::string hexStr) {
    std::string bin("");
    for (auto& i : hexStr) {bin += hexMap.at(i);}
    return bin;
}

#endif
