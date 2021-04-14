#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include "dstack.h"

int main(int argc, char** argv){
	dstack stack;
	init(&stack, 1);
	push(&stack, "one");
	push(&stack, "two");
	char* temp;
	temp = pop(&stack);
	printf("%s \n", temp);

	destroy(&stack);

	return 0;
}
	

