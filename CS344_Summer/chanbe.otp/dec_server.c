/**		Assignment 3: One-Time Pads  
  *		Written by Ben Chan for OSU CS 344, Summer 2020  
  *		
  *		This is the server-side decoder. This calls otp_s from otp.
  **/
  
#include "otp.h"

int main (int argc, char *argv[]){
	if (argc < 2) exit(0);	//Not enough arguments
	otp_s(argv[1], "dec");
	return 0;
}
