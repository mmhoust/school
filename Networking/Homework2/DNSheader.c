
#include "DNSheader.h"

int makeHeader(struct DNSHeader *head){
	srand(time(NULL));
	uint16_t r = rand();
	head->ID = htons(r);
	head->flags =  htons(0x0100);
	head->QuestionCount = htons(1);
	head->AnswerRecordCount = 0;
	head->AuthorityRecordCount = 0;
	head->AdditionalRecordCount = 0;

	return head->ID;
}

int makeRequest(char* IP, char* request){
	struct DNSHeader head;
	makeHeader(&head);
	memcpy(request,&head,12);
	//convert ip to correct format
	int placeholder = 0;
	int length = 0;
	char name[30] = ""; 
	char *token;
	// break IP into chunks using strtok and concatanate with "."
	token = strtok(IP,".");
	while(token != NULL){
		if(name == NULL){
			placeholder = 0;
		}else{
			placeholder = strlen(name);
		}
		name[placeholder] = strlen(token);
		strcat(name,token);	
		token = strtok(NULL,".");
	}
	length = strlen(name);
	// add 
	char post[5];
	memcpy(request+12,name,length);
	post[0] = 0;
	post[1] = 0;
	post[2] = 1;
	post[3] = 0;
	post[4] = 1;
	memcpy(request+12+length,post,5);
	length = length + 17;

	return length;

}

void readMessage(char* message,char* answer,char* name,int *queryLength,int auth){
	int position = *queryLength;
	uint16_t Type,Class,DataLength;
	uint32_t TTL;

	position = readName(message,name,position);
	position++;
	memcpy(&Type,message+position,2);
	Type = ntohs(Type);
	position = position+2;
	memcpy(&Class,message+position,2);
	Class = ntohs(Class);
	position = position +2;
	memcpy(&TTL,message+position,4);
	TTL = ntohl(TTL);
	position = position +4;
	memcpy(&DataLength,message+position,2);
	DataLength = ntohs(DataLength);
	position = position+2;
	convertFromDNS(name);
	if(Type == 0x05){
		//printf("%d \n",position);
		readName(message,answer,position);
		convertFromDNS(answer);
		printf("CNAME\t%s\t%s\t%u",name,answer,TTL);
	}else if(Type == 0x01){
		memcpy(answer,message+position,4);
		answer[4] = 0x00;
		char printable[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, answer,printable,INET_ADDRSTRLEN);
		printf("IP\t%s\t%d",printable,TTL);
	}
	if(auth == 1){
		printf("AUTH\n");
	}else{
		printf("\tNONAUTH\n");
	}
	position = position + DataLength;
	*queryLength = position;	
	
	
}

void convertFromDNS(char* alias){
	int length = strlen(alias);
	char temp[length];
	int totalLen = 0;
	int lengthOfToken;

	while((lengthOfToken = alias[totalLen]) != 0x00){
		memcpy(temp + totalLen, alias+totalLen+1 ,lengthOfToken);
		totalLen = lengthOfToken + totalLen;
		temp[totalLen] = '.';
		totalLen++;
	}
	temp[totalLen-1] = 0x00;
	strcpy(alias,temp);
}

int readName(char *message,char *alias,int begPos){
	int i = begPos;
	int isCompressed = 0;
	int aliasCount = 0;
	char c = 0xc0;
	while(message[i] != 0x00){		
		if(message[i] == c){
			isCompressed = 1;
			i = message[i+1];		
		}
		alias[aliasCount] = message[i];
		i++;
		aliasCount++;
	}
	alias[aliasCount] = 0x00;
	if(isCompressed == 1){
		i = begPos+1;
	}
	return i;
}

struct DNSHeader readHeader(char* msg){
	struct DNSHeader head;
	memcpy(&head,msg,12);
	return head;
}