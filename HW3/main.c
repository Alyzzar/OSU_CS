#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "hashMap.h"

/*
 the getWord function takes a FILE pointer and returns you a string which was
 the next word in the in the file. words are defined (by this function) to be
 characters or numbers separated by periods, spaces, or newlines.
 
 when there are no more words in the input file this function will return NULL.
 
 this function will malloc some memory for the char* it returns. it is your job
 to free this memory when you no longer need it.
 */
char* getWord(FILE *file); /* prototype */
void concordance(struct hashMap *ht, char * word, FILE *file);
/****************************************/

int main (int argc, const char * argv[]) {
    /*Write this function*/
	FILE* file = fopen(argv[1], "r");
	
	printf("Assignment 3\n");
	if(argc==2){
		hashMap* wordList;
		char* word;
		initMap(wordList, 100);
		while (word != NULL){
			concordance(wordList, word, file);
			word = getWord(file);
		}
		freeMap(wordList);
	} else {
		printf("Please enter a valid textfile.\n");
	}
	return 0;
}

char* getWord(FILE *file){
	int length = 0;
	int maxLength = 16;
	char character;
    
	char* word = (char*)malloc(sizeof(char) * maxLength);
	assert(word != NULL);
    
	while( (character = fgetc(file)) != EOF)
	{
		if((length+1) > maxLength)
		{
			maxLength *= 2;
			word = (char*)realloc(word, maxLength);
		}
		if((character >= '0' && character <= '9') || /*is a number*/
		   (character >= 'A' && character <= 'Z') || /*or an uppercase letter*/
		   (character >= 'a' && character <= 'z') || /*or a lowercase letter*/
		   character == 39) /*or is an apostrophe*/
		{
			word[length] = character;
			length++;
		}
		else if(length > 0)
			break;
	}
    
	if(length == 0)
	{
		free(word);
		return NULL;
	}
	word[length] = '\0';
	return word;
}

void concordance(struct hashMap *ht, char* word, FILE *file){
	if(containsKey(ht, word) != 0){
		insertMap(ht, word, *atMap(ht, word) + 1);
	} else {
		insertMap(ht, word, 1);
	}
}
