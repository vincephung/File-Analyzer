#include "stdlib.h"
#include "stdio.h"
#include "node.h"

void initNode(node* temp, char* word){
	temp->next = null;
	strncpy(temp->word, word);
}

void destroyNode(node* temp){
	free(temp->word);
	free(temp);
}
