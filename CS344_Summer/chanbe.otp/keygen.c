/**		Assignment 3: One-Time Pads  
  *		Written by Ben Chan for OSU CS 344, Summer 2020  
  *		
  *		This is the keygen program for OTP.
  **/

#include "otp.h"

/*	Descrip:	Generates a randomized key based on the char alph[27] array in coder.c
 *	Params:		int len
 *	Return:		void
 */ 
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

/*	Descrip:	Keygen main function. This creates a random seed, verifies parameters, and calls keygen.
 *	Params:		int len (argv[1])
 *	Return:		int 0 on exit
 */ 
int main (int argc, char* argv[]){
	if (argc == 2){
		srand(time(0));
		//Generate random key with the given length
		genKey(atoi(argv[1]));
		return 0;
	} else {
		printf("Wrong number of arguments provided. Expected one integer value.\n");
	}
	return 0;
}
