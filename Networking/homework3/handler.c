// handler.c

#include "handler.h"

#define RCVBUFSIZE 32

int handleClient(int sock,char*directory){

	char buffer[1000];
	char rcv[RCVBUFSIZE];
	int rcvMsgSize,ptr;

	rcvMsgSize = 1;
	ptr = 0;
	int flag = 0;
	while(rcvMsgSize > 0 && flag == 0){
		if((rcvMsgSize = recv(sock,rcv,RCVBUFSIZE,0)) < 0){
			err_n_die("Could not recieve message");
		}
		memcpy(buffer+ptr,rcv,rcvMsgSize);
		ptr = ptr + rcvMsgSize;
		if(strstr(buffer,"\r\n\r\n") != NULL){
			flag = 1;
		}
	}
	char method[10];
	char hostURL[1000];
	char uri[1000];
	//
	int error = parseRequest(buffer,method,uri,hostURL);
	
	char response[1000];
	char path[1000];
	strcpy(path,directory);
	strcat(path,uri);

	error = generateResponse(error,method,path,response);

	FILE *fp = fopen(path,"r");
	int len = strlen(response);
	int sendMessageSize;
	if((sendMessageSize = send(sock,response,len,0)) < len){
		perror("could not send header");
	}
	if(strcmp(method,"GET") == 0 && error == 200){
		long totalBytesWritten = 0;
		long bytes= 0;
		long size;
		struct stat info;
		stat(path,&info);
		size = (long)info.st_size;
		char buf[2];
		while(totalBytesWritten < size){
			bytes = fread(buf,1,1,fp);
			if((sendMessageSize = send(sock,buf,bytes,0)) < 0){
				perror("could not send file");
			}
			totalBytesWritten = totalBytesWritten + bytes;
		}
	}
	char currentDate[50];
	time_t now = time(NULL);
	strftime(currentDate,50, "Date: %a, %d %b %Y %k:%M:%S", localtime(&now));
	printf("%s %s %s %d\n",method,uri,currentDate,error);
	fflush(stdout);
	fclose(fp);
	close(sock);


	return 0;
}

int parseRequest(char *buffer,char *method,char *uri , char* hostURL){
	char delim[3] = "\r\n";
	char *token;
	char *savePtr;
	int flag = 0;
	token = strtok_r(buffer,delim,&savePtr);
//	printf("token = %s buffer = %s \n",token,buffer);
	flag = parseLine(token,method,uri,NULL);
	// check for valid header and return proper error code
	if(flag == 0){
		return 400;
	}else if(flag == 1){
		
	}else if(flag == 2){
		return 405;
	}

	flag = 0;
	while(token != NULL && flag == 0){
		token = strtok_r(NULL,delim,&savePtr);
		flag = parseLine(token,NULL,NULL,hostURL);
	}

	if (flag == 0){
		return 400;
	}

	return 200;
}

int parseLine(char *token,char *method, char *uri, char *hostURL){
	char *savePtr1;
	char *token1;
	int rv;
	token1 = strtok_r(token," ",&savePtr1);
	if((strcmp(token1,"Host:")) == 0){
		token1 = strtok_r(NULL,"\r\n",&savePtr1);
		strcpy(hostURL,token1);
		rv = 1;
	}else if((strcmp(token1,"GET")) == 0 || (strcmp(token1,"HEAD")) == 0){
		strcpy(method,token1);
		token1 = strtok_r(NULL," ",&savePtr1);
		strcpy(uri,token1);
		token1 = strtok_r(NULL,"\r\n",&savePtr1);
		if(strcmp(token1,"HTTP/1.1") == 0){
			rv = 1;
		}else{
			rv = 0;
		}
	}else if((strcmp(token1,"POST")) == 0 || (strcmp(token1,"PUT")) == 0
		|| (strcmp(token1,"DELETE")) == 0 || (strcmp(token1,"TRACE")) == 0
		|| (strcmp(token1,"OPTION")) == 0 || (strcmp(token1,"CONNECT")) == 0
		|| (strcmp(token1,"PATCH")) == 0) {

		rv = 2;
	}else{
		rv = 0;
	}

	return rv;
}

int generateResponse(int error,char *method,char *path, char *header){
	struct stat info;
	stat(path,&info);
	if(access(path,F_OK) != 0 && error == 200){
		error =404;
	}else if(access(path,R_OK) != 0 && error == 200){
		error = 403;
	}
	if(strstr(path,"/../")!= NULL){
		error = 403;
	}
	//get time information
	time_t timeSinceModified,now;
	timeSinceModified = info.st_mtime;
	now = time(NULL);
	// initatialize elements of response
	char dateSinceModified[50];
	char currentDate[50];
	char contentType[50];
	char contentLength[50];
	char server[30] = "MyHTTPServer/1.0\r\n";
	// get contentType
	strcpy(contentType,"Content-Type: ");

	if(strstr(path,".html") != NULL || strstr(path,".htm") != NULL){
		strcat(contentType,"text/html\r\n");
	}else if(strstr(path,".css") != NULL){
		strcat(contentType,"text/css\r\n");
	}else if(strstr(path,".js") != NULL){
		strcat(contentType,"application/javascript\r\n");
	}else if(strstr(path,".txt") != NULL){
		strcat(contentType,"text/plain\r\n");
	}else if(strstr(path,".jpg") != NULL){
		strcat(contentType,"image/jpeg\r\n");
	}else if(strstr(path,".pdf") != NULL){
		strcat(contentType,"application/pdf\r\n");
	}else{
		strcat(contentType,"application/octet-stream\r\n");
	}

	// get file time and current time 
	if(error == 200){
		strftime(dateSinceModified,50, "Last-Modified: %a, %d %b %Y %k:%M:%S\r\n", localtime(&timeSinceModified));
	}
	strftime(currentDate,50, "Date: %a, %d %b %Y %k:%M:%S\r\n", localtime(&now));
	// get content length
	sprintf(contentLength,"Content-Length: %d\r\n",(int)info.st_size);
	// create header
	sprintf(header,"HTTP/1.1 %d",error);
	if(error == 200){
		strcat(header," OK\r\n");
	}else if(error == 400){
		strcat(header," Bad Request\r\n");
	}else if(error == 403){
		strcat(header," Forbidden\r\n");
	}else if(error == 404){
		strcat(header," Not Found\r\n");
	}else if(error == 405){
		strcat(header,"Method Not Allowed\r\n");
		strcat(header,"Allow: GET, HEAD\r\n");
	}
	strcat(header,currentDate);
	if(error == 200){	
		strcat(header,dateSinceModified);
		strcat(header,contentType);
		strcat(header,contentLength);
	}
	strcat(header,server);
	strcat(header,"Connection: close\r\n");
	strcat(header,"\r\n");
	return error;
}