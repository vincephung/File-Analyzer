#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include "dqueue.h"

int dInit(dqueue *Q, unsigned capacity, int active)
{
	Q->count = 0;
	Q->head = 0;
	Q->active = active;
	Q->data = malloc(sizeof(char*)*capacity);
	Q->capacity = capacity;
	pthread_mutex_init(&Q->lock, NULL);
	pthread_cond_init(&Q->read_ready, NULL);
	pthread_cond_init(&Q->write_ready, NULL);
	
	return 0;
}

int dDestroy(dqueue *Q)
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
int dEnqueue(dqueue *Q, char* item)
{
	pthread_mutex_lock(&Q->lock);
	
	//if max capacity reallocate
	if(Q->count == Q->capacity){
		char** temp = realloc(Q->data, sizeof(char*)*(Q->capacity * 2));		Q->data = temp;
		Q->capacity = Q->capacity * 2;
	}
	
	//unsigned i = Q->head + Q->count;
	
	Q->data[Q->head] = malloc((sizeof item + 2)* sizeof(char));
	strcpy(Q->data[Q->head], item);
		
	++Q->count;
	
	pthread_cond_signal(&Q->read_ready);
	
	pthread_mutex_unlock(&Q->lock);
	
	return 0;
}


char* dDequeue(dqueue *Q)
{
	pthread_mutex_lock(&Q->lock);
	
	if(Q->count == 0){
		//(*Q->active)--;
		--Q->active;
		//if((*Q->active)==0){
		if((Q->active)==0){
			pthread_mutex_unlock(&Q->lock);
			pthread_cond_signal(&Q->write_ready);
			pthread_cond_signal(&Q->read_ready);
			return NULL;
		}
		//while(Q->count == 0 && (*Q->active) > 0){
		while(Q->count == 0 && (Q->active) > 0){
			pthread_cond_wait(&Q->read_ready, &Q->lock);
		}
		if(Q->count == 0){
			pthread_mutex_unlock(&Q->lock);
			return NULL;
		}
		//(*Q->active)++;
		++Q->active;
	}
	
	
	char* item = malloc((sizeof(Q->data[Q->head])+2)*sizeof(char));
	strcpy(item, Q->data[Q->head]);
	free(Q->data[Q->head]);
	--Q->count;
	++Q->head;
	if (Q->head == Q->capacity) Q->head = 0;
	
	pthread_cond_signal(&Q->write_ready);
	
	pthread_mutex_unlock(&Q->lock);
	
	return item;
}
