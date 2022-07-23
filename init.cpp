#include "WebFB.h"

int main(int argc, char *argv[]) {
	int result(0);
    WebFB* WFB(new WebFB(SOCK_IP, SOCK_PORT)); //!! ADD IP/PORT GRABBER FROM MOBILE

	printf("\n%s[STATUS]: ENTERING MAIN LOOP%s\n", M, RST);

	for(;;) {
		if (WFB->sockPoll() == 1) { //? Data Ready
			result = WFB->rdSockData(WFB->data.buf, MAXPKT);
            if (result > 0) {
				WFB->ParsePKTS(WFB->data.buf, result); 
			}
		}
	}
	printf("\n\n");
	restore_console(WFB->data.oldattr);
    delete WFB;
	return 0;
}