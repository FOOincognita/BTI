#include "WebFB.h"

std::string GetArincData(std::string Lbl) {
	
}

latitude_t GetLatData() {

}

int main() { //! APP MAIN WILL NEED TO USE THIS STRUCTURE
	WebFB WFB(DEFAULT_IP, DEFAULT_PORT); //! <--- NEEDS TO BE INSTANTIATED IN APP ONLY!!

	//! APP MAIN WILL NEED TO USE THIS STRUCTURE BELOW
	bool ExampleCondition;
	int result(0);
	for(;;) {
		if (WFB.sockPoll() == 1) { 
			result = WFB.rdSockData(WFB.data.buf, MAXPKT);
            if (result > 0) {
				WFB.ParsePKTS(WFB.data.buf, result); 
			}
		}

		if (ExampleCondition) break; //! Will stop read; possibly when back button is pressed?
	}

	return 0;
}