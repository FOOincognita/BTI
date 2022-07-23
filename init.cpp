#include "WebFB.h"
#include "global.h"

int main(int argc, char *argv[]) {
	int ch(0), result(0);
	uint16_t buf[MAXPKT];
	struct termios oldattr;
    WebFB *WFB(new WebFB); //!! ADD IP/PORT GRABBER FROM MOBILE

	//	Create a socket
	printf("\n%s[STATUS]: CREATING SOCKET%s\n", M, RST);

	if (!WFB->mkSock()) {
		printf("%s[ERROR]: Unable to create socket... Exiting (errno=%d)%s", R, errno, RST);
		exit(1);

	} else {
		printf("\n%s[SUCCESS]: SOCKET CREATED%s\n", G, RST);
        
	}

	//	Connect to BTIAIDSvc socket
	printf("\n%s[STATUS]: ATTEMPTING TO CONNECT%s\n", M, RST);

	if (!WFB->sockConnect()) {
		printf("%s[ERROR]: Unable to connect to socket... Exiting%s\n", R, RST);
		exit(1);

	} else {
		printf("\n%s[SUCCESS]: CONNECTION ESTABLISHED%s\n", G, RST);

	}

	//	Setup the socket signal polling
	printf("\n%s[STATUS]: INITIALIZING SIGNAL POLLING%s\n", M, RST);
	if (!WFB->initSockPoll()) {
		printf("%s[ERROR]: Unable to setup signal poll... Exiting (errno=%d)%s", R, errno, RST);
		exit(1);

	} else {
		printf("\n%s[SUCCESS]: SIGNAL POLL ACTIVE%s\n", G, RST);

	}

	init_console(&oldattr); //	Clear the console screen and set position 0,0
	printf("\n%s[STATUS]: ENTERING MAIN LOOP%s\n", M, RST);

	//	Start the main loop
	while (1) {
		switch (WFB->sockPoll()) {
			default: printf("\n%s[ERROR]: SocketPoll (errno=%d)%s\n", R, errno, RST); break;
			case 0: break; //Timeout
			case 2: break; // No Data
			case 1: //Data Ready
				result = WFB->rdSockData(buf, MAXPKT);
				if (result < 0) {
					printf("SocketDataRd returned %i",result);
				} else if (result > 0) {
					WFB->ParsePKTS(buf, result); 
				}
				break;
		}
	}
	printf("\n\n");
	restore_console(oldattr);
    delete WFB;
	return 0;
}