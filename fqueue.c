#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include "fqueue.h"

int fInit(fqueue *Q, unsigned capacity)
{
	Q->count = 0;
	Q->head = 0;
	Q->data = malloc(sizeof(char*)*capacity);
	Q->capacity = capacity;
	pthread_mutex_init(&Q->lock, NULL);
	pthread_cond_init(&Q->read_ready, NULL);
	pthread_cond_init(&Q->write_ready, NULL);
	
	return 0;
}

int fDestroy(fqueue *Q)
{
	for(int i = 0; i < Q->count; i++){
		char* item = Q->data[i];
		free(item);
	}
	free(Q->data);
	pthread_mutex_destroy(&Q->lock);
	pthread_cond_destroy(&Q->read_ready);
	pthread_cond_destroy(&Q->write_ready);

	return 0;
}


// add item to end of queue
// if the queue is full, block until space becomes available
int fEnqueue(fqueue *Q, char* item)
{
	pthread_mutex_lock(&Q->lock);
	
	while (Q->count == Q->capacity) {
		pthread_cond_wait(&Q->write_ready, &Q->lock);
	}
	
	unsigned i = Q->head + Q->count;
	if (i >= Q->capacity) i -= Q->capacity;
	
    //Q->data[Q->head] = malloc((sizeof (item) + 2)* sizeof(char));
	Q->data[i] = malloc((sizeof (item) + 2)* sizeof(char));
	//strcpy(Q->data[Q->head], item);
	strcpy(Q->data[i], item);
		
	++Q->count;
	
	pthread_cond_signal(&Q->read_ready);
	
	pthread_mutex_unlock(&Q->lock);
	
	return 0;
}


char* fDequeue(fqueue *Q)
{
	pthread_mutex_lock(&Q->lock);
	
	while (Q->count == 0) {
		pthread_cond_wait(&Q->read_ready, &Q->lock);
	}
	
	char* item = malloc((sizeof(Q->data[Q->head])+2)*sizeof(char));
	strcpy(item, Q->data[Q->head]);
	free(Q->data[Q->head]);
	--Q->count;
	++Q->head;
	//if (Q->head == 256) Q->head = 0;
	if (Q->head == Q->capacity) Q->head = 0;
	
	pthread_cond_signal(&Q->write_ready);
	
	pthread_mutex_unlock(&Q->lock);
	
	return item;
}
