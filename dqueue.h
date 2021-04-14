typedef struct {
	char** data;
	unsigned count;
	unsigned head;
	unsigned capacity;
	int* active;
	pthread_mutex_t lock;
	pthread_cond_t read_ready;
	pthread_cond_t write_ready;
} dqueue;

int init(dqueue*, unsigned, int*);
int destroy(dqueue*);
int enqueue(dqueue*, char*);
char* dequeue(dqueue*);
