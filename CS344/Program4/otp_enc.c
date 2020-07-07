#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "otp.h"

int main(int argc, char *argv[]) {

	// Check arguments
	if (argc < 4) {
		fprintf(stderr,"USAGE: %s plaintext key port\n", argv[0]);
		exit(0);
	}

	// Call otp() and tell it to encode
	otp(argv[1], argv[2], argv[3], "otp_enc");

	return 0;
}