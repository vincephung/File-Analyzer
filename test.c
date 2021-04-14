#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include "fqueue.h"

int main(int argc, char** argv){
	fqueue queue;
	init(&queue, 256);
	enqueue(&queue, "one");
	enqueue(&queue, "two");
	char* temp = dequeue(&queue);
	printf("%s \n", temp);

	destroy(&queue);
	free(temp);

	return 0;
}
	

