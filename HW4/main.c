#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/time.h>
#include "avl.h"

#define maxLen 100

int FindMinPath(struct AVLTree *tree, TYPE *path);
int travMinVal(struct AVLnode *current, int *v, TYPE *path, int n);
void printBreadthFirstTree(struct AVLTree *tree);
void printGivenLevel(struct AVLnode* root, int level);
void updateHeight(struct AVLTree *tree);
void travUpdateHeight(struct AVLnode *current);


/* -----------------------
The main function
  param: argv = pointer to the name (and path) of a file that the program reads for adding elements to the AVL tree
*/
int main(int argc, char** argv) {

	FILE *file;
	int outputLen, i, inputLen;
	TYPE num; /* value to add to the tree from a file */
	struct timeval stop, start; /* variables for measuring execution time */
	int pathArray[maxLen];  /* static array with values of nodes along the min-cost path of the AVL tree -- as can be seen, the tree cannot have the depth greater than 50 which is fairly sufficient for out purposes*/

	struct AVLTree *tree;
	
	tree = newAVLTree(); /*initialize and return an empty tree */
	
	file = fopen(argv[1], "r"); 	/* filename is passed in argv[1] */
	assert(file != 0);

	/* Read input file and add numbers to the AVL tree */
	inputLen = 0;
	while((fscanf(file, "%i", &num)) != EOF){
		addAVLTree(tree, num);	
		inputLen++;
	}
	/* Close the file  */
	fclose(file);
	
	/*Update height of the tree*/
	updateHeight(tree);
	
	/*Balancing tree*/
	/*_balance(tree->root);*/
	
	printf("inputLen: %d",inputLen);
	printf("\nPrinting the tree breadth-first : \n");
	printBreadthFirstTree(tree);

	gettimeofday(&start, NULL);
	
	/* Reset all values of pathArray to 0*/
	for (i = 0; i < maxLen; i++){
		pathArray[i] = 0;
	}

	/* Find the minimum-cost path in the AVL tree*/
	outputLen = FindMinPath(tree, pathArray);
	printf("\nOutputLen = %d", outputLen);
	gettimeofday(&stop, NULL);

	/* Print out all numbers on the minimum-cost path */
	printf("\nThe minimum-cost path is: \n");
	for(i = 0; i < outputLen; i++)
		printf("%d ", pathArray[i]);
	printf("\n");

	printf("\nYour execution time to find the mincost path is %f microseconds\n", (double) (stop.tv_usec - start.tv_usec));

        /* Free memory allocated to the tree */
	deleteAVLTree(tree); 
	
	return 0;
}

/* --------------------
Finds the minimum-cost path in an AVL tree
   Input arguments: 
        tree = pointer to the tree,
        path = pointer to array that stores values of nodes along the min-cost path, 
   Output: return the min-cost path length 

   pre: assume that
       path is already allocated sufficient memory space 
       tree exists and is not NULL
*/
int FindMinPath(struct AVLTree *tree, TYPE *path){
	int *outputVal = 0;
	int outputLen = travMinVal(tree->root, outputVal, path, 0);
	return outputLen + 1;
}

int travMinVal(struct AVLnode *current, int *v, TYPE *path, int n){
	int leftVal;
	int rightVal;
	if(current != NULL){
		if(current->right == NULL){
			/*If no right child*/
			rightVal = 1000000;
		} else {
			/*Recurssive -> right child*/
			rightVal = travMinVal(current->right, v, path, n + 1) + 1;
		}
		
		if(current->left == NULL){
			/*If no left child*/
			leftVal = 1000000;
		} else {
			/*Recurssive -> left child*/
			leftVal = travMinVal(current->left, v, path, n + 1) + 1;
		}
		
		/*Return the lower of the two values*/
		path[n] = current->val;
		*v += current->val;
		if(current->left->val > current->right->val){
			/*RIGHT IS SMALLER*/
			rightVal = travMinVal(current->right, v, path, n + 1) + 1;
			return rightVal;	
		} else {
			/*LEFT IS SMALLER*/
			return leftVal;
		}
		/*NEITHER EXISTS*/
		return 1;
	}
	/*Node doesn't exist*/
	return 0;
}

/* -----------------------
Printing the contents of an AVL tree in breadth-first fashion
  param: pointer to a tree
  pre: assume that tree was initialized well before calling this function
*/
void printBreadthFirstTree(struct AVLTree *tree)
{
    /* FIX ME */
    int height = h(tree->root);
	int i, j;
	printf("Height = %d \n", height);
    /*printf("- - PRINTING BREADTH FIRST\n");*/
	for (i = 1; i <= height + 1; i++) {
		/*printf("\n- - PRINT i = %d \n", i);*/
		for(j = 0; j <= height - i; j++){
			printf(" ");
		}
		printGivenLevel(tree->root, i);
		printf("\n");
		
	}
}

void printGivenLevel(struct AVLnode* current, int level) 
{
    if (current == NULL){
		printf(". ");
		return; 
	}
    if (level == 1){
		printf("%d ", current->val);
		return;
	} else { 
		/*printf("- - - RECURSSIVE CALL \n");*/
        /*printf("-> left: ");*/
		printGivenLevel(current->left, level - 1); 
        /*printf("-> right: ");*/
		printGivenLevel(current->right, level - 1); 
    } 
} 

void updateHeight(struct AVLTree *tree){
	/*calls traversal (post-order) to update height of each node*/
	travUpdateHeight(tree->root);
}

void travUpdateHeight(struct AVLnode* current){
	if(current != NULL){
		travUpdateHeight(current->left);
		travUpdateHeight(current->right);
		setHeight(current);
	}
}
