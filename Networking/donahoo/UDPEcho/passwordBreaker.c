
#include "passwordBreaker.h"

int main(int argc, char*argv[]){
	int sock;
	struct sockaddr_in clientAddr;
	struct sockaddr_in serverAddr;
	uint16_t servPort;
	char* servIP;
	int passwordSize;
	char guess[passwordSize+1] = "";

	servPort = atoi(argv[2]);
	servIP = argv[1];
	passwordSize = atoi(argv[3]);

	for(int i = 0; i < passwordSize; ++i){
		strcat()
	}


}