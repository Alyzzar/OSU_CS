#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>

#define InputLengthMax 2048

//Global variables, function declarations
int allBackground = 1;
void getInput(char*[], int*, char[], char[], int);
void execCmd(char*[], int*, struct sigaction, int*, char[], char[]);
void catchSIGTSTP(int);
void printExitStatus(int);

/* Main
 * Receives and parses user input
 * Calls child functions
 */
int main(){
	//Variables
	int i;
	int cont = 1;
	int exitStatus = 0;
	int background = 0;
	int pid = getpid();
	char inputFile[256] = "";
	char outputFile[256] = "";
	char* input[512];
	
	//Make sure input array is NULL
	for (i=0; i < 512; i++_ {
		input[i] = NULL;
	}
	
	// CTRL + C --> sigint
	struct sigaction sa_sigint = {0};
	sa_sigint.sa_handler = SIG_IGN;
	sigfillset(&sa_sigint.sa_mask);
	sa_sigint.sa_flags = 0;
	sigaction(SIGINT, &sa_sigint, NULL);

	// CTRL + Z --> sigstop
	struct sigaction sa_sigtstp = {0};
	sa_sigtstp.sa_handler = catchSIGTSTP;
	sigfillset(&sa_sigtstp.sa_mask);
	sa_sigtstp.sa_flags = 0;
	sigaction(SIGTSTP, &sa_sigtstp, NULL);
	
	do {
		// Get input
		getInput(input, &background, inputFile, outputFile, pid);

		// Skip comments/spaces
		if (input[0][0] == '#' ||
					input[0][0] == '\0') {
			continue;
		}
		
		// Exit
		else if (strcmp(input[0], "exit") == 0) {
			cont = 0;
		}
	
		// Change dir
		else if (strcmp(input[0], "cd") == 0) {
			// Explicit directory
			if (input[1]) {
				if (chdir(input[1]) == -1) {
					printf("Directory not found.\n");
					fflush(stdout);
				}
			} else {
			// Home directory
				chdir(getenv("HOME"));
			}
		}

		// Return file/dir status
		else if (strcmp(input[0], "status") == 0) {
			printExitStatus(exitStatus);
		}

		// Other built in commands
		else {
			execCmd(input, &exitStatus, sa_sigint, &background, inputFile, outputFile);
		}

		// Reset vars
		// Reset input array to NULL
		for (i=0; input[i]; i++) {
			input[i] = NULL;
		}
		background = 0;
		inputFile[0] = '\0';
		outputFile[0] = '\0';

	} while (cont);
	
	return 0;
}

/* getInput
 * parses input via stdin or from file
 * stores parsed input in variable 'input'
 */
 void getInput(char* arr[], int* background, char inputName[], char outputName[], int pid) {
	
	char input[INPUTLENGTH];
	int i, j;

	// Get input
	printf(": ");
	fflush(stdout);
	fgets(input, INPUTLENGTH, stdin);

	// Skip newline characters
	int found = 0;
	for (i=0; !found && i<INPUTLENGTH; i++) {
		if (input[i] == '\n') {
			input[i] = '\0';
			found = 1;
		}
	}

	// If input is empty, return nothing
	if (!strcmp(input, "")) {
		arr[0] = strdup("");
		return;
	}

	// Break up input using strtok
	const char space[2] = " ";
	char *token = strtok(input, space);

	for (i=0; token; i++) {
		// & == background process
		if (!strcmp(token, "&")) {
			*background = 1;
		}
		// < == input file
		else if (!strcmp(token, "<")) {
			token = strtok(NULL, space);
			strcpy(inputName, token);
		}
		// > == output file
		else if (!strcmp(token, ">")) {
			token = strtok(NULL, space);
			strcpy(outputName, token);
		}
		// Other:
		else {
			arr[i] = strdup(token);
			// Replace $$ with pid
			// Only occurs at end of string in testscirpt
			for (j=0; arr[i][j]; j++) {
				if (arr[i][j] == '$' &&
					 arr[i][j+1] == '$') {
					arr[i][j] = '\0';
					snprintf(arr[i], 256, "%s%d", arr[i], pid);
				}
			}
		}
		// Next!
		token = strtok(NULL, space);
	}
}

/* execCmd
 * Forks the process
 */
void execCmd(char* arr[], int* childExitStatus, struct sigaction sa, int* background, char inputName[], char outputName[]) {
	
	int input, output, result;
	pid_t spawnPid = -5;

	spawnPid = fork();
	switch (spawnPid) {
		
		case -1:	;
			perror("Hull Breach!\n");
			exit(1);
			break;
		
		case 0:	;
			// Take ^C as default
			sa.sa_handler = SIG_DFL;
			sigaction(SIGINT, &sa, NULL);

			// Handle the input file
			if (strcmp(inputName, "")) {
				// Open file
				input = open(inputName, O_RDONLY);
				if (input == -1) {
					perror("Unable to open input file\n");
					exit(1);
				}
				// Assign file
				result = dup2(input, 0);
				if (result == -1) {
					perror("Unable to assign input file\n");
					exit(2);
				}
				// Set input file to close on execution
				fcntl(input, F_SETFD, FD_CLOEXEC);
			}
			// Handle the ouput file
			if (strcmp(outputName, "")) {
				// Open file
				output = open(outputName, O_WRONLY | O_CREAT | O_TRUNC, 0666);
				if (output == -1) {
					perror("Unable to open output file\n");
					exit(1);
				}
				// Assign file
				result = dup2(output, 1);
				if (result == -1) {
					perror("Unable to assign output file\n");
					exit(2);
				}
				// Set output file to close on execution
				fcntl(output, F_SETFD, FD_CLOEXEC);
			}
			
			// Execute the command
			if (execvp(arr[0], (char* const*)arr)) {
				printf("%s: no such file or directory\n", arr[0]);
				fflush(stdout);
				exit(2);
			}
			break;
		
		default:	;
			// Execute background process if background process is currently available
			if (*background && allowBackground) {
				pid_t actualPid = waitpid(spawnPid, childExitStatus, WNOHANG);
				printf("background pid is %d\n", spawnPid);
				fflush(stdout);
			}
			// Else, execute as normal
			else {
				pid_t actualPid = waitpid(spawnPid, childExitStatus, 0);
			}

		// Check for terminated processes in background	
		while ((spawnPid = waitpid(-1, childExitStatus, WNOHANG)) > 0) {
			printf("child %d terminated\n", spawnPid);
			printExitStatus(*childExitStatus);
			fflush(stdout);
		}
	}
}

/* catchSIGTSTP
 * Toggles allowBackground between 1 and 0. Determines whether a background process can be run.
 */
void catchSIGTSTP(int signo) {

	// 1 --> 0, display matching message to terminal.
	if (allowBackground == 1) {
		char* message = "Entering foreground-only mode (& is now ignored)\n";
		write(1, message, 49);
		fflush(stdout);
		allowBackground = 0;
	}

	// 0 --> 1, display matching message to terminal.
	else {
		char* message = "Exiting foreground-only mode\n";
		write (1, message, 29);
		fflush(stdout);
		allowBackground = 1;
	}
}

/* printExitStatus
 * Input is the child process's exit method.
 * Prints the exit status. Called as the process is terminating.
 */
void printExitStatus(int childExitMethod) {
	
	if (WIFEXITED(childExitMethod)) {
		// Exit via status. Print the status.
		printf("exit value %d\n", WEXITSTATUS(childExitMethod));
	} else {
		// Exit via signal. Print the signal.
		printf("terminated by signal %d\n", WTERMSIG(childExitMethod));
	}
}
