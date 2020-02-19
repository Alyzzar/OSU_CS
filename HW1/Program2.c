/* CS261- HW1 - Program2.c*/
/* Name: Ben Chan
 * Date: Jan. 20, 2020
 * Solution description:
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

struct student{
	char initials[2];
	int score;
};

struct student* allocate(){
    /*Allocate memory for ten students*/
    struct student* stud = malloc(10*sizeof(struct student));
	/*return the pointer*/
	return stud;
}

void generate(struct student* students){
     /*Generate random initials and scores for ten students.
	The two initial letters must be capital and must be between A and Z. 
	The scores must be between 0 and 100*/
	int i;
	for( i = 0; i < 10; i++){
		students[i].score = rand()%101;
		students[i].initials[0] = rand()%26 + 'A'; 
		students[i].initials[1] = rand()%26 + 'A';
	}
}

void output(struct student* students){
     /*Output information about the ten students in the format:
              1. Initials  Score
              2. Initials  Score
              ...
              10. Initials Score*/
	int i;

	for( i = 0; i < 10; i++){
		printf("\n%d", i + 1);
		printf(". %c", students[i].initials[0]);
		printf("%c", students[i].initials[1]);
		printf("  %d", students[i].score);
	}
	printf("\n");
}

void summary(struct student* students){
     /*Compute and print the minimum, maximum and average scores of the ten students*/
	 int min, max, total, avg;
	 int i;
	 for( i = 0; i < 10; i++){
		total = total + students[i].score;
		if(students[i].score > max) max = students[i].score;
		else if(students[i].score < min) min = students[i].score;
	}
	avg = total/10;
	/*Print results*/
	printf("\nSummary:\n Min: %d", min);
	printf("\n Max: %d", max);
	printf("\n Avg: %d", avg);
	printf("\n");
}

void deallocate(struct student* stud){
     /*Deallocate memory from student */
	free (stud);
}

int main(){
    struct student* stud = NULL;
    srand(time(NULL));
	
    /*call allocate*/
    stud = allocate();
	printf("\nMemory allocated.");
    /*call generate*/
    generate(stud);
    /*call output*/
    output(stud);
    /*call summary*/
    summary(stud);
    /*call deallocate*/
	deallocate(stud);
	printf("\n Memory deallocated.");
	printf("\n Program 2 complete. \n");	
    return 0;
}
