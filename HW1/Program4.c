/* CS261- HW1 - Program4.c*/
/* Name: Ben Chan
 * Date: Jan. 20, 2020
 * Solution description:
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

struct student{
	char initials[2];
	int score;
};

int cmpfnc (const void *a, const void *b) {
	const struct student *stud1 = a;
	const struct student *stud2 = b;
	
	if(stud1->initials[0] > stud2->initials[0]){
		return 1;	
	} else if(stud1->initials[0] < stud2->initials[0]){
		return -1;
	}
	
	if(stud1->initials[1] > stud2->initials[1]){
		return 1;	
	} else if(stud1->initials[1] < stud2->initials[1]){
		return -1;
	}
	return 0;
}

void sort(struct student* students, int n){
     /*Sort n students based on their initials*/ 
	qsort(students, n, sizeof(struct student), cmpfnc);
}

int main(void){
	srand(time(NULL));
    /*Declare an integer n and assign it a value.*/
    int n = 5;
	/*Declaring i for use in for loops later.*/
	int i;
    /*Allocate memory for n students using malloc.*/
	struct student* students = malloc(n*sizeof(struct student));

    /*Generate random IDs and scores for the n students, using rand().*/
	i = 0;
	for( i = 0; i < n; i++){
		students[i].score = rand()%101;
		students[i].initials[0] = rand()%26 + 'A'; 
		students[i].initials[1] = rand()%26 + 'A';
	}
    /*Print the contents of the array of n students.*/
	i = 0;
	for( i = 0; i < n; i++){
		printf("\n%d", i + 1);
		printf(". %c", students[i].initials[0]);
		printf("%c", students[i].initials[1]);
		printf("  %d", students[i].score);
	}
	printf("\n");
    /*Pass this array along with n to the sort() function*/
    sort(students, n);
    /*Print the contents of the array of n students.*/
	i = 0;
	for( i = 0; i < n; i++){
		printf("\n%d", i + 1);
		printf(". %c", students[i].initials[0]);
		printf("%c", students[i].initials[1]);
		printf("  %d", students[i].score);
	}
	printf("\n");
	printf("\n Program 4 complete.");
    return 0;
}

