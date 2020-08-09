/*
This program parses and modifies the input
*/
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#define SIZE 20	// Assignment requirements has this value at 80
#define DEBUG 1	// [0 = DEBUG OFF],[1 = DEBUG ON] 
#define LOOPS 1000 // Logs are printed once per N loops. To disable, set LOOPS to a value >= 1000

int sign_runs = 0;
int sep_runs = 0;
int buffer [SIZE];
int count = 0;
int inp_idx = 0;
int out_idx = 0;
int sign_idx = 0;
int sep_idx = 0;

// Initialize the mutex
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// Initialize the condition variables
pthread_cond_t full = PTHREAD_COND_INITIALIZER;
pthread_cond_t sign_parsed = PTHREAD_COND_INITIALIZER;
pthread_cond_t sep_parsed = PTHREAD_COND_INITIALIZER;
pthread_cond_t empty = PTHREAD_COND_INITIALIZER;

int inp_parse(){
	int i, j;
	const char endcase [6] = {'\n', 'D', 'O', 'N', 'E', '\n'};
	char recent [6];
	for (i = 0; i < 6; i++){
		recent[i] = 0;
	}
	if(DEBUG) printf("	(INP_PARSE) - Starting inp_parse().\n");
	for (i = 0; i < 1000; i++){
		while (count == SIZE && i != 0){
			// Signal to the consumer that the buffer is no longer empty
			pthread_cond_signal(&full);
			// Unlock the mutex
			if(DEBUG) printf("	(INP_PARSE) - Mutex unlocked.\n");
			pthread_mutex_unlock(&mutex);
			if(DEBUG) printf("	(INP_PARSE) - Buffer is full. Waiting for output() to print.\n");
			// Buffer is full. Wait for the consumer to signal that the buffer has space
			pthread_cond_wait(&empty, &mutex);
			
			//Relock mutex once it is empty again
			pthread_mutex_lock(&mutex);
		}
		//Scan a char straight into the buffer
		buffer[inp_idx] = getchar();
		if(DEBUG && ((i % LOOPS) == 0)) printf("	(INP_PARSE) - Loop [%d], char [%c] saved to buffer.\n", i, buffer[inp_idx]);
		
		//if(DEBUG && ((i % LOOPS) == 0)) printf("	(INP_PARSE) - Checking for endcase, text = \"DONE\"?\n");
		//Check recent values to see if \nDONE\n was entered
		for (j = 0; j < 5; j++){ //Shift values over
			recent[j] = recent[j + 1];
		}
		//Insert new values at last index
		recent[5] = buffer[inp_idx];
		for (j = 0; j < 6; j++){ //Compare values to endcase array
			if (recent[j] != endcase[j]) break;
			else {
				if (j == 5){
					//Endcase found. Wipe 'DONE' from buffer, and return to parent
					inp_idx = (inp_idx + SIZE - 5) % SIZE;
					count -= 5;
					buffer[inp_idx] = '\0';
					if(DEBUG) printf("	(INP_PARSE) - Endcase was found on loop # [%d]. Last value in buffer was [%c]\n", i, buffer[(inp_idx + SIZE - 1) % SIZE]);
					
					// Signal to the consumer that the buffer is no longer empty
					if(DEBUG) printf("	(INP_PARSE) - cond_signal sent to output().\n");
					pthread_cond_signal(&full);
					// Unlock the mutex
					if(DEBUG) printf("	(INP_PARSE) - Mutex unlocked.\n");
					pthread_mutex_unlock(&mutex);
	
					return 0;
				}
			}
		}
		//if(DEBUG && ((i % LOOPS) == 0)) printf("	(INP_PARSE) - No endcase found, continuing loop.\n");
		//End this loop by updating values
		inp_idx = (inp_idx + 1) % SIZE;
		count++;
	}
	if(DEBUG) printf("	(INP_PARSE) - Loop terminated after 1000 loops.\n");
	return 0;
}

//Input thread
void *input(void *args){
	do {
		if(DEBUG) printf("	(INPUT) - Starting input().\n");
		//inputs text line-by-line from stdin
		if(DEBUG) printf("	(INPUT) - Parsing.\n");
		int inp_stts = inp_parse();
		if (inp_stts == 0){
			if(DEBUG) printf("	(INPUT) - - EXIT CASE.\n");
			return NULL;
		} else if (inp_stts == 1){
			if(DEBUG) printf("	(INPUT) - - NO EXIT CASE FOUND.\n");
		}
		sign_runs++;
		sep_runs++;
		//Run forever, exit case = break;
	} while (1);
}

//Output thread
void *output(void *args){
	if(DEBUG) printf("	(OUTPUT) - Starting output().\n");
	//outputs text to stdout
	do {
		// Lock the mutex before checking where there is space in the buffer
		pthread_mutex_lock(&mutex);
		while (count == 0){
			// Buffer is empty
			if(DEBUG) printf("	(OUTPUT) - Awaiting sep_parse().\n");
			pthread_cond_wait(&sep_parsed, &mutex);
		}
		
		if(DEBUG) printf("	(OUTPUT) - Outputting buffer to terminal.\n");
		while (count > 0){
			putchar(buffer[out_idx]);
			out_idx = (out_idx + 1) % SIZE;
			count--;
		}
		
		// Signal to the consumer that the buffer has space
		if(DEBUG) printf("\n	(OUTPUT) - Buffer is empty, awaiting input.\n");
		pthread_cond_signal(&empty);
		// Unlock the mutex
		if(DEBUG) printf("	(OUTPUT) - Mutex unlocked.\n");
		pthread_mutex_unlock(&mutex);
	} while (1);
	return NULL;
}

