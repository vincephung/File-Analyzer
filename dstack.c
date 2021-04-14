#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include "dstack.h"

#ifndef SIZE
#define SIZE 8
#endif

int init(dstack *Q, unsigned active)
{
	Q->head = NULL;
	Q->active = &active;
	pthread_mutex_init(&Q->lock, NULL);
	pthread_cond_init(&Q->ready, NULL);
	
	return 0;
}

int destroy(dstack *Q)
{
	//free memory
   	struct node* tmp;
	struct node* crnt = Q->head;

   	while (crnt != NULL)
    	{
       		tmp = crnt->next;
		free(crnt);
       		crnt = tmp;
   	 }	
	pthread_mutex_destroy(&Q->lock);
	pthread_cond_destroy(&Q->ready);

	return 0;
}


// push item on stack
int push(dstack *Q, char* item)
{
	pthread_mutex_lock(&Q->lock);
	
	struct node* tmp = malloc(sizeof(struct node));
	tmp->data = item;
	//check for empty stack
	if(Q->head == NULL){
		Q->head = tmp;
		tmp->next = NULL;
	}
	else{
		tmp->next = Q->head;
		Q->head = tmp;
	}
	
	pthread_cond_signal(&Q->ready);
	
	pthread_mutex_unlock(&Q->lock);
	
	return 0;
}


char* pop(dstack *Q)
{
	pthread_mutex_lock(&Q->lock);
	
	if(Q->head == NULL){
		(*Q->active)--;
		if((*Q->active)==0){
			pthread_mutex_unlock(&Q->lock);
			//pthread_mutex_signal(&Q->ready);
			return NULL;
		}
		while(Q->head == NULL && (*Q->active) > 0){
			pthread_cond_wait(&Q->ready, &Q->lock);
		}
		if(Q->head == NULL){
			pthread_mutex_unlock(&Q->lock);
			return NULL;
		}
		(*Q->active)++;
	}
	//perform dequeue
	struct node* tmp = Q->head;
	Q->head = tmp->next;
	char* value;
	strcpy(value, tmp->data);
	free(tmp);
	return value;

}
