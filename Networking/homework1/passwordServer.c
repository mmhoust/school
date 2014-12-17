// Matthew Houston //
// CPSC 360		   //
// This module is the password server module. It will continuosly accept guesses to a 
// password until terminated and reply with a return cod. It will take up to 3 
// arguments as inputs, the port number number of charachters of the password and 
// an optional initial password. After each correct guess a new random password will be
// generated.


#include "passwordServer.h"
// global variables in order to pass to sigint handler
struct linkedAddress *root;
int timesGuessedCorrectly;
long long timesGuessed;
int main(int argc, char*argv[]){
	int sock;
	int n; // number of chars
	struct sockaddr_in serverAddr;
	struct sockaddr_in clientAddr;
	unsigned int clientAddrLength;
	uint16_t  serverPort;
	timesGuessedCorrectly = 0;
	timesGuessed = 0;

	// parse command line inputs
	if(argc < 3 || argc > 4){
		printf("Usage: <port> <number of characters> \n");
		exit(1);
	}
	serverPort = atoi(argv[1]);
	n = atoi(argv[2]);
	if(n > 8){
		printf("Number of characters exceeds maximum size, Rounding to 8\n");
	}
	char password[n+1];
	if(argc == 4){
		strcpy(password,argv[3]);
	}else{
		generateRandomPassword(n,password);
		password[n] = '\0';
	}
	printf("password = %s \n",password);
	// handle signal
	signal(SIGINT,handleInterupt);

	//initialize incoming list
	root = malloc(sizeof(root));
	strcpy(root->IP,"root");
	struct linkedAddress *current;
	current = root;
	current->next = NULL;

	// create and bind sockets
	if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0){
		err_n_die("Could not open socket\n");
	}

	memset(&serverAddr,0,sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddr.sin_port = htons(serverPort);

	if(bind(sock,(struct sockaddr *) &serverAddr,sizeof(serverAddr)) < 0){
		err_n_die("Could not bind to socket\n");
	}

	
	int returnCode; 
	for(;;){
		clientAddrLength = sizeof(clientAddr);
		int recvMsgSize,sentMsgSize;
		char buffer[20];
		// recv message from server
		if((recvMsgSize = recvfrom(sock,buffer,20,0,(struct sockaddr*) &clientAddr,
			&clientAddrLength)) < 0) {
			err_n_die("Could not receive message\n");
		} 
		// store IP in linked list
		char IP[INET_ADDRSTRLEN+1];
		inet_ntop(AF_INET, &(clientAddr.sin_addr),IP,INET_ADDRSTRLEN+1);
		if(IP != NULL){
			addToList(IP);
		} else{
			printf("IP address for client can not be resolved\n");
		}
		// if password is correct regenerate new password
		if(strcmp(buffer,password)== 0){
			returnCode = 0;
			generateRandomPassword(n,password);
			password[n] = '\0';
			timesGuessedCorrectly++;			
		}else{
			returnCode = 1;
		}
		timesGuessed++;
		// send return code
		if((sentMsgSize = sendto(sock,&returnCode,20,0,(struct sockaddr*) &clientAddr,
			clientAddrLength)) < 0){
			err_n_die("Could not send message");
		}

	}

}

/* This method will generate a random password it takes in a pointer to the password 
	and the number of characters the password will be */
void generateRandomPassword(int n, char* password){
	// seed random gen
	srand(time(NULL));
	// generate password for each individual char
	int i;
	for(i= 0; i < n; ++i){
		int method = rand()%3;
		int r;
		switch (method){
			case 0:
				r = (rand() % 9) + 48;
				break;
			case 1:
				r = (rand() % 25) + 65;
				break;
			case 2:
				r = (rand() % 25) + 97;
				break;
			default:
				break;
		}
		password[i] = r;
	}
}
/* This function handles the control-C interupt by printing out the number of times the 
	password was guessed correctly and a list of the IP address that attempted to crack
	the password */
void handleInterupt(int signum){
	// print times guessed correctly
	printf("\n%lld %d\n",timesGuessed,timesGuessedCorrectly);
	// print and delete linked list

	struct linkedAddress *current;
	struct linkedAddress *next;
	current = root;

	while(current != NULL){
		
		if(current != root){
			printf("%s\n",current->IP);
		}
		next = current->next;
		free(current);
		current = next;
		next = NULL;
	} 

	exit(1);

}
/* This function adds an IP address to the linked list if it is not already present in 
	the list. It takes the printable dotted decimal version of the IP address as an input*/
void addToList(char *IP){
	struct linkedAddress *current;
	current = root;
	int isInList = 0;
	if(IP != NULL && current->IP != NULL){	
		while(current != NULL ){
			if(strcmp(current->IP,IP) == 0){
				isInList = 1;
				break;
			}
			if(current->next == NULL && isInList == 0){
				struct linkedAddress *next = malloc(sizeof(root));
				current->next = next;
				strcpy(next->IP,IP);
			}
			current = current->next;
		}
	}else{
		printf("invalid IP will not add to list\n");
	}		
}
