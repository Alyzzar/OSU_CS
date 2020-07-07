#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

#include "otp.h"

int main (int argc, char* argv[]) {
	//Initialize variables
	int i;
	int length = atoi(argv[1]);
	char key[length+1];
	srand(time(0));

	// Generate random key using loop and rand()
	for (i=0; i<length; i++) {
		int letter = rand() % 27;
		key[i] = code[letter];
	}

	// Cleaning up end of key strings for extraneous values
	key[length] = '\0';
	//Print the key
	printf("%s\n", key);

	return 0;
}
