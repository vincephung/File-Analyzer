//Linkedlist of all words in a file
typedef struct wordMap{
    int freq;
    char* word;
    struct wordMap* next;
} wordMap;

//WFD information about all files, linkedlist of files
typedef struct fileStruct{
    char* fileName;
    int numTokens; //number of tokens in the file
    wordMap* words; //the words inside this file
    struct fileStruct* next;
} fileStruct;

//Arguments that contain both queues
typedef struct threadArgs{
    struct dqueue* dirQueue;
    struct fqueue* fileQueue;
    struct fileStruct* fileHead; //head of all files
} threadArgs;


void tokenize(fileStruct* fileStruct);
void* fileHandler(void* args);
void* dirHandler(void* args);
int getFileSize(int fd);
void insertWord(char* word, fileStruct* fileStruct);
int getNumWords(char* fileName, fileStruct* fileStruct);
int isFile(char *name);
int isDir(char *name);