
#include "cracker.h"

void getNextPassword(char* password, int depth, int n){
	if(depth == 0){
		return;
	}
	// if end of range i.e 0-9 jump to next range
	if(password[depth-1] == '9'){
		password[depth-1] = 'A';
		return;
	}else if(password[depth-1] == 'Z'){
		password[depth-1] = 'a';
		return;
	}else if(password[depth-1] == 'z'){
		// get when one letter has iterated through ah nevermind idk how to explain what
		// i did it works though
		getNextPassword(password,depth-1,n);
		for(int i = depth -1; i<n; ++i){
			password[i] = '0';
		}
		return;
	}else {
		password[depth-1] += 1;
		return;
	}
}


void generateRandomPassword(int n, char* password){
	// seed random gen
	srand(time(NULL));
	// generate password for each individual char
	for(int i= 0; i < n; ++i){
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
