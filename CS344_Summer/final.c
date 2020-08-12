#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

int main (int argc, char* argv[]){
	char* aStr = malloc(2*sizeof(char));
	strcat(aStr, "OregonStateUniversityCorvallisGoBeavers");
	printf("%s", aStr);
}