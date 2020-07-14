/**
	Shell requirements
		3 built in commands [exit, cd, status]
		Ignore comments [Lines beginning with # characters]
		CTRL-C = SIGINT
		CTRL-Z = SIGTSTP
**/

#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

struct shell{
	int bg_status;
	int exit_status;
	int pid;
	char f_in [256];
	char f_out [256];
	char* input [512];
};

int bg_allowed = 1;		//Used to catch SIGTSTP
void initializeSmallsh (struct shell);
void runSmallsh();
char* getInput(struct shell);
void catchSIGTSTP(int);
void execCMD(struct shell*, int*, struct sigaction);

void intializeSmallsh(struct shell* smallsh){
	smallsh->bg_status = 0;
	smallsh->exit_status = 0;
	smallsh->pid = getPID();
	smallsh->f_in = "";
	smallsh->f_out = "";
	smallsh->input = "";
}

void runSmallsh(struct shell* smallsh){
	//Variables
	int i;
	int running = 1;

	//Get user input
	
	//CTRL+C
	struct sigaction sigint = {0};
	sigint.sa_handler = SIG_IGN;
	sigfillset(&sigint.sa_mask);
	sigint.sa_flags = 0;
	sigaction(SIGINT, &sigint, NULL);

	//CTRL+Z
	struct sigaction sigtstp = {0};
	sigtstp.sa_handler = catchSIGTSTP;
	sigfillset(&sigtstp.sa_mask);
	sigtstp.sa_flags = 0;
	sigaction(SIGTSTP, &sigtstp, NULL);
	
	do{
		//Set input array to null at beginning of each loop before receiving new input
		for (i = 0, i < 512; i++){
			smallsh->input[i] = NULL;
		}
		//Get user input
		smallsh->input = getInput(smallsh);
		
		//Skip comments
		if(smallsh->input[0] == "#"){
			continue;
		}
		if(strcmp(smallsh->input[0], "cd") == 0){
		//cd
			if (smallsh->input[1]){
				//cd to specified directory
				if (chdir(smallsh->input[1]) == -1){
					printf ("Directory \"%s\" could not be found. Directory has not been changed.\n", smallsh->input[1]);
				}
				// else, cd was successful
			} else {
				//cd to home directory
				chdir(getenv("HOME"));
			}
		} else if (strcmp(smallsh->input[0], "status") == 0) {
		//status
			printExitStatus(exitStatus);
		} else if (strcmp(smallsh->input[0], "exit") == 0) {
		//exit
			running = 0;
		} else {
		//other commands
			execCMD(&smallsh, &exitStatus, sigint);
		}
		
	} while (running);
	
}

void execCMD (struct shell* smallsh, int* exitStatus, struct sigaction sa){
	int input, output, result;
	pid_t cmdPID = -5;
	
	//Fork the child process
	cmdPID = fork();
	
	//Execute the command
	if (cmdPID < 0){
		//fork() returned a negative value, failed to create process
		perror ("Failed to create child process.\n");
		exit(1);
	} else if (cmdPID == 0){
		//fork() returned 0, returned to the newly created process
		// Deal with CTRL+C
		sa.handler = SIG_DFL;
		sigaction(SIGINT, &sa, NULL);
		// Input file
		if (strcmp(smallsh->f_in, "") != 0) {
			// Open file
			input = open(smallsh->f_in, O_RDONLY);
			if (input == -1) {
				perror("Could not open input file\n");
				exit(1);
			}
			/*// Assign file
			result = dup2(input, 0);
			if (result == -1) {
				perror("Could not assign input file\n");
				exit(2);
			}*/
			// Close input file on exec
			fcntl(input, F_SETFD, FD_CLOEXEC);
		}
		// Output file
		if (strcmp(smallsh->f_out, "") != 0) {
			// Open file
			output = open(smallsh->f_out, O_WRONLY | O_CREAT | O_TRUNC, 0666);
			if (output == -1) {
				perror("Could not open output file\n");
				exit(1);
			}
			/*// Assign file
			result = dup2(output, 1);
			if (result == -1) {
				perror("Could not assign output file\n");
				exit(2);
			}*/
			// Close output file on exec
			fcntl(output, F_SETFD, FD_CLOEXEC);
		}
		
		// Execute the command
			if (execvp(smallsh->input[0], smallsh->input)) {
				// If cmd couldn't be executed
				printf("%s could not be found. Command not executed\n", arr[0]);
				fflush(stdout);
				exit(2);
			}
	} else {
		//fork() returned a positive value. Returned to parent process. cmdPID = new child's PID
		//Execute background process, if there is an available slot for bg process to run.
		if (smallsh->bg_status && bg_allowed){
			pid_t truePID = waitpid(cmdPID, exitStatus, WNOHANG);
			printf("Background process's PID is %d\n", cmdPID);
			fflush(stdout);
		} else {
			pid_t truePID = waitpid(cmdPID, exitStatus, 0);
		}
		
		while ((cmdPID = waitpid(-1, exitStatus, WNOHANG)) > 0) {
			printf("Child process with PID %d was terminated.\n", spawnPid);
			printExitStatus(*exitStatus);
			fflush(stdout);
		}
		
	}
	
	
}

