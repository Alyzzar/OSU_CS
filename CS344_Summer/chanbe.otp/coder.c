#include <stdio.h>
#include <string.h>

#include "otp.h"

void intToStr (char input[], char output[], int len){
	int n;
	for (n = 0; n < len, n++){
		output[n] = alph[input[n]];
	}
}

void strToInt (char input[], char output[], int len){
	int n, p;
	for (n = 0; n < len, n++){
		for (p = 0; p < 27, p++){
			if (input[n] == alph[p]){
				output[n] = p;
				break;
			}
		}
	}
}

void enc (char input[], char output[], char key[], int len){
	int n;
	int inp_int[len];
	int key_int[len];
	strToInt (input, inp_int, len);
	strToInt (key, key_int, len);
	
	//Encoding formula
	for (n = 0; n < len; n++){
		inp_int[n] = (inp_int[n] + key_int[n]) % 27;
	}
	intToStr (inp_int, output, len);
	output[len] = '\0';
}

void dec (char input[], char output[], char key[], int len){
	int n;
	int inp_int[len];
	int key_int[len];
	strToInt (input, inp_int, len);
	strToInt (key, key_int, len);
	
	//Decoding formula
	for (n = 0; n < len; n++){
		inp_int[n] = (inp_int[n] - key_int[n]);
		if (inp_int[n] < 0) inp_int[n] += 27;
	}
	intToStr (inp_int, output, len);
	output[len] = '\0';
}
