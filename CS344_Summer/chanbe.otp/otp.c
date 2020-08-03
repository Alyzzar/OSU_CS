#include "otp.h"
#define DEBUG 1
 
/*	Descrip:	Error reporting. Outputs the perror and exit value in the case of a detected crash.
 *	Params:		char* report, int value
 *  Return:		void
 */ 
void error(const char *report, int value){
	perror(report);
	exit(value);
}

/*	Descrip:	Validates that the text does not contain forbidden characters.
 *	Params:		char text[], int len
 *  Return:		void
 */
void validateText(char text[], int len){
	int n, p;
	for (n = 0; n < len; n++) {
		for (p = 0; p < 28; p++) {
			// If p == 27, no match was found. Error
			// printf("Character [%d]: %c == %c?\n", n, text[n], alph[p]);
			if (p == 27) {
				fprintf(stderr, "ERROR: Invalid characters found in input.\n");
				exit(1);
			}
			// If current char is found in alph array, go to next n.
			if (text[n] == alph[p]) {
				break;
			}
		}
		//Finished checking this character in the input. Move to next char.
	}
	//Else, no invalid characters found.
}

/*	Descrip:	Validates that the key is longer than the plaintext.
 *				If the key is longer, trim the length down to match the plaintext.
 *	Params:		char plaintext[], char key[]
 *  Return:		length of plaintext (if valid)
 */
int validateLen(char plaintext[], char key[]){
	int textLen;
	int keyLen;
	textLen = strlen (plaintext);
	keyLen  = strlen (key);
	
	if (textLen < keyLen){
		//Trim it
		key[textLen] = '\0';
		return textLen;
	}
	//Else, key is too short
	fprintf(stderr, "ERROR: Key length [%d] less than plaintext length [%d].\n", keyLen, textLen);
	exit(1);
}	

/*	Descrip:	Opens and reads files. Also marks the end of the file by locating '\n' chars.
 *	Params:		char* input file, char (*output)[]
 *  Return:		void
 */
void parseFile(char *f_in, char (*output) []){
	int len;
	FILE* fd_text = fopen(f_in, "r");
	fgets(*output, 80000, fd_text);
	fclose(fd_text);
	//Replace '/n' at end of file with '/0'
	len = strlen (*output);
	if ((*output)[len - 1] == '\n'){
		(*output)[len - 1] = '\0';
	}
}

/*	Descrip:	Client-side function. Parses input files, and creates a connection to the server side.
 *				Once server sends back data, this function also prints it to the console.
 *	Params:		char* plaintext file, char* key file, char* port number, char* option (enc//dec)
 *				argv[1]		argv[2]		argv[3] 	'e'||'d'
 *  Return:		int 0 on success
 */
