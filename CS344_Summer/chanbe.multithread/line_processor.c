/*
This program parses and modifies the input
*/
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#define SIZE 1000	// Assignment recommends size = 1000
#define OUT_LEN 80	// Assignment requires this to be 80

#define DEBUG_INP 0
#define DEBUG_SEP 0
#define DEBUG_SIGN 0
#define DEBUG_OUT 0

int DEBUG = 0;		// [0 = DEBUG OFF],[1 = DEBUG ON] 

char recent [6];	
int c = 0;			//Count for skipped chars between buf_1 and buf_2
int d = 0;			//Count for skipped chars between buf_2 and buf_3
int buf_1 [SIZE];
int buf_2 [SIZE];
int buf_3 [SIZE];
int count_1 = 0;			//Original c count
int count_2 = 0;		//c count after swapping '++'
int count_3 = 0;		//c count after swapping '\n', output()
int inp_idx = 0;
int sign_idx = 0;
int sep_idx = 0;
int out_idx = 0;
int outputting = 0;		//Global, since separator() has to interact with this

// Initialize the mutex
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// Initialize the condition variables
pthread_cond_t inp_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t sign_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t sep_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t out_cond = PTHREAD_COND_INITIALIZER;

/*
*	Detail: Input parser. Reads input from the terminal one character per loop.
*			Stores the most recent six characters read to find '\nDONE\n'. This signifies
*			the end condition, and triggers each thread to begin termination.
*	Params: None
*	Return: 0 on exit case, 1 on normal loop completion
*/
int inp_parse(){
	int i;
	// Initialize the recent array. Keeps track of DONE
	const char endcase [6] = {'\n', 'D', 'O', 'N', 'E', '\n'};
	//if(DEBUG && DEBUG_INP) printf("	(INP_PARSE) - Starting inp_parse().\n");
	if (count_1 == SIZE){
		//Buf_1 is full. End this iteration.
	}
	//Scan a char straight into the buffer
	buf_1[inp_idx] = getchar();
	if(DEBUG && DEBUG_INP) printf("	(INP_PARSE) - Index [%d] char [%c] saved to buf_1.\n", inp_idx, buf_1[inp_idx]);

	//if(DEBUG && DEBUG_SIGN) printf("	(INP_PARSE) - Checking for endcase, text = \"DONE\"?\n");
	for (i = 0; i < 5; i++){ //Shift values over
		recent[i] = recent[i + 1];
	}

	//Insert new values at last index
	recent[5] = buf_1[inp_idx];
	for (i = 0; i < 6; i++){ //Compare values to endcase array
		if (recent[i] != endcase[i]) break;
		else {
			if (i == 5){
				if(DEBUG && DEBUG_INP) printf("	(INP_PARSE) - Endcase was found on loop # [%d]. count_1: [%d], value in buf_1 is [%c]\n", i, count_1, buf_1[inp_idx]);
				//Endcase found. Wipe 'DONE' from buffer, and return to parent
				inp_idx = (inp_idx + SIZE - 5) % SIZE;
				//count_1 = (count_1 - 5);
				buf_1[inp_idx] = '\0';
				if(DEBUG && DEBUG_INP) printf("	(INP_PARSE) - count_1 changed: [%d]. Last value in buf_1 is [%c]\n", count_1, buf_1[(inp_idx + SIZE - 1) % SIZE]);
				//Program terminating.
				return 0;
			}
		}
	}
	
	return 1;
}

/*
*	Detail: Input thread
*	Params: None
*	Return: NULL on termination
*/
void *input(void *args){
	int i;
	int inp_running = 1;
	for (i = 0; i < 6; i++){
		recent[i] = 0;
	}
	if(DEBUG) printf("	(INPUT) - Starting input().\n");	
	do{
		while (count_1 == SIZE){
			// Buffer is full. Wait for output
			if(DEBUG && DEBUG_INP) printf("	(INP_PARSE) - Buf_1 is full. Waiting for sep_parse() to consume.\n");
			pthread_cond_wait(&inp_cond, &mutex);
		}
		//inputs text line-by-line from stdin
		//if(DEBUG && DEBUG_OUT) printf("	(INPUT) - Parsing.\n");
		int inp_stts = inp_parse();

		if (inp_stts == 0){
			if(DEBUG && DEBUG_INP) printf("	(INPUT) - - EXIT CASE [%d].\n", inp_stts);
			inp_running = 0;
		}
		//End this loop by updating values
		inp_idx = (inp_idx + 1) % SIZE;
		count_1++;
		
		// Signal to the consumer that the buffer is no longer empty
		pthread_cond_signal(&sep_cond);	//Buf_2
		// Unlock the mutex
		//if(DEBUG && DEBUG_OUT) printf("	(INP_PARSE) - Mutex unlocked.\n");
		//Run until exit case => DONE;
	} while (inp_running > 0);
	//This should output last value before '\0'.
	if(DEBUG) printf("	(INPUT) - Input() has terminated. Last value in buf_1 was [%c] at index [%d].\n", buf_1[(inp_idx + SIZE - 1) % SIZE], (inp_idx + SIZE - 1) % SIZE);
	return NULL;
}

