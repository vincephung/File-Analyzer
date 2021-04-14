//Holds all words in a file with their mappings
typedef struct wordMap{
    int freq;
    char* word;
    struct wordMap* next;
} wordMap;

//WFD information about all files
typedef struct fileStruct{
    char* fileName;
    int numTokens; //number of tokens in the file
    wordMap* words; //the words inside this file
    struct fileStruct* next;
} fileStruct;

//Arguments for file to be passed into tokenize
typedef struct fileArgs{
    int fd; //file descriptor
    struct fileStruct* file; //First file in the WFD struct
} fileArgs;


//struct to hold directory queue
typedef struct directoryQueue{

} directoryQueue;

//struct to hold file queue
typedef struct fileQueue{

} fileQueue;

void tokenize(int fd, fileStruct* fileStruct);
int getFileSize(int fd);
void insertWord(char* word, fileStruct* fileStruct);
int getNumWords(char* fileName, fileStruct* fileStruct);
int isFile(char *name);
int isDir(char *name);