int otp_c (char* f_plaintext, char* f_key, char* port_str, char* option) {
	//OTP for client side
	int len, sock_fd, c_write, c_read;
	int port = atoi(port_str);
	
	struct sockaddr_in serverAddress;
	struct hostent* serverHostInfo;
	
	char buffer[512];
	char output[80000];
	char plaintext[80000];
	char key[80000];
	
	parseFile(f_plaintext, &plaintext);
	parseFile(f_key, &key);
	
	//Validation stuff (Calls validation functions)
	if(DEBUG) printf("	(CLIENT) - DEBUG: Validating text:		");
	len = validateLen(plaintext, key);
	validateText(plaintext, len);
	validateText(key, len);
	if(DEBUG) printf("DONE\n");
	
	//Create and allocate a server struct
	if(DEBUG) printf("	(CLIENT) - DEBUG: Creating server:		");
	memset((char*)&serverAddress, '\0', sizeof(serverAddress));
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(port);
	//Host should be localhost
	serverHostInfo = gethostbyname("localhost");
	if (serverHostInfo == NULL) {
		//Couldn't connect to localhost
		fprintf(stderr, "ERROR: Could not connect to localhost\n.");
		exit(0);
	}
	//If host connected, copy address
	memcpy((char*)&serverAddress.sin_addr.s_addr, (char*)serverHostInfo->h_addr, serverHostInfo->h_length);
	if(DEBUG) printf("DONE\n");
	
	// Set up the socket
	if(DEBUG) printf("	(CLIENT) - DEBUG: Establishing socket:		");
	sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (sock_fd < 0) error("ERROR: Could not establish socket.\n", 1);
	if(DEBUG) printf("DONE\n");
	
	// Connect to the socket
	if(DEBUG) printf("	(CLIENT) - DEBUG: Connecting to socket:		");
	if (connect(sock_fd, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0){
		error("ERROR: Could not connect socket to address.\n", 1);
	}
	if(DEBUG) printf("DONE\n");
	
	if(DEBUG) printf("	(CLIENT) - DEBUG: Sending message to server:	");
	// Compose the message
	memset(buffer, '\0', sizeof(buffer));
	sprintf(buffer, "%s\n%s\n%s", plaintext, key, option);
	// Send the message
	c_write = send(sock_fd, buffer, strlen(buffer), 0);
	//No data was sent
	if (c_write < 0){
		error("ERROR: Could not write to socket.\n", 1);
	}
	//Not all data was sent
	if (c_write < strlen(buffer)){
		printf("ERROR: Not all data was sent to socket.\n");
	}
	if(DEBUG) printf("DONE\n");
		
	//Get message from server
	if(DEBUG) printf("	(CLIENT) - Debug: Receiving message from server:	");
	memset(buffer, '\0', sizeof(buffer));
	c_read = recv(sock_fd, buffer, (sizeof(buffer) - 1), 0);
	if (c_read < 0){
		error("ERROR: Could not read from socket.\n", 1);
	}
		
	FILE* f_recv = fopen(buffer, "r");
	fgets(output, 80000, f_recv);
	fclose(f_recv);
	
	remove(buffer);
	if(DEBUG) printf("DONE\n");
	
	//Printing final output
	printf("%s\n", output);
	close(sock_fd);
	return 0;
}

/*	Descrip:	Server-side function. Creates an open connection to matching client-side function.
 *				Once data is received, parse and create child process to enc/dec based on values in data.
 *				Send data back once it has been correctly enc/dec'd.
 *	Params:		char* port number, char* option (enc//dec)
 *				argv[1]		'e'||'d'
 *  Return:		int 0 on success
 */
int otp_s (char* port_str, char* option) {
	//OTP for server side
	int sock_fd, conn_fd, c_read, wrongFile;
	int port = atoi(port_str);
	pid_t pid, ch_pid;
	
	socklen_t clientSize;
	struct sockaddr_in serverAddress, clientAddress;
	
	char buffer[512];
	char output[80000];
	//char *f_output = malloc(256 * sizeof(char));
	char f_output[32]; 
	char plaintext[80000];
	//char *f_plaintext = malloc(256 * sizeof(char));
	char f_plaintext[256];
	char key[80000];
	char f_key[256];
	
	//Create and allocate a server struct (same as above)
	if(DEBUG) printf("\n	(SERVER) - DEBUG: Creating server:		");
	memset((char*)&serverAddress, '\0', sizeof(serverAddress));
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(port);
	//Host and address are set up differently (not localhost)
	serverAddress.sin_addr.s_addr = INADDR_ANY;
	if(DEBUG) printf("DONE\n");
	
	// Set up the socket
	if(DEBUG) printf("	(SERVER) - DEBUG: Establishing socket:		");
	sock_fd = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
	if (sock_fd < 0){
		//free (f_output);
		//free (f_plaintext);
		error("ERROR: Could not open socket.\n", 1);
	}
	if (bind(sock_fd, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0){
		//free (f_output);
		//free (f_plaintext);
		error ("ERROR: Could not bind socket.\n", 1);
	}
	//Turn on the socket with max connections = 5
	listen(sock_fd, MAX_CONNECTIONS);
	if(DEBUG) printf("DONE\n");
		
	while(1){
		if(DEBUG) printf("	(SERVER) - DEBUG: Connecting to socket:		");
		// Get address size for client
		clientSize = sizeof(clientAddress);
		// Accept connection
		conn_fd = accept(sock_fd, (struct sockaddr *)&clientAddress, &clientSize);
		if (conn_fd < 0){
			//printf("conn_fd === %d\n", conn_fd);
			//free (f_output);
			//free (f_plaintext);
			error("ERROR: Could not accept connection.\n", 1);
		}
		if(DEBUG) printf("DONE\n");
		
		// Create child process
		if(DEBUG) printf("	(SERVER) - DEBUG: Creating child process:	");
		pid = fork();
		switch (pid) {
			case -1:	;
				//Error. Terminate program
				//free (f_output);
				//free (f_plaintext);
				error ("ERROR: Could not create child process.\n", 1);
				exit(0);
				break;
			case 0:		;
				//Process created successfully
				if(DEBUG) printf("DONE\n");
				
				memset(buffer, '\0', sizeof(buffer));
				c_read = recv(conn_fd, buffer, (sizeof(buffer) - 1), 0);
				if (c_read < 0) {
					//free (f_output);
					//free (f_plaintext);
					error ("ERROR: Could not read from socket.\n", 1);
				}
				
				if(DEBUG) printf("	(SERVER) - DEBUG: Parsing message.\n");
				// Array to find /0 and /n delimiters via strtok
				const char newline[2] = {'\n', '\0'};
				
				//Grab plaintext from top of buffer
				char *token = strtok(buffer, newline);
				strcpy(f_plaintext, token);
				if(DEBUG) printf("	(SERVER) - DEBUG: f_plaintext:\n%s\n", f_plaintext);
				
				//Grab key from buffer (NULL --> pointer continues from previous call)
				token = strtok(NULL, newline);
				strcpy(f_key, token);
				if(DEBUG) printf("	(SERVER) - DEBUG: f_key:\n%s\n", f_key);

				// Make sure the right program is connecting
				if(DEBUG) printf("	(SERVER) - DEBUG: Validating coder option:	");
				wrongFile = 0;
				token = strtok(NULL, newline);
				if (strcmp(option, token)) {
					fprintf(stderr, "ERROR \"%c\" is not equal to \"%s\".\n", *token, option );
					wrongFile = 1;
				}
				if(DEBUG) printf("DONE\n");
				if(DEBUG) printf("	(SERVER) - DEBUG: Generating output file:	");	
				ch_pid = getpid();
				//Generate file, export contents of output
				//printf(" - option: %s\n - child PID: %d\n", option, ch_pid);
				sprintf(f_output, "%s_f.%d", option, ch_pid);
				//printf(" - f_output: %s\n", f_output);
				FILE* fd_output = fopen(f_output, "w+");
				if(DEBUG) printf("DONE\n");
				
				//Correct file
				if (wrongFile == 0){
					//Parsing files
					if(DEBUG) printf("	(SERVER) - DEBUG: Parsing text files:		");
					
					parseFile(f_key, &key);
					parseFile(f_plaintext, &plaintext);
					
					if (strcmp(option, "e")){
						enc(plaintext, output, key, strlen(plaintext));
					} else if (strcmp(option, "d")){
						dec(plaintext, output, key, strlen(plaintext));
					}
					//Correct ouput. Print the en/decoded data into the file
					fprintf(fd_output, "%s", output);
					if(DEBUG) printf("DONE\n");
					
				} else {
					//Else: incorrect output. Save and close the empty file.
					if(DEBUG) printf("	(SERVER) - DEBUG: Wrong option detected.\n");
				}
				fclose(fd_output);
				
				//Send the file back to the client, close the connection
				c_read = send(conn_fd, f_output, strlen(f_output), 0);
				if (c_read < 0){
					//free (f_output);
					//free (f_plaintext);	
					error ("ERROR: Could not write to socket.\n", 1);
				}
				close(conn_fd);
				conn_fd = -1;
				exit(0);
				
				break;
			default:		;
				if(DEBUG) printf("DEFAULT\n");
			//Error 404: There's nothing in here!
		}
		//End specific connections, kill processes
		close(conn_fd);
		conn_fd = -1;
		wait(NULL);
	}
	if(DEBUG) printf("	(SERVER) - DEBUG: Actions complete. Terminating connection.\n");
	//Free memory, close connection to the server completely
	//free (f_output);
	//free (f_plaintext);
	close (sock_fd);
	return 0;
}
