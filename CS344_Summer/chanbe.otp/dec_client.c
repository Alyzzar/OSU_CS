/**		Assignment 3: One-Time Pads  
  *		Written by Ben Chan for OSU CS 344, Summer 2020  
  *		
  *		This is the client-side decoder. This calls otp_c from otp.
  **/
  
#include "otp.h"

int main (int argc, char *argv[]){
	if (argc < 4) exit(0);	//Not enough arguments
	otp_c(argv[1], argv[2], argv[3], "dec");
	return 0;
}
