//main.cpp

#include "DNSheader.h"
#include <unistd.h>

int main(int argc, char** argv){

	int c;
	int retries,timeout;
	uint16_t port;
	// set default values
	timeout = 5;
	retries = 3;
	port = 53;
	// parse command line arguments
	if(argc > 3){
		while((c = getopt(argc,argv,"t:p:r:")) != -1){
			switch (c){
				case 't':
					timeout = atoi(optarg);
					break;
				case 'p':
					port = atoi(optarg);
					break;
				case 'r':
					retries = atoi(optarg);
					break;
			}
		}
	}
	char * server = argv[optind];
	server = server + 1;
	char * domainName = argv[optind + 1];
	
	char request[100];
	int requestLength;
	requestLength = makeRequest(domainName,request);

	int sock = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
	if(sock < 0 ){
		err_n_die("could not create socket");
	}

	struct sockaddr_in servAddr;
	memset(&servAddr,0,sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	int rtnVal = inet_pton(AF_INET,server,&servAddr.sin_addr.s_addr);
	if (rtnVal < 0){
		err_n_die("Could not convert IP address");
	}
	servAddr.sin_port = htons(port);
	// set send and recieve timeout and retries
	int numTries = 0;
	int isTimeout = 1;
	struct timeval tv;
	tv.tv_sec = timeout;
	tv.tv_usec = 0;
	if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO,&tv,sizeof(tv)) < 0){
		err_n_die("Could not set timeout");
	}
	if (setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO,&tv,sizeof(tv)) < 0){
		err_n_die("Could not set timeout");
	}
	// initialize recieiving variables
	char response[1024];
	int responseLength;
	struct sockaddr_in fromAddr;
	memset(&fromAddr,0,sizeof(fromAddr));
	unsigned int fromSize = sizeof(fromAddr);
	// send and recieve response coun
	while(numTries < retries && isTimeout == 1){
		if (sendto(sock, request, requestLength, 0, 
    	         (struct sockaddr *) &servAddr, sizeof(servAddr)) != requestLength){
			err_n_die("Did not send appropriate number of bytes");
		}

		if ((responseLength = recvfrom(sock,response,1024,0,
				(struct sockaddr *) &fromAddr, &fromSize)) < 0){
			isTimeout = 1;
			numTries++;
		}else{
			isTimeout = 0;
		}
	}
	if(numTries == retries){
		printf("%d timeouts reached, exiting.. \n",numTries);
	}
	// get answer count
	struct DNSHeader head;
	memcpy(&head,response,12);
	int i ;
	char name[1000];
	char answer[1000];
	int pos = requestLength;
	uint16_t rcode = ntohs(head.flags);
	rcode = rcode & 0x000f;
	// check error codes 
	if(rcode != 0 && rcode != 3){
		printf("ERROR\t");
		switch(rcode){
			case 1:
				printf("Format Error");
				break;
			case 2:
				printf("Server Failure");
				break;
			case 3:
				printf("Name Error");
			case 4:
				printf("Query Type not implemented");
				break;
			case 5: 
				printf("Query Refused");
				break;
			case 6:
				printf("Name exists when it should not");
				break;
			case 7:
				printf("A RR exists that should not");
				break;
			case 8:
				printf("A RR that should Exist does not");
				break;
			case 9:
				printf("Not Authorized");
				break;
			case 10:
				printf("Not in Zone");
				break;
			default:
				printf("Unknown Error");
				break;
		}
		printf("\n");
	}

	// check to see if authorized source
	uint16_t AA = head.flags;
	AA = AA >> 10;
	AA =AA & 0x0001;

	int answers = ntohs(head.AnswerRecordCount);
	for(i = 0; i < answers; i++){	
		readMessage(response,answer,name,&pos,AA);
	}

	if(answers == 0 && numTries != retries ){
		printf("NOTFOUND\n");
	}
	return 0;
}

