#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#ifndef SIZE
#define SIZE 8
#endif

int init(dqueue_t *Q, size_t size, unsigned active)
{
	Q->count = 0;
	Q->head = 0;
	Q->active = active;
	Q->data = malloc(sizeof(char*)*size);
	Q->size = size;
	for(int i=0; i<size; i++){
		data[i] = malloc(sizeof(char)*LENGTH);
}
	pthread_mutex_init(&Q->lock, NULL);
	pthread_cond_init(&Q->read_ready, NULL);
	pthread_cond_init(&Q->write_ready, NULL);
	
	return 0;
}

int destroy(dqueue_t *Q)
{
	//free memory
	for(int i=0; i<Q->size; i++){
		free(Q->data[i]);
	}
	free(Q->size);
	pthread_mutex_destroy(&Q->lock);
	pthread_cond_destroy(&Q->read_ready);
	pthread_cond_destroy(&Q->write_ready);

	return 0;
}


// add item to end of queue
// if the queue is full, block until space becomes available
int enqueue(queue_t *Q, int item)
{
	pthread_mutex_lock(&Q->lock);
	
	//if queue is full allocated more space
	if(Q->count == QSIZE){

	while (Q->count == QSIZE) {
		pthread_cond_wait(&Q->write_ready, &Q->lock);
	}
	
	unsigned i = Q->head + Q->count;
	if (i >= QSIZE) i -= QSIZE;
	
	Q->data[i] = item;
	++Q->count;
	
	pthread_cond_signal(&Q->read_ready);
	
	pthread_mutex_unlock(&Q->lock);
	
	return 0;
}


int dequeue(queue_t *Q, int *item)
{
	pthread_mutex_lock(&Q->lock);
	
	while (Q->count == 0) {
		pthread_cond_wait(&Q->read_ready, &Q->lock);
	}
	
	*item = Q->data[Q->head];
	--Q->count;
	++Q->head;
	if (Q->head == QSIZE) Q->head = 0;
	
	pthread_cond_signal(&Q->write_ready);
	
	pthread_mutex_unlock(&Q->lock);
	
	return 0;
}
