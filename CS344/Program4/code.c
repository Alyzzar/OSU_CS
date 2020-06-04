#include <stdio.h>
#include <string.h>
#include "otp.h"

// Convert a chracter to an integer based on code[] (in code.h)
void charToInt(char input[], int output[], int size) {
	int i, j;
	for (i=0; i<size; i++) {
		for (j=0; j<27; j++) {
			if (input[i] == code[j]) {
				output[i] = j;
				break;
			}
		}
	}
}

// Covert an integer to a character based on code[] (in code.h)
void intToChar(int input[], char output[], int size) {
	int i;
	for (i=0; i<size; i++) {
		output[i] = code[input[i]];
	}
}

/**
Encodes a string using a provided key
Inputs:
	char key[]		The key to base it on.
	char input[]	The string to encode.
	char output[]	A string to store the encoded string as output.
	int size		The size of the string.
 **/
void encode(char key[], char input[], char output[], int size) {
	int i;

	// Convert input array to integers
	int intIn[size];
	charToInt(input, intIn, size);

	// Convert key to integers
	int intKey[size];
	charToInt(key, intKey, size);

	// Generate output in integer form
	int intOut[size];
	for (i=0; i<size; i++) {
		intOut[i] = (intIn[i] + intKey[i]) % 27;
	}

	// Convert output to characters
	intToChar(intOut, output, size);
	output[size] = '\0';
}

/**
Encodes a string using a provided key
Inputs:
	char key[]		The key the string has been encoded by.
	char input[]	The string to decode.
	char output[]	A string to store the decoded string as output.
	int size		The size of the string.
**/
void decode(char key[], char input[], char output[], int size) {
	int i;

	// Convert input array to integers
	int intIn[size];
	charToInt(input, intIn, size);

	// Convert key to integers
	int intKey[size];
	charToInt(key, intKey, size);

	// Generate output in integer form
	int intOut[size];
	for (i=0; i<size; i++) {
		intOut[i] = intIn[i] - intKey[i];
		if (intOut[i] < 0) intOut[i] += 27;
	}

	// Convert output to characters
	intToChar(intOut, output, size);
	output[size] = '\0';
}
