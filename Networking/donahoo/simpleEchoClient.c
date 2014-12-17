// simple client server

#include "methods.h"

int main(int argc, char*argv[]){
	if (argc != 4){
		err_n_die("Usage: <ip> <serverport> <message>");
	}
	char * servIP = argv[1];
	char * message = argv[3];

	in_port_t servPort = atoi(argv[2]);

	int sock = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if(sock < 0 ){
		err_n_die("could not create socket");
	}

	struct sockaddr_in servAddr;
	memset(&servAddr,0,sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	int rtnVal = inet_pton(AF_INET,servIP,&servAddr.sin_addr.s_addr);
	if (rtnVal < 0){
		err_n_die("Could not convert IP address");
	}
	servAddr.sin_port = htons(servPort);

	if(connect(sock,(struct sockaddr*) &servAddr,sizeof(servAddr)) < 0){
		err_n_die("Could not connect");
	}

	size_t messageLength = strlen(message);

	ssize_t bytesSent = send(sock,message,messageLength,0);
	if (bytesSent <0 || bytesSent != messageLength){
		err_n_die("Could not send message");
	}

	printf("Message sent waiting for reply\n");

	char buffer[1024];
	ssize_t bytesRecieved = recv(sock,buffer,1024,0);
	strcat(buffer,"\0");
	printf("%s\n",buffer);
	

	close(sock);
	exit(0);

}