/* CS261- HW1 - Program1.c*/
/* Name: Ben Chan
 * Date: Jan. 20, 2020
 * Solution description:
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int foo(int* a, int* b, int c){
    *a = *a + 1;
	*b = *b - 1;
    c = *a - *b;
	return c;
}

int main(){
	srand(time(NULL));
	/* Initialize x, y, z */
      	int *x = malloc(sizeof(int));
	int *y = malloc(sizeof(int));
	int z; 
	/*srand((unsigned) time(&t));*/
	/* Give x, y, z random values between 0 and 10 */
	*x = rand() % 11;
	*y = rand() % 11;
	z = rand() % 11;
	/* Print values of x, y, z */
	printf("\n x: %d", *x);
	printf("\n y: %d", *y);
	printf("\n z: %d", z);
  	  /* Call foo() appropriately, passing x,y,z as parameters */
	int zOut = foo(x, y, z);
   	 /* Print the values of x, y and z */
	    printf("\n x: %d", *x);
	printf("\n y: %d", *y);
	printf("\n z: %d", z);
    	/* Print the value returned by foo */
	printf("\n foo() output: %d", zOut);
	printf("\n Program 1 complete.\n");  
 	/* Is the return value different than the value of z?  Why? */
    return 0;
}
/* Why are z and zOut not equivalent?
The value of Z does not change after foo() is run because foo() can only change the value of the local variable c. Z is passed into the function foo() as a parameter, and it's value is copied into the local variable c. This is why z and zOut have different values. 

This behavior is not necessarily true for x and y though. Because the parameters a and b (and hence x and y) are pointers, thhey instead point to memory addresses. In doing so, foo() is able to access the values of x and y directly rather than working with a localized copy of their values.*/
