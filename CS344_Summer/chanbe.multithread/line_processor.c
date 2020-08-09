/*
This program parses and modifies the input
*/
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#define SIZE 20
#define DEBUG 1	// [0 = DEBUG OFF],[1 = DEBUG ON] 
#define LOOPS 1000 // Logs are printed once per N loops. To disable, set LOOPS to a value >= 1000

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
		while (count == SIZE){
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
	if(DEBUG) printf("	(INPUT) - Starting input().\n");
	//inputs text line-by-line from stdin
	if(DEBUG) printf("	(INPUT) - Parsing.\n");
	inp_parse()

	if(DEBUG) printf("	(INPUT) - Parsed through to end case.\n");
	return NULL;
	//Run forever, exit case = break;
}

//Output thread
void *output(void *args){
	if(DEBUG) printf("	(OUTPUT) - Starting output().\n");
	//outputs text to stdout
	do{
		// Lock the mutex before checking where there is space in the buffer
		pthread_mutex_lock(&mutex);
		while (count < 5){
			// Buffer is empty
			if(DEBUG) printf("	(OUTPUT) - Buffer is empty. Awaiting inp_parse()\n");
			pthread_cond_wait(&full, &mutex);
		}
		/*
		if(DEBUG) printf("	(OUTPUT) - Awaiting sign_parse().\n");		
		pthread_cond_wait(&sign_parsed, &mutex);
		if(DEBUG) printf("	(OUTPUT) - Awaiting sep_parse().\n");
		pthread_cond_wait(&sep_parsed, &mutex);
		*/
		//output a char to stdout with putchar
		putchar(buffer[out_idx]);
		out_idx = (out_idx + 1) % SIZE;
		count--;
		
		// Signal to the consumer that the buffer has space
		pthread_cond_signal(&empty);
		// Unlock the mutex
		pthread_mutex_unlock(&mutex);
	} while (1);
	return NULL;
}

//Main functionality for sign
int sign_parse(){
	//Check for pairs of '+'
	//Vars used in for loop
	int i, a, b;
	//Determines how many values to shift by (scales with number of ++ replaced)
	int shift = 0;
	a = out_idx;
	if (out_idx > inp_idx){
		b = inp_idx + SIZE - 1;
	} else b = inp_idx - 1;
	for (i = a; i < b - 1; i++){
		if (buffer[i % SIZE] == '\0'){
			//Exit case; DONE found by input()
			//Return 0 to tell parent to 'stop running'. DONE found.
			return 0;
			break;
		} else if (buffer[(i + shift) % SIZE] == '+'
				&& buffer[(i + shift + 1) % SIZE] == '+'){
			//Matching pair found
			buffer[(i + shift) % SIZE] = '^';
			//Set this value to '*' for clarity purposes (Should be overwritten anyways)
			buffer[(i + shift + 1) % SIZE] = '*';
			//Perform an index shift
			shift++;
		}
		if (shift){
			//Perform shift after checking for ++, otherwise a shift could create a duplicate +.
			buffer[i % SIZE] = buffer[(i + shift) % SIZE];
			inp_idx -= shift;
			b -= shift; 
			count -= shift;
		}
	}
	//Finished running. Return 0 for completed loop.
	return 0;
}

//Plus sign thread
void *sign(void *args){
	if(DEBUG) printf("	(SIGN) - Starting sign().\n");
	//int i;
	//for(i = 0; i < count + 10; i++){
		pthread_mutex_lock(&mutex);
		while (count == 0)
			// Buffer is empty
			pthread_cond_wait(&full, &mutex);
		//Run
		if(DEBUG) printf("	(SIGN) - Parsing:		");
		if (sign_parse() == 0){
			if(DEBUG) printf("EXIT CASE\n");
			i = count + 10;
		}
		//if(DEBUG) printf("DONE\n");
		// Signal to the consumer that the buffer has been sign parsed
		if(DEBUG) printf("	(SIGN) - cond_signal sent.\n");
		pthread_cond_signal(&sign_parsed);
		// Unlock the mutex
		if(DEBUG) printf("	(SIGN) - Mutex unlocked.\n");
		pthread_mutex_unlock(&mutex);
	//}
	return NULL;
	//Run forever, exit case = break;
}

//Main functionality for separator
int sep_parse(){
	//Check for '\n' chars
	//Vars used in for loop
	int i, a, b;
	a = out_idx;
	if (out_idx > inp_idx){
		b = inp_idx + SIZE - 1;
	} else b = inp_idx - 1;
	for (i = a; i < b - 1; i++){
		if (buffer[i % SIZE] == '\0'){
			//Exit case; DONE found by input()
			//Return 0 to tell parent to 'stop running'. DONE found.
			return 0;
		} else if (buffer[i % SIZE] == '\n'){
			//Newline found
			buffer[i % SIZE] = ' ';
		}
	}
	//Finished running. Return 0
	return 0;
}

//Separator thread
void *separator(void *args){
	if(DEBUG) printf("	(SEPARATOR) - Starting separator().\n");
	//int i;
	//for(i = 0; i < count + 10; i++){
		pthread_mutex_lock(&mutex);
		while (count == 0)
			// Buffer is empty.
			pthread_cond_wait(&full, &mutex);
		//Run
		if(DEBUG) printf("	(SEPARATOR) - Parsing:		");
		if (sep_parse() == 0){
		if(DEBUG) printf("EXIT CASE\n");
			i = count + 10;
		}
		//if(DEBUG) printf("DONE\n");
		// Signal to the consumer that the buffer has been sep parsed
		if(DEBUG) printf("	(SEPARATOR) - cond_signal sent.\n");
		pthread_cond_signal(&sep_parsed);
		// Unlock the mutex
		if(DEBUG) printf("	(SEPARATOR) - Mutex unlocked.\n");
		pthread_mutex_unlock(&mutex);
	//}
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
	pthread_create(&sep_t, NULL, separator, NULL);
	pthread_create(&sign_t, NULL, sign, NULL);
	//Create output thread
	pthread_create(&out_t, NULL, output, NULL);
	
	if(DEBUG) printf("	(EXEC) - Waiting for threads to complete.\n");
	//Wait for threads to complete
	pthread_join(inp_t, NULL);
	pthread_join(sep_t, NULL);
	pthread_join(sign_t, NULL);
	pthread_join(out_t, NULL);
}

int main (int argc, char* argv[]){
	//Run exec to do all the thread stuff
	if(DEBUG) printf("	(MAIN) - Program starting.\n");
	exec();
	if(DEBUG) printf("	(MAIN) - Program terminating.\n");
	return 0;
}