/*
*	Detail: Output thread. Only outputs characters if there are at least 80 in the buffer.
*	Params: None
*	Return: NULL on termination
*/
//Output thread
void *output(void *args){
	int i = 0;
	//int cont_loops = 0;	//Continuous loops. Used to force quit the function if it enters an infinite loop (race condition)
	outputting = 1;
	if(DEBUG) printf("	(OUTPUT) - Starting output().\n");
	//outputs text to stdout. Don't need to lock mutex.
	do {
		while (count_3 < OUT_LEN){
			if(outputting == 0){
				//Exit case
				break;
			}
			pthread_cond_wait(&out_cond, &mutex);
		}
		//if(DEBUG && DEBUG_OUT) printf ("	(OUTPUT) - Beginning of loop. outputting = [%d].\n", outputting);
		if(DEBUG && DEBUG_OUT){
			if(count_3 >= OUT_LEN) printf("	(OUTPUT) - Outputting buf_3 to terminal. Outputting = [%d]\n", outputting);
			else printf("	(OUTPUT) - count_3 < [%d]. No output. Outputting = [%d]\n", OUT_LEN, outputting);
		}
		//cont_loops++;
		
		//Only output if there are more than 80 chars in the buffer
		while (count_3 >= OUT_LEN){
			//Output 80 chars from the buffer
			for (i = 0; i < OUT_LEN; i++){
				putchar(buf_3[out_idx]);
				out_idx = (out_idx + 1) % SIZE;
				count_3--;
			}
			//Print a new line
			printf("\n");
		}
		
		if (outputting == 0) break;
		
		//This goes after the while loop, in case separator() terminated while there was more than OUT_LEN chars in the buffer.
		pthread_cond_signal(&sign_cond);
	} while (outputting > 0);
	if(DEBUG) printf("	(OUTPUT) - Output() has terminated.\n");
	return NULL;
}


/*
*	Detail: Separator parser. Reads values in buf_1 to buf_2. Looks for '\n' newline
*			characters, and replaces them with ' ' spaces.
*	Params: None
*	Return: 0 on exit case, 1 on normal loop completion
*/
int sep_parse(){
	if(DEBUG && DEBUG_SEP) printf("	(SEP_PARSE) - count_1: [%d], Checking buf_1[%d] with value [%c].\n", count_1, ((sep_idx + d) % SIZE), buf_1[(sep_idx + d) % SIZE]);
	if (buf_1[(sep_idx + d) % SIZE] == '\0'){
		//Exit case; DONE found by input()
		//Return 0 to tell parent to 'stop running'. DONE found.
		buf_2[sep_idx] = buf_1 [(sep_idx + d) % SIZE];
		return 0;
	} else if (buf_1[(sep_idx + d) % SIZE] == '\n'){
		if(DEBUG && DEBUG_SEP) printf("	(SEP_PARSE) - '\\n' found. Replacing with ' '.\n");
		//Newline found
		buf_2[sep_idx] = ' ';
	} else {
		//Else, copy value from Buf_1 directly to Buf_2
		buf_2[sep_idx] = buf_1 [(sep_idx + d) % SIZE];
	}
	return 1;
}


/*
*	Detail: Separator thread. Calls sep_parse().
*	Params: None
*	Return: NULL on termination
*/
void *separator(void *args){
	if(DEBUG) printf("	(SEPARATOR) - Starting separator().\n");
	int sep_running = 1;
	do {
		//Lock mutex since this will affect buf_1 and buf_2
		pthread_mutex_lock(&mutex);
		while (count_1 == 0){	// Buffer hasn't been sign parsed || Buffer is empty
			if(DEBUG && DEBUG_SEP) printf("	(SEPARATOR) - count_2: [%d], buf_1 empty, waiting for input().\n", count_2);
			pthread_cond_wait(&sep_cond, &mutex);
		}
		
		//Run
		//if(DEBUG && DEBUG_SEP) printf("	(SEPARATOR) - Parsing.\n");
		int sep_stts = sep_parse();
		if (sep_stts == 0){
			if(DEBUG && DEBUG_SEP) printf("	(SEPARATOR) - - EXIT CASE\n");
			sep_running = 0;	//Terminates this thread
		}
		sep_idx = (sep_idx + 1) % SIZE;
		count_2++;
		count_1--;

		// Signal to the consumer that the buffer has been sep parsed
		//if(DEBUG && DEBUG_SEP) printf("	(SEPARATOR) - cond_signal sent - ");
		pthread_cond_signal(&inp_cond);		//Buf_2
		pthread_cond_signal(&sign_cond);	//Output
		// Unlock the mutex
		//if(DEBUG && DEBUG_SEP) printf("Mutex unlocked.\n");
		pthread_mutex_unlock(&mutex);
	} while (sep_running > 0);
	//This should output last value before '\0'.
	if(DEBUG) printf("	(SEPARATOR) - Separator() has terminated. Last value in buf_2 was [%c].\n", buf_2[(sep_idx + SIZE - 1) % SIZE]);
	return NULL;
	//Run forever, exit case = break;
}

