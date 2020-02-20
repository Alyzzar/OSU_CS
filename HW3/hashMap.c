#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "hashMap.h"
#include "structs.h"
#include <string.h>

int stringHash1(char * str)
{
	int i;
	int r = 0;
	for (i = 0; str[i] != '\0'; i++)
		r += str[i];
	return r;
}

int stringHash2(char * str)
{
	int i;
	int r = 0;
	for (i = 0; str[i] != '\0'; i++)
		r += (i+1) * str[i]; /*the difference between 1 and 2 is on this line*/
	return r;
}

void initMap (struct hashMap * ht, int tableSize)
{
	int index;
	if(ht == NULL){
		return;
	}
	printf("- - Allocating Memory - tableSize: %d\n", tableSize);
	ht->table = (hashLink**)malloc(sizeof(hashLink*) * tableSize);
	
	printf("- - Defining table size \n");
	ht->tableSize = tableSize;
	
	printf("- - Setting count to 0 \n");
	ht->count = 0;
	
	printf("- - Filling with null values \n");
	for(index = 0; index < tableSize; index++)
		ht->table[index] = NULL;
}


void freeMap (struct hashMap * ht)
{  /*write this*/
	struct hashLink *cur;
	
	cur = ht->table[0];

	while(cur != 0){
		/*REMOVE and free all*/
		/*Free the individual elements*/
		cur = cur->next;
		free(cur);
		cur = 0;
		ht->count--;
	}
	/*Free the map*/
	free(ht);
}

void insertMap (struct hashMap * ht, KeyType k, ValueType v)
{  /*write this*/
	int hash = stringHash1(k);
	int hashIndex = (int) (labs(hash) % ht->tableSize);
	/*STEP 2 - Allocate memory for new elem*/
	struct hashLink * newLink = (struct hashLink *) malloc(sizeof(struct hashLink));
	assert (newLink);
	/*Assign value to the new link*/
	newLink->value = v;
	newLink->key = (KeyType) malloc(strlen(k) + 1);
	strcpy(newLink->key, k);
	/*STEP 3 - Insert elem into the table*/
	/*Add to bucket at the front*/
	newLink->next = ht->table[hashIndex];
	ht->table[hashIndex] = newLink;
	/*STEP 4 - Increment number of elements*/
	newLink->next = ht->table[hashIndex];
	ht->table[hashIndex] = newLink;
	ht->count++;
}

ValueType* atMap (struct hashMap * ht, KeyType k)
{ /*write this*/
	int hash = stringHash1(k);
	int hashIndex = (int)(labs(hash) % ht->tableSize);
	struct hashLink *cur;
	cur = ht->table[hashIndex];
	while (cur != 0){
		if(strcmp(cur->key, k) == 0){
			return &cur->value;
		}
		cur = cur->next;
	}
	return NULL;	
}

int containsKey (struct hashMap * ht, KeyType k)
int containsKey (struct hashMap * ht, KeyType k)
{  /*write this*/
	int hash = stringHash1(k);
	int hashIndex = (int)(labs(hash) % ht->tableSize);
	struct hashLink *cur;
	cur = ht->table[hashIndex];
	while (cur != 0){
		if(strcmp(cur->key, k) == 0){
			return 1;
		}
		cur = cur->next;
	}
	return 0;
}

void removeKey (struct hashMap * ht, KeyType k)
{  /*write this?*/
	int hash = stringHash1(k);
	int hashIndex = (int) (labs(hash) % ht->tableSize);
	struct hashLink *cur, *last;
	
	cur = ht->table[hashIndex];
	last = ht->table[hashIndex];

	while(cur != 0){
		if(strcmp(cur->key, k) == 0){
			/*REMOVE*/
			/*The special case*/
			if (cur == ht->table[hashIndex]){
				ht->table[hashIndex] = cur->next;
			}
			last->next = cur->next;
			free(cur);
			/*Free elem*/
			cur = 0;
			ht->count--;
		} else {
			last = cur;
			cur = cur->next;
		}
	}
}

int sizeMap (struct hashMap *ht)
{  /*write this*/
	return ht->count;
}

int capacityMap(struct hashMap *ht)
{  /*write this*/
	return ht->tableSize;
}


int emptyBuckets(struct hashMap *ht)
{  /*write this*/
	int size;
	int i;
	for(i = 0; i < ht->tableSize; i++){
		if(ht->table[i] == NULL){
			size++;
		}
	}
	return size;
}

float tableLoad(struct hashMap *ht)
{  /*write this*/
	return ht->count/(float)ht->tableSize;
}
