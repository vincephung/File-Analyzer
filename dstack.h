struct node{
	char* data;
	struct node* next;
};

typedef struct{
	struct node* head;
	unsigned* active;
	pthread_mutex_t lock;
	pthread_cond_t ready;
} dstack;

int init(dstack *, unsigned);
int destroy(dstack *);
int push(dstack *, char*  item);
char* pop(dstack *);
