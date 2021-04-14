typedef struct {
	char** data;
	unsigned count;
	unsigned head;
	unsigned capacity;
	pthread_mutex_t lock;
	pthread_cond_t read_ready;
	pthread_cond_t write_ready;
} fqueue;

int init(fqueue*, unsigned);
int destroy(fqueue*);
int enqueue(fqueue*, char*);
char* dequeue(fqueue*);
