#include "otp.h"

void genKey(int len){
	int n, r;
	char key[len + 1];
	
	for (n = 0; n < len; n++){
		//Rand int from 0 to 27
		r = rand() % 27;
		//Use rand into to pull from array in otp.h
		key[n] = alph[r];
	}
	//Good practice to null terminate generated strings
	key[len] = '\0';
	printf("%s\n", key);
}

int main (int argc, char* argv[]){
	srand(time(0));
	//Generate random key with the given length
	genKey(atoi(argv[1]));
	return 0;
}
