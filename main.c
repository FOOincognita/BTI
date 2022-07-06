/**
*
*  Data display example  (06/15/2022)
*  Copyright (c) 2022
*  Astronics AES, Inc.
#  Ballard Technology
*  www.ballardtech.com
*  Ballard.Support@astronics.com
*  ALL RIGHTS RESERVED
*
*  NAME:   examp3.c
*
**/

/**
*
*  This program opens a socket to the AID service on the
*  webFB or webCS. It then prints records to the console as they
*  are parsed.
*
*
**/

#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include "btiutil.h"
#include "btisocket.h"

#define C "\033[1;36m"	// Cyan
#define R "\033[1;31m" 	// Red
#define G "\033[1;32m" 	// Green
#define M "\033[1;35m"	// Magenta 
#define RST "\033[0m" 	// Reset

void init_console(struct termios *oldattr);
void restore_console(struct termios oldattr);
int getch(void);

int ParsePackets(LPUINT16 buf, uint32_t wordcount);

int main(int argc, char *argv[])
{
	int ch;
	UINT16 buf[MAXPACKETSIZE];
	int result;

	struct termios oldattr;

	//
	//	Create a socket
	//
	printf("\n%s[STATUS]: CREATING SOCKET%s\n", M, RST);

	if (!SocketCreate())
	{
		printf("%s[ERROR]: Unable to create socket... Exiting (errno=%d)%s", R, errno, RST);
		exit(1);
	} else {
		printf("\n%s[SUCCESS]: SOCKET CREATED%s\n", G, RST);
	}

	//
	//	Connect to BTIAIDSvc socket
	//

	printf("\n%s[STATUS]: ATTEMPTING TO CONNECT%s\n", M, RST);

	if (!SocketConnect())
	{
		printf("%s[ERROR]: Unable to connect to socket... Exiting%s\n", R, RST);
		exit(1);
	} else {
		printf("\n%s[SUCCESS]: CONNECTION ESTABLISHED%s\n", G, RST);
	}

	//
	//	Setup the socket signal polling
	//
	printf("\n%s[STATUS]: INITIALIZING SIGNAL POLLING%s\n", M, RST);
	if (!SocketSetupPoll())
	{
		printf("%s[ERROR]: Unable to setup signal poll... Exiting (errno=%d)%s", R, errno, RST);
		exit(1);
	} else {
		printf("\n%s[SUCCESS]: SIGNAL POLL ACTIVE%s\n", G, RST);
	}

	//
	//	Clear the console screen and set position 0,0
	//

	init_console(&oldattr);

	printf("\n%s[STATUS]: ENTERING MAIN LOOP%s\n", M, RST);
	printf("\n\n%sPRESS ANY KEY TO STOP%s\n\n", C, RST);

	//
	//	Start the main loop
	//
	while (1)
	{
		ch = getch();
		if (ch != -1) break;

		//
		//	ANSI escape sequence to move cursor to top
		//

		switch (SocketPoll())
		{
			default:
				printf("\n%s[ERROR]: SocketPoll (errno=%d)%s\n", R, errno, RST);
				break;
			case 0: //Timeout
				break;
			case 1: //Data Ready
				result = SocketDataRd(buf, MAXPACKETSIZE);
				if (result < 0)
				{
					printf("SocketDataRd returned %i",result);
				}
				else if (result > 0)
				{
					//do something with the data!!!!!!!!
					ParsePackets(buf, result);
				}
				break;
			case 2: //No Data
				break;
		}
	}

	printf("\n\n");

	restore_console(oldattr);

	return 0;
}

//
//	Parse data packets read from the socket
//

