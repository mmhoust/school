
#include "methods.h"
#include <time.h>
#include <signal.h>


void generateRandomPassword(int n,char *password);
void handleInterupt(int signum);
void addToList();

struct linkedAddress{
	char IP[200];
	struct linkedAddress* next;
};