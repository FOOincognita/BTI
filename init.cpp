#include "WebFB.h"

int Connect(std::string _IPStr=SOCK_IP, uint16_t _portNo=SOCK_PORT) {
	int result(0);
    WebFB* WFB(new WebFB(_IPStr, _portNo)); //!! ADD IP/PORT GRABBER FROM MOBILE

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

int main() {

}