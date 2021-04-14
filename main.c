#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include "main.h"

//method to calculate WFD

//method to recursively traverse directory

//method to handle JSD

//file handler
void* fileHandler(void* args){
    tokenize(args->fd,args->file);
}

/*
Given a file descriptor, tokenize the file by
reading the file and determing what words it contains
A word is defined by a sequence of characters including: letters,numbers and dash(hypen)
*/
void tokenize(int fd, fileStruct* file){
    int fileSize = getFileSize(fd);
    char buf[fileSize];

    read(fd,buf,fileSize);

    int start = 0; //beginning index of current word
    int end = 0; //end index of current word;

    //loop through buffer and get words
    for(int i = 0; i < fileSize; i++){
        char curLetter = buf[i];
        curLetter = tolower(curLetter);

    /*
        //Only tokens allowed are letters, numbers, dash and spaces
        if(!isalpha(curLetter) && (curLetter != '-') && !isspace(curLetter)){
            continue;
        }
    */

        //handle duplicate spaces like hi <space><space> there
        if(isspace(buf[start])){
            start = i+1;
            continue;
        }

        //space denotes end of word
        //fileSize -1 means end of file
        if(isspace(curLetter) || i == fileSize-1){
            //i+1 because if file was: "hi there", i would be at "e"
            end = ( i== fileSize-1) ? i+1 : i;

            int maxWordLength = end - start + 1; //+1 for null term, could be smaller if the input is contains nonalphanumerical or "-"

            //check if malloc fialed
            char* word = malloc(maxWordLength);
            int curWordLength = 0; //length of the current word built.

            //Initialize word with letters from buffer
            for(int j = 0; j < maxWordLength-1;j++){
                //Only tokens allowed are letters, numbers, dash and spaces
                char bufLetter = buf[start+j]; //current letter in the buffer
                
                if(!isalpha(bufLetter) && (bufLetter != '-') && !isspace(bufLetter)){   
                    continue;
                }

                word[curWordLength] = bufLetter; 
                curWordLength++;
            }
            word[curWordLength] = '\0';
            start = i+1;
            
            insertWord(word,file);

            //free(word);

        }
    }
}

//insert word into wordMap in alphabetical order, uses insertion sort
void insertWord(char* word, fileStruct* file){
    wordMap* words = file->words;
    //wordMap* prev = words;
    wordMap* prev = NULL;

    //First word added
    if(words->word == NULL){
        words->word = word;
        words->freq = 1;
        //file->numTokens += 1;
        words->next = NULL;
        return;
    }

    //Iterate through all words in the file
    while(words != NULL){
        //if word already exists, increment frequency/count
        if(strcmp(words->word,word) == 0){
            words->freq += 1;
            //file->numTokens += 1;
            return;
        }else if(strcmp(word,words->word) < 0){ //if input word < current word alphabetically
            wordMap* newWord = malloc(sizeof(wordMap));
            newWord->word = word;
            newWord->freq = 1;
            newWord->next = words;

            //Handle case where newWord becomes the new head
            if(prev == NULL){
                //set head of list to newWord
                file->words = newWord;
            }else{
                prev->next = newWord;
            }
            return;
        }
        
        //if input word > curWord, move onto next word in the file
        prev = words;
        words = words->next;  
         
    }
    
    //If input word is greater than all words in the list, add to the tail
    if(words == NULL){        
        words= malloc(sizeof(wordMap));
        prev->next = words;
        words->freq = 1;
        //file->numTokens += 1;
        words->word = word;
    } 
    //free all words?
}

//Return total number of words in a specific file
int getNumWords(char* fileName, fileStruct* file){
    fileStruct* curFile = file;
    while(curFile != NULL){
        //get info for specific file
        if(strcmp(curFile->fileName,fileName) == 0){
            break;
        }
        curFile = curFile->next;
    }

    if(curFile == NULL){
        //error, this mean file was not found
        //exit(1);
    }
    
    int numWords = 0;
    wordMap* word = curFile->words;
    while(word!= NULL){
        numWords += word->freq;
        word = word->next;
    }

    return numWords;

}

int getFileSize(int fd){
    struct stat data;
    int err = fstat(fd,&data);
    
    if(err){
        //perror
        return 0;
    }
    
    return (int) data.st_size;
}

