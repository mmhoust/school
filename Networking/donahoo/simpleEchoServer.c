// simple server


#include "methods.h"

static const int MESSAGESIZE = 1000000;

void handleClient(int);

int main(int argc, char *argv[]){
	if (argc != 2){
		err_n_die("Parameters: <Port>");
	}
	in_port_t servPort = atoi(argv[1]);
	// create socket
	int serverSocket;
	serverSocket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if(serverSocket < 0){
		err_n_die("Could not create socket");
	}
	// local address struct
	struct sockaddr_in servAddr;
	memset(&servAddr,0,sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servAddr.sin_port = htons(servPort);
	// bind and listen
	if(bind(serverSocket,(struct sockaddr*) &servAddr,sizeof(servAddr)) < 0){
		err_n_die("Could not bind ServerSocket");
	}

	if (listen(serverSocket, 5) < 0){
		err_n_die("Could not listen");
	}

	while(1){
		//
		struct sockaddr_in clntAddr;
		socklen_t clntAddrLen = sizeof(clntAddr);

		int client = accept(serverSocket,(struct sockaddr *) &clntAddr,&clntAddrLen);
		if(client < 0){
			err_n_die("Could not accept connection");
		}

		char clientName[INET_ADDRSTRLEN];
		if(inet_ntop(AF_INET, &clntAddr.sin_addr.s_addr,clientName,sizeof(clientName)) != NULL){
			printf("Connected to client %s on port %d \n",clientName, ntohs(clntAddr.sin_port));
		}
		else {
			printf("Unable to get client address");
		}

		handleClient(client);

	}
}

void handleClient(int socket){
	char buffer[1024];
	// recieve message
	ssize_t bytesReceived = recv(socket, buffer,1024,0);
	if(bytesReceived < 0){
		err_n_die("Receive failed");
	}
	strcat(buffer,"\0");
	printf("%s\n",buffer);

	ssize_t numBytesSent = send(socket, buffer, bytesReceived,0);
	printf("Echoed %ld bytes\n",numBytesSent);
	close(socket);
}