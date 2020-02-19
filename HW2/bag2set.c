/* bag2set.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dynArray.h"


/* Converts the input bag into a set using dynamic arrays 
	param: 	da -- pointer to a bag 	
	return value: void
        result: after exiting the function da points to a set 		
*/
void bag2set(struct DynArr *da) {
	/* FIX ME */
	int i = 0;
	int j = 0;
	/* Debugging information
	printf("\nRunning bag2set(DynArr)");
	*/
	for(i = 0; i < da->size - 1; i++){
		for(j = i + 1; j < da->size; j++){
			/* Debugging information
			printf("\ni, j: (%d, %d", i, j);
			printf(")");
			printf("\n   data[i], data[j]: (%2.1f, %2.1f", da->data[i], da->data[j]);
			*/
			if(da->data[i] == da->data[j]){
				/*printf(": value removed");*/
				removeAtDynArr(da, i);
				i--;
				break;
			}
		}
	}

}

/* An example how to test your bag2set() */
int main(int argc, char* argv[]){

	int i;
    struct DynArr da;  /* bag */
    initDynArr(&da, 100);
    da.size = 8;
    da.data[0] = 1.3;
	
	for (i=1;i<da.size-1;i++){
	    da.data[i] = 1.2;
	}
    da.data[da.size-1] = 1.4;

    printf("Bag:\n\n");
	for (i=0;i<da.size;i++){
        printf("%g  \n", da.data[i]);
    }
	
	bag2set(&da);
	
	printf("\n\n\n");
	printf("Set:\n\n");
	for (i=0;i<da.size;i++){
		printf("%g ", da.data[i]);
    }
		printf("\n");
	freeDynArr(&da);
	printf("Program complete. Terminating.\n");
	return 0;	
}


