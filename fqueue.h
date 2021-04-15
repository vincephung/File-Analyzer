typedef struct fqueue{
	char** data;
	unsigned count;
	unsigned head;
	unsigned capacity;
	int* active;
	pthread_mutex_t lock;
	pthread_cond_t read_ready;
	pthread_cond_t write_ready;
} fqueue;

int fInit(fqueue*, unsigned);
int fDestroy(fqueue*);
int fEnqueue(fqueue*, char*);
char* fDequeue(fqueue*);
