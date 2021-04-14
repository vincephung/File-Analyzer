#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include "dqueue.h"

int main(int argc, char** argv){
	dqueue queue;
	int thread = 1;
	int* num = &thread;
	init(&queue, 1, num);
	enqueue(&queue, "one");
	enqueue(&queue, "two");
	char* temp = dequeue(&queue);
	printf("%s \n", temp);

	destroy(&queue);
	free(temp);

	return 0;
}
	

