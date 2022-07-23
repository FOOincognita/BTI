#include "WebFB.h"

int main(int argc, char *argv[]) {
	int result(0);
    WebFB* WFB(new WebFB); //!! ADD IP/PORT GRABBER FROM MOBILE

	printf("\n%s[STATUS]: ENTERING MAIN LOOP%s\n", M, RST);

	//	Start the main loop
	while (1) {
		switch (WFB->sockPoll()) {
			default: printf("\n%s[ERROR]: SocketPoll (errno=%d)%s\n", R, errno, RST); break;
			case 0: break; //? Timeout
			case 2: break; //?  No Data
			case 1: //? Data Ready
				result = WFB->rdSockData(WFB->data.buf, MAXPKT);
				if (result < 0) { //? Error
					printf("WebFB::rdSockData returned %i", result);
				} else if (result > 0) {
					WFB->ParsePKTS(WFB->data.buf, result); 
				}
				break;
		}
	}
	printf("\n\n");
	restore_console(WFB->data.oldattr);
    delete WFB;
	return 0;
}