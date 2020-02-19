#include <stdlib.h>
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
	if(ht == NULL)
		return;
	ht->table = (hashLink**)malloc(sizeof(hashLink*) * tableSize);
	ht->tableSize = tableSize;
	ht->count = 0;
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
	/*STEP 1 - Compute the index of elem in the table from the key*/
	/*Use elem.key, since we passed in elem by value, not address*/
	int hash = stringHash1(k);
	int hashIndex = (int) (labs(hash) % ht->tableSize);
	/*STEP 2 - Allocate memory for new elem*/
	struct hashLink * newLink = (struct hashLink *) malloc(sizeof(struct hashLink));
	assert (newLink);
	/*Assign value to the new link*/
	newLink->value = v;
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
{ /*write this?*/
	int hash = stringHash1(k);
	int hashIndex = (int)(labs(hash) % ht->tableSize);
	struct hashLink *cur;
	cur = ht->table[hashIndex];
	while (cur != 0){
		if(EQ(cur->value, k)){
			&return cur->value;
		}
		cur = cur->next;
	}
	return 0;	
}

int containsKey (struct hashMap * ht, KeyType k)
{  /*write this*/
	int hash = stringHash1(k);
	int hashIndex = (int)(labs(hash) % ht->tableSize);
	struct hashLink *cur;
	cur = ht->table[hashIndex];
	while (cur != 0){
		if(EQ(cur->value, k)){
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
		if(EQ(cur->value, k)){
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
	int size;
	int i;
	for(i = 0; i < ht->tableSize; i++){
		if(ht->table[i] != NULL){
			size++;
		}
	}
	return size;
}

/*
int capacityMap(struct hashMap *ht)
{  /*write this*
	return tableSize;
}

int emptyBuckets(struct hashMap *ht)
{  /*write this*

}

float tableLoad(struct hashMap *ht)
{  /*write this*
}
*/
