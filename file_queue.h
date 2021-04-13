typedef struct {
	int data[QSIZE];
	unsigned count;
	unsigned head;
	pthread_mutex_t lock;
	pthread_cond_t read_ready;
	pthread_cond_t write_ready;
} fqueue;

int init(fqueue *Q);
int destroy(fqueue *Q);
int enqueue(fqueue *Q, char*  item);
int dequeue(fqueue *Q, char* item);
