typedef struct dqueue{
	char** data;
	unsigned count;
	unsigned head;
	unsigned capacity;
	int* active;
	pthread_mutex_t lock;
	pthread_cond_t read_ready;
	pthread_cond_t write_ready;
} dqueue;

int dInit(dqueue*, unsigned, int*);
int dDestroy(dqueue*);
int dEnqueue(dqueue*, char*);
char* dDequeue(dqueue*);
