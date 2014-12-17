#include "methods.h"
#include "string.h"

struct DNSHeader {
	uint16_t ID;
	uint16_t flags;
	uint16_t QuestionCount;
	uint16_t AnswerRecordCount;
	uint16_t AuthorityRecordCount;
	uint16_t AdditionalRecordCount;
};

int makeHeader(struct DNSHeader *head);
int makeRequest(char* IP,char* request);
void readMessage(char* message,char* IP,char* Alias,int *queryLength,int auth);
int readName(char *, char*, int);
void convertFromDNS(char* alias);
void printName(char* name);
struct DNSHeader readHeader(char* msg);


