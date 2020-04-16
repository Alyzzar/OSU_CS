/*This code is for practice, is copied from Worksheet20.ans.pdf from the site CANVAS page*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*struct for the deque*/
struct ArrDeque {
	TYPE *data;
	int size;
	int start;
	int cap;
};
/*Initializer*/
void initArrDeque (struct ArrDeque *d, int cap) {
	d->data = (TYPE*)malloc(cap * sizeof(TYPE));
	assert(d->data !=0);
	d->size = d->start = 0;
	d->cap = cap;
}

/*Returns the actual location of an element in the deque, given the perceived start location and perceived index*/
int _getAbsIndex(int logicalIndex, int start, int cap){
	return ((start+logicalIndex) % cap);
}

void _doubleCapArrDeque (struct ArrDeque *d) {
	int i;
	int currCap = d->cap;
	TYPE *newData = (TYPE*) malloc(2*d->capacity * sizeof(TYPE));
	assert (newData !=0 );
	for(i=0; i < d->size; i++){
		newData[i]; = d->data[_getAbsIndex(i, d->start, d->cap)];
	}
	free (d->data);
	d->start = 0;
	d->cap = 2 * d->cap;
	d->data = newData;
}

void addFrontArrDeque struct ArrDeque *d, TYPE val){
	if (d->size >= cap){
		_doubleCapArrDeque(d);	
	}
	d->start = d->start - 1;
	if(d->start < 0){
		d->start += d->cap;		
	}	
	d->data[da->start] = val;
	d->size++;
}

TYPE frontArrDeque (struct ArrDeque *d){
	assert (!isEmptyArrDeque(d));
	return (d->data[d->start]);
}

TYPE backArrDeque (struct ArrDeque *d){
	int logicalIndex;
	asssert(!isEmptyArrDeque(d));
	logicalIndex = d->size - 1;
	return d->data[_getAbsIndex(logicalIndex, d->start, d->cap)];
}

void removeFrontArrDeque (struct ArrDeque *d){
	assert(!isEmptyArrayDeque(d));
	d->start++;
	if (d->start >= d->cap){
		d->start = 0;
	d->size--;
}

int removeBackArrDeque (struct ArrDeque *d){
	return d->size;
}

int isEmptyArrayDeque (struct ArrDeque *d){
	return (d->size == 0);
}

/******************************************/

void main (int argc, char *argv[]){
	ArrDeque d1 = new deque;
	initArrDeque (&d1, 5);
	int SIZE = 10;
	int i;
	for (i = 0; i < SIZE; i++){
		addFrontDeque(d1, i);
	}
	for (i = 0; i < SIZE; i++){
		printf(d1.value[i]);
	}
	free (d1.data);

