
#include "passwordBreaker.h"
#include <signal.h>
// global variables to allow signal handler to print times attempted and time ran
int timesAttempted;
double t; // time 

int main(int argc, char*argv[]){
	int sock;
	struct sockaddr_in serverAddr;
	uint16_t servPort;
	char* servIP;
	int passwordSize;
	
	uint8_t returnCode; // 0 = success 1 = failure 2 = timeout
	double t;
	char guess[10]; // max password size is 10
	t = getTime();
	printf("time initial = %f \n",t);
	signal(SIGINT,handleInterupt);

	servPort = atoi(argv[2]);
	servIP = argv[1];
	passwordSize = atoi(argv[3]);
	
	printf("passwordsize = %d\n",passwordSize);
	// get initial password
	int i;
	for(i = 0; i < passwordSize; ++i){
		strcat(guess,"0");
	}
	memset(&serverAddr,0,sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(servPort);
	inet_pton(AF_INET,servIP,&serverAddr.sin_addr.s_addr);

	
   if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
        err_n_die("could not create socket \n");

	returnCode = tryPassword(guess, passwordSize, serverAddr,sock);
	timesAttempted = 1;
	while(returnCode != 0){
		getNextPassword(guess,passwordSize,passwordSize);
		returnCode = tryPassword(guess, passwordSize, serverAddr,sock);	
		//printf("Code %s is a %d \n",guess,returnCode);
		timesAttempted++;	
	}
	t = getTime() - t;
	printf("Password Cracked.\n Password = %s \n took %d tries and %f seconds\n"
		,guess,timesAttempted,t);
}

uint8_t tryPassword(char* guess, int n, struct sockaddr_in addr,int sock){
	int bytesSent, bytesRecieved;
	uint8_t returnCode;
	socklen_t sizeOfAddr = sizeof(addr);
	// set timeouts
	struct timeval tv;
	tv.tv_sec = 1;
	tv.tv_usec = 0;
	if(setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO,&tv,sizeof(tv)) <0){
		err_n_die("Could not set timeout");
	}
	if((bytesSent = sendto(sock,guess,n+1,0,(struct sockaddr*) &addr, sizeOfAddr)<0)){
		err_n_die("Could not send guess");
	}
	if((bytesRecieved = recvfrom(sock,&returnCode,1,0,
		(struct sockaddr*) &addr, &sizeOfAddr)< 0)){
		returnCode = 2; // set return code to timeout
		printf("Timeout\n");
	}

	return returnCode;
}

void getNextPassword(char* password, int depth, int n){
	if(depth == 0){
		return;
	}
	// if end of range i.e 0-9 jump to next range
	if(password[depth-1] == '9'){
		password[depth-1] = 'A';
		return;
	}else if(password[depth-1] == 'Z'){
		password[depth-1] = 'a';
		return;
	}else if(password[depth-1] == 'z'){
		// get when one letter has iterated through ah nevermind idk how to explain what
		// i did it works though
		getNextPassword(password,depth-1,n);
		int i;
		for(i = depth -1; i<n; ++i){
			password[i] = '0';
		}
		return;
	}else {
		password[depth-1] += 1;
		return;
	}
}

double getTime(){
	struct timeval curTime;
	gettimeofday(&curTime,NULL);
	double ret;
	ret = (curTime.tv_sec * 1000000 + curTime.tv_usec) / 1000000.0;
	return ret;
}



void handleInterupt(int signum){
	printf("%f \n ",getTime());
	t = getTime() - t;
	printf("Program terminated \n Guessed %d times and ran for %f seconds \n"
		,timesAttempted,t);
	exit(1);
}
	


