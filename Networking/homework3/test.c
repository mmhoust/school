// test.c 
#include "handler.h"
int main(int argc,char **argv){
	int error;
	char method[20];
	char testString[] = "GET image.jpg HTTP/1.1\r\nHost: www.cs.clemson.edu\r\n\r\n";
	char directory[] = "documents/test/";
	char uri[1000];
	char path[1000];
	char hostURL[1000];
	char header[5000];
	error = parseRequest(testString,method,uri,hostURL);
	strcpy(path,directory);
	strcat(path,uri);
	generateResponse(error,method,path,header);
	printf("%s",header);
	//return 0;
}