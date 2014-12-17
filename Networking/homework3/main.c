// main
#include "handler.h"

int main(int argc, char**argv){
	int c;
	uint16_t port = 8080;
	char directory[1000];

	while((c = getopt(argc,argv,"p:")) != -1){
		switch(c) {
			case 'p':
				port = atoi(optarg);
				break;
			default :
				perror("usage: -p <port> <directory>");
				break;
		}	
	}

	if(optind < argc){
		strcpy(directory,argv[optind]);
	}else{
		strcpy(directory,"./");
	}

	int servSock;                    /* Socket descriptor for server */
    int clntSock;                    /* Socket descriptor for client */
    struct sockaddr_in httpServAddr; /* Local address */
    struct sockaddr_in httpClntAddr; /* Client address */
    unsigned short httpServPort;     /* Server port */
    unsigned int clntLen;            /* Length of client address data structure */

	httpServPort = port;

	if ((servSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        err_n_die("socket() failed");
      
    /* Construct local address structure */
    memset(&httpServAddr, 0, sizeof(httpServAddr));   /* Zero out structure */
    httpServAddr.sin_family = AF_INET;                /* Internet address family */
    httpServAddr.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
    httpServAddr.sin_port = htons(httpServPort);      /* Local port */

    /* Bind to the local address */
    if (bind(servSock, (struct sockaddr *) &httpServAddr, sizeof(httpServAddr)) < 0)
        err_n_die("bind() failed");

    /* Mark the socket so it will listen for incoming connections */
    if (listen(servSock, 100) < 0)
        err_n_die("listen() failed");

    for (;;){
    	clntLen = sizeof(httpClntAddr);
    	if ((clntSock = accept(servSock, (struct sockaddr *) &httpClntAddr, 
      	    	&clntLen)) < 0){
            err_n_die("accept() failed");
    	}

    	handleClient(clntSock,directory);
    }
	printf("directory = %s \n port = %d \n",directory,port);

	return 0;
}