#include "otp.h"

int main (int argc, char *argv[]){
	
	if (argc < 2) {
		//Not enough arguments
		exit(0);
	}
	
	otp_s(argv[1], 'e');
	
	return 0;
}
