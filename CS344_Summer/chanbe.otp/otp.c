#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

#include "otp.h"
//type variable keeps track of whether input is plaintext or key for error reporting
void validateText(char text[], int len, char inp_type[]){
	int n, p;
	for (n = 0; n < len; n++) {
		for (p = 0; p < 28; p++) {
			// If p == 27, no match was found. Error
			if (p == 27) {
				fprintf(stderr, "ERROR: Invalid characters found in %s.\n", inp_type);
				exit(1);
			}
			// If current char is found in alph array, go to next n.
			if (text[n] == alph[p]) {
				break;
			}
		}
		//Else, no invalid characters found.
	}
}

//return length if valid
int validateLen(char plaintext[], char key[]){
	int plaintextLen;
	int keyLen;
	textLen = strlen (plaintext);
	keyLen  = strlen (key);
	
	if (textLen < keyLen) return textLen;
	
	//Else, key is too short
	fprintf(stderr, "ERROR: Key length [%d] less than plaintext length [%d].\n", keyLen, plaintextLen);
	exit(1);
}	

void parseFile(char* f_in, char* output){
	FILE* fd_text = fopen(f_in, 'r');
	fgets(output, 80000, fd_text);
	fclose(fd_text);
	//Reads file until it finds a newline character
	output[strcspn(output, '\n')] = '\0';
}

//OTP_c args
//argv[1]	argv[2]		argv[3] 	'e'||'d'
//text		key 		port #		option
otp_c (char* f_plaintext, char* f_key, char* port_str, char option) {
	//OTP for client side
	int len, sock_fd, c_write, c_read;
	int port = atoi(port_str);
	
	struct sockaddr_in serverAddress;
	struct hostent* serverHostInfo;
	char[] host = 'localhost';
	
	char buffer[512];
	char output[80000];
	char plaintext[80000];
	char key[80000];
	
	parseFile(f_plaintext, &plaintext);
	parseFile(f_key, &key);
	
	//Validation stuff (Calls validation functions)
	len = validateLen(plaintext, key);
	validateText(plaintext, len, 'plaintext');
	validateText(key, len, 'key');
	
	//Create and allocate a server struct
	memset((char*)&serverAddress, '\0', sizeof(serverAddress));
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(port);
	//Host should be localhost
	serverHostInfo = gethostbyname(host);
	if (serverHostInfo = NULL) {
		//Couldn't connect to localhost
		fprintf(stderr, "ERROR: Could not connect to [%s]\n.", host);
		exit(0);
	}
	//If host connected, copy address
	memcpy((char*)&serverAddress.sin_addr.s_addr, (char*)serverHostInfo->h_addr, serverHostInfo->h_length);
	
	// Set up the socket
	sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (sock_fd < 0) error("ERROR: Could not establish socket.\n", 1);
	// Connect
	if (connect(sock_fd, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0)
		error("ERROR: Could not connect socket to address.\n", 1);
	// Compose the message
	memset(buffer, '\0', sizeof(buffer));
	sprintf(buffer, '%s\n%s\n%s', plaintext, key, option);
	// Send the message
	c_write = send(sock_fd, buffer, strlen(buffer), 0);
	//No data was sent
	if (c_write < 0){
		error("Error: Could not write to socket.\n", 1);
	}
	//Not all data was sent
	if (c_write < strlen(buffer)){
		printf("ERROR: Not all data was sent to socket.\n");
	}
	
	//Get message from server
	memset(buffer, '\0', sizeof(buffer));
	c_read = recv(sock_fd, buffer, (sizeof(buffer) - 1), 0);
	if (c_read < 0){
		error("ERROR: Could ont read from socket.\n", 1);
	}
	
	FILE* f_recv = fopen(buffer, "r");
	fgets(output, 80000, f_recv);
	close(f_recv);
	
	remove(buffer);
	
	printf("%s\n", output);
	close(sock_fd);
	return 0;
}

//OTP_c args
//argv[1]		'e'||'d'
//port #		option
otp_d (char* port_str, char option) {
	//OTP for server side
	int len, sock_fd, conn_fd, c_read, clientSize;
	int pid = getpid();
	int port = atoi(port_str);
	
	struct sockaddr_in serverAddress, clientAddress;
	
	char buffer[512];
	char output[80000];
	char f_output[256];
	char plaintext[80000];
	char f_plaintext[256];
	char key[80000];
	char f_key[256];
	
	//Create and allocate a server struct (same as above)
	memset((char*)&serverAddress, '\0', sizeof(serverAddress));
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(port);
	//Host and address are set up differently (not localhost)
	serverAddress.sin_addr.s_addr = INADDR_ANY;
	
	// Set up the socket
	sock_fd = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
	if (sock_fd < 0) error("ERROR: Could not open socket.\n");
	if (bind(sock_fd, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0){
		error ("ERROR: Could not bind socket.\n");
	}
	//Turn on the socket with max connections = 5
	listen(sock_fd, MAX_CONNECTIONS);
	
	while(1){
		// Get address size for client
		clientSize = sizeof(clientAddress);
		// Accept connection
		conn_fd = accept(sock_fd, (struct sockaddr *)&clientAddress, &clientSize);
		if (conn_fd < 0) error("ERROR: Could not accept connection.\n");
		
		// Create child process
		pid = fork();
		switch (pid) {
			case -1:	;
				//Error. Terminate program
				error ("ERROR: Could not create child process.\n");
				exit(0);
				break;
			case 0:		;
				//Process created successfully
				memset(buffer, '\0', sizeof(buffer));
				c_read = recv(conn_fd, buffer, (sizeof(buffer) - 1), 0);
				if (c_read < 0) {
					error ("ERROR: Could not read from socket.\n");
				}
				
				// Array to find /0 and /n delimiters via strtok
				const char newline[2] = {'\n', '\0'};
				
				//Grab plaintext from top of buffer
				char *token = strtok(buffer, newline);
				strcpy(f_plaintext, token);
				//Grab key from buffer (NULL --> pointer continues from previous call)
				token = strtok(NULL, newline);
				strcpy(f_key, token);

				// Make sure the right program is connecting
				token = strtok(NULL, newline);
				if (strcmp(option, token)) {
					fprintf(stderr, "ERROR %s cannot use %s_d.\n", token, option );
					wrongFile = 1;
				}
				
				//Generate file, export contents of output
				sprintf(f_output, "%s_f.%d", option, pid);
				FILE* fd_output = fopen(f_output, "w+");
				
				//Correct file
				if (wrongfile == 0){
					parseFile(f_plaintext, &plaintext);
					parseFile(f_key, &key);
					
					if (option == 'e'){
						enc(plaintext, output, key, strlen(plaintext));
					} else if (option == 'd'){
						dec(plaintext, output, key, strlen(plaintext));
					}
					//Correct ouput. Print the en/decoded data into the file
					fprintf(fd_output, "%s", output);
				} else {
					//Incorrect output. Save and close the empty file.
					fprintf(fd_output, "");
				}
				fclose(fd_output);
				
				//Send the file back to the client, close the connection
				c_read = send(conn_fd, f_output, strlen(f_output), 0);
				if (c_read < 0) error ("ERROR: Could not write to socket.\n");
				close(conn_fd);
				conn_fd = -1;
				exit(0);
				
				break;
			default:		;
		}
		//End specific connections, kill processes
		close(conn_fd);
		conn_fd = -1;
		wait(NULL);
	}
	
	//Close connection to the server completely
	close (sock_fd);
	return 0;
}
