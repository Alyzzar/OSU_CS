/*
This program parses and modifies the input
*/
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#define SIZE 10000	// Assignment recommends size = 10000
#define OUT_LEN 80	// Assignment requires this to be 80

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

int inp_parse(){
	int i;
	// Initialize the recent array. Keeps track of DONE
	const char endcase [6] = {'\n', 'D', 'O', 'N', 'E', '\n'};
	if(DEBUG) printf("	(INP_PARSE) - Starting inp_parse().\n");
	if (count_1 == SIZE){
		//Buf_1 is full. End this iteration.
	}
	//Scan a char straight into the buffer
	buf_1[inp_idx] = getchar();
	if(DEBUG) printf("	(INP_PARSE) - Indx [%d] char [%c] saved to buf_1.\n", inp_idx, buf_1[inp_idx]);

	//if(DEBUG) printf("	(INP_PARSE) - Checking for endcase, text = \"DONE\"?\n");
	for (i = 0; i < 5; i++){ //Shift values over
		recent[i] = recent[i + 1];
	}

	//Insert new values at last index
	recent[5] = buf_1[inp_idx];
	for (i = 0; i < 6; i++){ //Compare values to endcase array
		if (recent[i] != endcase[i]) break;
		else {
			if (i == 5){
				//Endcase found. Wipe 'DONE' from buffer, and return to parent
				inp_idx = (inp_idx + SIZE - 5) % SIZE;
				count_1 -= 4;
				buf_1[inp_idx] = '\0';
				if(DEBUG) printf("	(INP_PARSE) - Endcase was found on loop # [%d]. Last value in buf_1 was [%c]\n", i, buf_1[(inp_idx + SIZE - 1) % SIZE]);
				//Program terminating.
				return 0;
			}
		}
	}
	//End this loop by updating values
	inp_idx = (inp_idx + 1) % SIZE;
	count_1++;
	
	return -1;
}

//Input thread
void *input(void *args){
	int i;
	int inp_running = 1;
	for (i = 0; i < 6; i++){
		recent[i] = 0;
	}
	do{
		if(DEBUG) printf("	(INPUT) - Starting input().\n");

		while (count_1 == SIZE){
			// Buffer is full. Wait for output
			if(DEBUG) printf("	(INP_PARSE) - Buf_1 is full. Waiting for sign_parse() to consume.\n");
			pthread_cond_wait(&inp_cond, &mutex);
		}
		//inputs text line-by-line from stdin
		if(DEBUG) printf("	(INPUT) - Parsing.\n");
		int inp_stts = inp_parse();

		if (inp_stts == 0){
			if(DEBUG) printf("	(INPUT) - - EXIT CASE [%d].\n", inp_stts);
			inp_running = 0;
		} else if (inp_stts == 1){
			if(DEBUG) printf("	(INPUT) - - NO EXIT CASE FOUND. Buf_1 was filled\n");
		}
		// Signal to the consumer that the buffer is no longer empty
		pthread_cond_signal(&sign_cond);	//Buf_2
		// Unlock the mutex
		//if(DEBUG) printf("	(INP_PARSE) - Mutex unlocked.\n");
		//Run until exit case => DONE;
	} while (inp_running > 0);
	if(DEBUG) printf("	(INPUT) - Input() has terminated.\n");
	return NULL;
}

//Output thread
void *output(void *args){
	int i = 0;
	outputting = 1;
	if(DEBUG) printf("	(OUTPUT) - Starting output().\n");
	//outputs text to stdout. Don't need to lock mutex.
	do {
		if(DEBUG) printf ("	(OUTPUT) - Beginning of loop. outputting = [%d].\n", outputting);
		
		while (count_3 < OUT_LEN){
			// Buffer is empty
			if(outputting == 0){
				if(DEBUG) printf("	(OUTPUT) - Break: outputting = [0].\n");
				break;
			}
			if(DEBUG) printf("	(OUTPUT) - Awaiting sep_parse(). outputting = [%d]\n", outputting);
			pthread_cond_wait(&out_cond, &mutex);
		}
		
		if(DEBUG) printf("	(OUTPUT) - Outputting buf_3 to terminal.\n");
		
		while (count_3 >= OUT_LEN){
			for (i = 0; i < OUT_LEN; i++){
				putchar(buf_3[out_idx]);
				out_idx = (out_idx + 1) % SIZE;
				count_3--;
			}
			printf("\n");
		}
		
		//This goes after the while loop, in case separator() terminated while there was more than OUT_LEN chars in the buffer.
		if (outputting == 0){
			break;
		}
		// Signal to the consumer that the buffer has space
		pthread_cond_signal(&sep_cond);
	} while (outputting > 0);
	if(DEBUG) printf("	(OUTPUT) - Output() has terminated.\n");
	outputting = -1;
	return NULL;
}