int isFile(char *name) {
	struct stat data;
	stat(name, &data);
	return S_ISREG(data.st_mode);
}

int isDir(char *name) {
	struct stat data;
	stat(name, &data);
	return S_ISDIR(data.st_mode);
}

int main(int argc, char** argv){


    //Parameters initialized with default values
    int dirThreads = 1;
    int fileThreads = 1;
    int aThreads = 1;
    //char* fileSuffix = ".txt";
    int suffixSize = 4;
    char* fileSuffix = malloc((suffixSize * sizeof(char)+1)); //size 5 for ".txt" default
    strcpy(fileSuffix, ".txt");

    /*
        Handle arguments, regular and optional
    */
    for(int i = 1; i < argc; i++){
        
        //handle optional argument
        if(argv[i][0] == '-'){
            if(strlen(argv[i])!= 3 && argv[i][1] != 's'){
                perror("Missing or invalid argument");
                //halt
            }

            //argument type must be positive number for threads
            if(argv[i][2] <= 0 && argv[i][1] != 's'){
                perror("Invalid argument");
                //halt
            }

            if(argv[i][1] == 'd'){
                dirThreads = argv[i][2] - '0'; //convert char to integer
            }else if(argv[i][1] == 'f'){
                fileThreads = argv[i][2] - '0';
            }else if(argv[i][1] == 'a'){
                aThreads = argv[i][2] - '0';
            }else if(argv[i][1] == 's'){
                //any string after "s" will be the suffix
                //if it is null, then it is the empty string
                int wordIndex = 0; //index of the new string
                char* tempSuffix = malloc(strlen(argv[i])+1);
                
                //Get the string from user input, ex. "-stest" puts string "test" into tempSuffix
                for(int j = 2; j < strlen(argv[i]); j++){
                    tempSuffix[wordIndex] = argv[i][j];
                    wordIndex++;
                }
                tempSuffix[wordIndex] = '\0';

                //Copy tempSuffix into fileSuffix
                memset(fileSuffix, '\0', strlen(fileSuffix));
                for(int j = 0; j < wordIndex; j++){
                    if(suffixSize == j){
                        fileSuffix = (char*) realloc(fileSuffix,suffixSize*2);
                    }
                    fileSuffix[j] = tempSuffix[j];
                }
                free(tempSuffix);                
            }
        }else{
            //handle regular arguments (file/directory)
            if(!isFile(argv[i]) && !isDir(argv[i])){
                perror(argv[i]);
            }

            //start file threads
            for(int i = 0; i < fileThreads; i++){
                int fd = open(argv[i],O_RDONLY);
                fileStruct* file = malloc(sizeof (fileStruct));

                file->fileName = argv[i];
                file->numTokens = 0;
                file->words = malloc(sizeof (wordMap));

                fileArgs* tokenArgs = malloc(sizeof(fileArgs));
                tokenArgs->fd = fd;
                tokenArgs->file = file;

                pthread_t tid;
                int err;
                err = pthread_create(&tid,NULL,fileHandler,tokenArgs);
                if(err != 0){
                   // errno = err;
                    perror("pthread_create");
                }
            }

            //start directory threads

            //traverse directories
            //add files to fileQueue
            //add directories to directoryQueue
        }
    }

    //testing input
    printf("dir threads: %d\n",dirThreads);
    printf("file threads: %d\n",fileThreads);
    printf("analysis threads: %d\n",aThreads);
    printf("suffix is: %s\n", fileSuffix);


   int fd = open(argv[1],O_RDONLY);
   fileStruct* file = malloc(sizeof (fileStruct));
      //need to get filepath to name the file , maybe get this with dir
    file->fileName = argv[1];
    file->numTokens = 0;
    file->words = malloc(sizeof (wordMap));


    tokenize(fd,file);

    file->numTokens = getNumWords(file->fileName,file);
    wordMap* words = file->words;

    while(words != NULL){
        printf("%s\n",words->word);
        words = words->next;
    }
    int num = getNumWords(argv[1],file);
    printf("%d\n",num);

    return EXIT_SUCCESS;

    //FREE all structs and mallocs
    free(fileSuffix);

}