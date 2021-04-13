#include "node.h"

typedef struct {
	int data[QSIZE];
	unsigned count;
	struct node* head;
	struct node* last;
	unsigned activeThreads;
	pthread_mutex_t lock;
	pthread_cond_t read_ready;
	pthread_cond_t write_ready;
} dqueue;

int init(dqueue *Q, unsigned activeThreads);
int destroy(dqueue *Q);
int enqueue(dqueue *Q, char*  item);
int dequeue(dqueue *Q, char* item);