int ParsePackets(LPUINT16 buf, uint32_t wordcount)
{
	ERRVAL           errval;
	SEQFINDINFO      sfinfo;
	UINT16           seqtype;
	LPUINT16         pRec;

	LPSEQRECORD717SF pRec717;
	LPSEQRECORD429   pRec429;

	errval = BTICard_SeqFindInit(buf, wordcount, &sfinfo);

	if (errval)
	{
		syslog(LOG_ERR,"ParsePackets - SeqFindInit Failed (%i)", errval);
		return -1;
	}

	//
	//	Walk the record stream using our modified find-next method
	//

	while(!BTIUTIL_SeqFindNext(&pRec,&seqtype,&sfinfo))
	{
		switch(seqtype)
		{
			default:			
				break;
			case SEQTYPE_429:	
				pRec429 = (LPSEQRECORD429)pRec;

                //Convert the timestamp from binary to BCD
				BTICard_IRIGTimeBinToBCD(&pRec429->timestamph,
                                         &pRec429->timestamp,
                                         ntohl(pRec429->timestamph),
                                         ntohl(pRec429->timestamp));


        		//Output record info
				printf("\nCh:%-2d  Data:0x%04x (Hex) ",
					(ntohs(pRec429->activity) & MSGACT429_CHMASK) >> MSGACT429_CHSHIFT,					
					ntohl(pRec429->data));

                //Output record timestamp
				printf("  TimeStamp:%02u:%02u %02u.%03u.%03u",
				BTICard_IRIGFieldGetHours(pRec429->timestamph,pRec429->timestamp),
				BTICard_IRIGFieldGetMin(pRec429->timestamph,pRec429->timestamp),
				BTICard_IRIGFieldGetSec(pRec429->timestamph,pRec429->timestamp),
				BTICard_IRIGFieldGetMillisec(pRec429->timestamph,pRec429->timestamp),
				BTICard_IRIGFieldGetMicrosec(pRec429->timestamph,pRec429->timestamp));


				break;
			case SEQTYPE_717SF:
				pRec717 = (LPSEQRECORD717SF)pRec;

				//Convert the timestamp from binary to BCD
				BTICard_IRIGTimeBinToBCD(&pRec717->timestamph,
                                         &pRec717->timestamp,
                                         ntohl(pRec717->timestamph),
                                         ntohl(pRec717->timestamp));

				//Output record info
				printf("\nCh:%-2d  SF:%-2d  SyncWord:%04o (Octal)  DataCount:%d ",
					(ntohs(pRec717->activity) & MSGACT717_CHMASK) >> MSGACT717_CHSHIFT,					
					ntohs(pRec717->subframe),					
					ntohs(pRec717->data[0]),
					ntohs(pRec717->datacount));

				//Output record timestamp
				printf("  TimeStamp:%02u:%02u %02u.%03u.%03u",
				BTICard_IRIGFieldGetHours(pRec717->timestamph,pRec717->timestamp),
				BTICard_IRIGFieldGetMin(pRec717->timestamph,pRec717->timestamp),
				BTICard_IRIGFieldGetSec(pRec717->timestamph,pRec717->timestamp),
				BTICard_IRIGFieldGetMillisec(pRec717->timestamph,pRec717->timestamp),
				BTICard_IRIGFieldGetMicrosec(pRec717->timestamph,pRec717->timestamp));

				break;
		}
	}

	return 0;
}


//
//	Save console settings in oldattr and setup console with
//	new settings
//

void init_console(struct termios *oldattr)
{
	struct termios newattr;

	tcgetattr(0,oldattr);
	newattr = *oldattr;
	newattr.c_lflag &= ~ICANON;
	newattr.c_lflag &= ~ISIG;
	newattr.c_lflag &= ~(ECHO | ECHOE | ECHOK | ECHONL);
	newattr.c_cc[VMIN] = 0;
	newattr.c_cc[VTIME] = 0;
	tcsetattr(STDIN_FILENO, TCSANOW, &newattr);

	printf("\033[2J");    //clear screen
	printf("\033[0;0H");  //set position to 0,0
}

//
//	Restore console setting contained in oldattr
//

void restore_console(struct termios oldattr)
{
	tcsetattr( STDIN_FILENO, TCSANOW, &oldattr);
}

//
//	Reads from key-press, doesn't echo
//

int getch(void)
{
	int ch;
	ch = getchar();
	return ch;
}
