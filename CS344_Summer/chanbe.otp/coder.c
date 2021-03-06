/**		Assignment 3: One-Time Pads  
  *		Written by Ben Chan for OSU CS 344, Summer 2020  
  *		
  *		This is the coder program. It includes functions for encoding and decoding messages. 
  *		These are called by otp_s in otp.c
  **/

#include "otp.h"

const char alph[27] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ ";

/*	Descrip:	Converts an array of integers into an array of chars
 *	Params:		int input[], char output[], int len
 *	Return:		void
 */ 
void intToStr (int input[], char output[], int len){
	int n;
	for (n = 0; n < len; n++){
		output[n] = alph[input[n]];
	}
}

/*	Descrip:	Converts an array of chars into an array of integers
 *	Params:		char input[], int output[], int len
 *	Return:		void
 */ 
void strToInt (char input[], int output[], int len){
	int n, p;
	for (n = 0; n < len; n++){
		for (p = 0; p < 27; p++){
			if (input[n] == alph[p]){
				output[n] = p;
				break;
			}
		}
	}
}

/*	Descrip:	Encodes a message using the formula [(input + key) % 27]	
 *	Params:		char input[], char output[], char key[], int len
 *	Return:		void
 */ 
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

/*	Descrip:	Decodes a message using the formula [((input - key) + 27) % 27]	
 *	Params:		char input[], char output[], char key[], int len
 *	Return:		void
 */ 
void dec (char input[], char output[], char key[], int len){
	int n;
	int inp_int[len];
	int key_int[len];
	strToInt (input, inp_int, len);
	strToInt (key, key_int, len);
	
	//Decoding formula
	for (n = 0; n < len; n++){
		inp_int[n] = ((inp_int[n] - key_int[n]) + 27) % 27;
	}
	intToStr (inp_int, output, len);
	output[len] = '\0';
}