//Main functionality for sign
int sign_parse(){
	//Check for pairs of '+'
	if(DEBUG) printf("	(SIGN_PARSE) - Testing chars [%c] & [%c].\n", buf_1[sign_idx], buf_1[(sign_idx + 1) % SIZE]);
	if (buf_1[(sign_idx + c) % SIZE] == '\0'){
		//Exit case; DONE found by input()
		//Return 0 to tell parent to 'stop running'. DONE found.
		buf_2[sign_idx] = buf_1 [(sign_idx + c) % SIZE];
		return 0;
	} else if (((sign_idx + c + 1) <= inp_idx)		//Check if next cell is within bounds
			&& buf_1[(sign_idx + c) % SIZE] == '+'
			&& buf_1[(sign_idx + c + 1) % SIZE] == '+'){
		if(DEBUG) printf("	(SIGN_PARSE) - Matching '++' found.\n");
		//Matching pair found in Buf_1. Set value in Buf_2
		buf_2[sign_idx] = '^';
		c++;
	} else {
		//Else, copy value from Buf_1 directly to Buf_2
		buf_2[sign_idx] = buf_1 [(sign_idx + c) % SIZE];
	}
	return 1;
}

//Plus sign thread (Buf_2)
void *sign(void *args){
	if(DEBUG) printf("	(SIGN) - Starting sign().\n");
	int sign_running = 1;
	do {
		//Lock mutex since this will affect buf_1 and buf_2
		pthread_mutex_lock(&mutex);
		while (count_1 == 0)	// Buffer is empty
			pthread_cond_wait(&sign_cond, &mutex);
		
		//Run
		int sign_stts = sign_parse();
		if (sign_stts == 0){
			if(DEBUG) printf("	(SIGN_PARSE) -  - EXIT CASE\n");
			sign_running = 0;
		} else if (sign_stts == 1){
			//if(DEBUG) printf("	(SIGN_PARSE) -  - NO EXIT CASE FOUND\n");
			sign_idx = (sign_idx + 1) % SIZE;
		}
		count_2++;
		count_1--;
		
		// Signal to the consumer that the buffer has been sign parsed
		if(DEBUG) printf("	(SIGN) - cond_signal sent - ");
		pthread_cond_signal(&inp_cond);	//Buf_1
		pthread_cond_signal(&sep_cond);	//Buf_3
		// Unlock the mutex
		if(DEBUG) printf("Mutex unlocked.\n");
		pthread_mutex_unlock(&mutex);
	} while (sign_running > 0);
	if(DEBUG) printf("	(SIGN) - Sign() has terminated. Last value in buf_2 was [%c].\n", buf_2[(sign_idx + SIZE - 1) % SIZE]);
	return NULL;
	//Run forever, exit case = break;
}

//Main functionality for separator
int sep_parse(){
	if (buf_2[(sep_idx + d) % SIZE] == '\0'){
		//Exit case; DONE found by input()
		//Return 0 to tell parent to 'stop running'. DONE found.
		buf_3[sep_idx] = buf_2 [(sep_idx + d) % SIZE];
		return 0;
	} else if (buf_2[(sep_idx + d) % SIZE] == '\n'){
		if(DEBUG) printf("	(SEP_PARSE) - '\\n' found. Replacing with ' '.\n");
		//Newline found
		buf_3[sep_idx] = ' ';
	} else {
		//Else, copy value from Buf_2 directly to Buf_3
		buf_3[sep_idx] = buf_2 [(sep_idx + d) % SIZE];
	}
	return 1;
}

//Separator thread (Buf_3)
void *separator(void *args){
	if(DEBUG) printf("	(SEPARATOR) - Starting separator().\n");
	int sep_running = 5;
	do {
		//Lock mutex since this will affect buf_2 and buf_3
		pthread_mutex_lock(&mutex);
		if(outputting < 0){
			if(DEBUG) printf("	(SEPARATOR) - Output() has terminated. Forcing early termination.\n");
			sep_running = 0;
			break;
		}
		while (count_2 == 0)	// Buffer hasn't been sign parsed || Buffer is empty
			pthread_cond_wait(&sep_cond, &mutex);
		
		//Run
		if(DEBUG) printf("	(SEPARATOR) - Parsing.\n");
		int sep_stts = sep_parse();
		if (sep_stts == 0){
			if(DEBUG) printf("	(SEPARATOR) - - EXIT CASE\n");
			//Let it run a few extra times to let output() terminate on it's own.
			sep_running--;
			outputting = 0;
			//pthread_kill(out_t, SIGUSR1);
		} else if (sep_stts == 1){
			//if(DEBUG) printf("	(SEPARATOR) - - NO EXIT CASE FOUND\n");
			sep_idx = (sep_idx + 1) % SIZE;
			count_2--;
		}
		if (sep_running == 5) count_3++;
	
		// Signal to the consumer that the buffer has been sep parsed
		if(DEBUG) printf("	(SEPARATOR) - cond_signal sent - ");
		pthread_cond_signal(&sign_cond);//Buf_2
		pthread_cond_signal(&out_cond);	//Output
		// Unlock the mutex
		if(DEBUG) printf("Mutex unlocked.\n");
		pthread_mutex_unlock(&mutex);
	} while (sep_running > 0);
	if(DEBUG) printf("	(SEPARATOR) - Separator() has terminated. Last value in buf_2 was [%c].\n", buf_3[(sep_idx + SIZE - 1) % SIZE]);
	outputting = 0;
	return NULL;
	//Run forever, exit case = break;
}

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

int main (int argc, char* argv[]){
	//Run exec to do all the thread stuff
	if (argc == 2) DEBUG = atoi(argv[1]);
	if(DEBUG) printf("	(MAIN) - Program starting.\n");
	exec();
	if(DEBUG) printf("	(MAIN) - Program terminating.\n");
	return 0;
}
