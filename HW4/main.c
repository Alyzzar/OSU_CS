#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/time.h>
#include "avl.h"


int FindMinPath(struct AVLTree *tree, TYPE *path);
void printBreadthFirstTree(struct AVLTree *tree);
void printGivenLevel(struct AVLnode* root, int level);
void getTargetLeaf(struct AVLnode* node, int* min_sum_ref, int curr_sum, struct AVLnode** target_leaf_ref);

/* -----------------------
The main function
  param: argv = pointer to the name (and path) of a file that the program reads for adding elements to the AVL tree
*/
int main(int argc, char** argv) {

	FILE *file;
	int len, i;
	TYPE num; /* value to add to the tree from a file */
	struct timeval stop, start; /* variables for measuring execution time */
	int pathArray[50];  /* static array with values of nodes along the min-cost path of the AVL tree -- as can be seen, the tree cannot have the depth greater than 50 which is fairly sufficient for out purposes*/

	struct AVLTree *tree;
	
	tree = newAVLTree(); /*initialize and return an empty tree */
	
	file = fopen(argv[1], "r"); 	/* filename is passed in argv[1] */
	assert(file != 0);

	/* Read input file and add numbers to the AVL tree */
	while((fscanf(file, "%i", &num)) != EOF){
		addAVLTree(tree, num);		
	}
	/* Close the file  */
	fclose(file);
	
	printf("\nPrinting the tree breadth-first : \n");
	printBreadthFirstTree(tree);

	gettimeofday(&start, NULL);

	/* Find the minimum-cost path in the AVL tree*/
	len = FindMinPath(tree, pathArray);
	
	gettimeofday(&stop, NULL);

	/* Print out all numbers on the minimum-cost path */
	printf("\nThe minimum-cost path is: \n");
	for(i = 0; i < len; i++)
		printf("%d ", pathArray[i]);
	printf("\n");

	printf("\nYour execution time to find the mincost path is %f microseconds\n", (double) (stop.tv_usec - start.tv_usec));

        /* Free memory allocated to the tree */
	deleteAVLTree(tree); 
	
	return 0;
}

/* -------------------
	This function Sets the target_leaf_ref to refer 
	the leaf node of the maximum path sum. Also, 
	returns the min_sum using min_sum_ref 
*/
void getTargetLeaf(struct AVLnode* node, int* min_sum_ref, int curr_sum, struct AVLnode** target_leaf_ref){ 
    if (node == NULL) 
        return; 
  
    /*Update current sum to hold sum of nodes on path */
    /*from root to this node */
    curr_sum = curr_sum + node->data; 
  
    /* If this is a leaf node and path to this node has */
    /* maximum sum so far, then make this node target_leaf */
    if (node->left == NULL && node->right == NULL) { 
        if (*min_sum_ref == 0 || curr_sum < *min_sum_ref) { 
            *min_sum_ref = curr_sum; 
            *target_leaf_ref = node; 
        } 
    } 
  
    // If this is not a leaf node, then recur down 
    // to find the target_leaf 
    getTargetLeaf(node->left, min_sum_ref, curr_sum, target_leaf_ref); 
    getTargetLeaf(node->right, min_sum_ref, curr_sum, target_leaf_ref); 
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
int FindMinPath(struct AVLTree *tree, TYPE *path)
{
               /* FIX ME */
    int min_sum_ref = 0;
	struct AVLnode* target_leaf;
	if(tree == NULL || tree->root == 0) return 0;
	getTargetLeaf(node, &min_sum_ref, 0, &target_leaf);
	return length;
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
    int i;
    for (i=1; i<=height; i++) {
		printGivenLevel(tree->root, i);
	}
}

void printGivenLevel(struct AVLnode* root, int level) 
{
    if (root == NULL) 
        return; 
    if (level == 1) 
        printf("%d ", root->val); 
    else if (level > 1) 
    { 
        printGivenLevel(root->left, level-1); 
        printGivenLevel(root->right, level-1); 
    } 
} 
