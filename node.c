#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "node.h"

void initNode(node* temp, char* word){
	temp->next = NULL;
	strcpy(temp->word, word);
};
