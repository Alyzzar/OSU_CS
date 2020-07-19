/**
	Assigned:	CS 344 Operating Systems, Summer 2020
	Coded: 		Ben Chan
	Shell requirements:
				3 built in commands [exit, cd, status]
				Run other commands via execvp() function
				Use of parent/child processes
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
void initializeSmallsh (struct shell*);
void runSmallsh(struct shell*, struct sigaction, struct sigaction);
void getInput(struct shell*);
void catchSIGTSTP(int);
void execCMD(struct shell*, struct sigaction);
void printExitStatus(int);

/**
Function:	initializeSmallsh()
Details:	Initializes values in the type 'struct shell'
Params:		Pointer to shell struct
Return:		N/A
**/
void initializeSmallsh(struct shell* smallsh){
	int i;
	smallsh->bg_status = 0;
	smallsh->exit_status = 0;
	smallsh->pid = getpid();	//PID of the parent process
	//Fill strings with NULL characters to avoid memory issues
	for (i = 0; i < 512; i++){
		if (i < 256){
			smallsh->f_in[i] = '\0';
			smallsh->f_out[i] = '\0';
		}
		smallsh->input[i] = NULL;	
	}
}

/**
Function:	runSmallsh()
Details:	Runs the shell. This is the highest level function in this code. Calls getInput()
			runs built-in commands, and makes call to execCMD() for other functionalities.
Params:		Pointer to shell struct, and both sigaction structs (though at the moment, only sigint is used).
Return:		N/A
**/
void runSmallsh(struct shell* smallsh, struct sigaction sigint, struct sigaction sigtstp){
	//Variables
	int i;
	int running = 1;

	//Get user input
	while (running){
		//Set input array to null at beginning of each loop before receiving new input
		for (i = 0; i < 512; i++){
			smallsh->input[i] = NULL;
		}
		smallsh->bg_status = 0;
		smallsh->f_in[0] = '\0';
		smallsh->f_out[0] = '\0';
		
		//Get user input
		getInput(smallsh);
		
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
			printExitStatus(smallsh->exit_status);
		} else if (strcmp(smallsh->input[0], "exit") == 0) {
		//exit
			running = 0;
		} else {
		//other commands
			execCMD(smallsh, sigint);
		}
	}
}

/**
Function:	execCMD()
Details:	Executes non-built-in commands. First forks off process. If successful, will
			check validity of input/output files, and run the commands via execvp(). If unsuccessful,
			function will print appropriate errors to the terminal and/or manage background processes.
Params:		Pointer to shell struct, sigaction struct
Return:		N/A
**/
void execCMD (struct shell* smallsh, struct sigaction sa){
	int in_status, out_status, assign_status;
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
		sa.sa_handler = SIG_DFL;
		sigaction(SIGINT, &sa, NULL);
		// Input file
		if (strcmp(smallsh->f_in, "") != 0) {
			// Open file
			in_status = open(smallsh->f_in, O_RDONLY);
			if (in_status == -1) {
				perror("Could not open input file\n");
				exit(1);
			}
			// Assign file
			assign_status = dup2(in_status, 0);
			if (assign_status == -1) {
				perror("Could not assign input file\n");
				exit(2);
			}
			// Close input file on exec
			fcntl(in_status, F_SETFD, FD_CLOEXEC);
		}
		// Output file
		if (strcmp(smallsh->f_out, "") != 0) {
			// Open file
			out_status = open(smallsh->f_out, O_WRONLY | O_CREAT | O_TRUNC, 0666);
			if (out_status == -1) {
				perror("Could not open output file\n");
				exit(1);
			}
			// Assign file
			assign_status = dup2(out_status, 1);
			if (assign_status == -1) {
				perror("Could not assign output file\n");
				exit(2);
			}
			// Close output file on exec
			fcntl(out_status, F_SETFD, FD_CLOEXEC);
		}
		// Execute the command
		if (execvp(smallsh->input[0], smallsh->input)) {
			// If cmd couldn't be executed
			printf("%s could not be found. Command not executed\n", smallsh->input[0]);
			fflush(stdout);
			exit(2);
		}	
	} else {
		//fork() returned a positive value. Returned to parent process. cmdPID = new child's PID
		//Execute background process, if there is an available slot for bg process to run.
		if (smallsh->bg_status && bg_allowed){
			pid_t truePID = waitpid(cmdPID, &smallsh->exit_status, WNOHANG);
			printf("Background process's PID is %d\n", cmdPID);
			fflush(stdout);
		} else {
			pid_t truePID = waitpid(cmdPID, &smallsh->exit_status, 0);
		}
		while ((cmdPID = waitpid(-1, &smallsh->exit_status, WNOHANG)) > 0) {
			printf("Child process with PID %d was terminated.\n", cmdPID);
			printExitStatus(smallsh->exit_status);
			fflush(stdout);
		}
	}
}

