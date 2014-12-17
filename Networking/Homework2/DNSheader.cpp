
#include "DNSheader.h"

struct DNSHeader makeHeader(){
	srand(time(NULL));
	uint16_t r = rand();
	struct DNSHeader header;
	header.ID = r;
	header.QR = 0;
	header.OPCode = 0;
	header.TC = 0;
	header.RD = 1;
	header.RA = 0;
	header.Zero = 0;
	header.ResponseCode = 0;
	header.QuestionCount = 1;
	header.AnswerRecordCount = 0;
	header.AuthorityRecordCount = 0;
	header.AdditionalRecordCount = 0;

	return header;
}

void makeRequest(char* IP, char* request){
	int strlength = strlen(IP);
	request = (char*) malloc(12+strlength+5);
	struct DNSHeader head = makeHeader();
	memcpy(request,&head,12);
	int i;
	for(i = 0; i < strlength; i++){
		request[12+i] = IP[i];
	}
	request[12+strlength] = 0;
	request[12+strlength+1] = 0;
	request[12+strlength+2] = 1;
	request[12+strlength+3] = 0;
	request[12+strlength+5] = 1;

}

struct DNSHeader readHeader(char* msg){
	struct DNSHeader head;
	memcpy(&head,msg,12);
	return head;
}