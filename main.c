#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>
#include <unistd.h>
#include <string.h>

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

//method to calculate WFD

//method to recursively traverse directory

//method to handle JSD

/*
Given a file descriptor, tokenize the file by
reading the file and determing what words it contains
A word is defined by a sequence of characters including: letters,numbers and dash(hypen)
*/
void tokenize(int fd, fileStruct* fileStruct){
    int fileSize = getFileSize(fd);
    char buf[fileSize];

    read(fd,buf,fileSize);

    int start = 0; //beginning of current word
    int end = 0; //end of current word;

    //loop through buffer and get words
    for(int i = 0; i < fileSize; i++){
        char curLetter = buf[i];
        curLetter = tolower(curLetter);

        //Only tokens allowed are letters, numbers, dash and spaces
        if(!isalpha(curLetter) && (curLetter != '-') && !isspace(curLetter)){
            continue;
        }

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

            int wordLength = end - start + 1; //+1 for null term

            //check if malloc fialed
            char* word = malloc(wordLength);

            //Initialize word with letters from buffer
            for(int j = 0; j < wordLength-1;j++){
                word[j] = buf[start+j]; 
            }
            word[wordLength-1] = '\0';
            start = i+1;
            
            insertWord(word,fileStruct);
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


int main(int argc, char** argv){

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

    return 1;

//FREE all structs and mallocs

}