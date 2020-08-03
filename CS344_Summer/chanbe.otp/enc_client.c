#include "otp.h"

int main (int argc, char *argv[]){
	
	if (argc < 4) {
		//Not enough arguments
		exit(0);
	}
	
	otp_c(argv[1], argv[2], argv[3], "e");
	
	return 0;
}