//Main functionality for sign
int sign_parse(){
	if(DEBUG) printf("	(SIGN_PARSE) - Running sign_parse().\n");
	//Check for pairs of '+'
	//Vars used in for loop
	int i, a, b;
	a = out_idx;
	if (out_idx > inp_idx - 1){
		b = inp_idx + SIZE - 1;
	} else b = inp_idx - 1;
	for (i = a; i < b; i++){
		if(DEBUG) printf("	(SIGN_PARSE) - Testing chars [%c] & [%c].\n",buffer[i % SIZE], buffer[(i + 1) % SIZE]);
		if (buffer[i % SIZE] == '\0'){
			//Exit case; DONE found by input()
			//Return 0 to tell parent to 'stop running'. DONE found.
			return 0;
		} else if (buffer[i % SIZE] == '+'
				&& buffer[(i + 1) % SIZE] == '+'){
			if(DEBUG) printf("	(SIGN_PARSE) - Matching '++' found.\n");
			//Matching pair found. Set first to * for debugging purposes. This gets overwritten by the second value.
			buffer[i % SIZE] = '*';
			buffer[(i + 1) % SIZE] = '+';
			//Perform an index shift, from curr index to end of buffer
			int j;
			for(j = i; j < b; j++){
				buffer[j % SIZE] = buffer[(j + 1) % SIZE];
			}
			inp_idx -= 1;
			b -= 1; 
			count -= 1;
		}
	}
	//Finished running. Return 0 for completed loop.
	return 1;
}

//Plus sign thread
void *sign(void *args){
	if(DEBUG) printf("	(SIGN) - Starting sign().\n");
	//int i;
	do {
		pthread_mutex_lock(&mutex);
		while (count == 0)
			// Buffer is empty
			pthread_cond_wait(&full, &mutex);
		//Run
		//if(DEBUG) printf("	(SIGN) - Parsing:		");
		int sign_stts = sign_parse();
		if (sign_stts == 0){
			if(DEBUG) printf("	(SIGN_PARSE) -  - EXIT CASE\n");
		} else if (sign_stts == 1){
			if(DEBUG) printf("	(SIGN_PARSE) -  - NO EXIT CASE FOUND\n");
		}
		//if(DEBUG) printf("DONE\n");
		// Signal to the consumer that the buffer has been sign parsed
		if(DEBUG) printf("	(SIGN) - cond_signal sent.\n");
		pthread_cond_signal(&sign_parsed);
		// Unlock the mutex
		if(DEBUG) printf("	(SIGN) - Mutex unlocked.\n");
		pthread_mutex_unlock(&mutex);
		sign_runs--;
	} while (sign_runs > 0);
	return NULL;
	//Run forever, exit case = break;
}

//Main functionality for separator
int sep_parse(){
	if(DEBUG) printf("	(SEP_PARSE) - Running sep_parse().\n");
	//Check for '\n' chars
	//Vars used in for loop
	int i, a, b;
	a = out_idx;
	if (out_idx > inp_idx - 1){
		b = inp_idx + SIZE - 1;
	} else b = inp_idx - 1;
	for (i = a; i < b; i++){
		if (buffer[i % SIZE] == '\0'){
			//Exit case; DONE found by input()
			//Return 0 to tell parent to 'stop running'. DONE found.
			return 0;
		} else if (buffer[i % SIZE] == '\n'){
			if(DEBUG) printf("	(SEP_PARSE) - '\\n' found. Replacing with ' '.\n");
			//Newline found
			buffer[i % SIZE] = ' ';
		}
	}
	//Finished running. Return 0
	return 1;
}

//Separator thread
void *separator(void *args){
	if(DEBUG) printf("	(SEPARATOR) - Starting separator().\n");
	//int i;
	do {
		pthread_mutex_lock(&mutex);
		while (count == 0)
			// Buffer hasn't been sign parsed
			pthread_cond_wait(&sign_parsed, &mutex);
		//Run
		if(DEBUG) printf("	(SEPARATOR) - Parsing.\n");
		int sep_stts = sep_parse();
		if (sep_stts == 0){
			if(DEBUG) printf("	(SEPARATOR) - - EXIT CASE\n");
		} else if (sep_stts == 1){
			if(DEBUG) printf("	(SEPARATOR) - - NO EXIT CASE FOUND\n");
		}
		//if(DEBUG) printf("DONE\n");
		// Signal to the consumer that the buffer has been sep parsed
		if(DEBUG) printf("	(SEPARATOR) - cond_signal sent.\n");
		pthread_cond_signal(&sep_parsed);
		// Unlock the mutex
		if(DEBUG) printf("	(SEPARATOR) - Mutex unlocked.\n");
		pthread_mutex_unlock(&mutex);
		sep_runs--;
	} while (sep_runs > 0);
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
	if(DEBUG) printf("	(MAIN) - Program starting.\n");
	exec();
	if(DEBUG) printf("	(MAIN) - Program terminating.\n");
	return 0;
}