/**
Function:	getInput()
Details:	Parses users input. This function parses the input to terminal, and passes it to runSmallsh().
			Input parsing consists of removing '/n' characters, and replacing '$$' with the pre-generated PID.
			This function also saves input/output file names to the shell struct (Marked by < or > characters)
Params:		Pointer to struct
Return:		N/A
**/
void getInput (struct shell* smallsh) {
	//Note, because smallsh is passed in as a pointer, no return values are needed.
	int MaxLength = 2048;
	char fullInput [MaxLength];
	int i, j, empty;
	
	/** 
	//Turns out this caused more issues than it solved. Left it here since it was good for debugging.
	//Fill fullInput with empty '\0' characters (Avoid garbage from previous runs)
	for(i = 0; i < MaxLength; i++){
		fullInput[i] = '\0';
	}
	**/
	
	//Make sure input is not empty, to avoid a seg fault
	empty = 1;
	while (empty) {
		//As per assignment instructions, input lines are marked by ": "
		printf(": ");
		fflush(stdout);
		//Gets the input
		fgets(fullInput, 2048, stdin);	
		//Check that input is not empty (If the first index of the array is not NULL, it is not empty)
		if (strcmp(fullInput, "") == 0){
			//strcmp returns 0 if equal --> means array is empty.
			printf("Input empty. For help, type 'help'.\n");
		} else empty = 0;
	}
	//Since input is not empty, convert "\n" to "\0" to signify seperate commands
	for (i = 0; i < MaxLength; i++){
		if (fullInput[i] == '\n') fullInput[i] = '\0';
	}
	//Use string token to further parse input
	const char space[2] = " ";
	char *token = strtok(fullInput, space);
	
	for (i = 0; token; i++) {
		if (!strcmp(token, "<")) {
			// < Input file
			token = strtok(NULL, space);
			strcpy(smallsh->f_in, token);
			//printf("f_in == %s\n", smallsh->f_in);	//Print f_in
		} else if (!strcmp(token, ">")) {
			// > Output file
			token = strtok(NULL, space);
			strcpy(smallsh->f_out, token);
			//printf("f_out == %s\n", smallsh->f_out);	//Print f_out
		} else if (!strcmp(token, "&")) {
			// & Background process
			smallsh->bg_status = 1;
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

/**
Function:	catchSIGTSTP()
Details:	Catches SIGTSTP signal, and toggles the bg_allowed variable.
Params:		Pointer to struct
Return:		N/A
**/
void catchSIGTSTP (int sig_o) {
	char* message;
	//Toggle bg_allowed between 0 and 1
	//bg_allowed = (bg_allowed * -1) + 1;
	//Toggles value in one line. Not using this, since if() statement is easier to read
	if (bg_allowed == 0){
		bg_allowed = 1;
		message = "\nExiting foreground-only mode.\n";
		write (1, message, 30);
		fflush(stdout);
	} else { //bg_allowed == 1
		bg_allowed = 0;
		message = "\nEntering foreground-only mode. (& is now ignored)\n";
		write(1, message, 50);
		fflush(stdout);
	}
	/**
	As an unrelated note, originally I planned on including bg_allowed inside of the shell struct, 
	however I had difficulty passing a pointer to the shell struct in as a paramter, since this 
	function is only called by the line 'sigtstp.sa_handler = catchSIGTSTP;' in main() below.
	**/
}

/**
Function:	printExitStatus()
Details:	Prints a generic exit status to the console, using exit_status saved in the shell struct.
Params:		int exit_method
Return:		N/A
**/
void printExitStatus(int exit_method) {
	//Prints exit method from child process.
	if (WIFEXITED(exit_method)) {
		//Used this one if child terminated normally
		printf("exit value %d.\n", WEXITSTATUS(exit_method));
	} else {
		//Use this one if child was terminated by a signal or other cause
		printf("terminated by signal %d.\n", WTERMSIG(exit_method));
	}
}

/**
Function:	main()
Details:	Main function. Creates SIGINT and SIGTSTP trackers, intializes smallsh shell, and calls
			runShell(). SIGINT and SIGTSTP were being tracked in runShell(), but were moved here.
Params:		N/A
Return:		0 (On succesful exit)
**/
int main(void){
	//Create and initialize the shell struct
	struct shell smallsh;
	initializeSmallsh(&smallsh);
	
	//Mostly from class notes. This tracks CTRL+C and CTRL+Z sigint and sigtstp.
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
	
	//Runs the shell
	runSmallsh(&smallsh, sigint, sigtstp);
	
	//If all goes well, return 0.
	return 0;
}
