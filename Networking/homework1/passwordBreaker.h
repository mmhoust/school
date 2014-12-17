// passwordBreaker

#ifndef BREAKER
#define BREAKER

#include "methods.h"
#include <time.h>


uint8_t tryPassword(char *guess,int n,struct sockaddr_in servAddr, int sock);

void getNextPassword(char *password,int depth, int n);

double getTime();

void handleAlarm(int);

void handleInterupt(int);


#endif