void getInput (struct shell* smallsh) {
	int MaxLength = 2048;
	char fullInput [MaxLength];
	int i, j;
	
	//Fill fullInput with NULL characters
	for(i = 0; i < MaxLength, i++){
		fullInput[i] = NULL;
	}
	//As per assignment instructions, input lines are marked by ": "
	printf(": ");
	fflush(stdout);
	
	//Gets the input
	fgets(fullInput, 2048, stdin);
	
	//Check that input is not empty
	if (fullInput[i] = NULL){
		//Input empty, return from function
		smallsh->input[0] = strdup("");
		return;
	}
	//Since input is not empty, convert "\n" to "\0" to signify seperate commands
	for (i = 0; (i < MaxLength && fullInput[i] != NULL), i++){
		if (fullInput[i] == "\n") fullInput [i] = "\0";
	}
	//Use string token to further parse input
	const char space[2] = " ";
	char *token = strtok(input, space);
	
	for (i = 0; token; i++) {
		if (!strcmp(token, "<")) {
			// < Input file
			token = strtok(NULL, space);
			strcpy(inputName, token);
		} else if (!strcmp(token, ">")) {
			// > Output file
			token = strtok(NULL, space);
			strcpy(outputName, token);
		} else if (!strcmp(token, "&")) {
			// & Background process
			smallsh->background = 1;
		} else {
			smallsh->input[i] = strdup(token);
			// Replace $$ with pid
			for (j=0; smallsh->input[i][j]; j++) {
				if (smallsh->input[i][j] == '$'
				 && smallsh->input[i][j+1] == '$') {
					smallsh->input[i][j] = '\0';
					snprintf(smallsh->input[i], 256, "%s%d", smallsh->input[i], smallsh->pid);
				}
			}
		}
		//Set token to NULL in prepartion of next loop
		token = strtok(NULL, space);
	}
}

void catchSIGTSTP (int sig_o) {
	char* message;
	//Toggle bg_allowed between 0 and 1
	bg_allowed = ((bg_allowed - 1) * -1);
	if (bg_allowed == 1){
		message = "Exiting foreground-only mode.\n";
		write (1, message, 29);
		fflush(stdout);
	} else { //bg_allowed == 0
		message = "Entering foreground-only mode. (& is now ignored)\n";
		write(1, message, 49);
		fflush(stdout);
	}
}

void printExitStatus(int exit_method) {
	//Prints exit method from child process.
	if (WIFEXITED(exit_method)) {
		printf("exit value %d.\n", WEXITSTATUS(exit_method));
	} else {
		printf("terminated by signal %d.\n", WTERMSIG(exit_method));
	}
}

int main(void){
	struct shell smallsh;
	initializeShell(smallsh);
	
	runShell(&smallsh);
	
	return 0;
}
