// receive request
#ifndef METHODS
#define METHODS
#include "methods.h"

int handleClient(int sock,char *directory);
int parseRequest(char*,char*,char*,char*);
int parseLine(char *token,char*method,char*uri,char *hostURL);
int generateResponse(int error,char *,char *,char *);

#endif