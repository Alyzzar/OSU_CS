/* Property of Log Inc */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hashMap.h"
#include "structs.h"
#include "linkedList.h"

void print(struct hashMap* a){
	int i;

	printf("tableSize:%d\n", a->tableSize);
	printf("count: %d\n", a->count);

	for(i = 0; i < a->tableSize; i++){
		struct hashLink* current = a->table[i];
		printf("hash = %d:\n", i);
		while(current != NULL){
			printf("\tkey: \"%s\" value: %d\n", current->key, current->value);
			current = current->next;
		}
	}

	printf("==========================\n");
}

KeyType getKey(ValueType v){
	KeyType k = (char*)malloc(sizeof(char) * 32);
	printf("Enter key for %d: ", v);
	scanf("%s", k);
	return k;
}

void testHashMap(){
	const int SIZE = 5;
	int i = 0;
	KeyType k = NULL;
	struct hashMap* a = (struct hashMap*)malloc(sizeof(struct hashMap));

	initMap(a, SIZE);

	for(i = 0; i < SIZE; i++){
		k = getKey(i);
		insertMap(a, k, i);
		free(k);

		k = getKey(i);
		insertMap(a, k, i);
		removeKey(a, k);
		free(k);

		print(a);
	}

	freeMap(a);

	print(a);

	free(a);
}

void testLinkedList(){
	LinkedList l;
	int i = 0;

	initLinkedList(&l);

	for(i = 0; i < 5; i++){
		pushLinkedList(&l, i);
	}

	while(!isEmptyLinkedList(&l)){
		printf("%g\n", topLinkedList(&l));
		popLinkedList(&l);
	}

	for(i = 6; i < 11; i++){
		addLinkedList(&l, i);
	}

	for(i = 0; i < 5; i++){
		addLinkedList(&l, 6);
	}

	for(i = 6; i < 11; i++){
		printf("contains %g: %d, contains %g: %d\n", (double)i, containsLinkedList(&l, i), (double)(i * 5), containsLinkedList(&l, i * 5));
		printf("Removed %g\n", (double)i);
		removeLinkedList(&l, i);
		printf("contains %g: %d, contains %g: %d\n\n", (double)i, containsLinkedList(&l, i), (double)(i * 5), containsLinkedList(&l, i * 5));
	}

	freeLinkedList(&l);
}

int main(int argc, char* argv[]){
	testHashMap();

	printf("\n\n===================================================================\n\n");

	testLinkedList();
	return 0;
}
