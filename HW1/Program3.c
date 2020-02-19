/* CS261- HW1 - Program3.c*/
/* Name: Ben Chan
 * Date: Jan. 20, 2020
 * Solution description:
 */
 
#include <stdio.h>
#include<stdlib.h>
#include <time.h>

int cmpfnc (const void * a, const void * b) {
	return ( *(int*)a - *(int*)b );
}

void sort(int* number, int n){
     /*Sort the array of integers of length n*/     
	 qsort(number, n, sizeof(int), cmpfnc);
}

int main(){
	srand(time(NULL));
	
    /*Declare an integer n and assign it a value of 20.*/
    int n = 20;
	if (n == 0){
		/*If n is 0, break out of function*/
		return 0;
	}
	
    /*Allocate memory for an array of n integers using malloc.*/
    int *numbers = malloc(n);
    /*Fill this array with random numbers, using rand().*/
    int i;
	for(i = 0; i < n; i++){
		numbers[i] = rand()%101;
	}
    /*Print the contents of the array.*/
	printf("\n Array contents: \n");
	for(i = 0; i < n; i++){
		printf("%d ", numbers[i]);
	}
    /*Pass this array along with n to the sort() function of part a.*/
    sort(numbers, n);
    /*Print the contents of the array.*/    
	printf("\n Array contents: \n");
	for(i = 0; i < n; i++){
		printf("%d ", numbers[i]);
	}
    printf("\n Program 3 complete. \n");
    return 0;
}