/*
*	Detail: Sign parser. Reads values in buf_2 to buf_3. Looks for pairs of '+' signs. 
*			Converts the value in the first index to '^' and skips the second value.
*	Params: None
*	Return: 0 on exit case, 1 on normal loop completion
*/
int sign_parse(){
	//Check for pairs of '+'
	if(DEBUG && DEBUG_SIGN) printf("	(SIGN_PARSE) - count_3: [%d], Index: [%d], Testing chars [%c] & [%c].\n", count_3, sign_idx, buf_2[(sign_idx + c) % SIZE], buf_2[(sign_idx + c + 1) % SIZE]);
	if (buf_2[(sign_idx + c) % SIZE] == '\0'){
		if(DEBUG && DEBUG_SIGN) printf("	(SIGN_PARSE) -  - '\\0' found. Exiting.\n");
		//Exit case; DONE found by input(). Assign null terminator in buffer 3
		//Return 0 to tell parent to 'stop running'. DONE found.
		buf_3[sign_idx] = buf_1 [(sign_idx + c) % SIZE];
		return 0;
	} else if (((sign_idx + c + 1) <= inp_idx)		//Check if next cell is within bounds
			&& (buf_2[(sign_idx + c) % SIZE] == '+')
			&& (buf_2[(sign_idx + c + 1) % SIZE] == '+')){
		if(DEBUG && DEBUG_SIGN) printf("	(SIGN_PARSE) - - Matching '++' found.\n");
		//Matching pair found in Buf_2. Set value in Buf_3
		buf_3[sign_idx] = '^';
		c++;
		count_2--;
	} else {
		//if(DEBUG && DEBUG_SIGN) printf("	(SIGN_PARSE) -  - Values copied normally.\n");
		//Else, copy value from Buf_1 directly to Buf_2
		buf_3[sign_idx] = buf_2 [(sign_idx + c) % SIZE];
	}
	return 1;
}

/*
*	Detail: Sign thread. Calls sign_parse().
*	Params: None
*	Return: NULL on termination
*/
void *sign(void *args){
	if(DEBUG) printf("	(SIGN) - Starting sign().\n");
	int sign_running = 1;
	do {
		//Lock mutex since this will affect buf_2 and buf_3
		pthread_mutex_lock(&mutex);
		while (count_2 == 0){	// Buffer is empty
			if(DEBUG && DEBUG_SEP) printf("	(SIGN) - count_3: [%d], buf_2 empty, waiting for input().\n", count_3);
			pthread_cond_wait(&sign_cond, &mutex);
		}
		//Run
		int sign_stts = sign_parse();
		if (sign_stts == 0){
			if(DEBUG && DEBUG_SIGN) printf("	(SIGN_PARSE) -  - EXIT CASE\n");
			sign_running = 0;
			outputting = 0;
		}
		sign_idx = (sign_idx + 1) % SIZE;
		count_3++;
		count_2--;
		
		// Signal to the consumer that the buffer has been sign parsed
		//if(DEBUG && DEBUG_SIGN) printf("	(SIGN) - cond_signal sent - ");
		pthread_cond_signal(&sep_cond);	//Buf_2
		pthread_cond_signal(&out_cond);	//Output
		// Unlock the mutex
		//if(DEBUG && DEBUG_SIGN) printf("Mutex unlocked.\n");
		pthread_mutex_unlock(&mutex);
	} while (sign_running > 0);
	//This should output last value before '\0'.
	if(DEBUG) printf("	(SIGN) - Sign() has terminated. Last value in buf_3 was [%c].\n", buf_3[(sign_idx + SIZE - 1) % SIZE]);
	outputting = 0;	
	return NULL;
	//Run forever, exit case = break;
}

/*
*	Detail: Creates and joins all threads.
*	Params: None
*	Return: void
*/
void exec(){
	//Do the fork part here. Call relevant program.
	//Create input thread
	pthread_t inp_t, sep_t, sign_t, out_t;
	
	if(DEBUG) printf("	(EXEC) - Creating threads.\n");
	//Create input thread
	pthread_create(&inp_t, NULL, input, NULL);
	//Create parsing threads
	pthread_create(&sign_t, NULL, sign, NULL);
	pthread_create(&sep_t, NULL, separator, NULL);
	//Create output thread
	pthread_create(&out_t, NULL, output, NULL);
	
	if(DEBUG) printf("	(EXEC) - Waiting for threads to complete.\n");
	//Wait for threads to complete
	pthread_join(inp_t, NULL);
	pthread_join(sign_t, NULL);
	pthread_join(sep_t, NULL);
	pthread_join(out_t, NULL);
}

/*
*	Detail: Main function. Sets value of the DEBUG flag, and runs exec().
*	Params:	argv[1]
*	Return: 0 on success
*/
int main (int argc, char* argv[]){
	//Run exec to do all the thread stuff
	if (argc == 2) DEBUG = atoi(argv[1]);
	if(DEBUG) printf("	(MAIN) - Program starting.\n");
	exec();
	if(DEBUG) printf("	(MAIN) - Program terminating.\n");
	return 0;
}
