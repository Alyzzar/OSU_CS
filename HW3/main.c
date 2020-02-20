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
void printWords(struct hashMap * ht);
/****************************************/

int main (int argc, const char * argv[]) {
	FILE* file;
	hashMap* wordList = (hashMap*)malloc(sizeof(hashMap));
	char* word;
	printf("Assignment 3\n");
    /*Write this function*/
	/*printf("- FOpen \n");*/
	file = fopen(argv[1], "r");
	if(argc==2){
		/*printf("- initMap() \n");*/
		initMap(wordList, 100);
		/*printf("- while() for concordance \n");*/
		word = getWord(file);
		while (word != NULL){
			if(containsKey(wordList, word) != 0){
				printf("- - Recurring word, atMap()value: %d, word: %s\n", *atMap(wordList, word) + 1, word);
				(*atMap(wordList, word))++;
			} else {
				printf("- - First occurance of word - %s\n", word);
				insertMap(wordList, word, 1);
			}
			free(word)
			word = getWord(file);
		}
		printWords(wordList);
		
		freeMap(wordList);
	} else {
		printf("Please enter a valid textfile.\n");
	}
	return 0;
}

void printWords(struct hashMap * ht){
	/*Prints out the contents of the hashmap*/
	int index;
	printf("- Hashmap Contents: \n");
	for(index = 0; index < ht->tableSize; index++){
		if(ht->table[index] != NULL){
			printf("%10s - # of occurances: %d\n", ht->table[index]->key, ht->table[index]->value);
		}
	}
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
