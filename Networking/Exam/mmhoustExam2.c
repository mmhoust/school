// Matthew Houston
#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), bind(), and connect() */
#include <netdb.h>  
#include <arpa/inet.h>  /* for sockaddr_in and inet_ntoa() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h> 

unsigned long fsize(char*);
void handleInterupt(int);
int timesWritten;
int main(int argc, char**argv){

	int client_Server;
	char *fileName;
	char *serverName;
	client_Server = atoi(argv[1]);

	if(client_Server == 1){
		int servSock;                    /* Socket descriptor for server */
  	 	int clntSock;                    /* Socket descriptor for client */
    	struct sockaddr_in servAddr; /* Local address */
   		struct sockaddr_in echoClntAddr; /* Client address */
    	unsigned short servPort;     /* Server port */
    	unsigned int clntLen;            /* Length of client address data structure */

    	servPort = atoi(argv[2]);
    	fileName = argv[3];
    	timesWritten = 0;
    	signal(SIGINT,handleInterupt);
    	/* Create socket for incoming connections */
    	if ((servSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0){
    		perror("Could not create socket");
    		exit(1);
    	}
    	/* Construct local address structure */
    	memset(&servAddr, 0, sizeof(servAddr));   /* Zero out structure */
    	servAddr.sin_family = AF_INET;                /* Internet address family */
    	servAddr.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
    	servAddr.sin_port = htons(servPort);  

    	    /* Bind to the local address */
  		if (bind(servSock, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0){
        	perror("Could not bind socket");
        	exit(1);
        }
    	/* Mark the socket so it will listen for incoming connections */
    	if (listen(servSock, 100) < 0){
        	perror("Could not listen for incoming connections");
        	exit(1);
        }		
	    for (;;) /* Run forever */
    	{
        	/* Set the size of the in-out parameter */
        	clntLen = sizeof(echoClntAddr);

        	/* Wait for a client to connect */
        	if ((clntSock = accept(servSock, (struct sockaddr *) &echoClntAddr, 
        	                       &clntLen)) < 0){
        		perror("Could not accept incoming connection");
        		exit(1);
        	}

        	
        	char recvChar[100];
        	FILE *fp;
        	fp = fopen(fileName,"w");
        	int returnLength;
        	
        	while((returnLength = read(clntSock,recvChar,100)) > 0){
        		fwrite(recvChar,sizeof(char), 100,fp);
        	}
        	fclose(fp);
        	timesWritten++;
        	if(returnLength < 0 ){
        		perror("error transferring the file");
        		exit(1);
        	}

    	}    
    }else{
    	int sock;                        /* Socket descriptor */
   	 	struct sockaddr_in servAddr; /* Echo server address */
  		struct hostent *thehost;         /* Hostent from gethostbyname() */
    	unsigned short servPort;

    	serverName = argv[2];
    	servPort = atoi(argv[3]);
    	fileName = argv[4];
    	    /* Create a reliable, stream socket using TCP */
    	if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0){
    	    perror("could not create socket");
    	    exit(1);
    	}
    		/* Construct the server address structure */
    	memset(&servAddr, 0, sizeof(servAddr));     /* Zero out structure */
    	servAddr.sin_family      = AF_INET;             /* Internet address family */
   		servAddr.sin_addr.s_addr = inet_addr(serverName);   /* Server IP address */
    	servAddr.sin_port        = htons(servPort); /* Server port */
    		
    	if (servAddr.sin_addr.s_addr == -1) {
    	   	thehost = gethostbyname(serverName); 	
       		servAddr.sin_addr.s_addr = *((unsigned long *) thehost->h_addr_list[0]);
     	}
    	/* Establish the connection to the echo server */
    	if (connect(sock, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0){
    	   	perror("Could not connect");
    	   	exit(1);
    	}

    	unsigned long size = fsize(fileName);
    	FILE *fp = fopen(fileName,"r");
    	unsigned long totalBytesSent = 0;
    	int rv; // return value
    	char buffer[100];
    	while(totalBytesSent < size){
    		int bytesRead = fread(buffer,100,1,fp);
    		int bytesSent;
    		if((bytesSent = write(sock,&buffer,100)) < 0){
    			perror("could not write to socket");
    			exit(1);
    		}
    		totalBytesSent = totalBytesSent + bytesSent;
    	}
    	fclose(fp);
    	close(sock);
    	printf("File %s successfully transmitted\n",fileName);
    	
    }
}

unsigned long fsize(char* file){
	FILE * f = fopen(file,"r");
	fseek(f,0,SEEK_END);
	unsigned long len = (unsigned long) ftell(f);
	fclose(f);
	return len;
}

void handleInterupt(int signum){
	printf("\n %d times written \n",timesWritten);
	exit(1);
}