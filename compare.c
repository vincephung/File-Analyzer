#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <dirent.h>
#include <math.h>
#include "compare.h"
#include "dqueue.h"
#include "fqueue.h"


//add running word count to method and return
//Gets a file struct, reads the file separates the words
void tokenize(fileStruct* file){
    int fd = open(file->fileName,O_RDONLY);
    int fileSize = getFileSize(fd);

    //char buf[fileSize];
    char* buf = malloc(fileSize * sizeof(char));
    read(fd,buf,fileSize);

    int start = 0; //beginning index of current word
    int end = 0; //end index of current word;

    //loop through buffer and get words
    for(int i = 0; i < fileSize; i++){
        buf[i] = tolower(buf[i]);
        char curLetter = buf[i];

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
                
                if(!isalnum(bufLetter) && (bufLetter != '-') && !isspace(bufLetter)){   
                    continue;
                }

                word[curWordLength] = bufLetter; 
                curWordLength++;
            }
            word[curWordLength] = '\0';
            start = i+1;
            
            insertWord(word,file);
	        file->numTokens += 1;

        }
    }
    free(buf);
    close(fd);
}

//insert word into wordMap in alphabetical order, uses insertion sort
void insertWord(char* word, fileStruct* file){
    wordMap* words = file->words;
    wordMap* prev = NULL;

    //First word added
    if(words->word == NULL){
        words->word = word;
        words->freq = 1;
        words->next = NULL;
        return;
    }
    
    //Iterate through all words in the file
    while(words != NULL){
        //if word already exists, increment frequency/count
        if(strcmp(words->word,word) == 0){
            words->freq += 1;   
            free(word);         
            return;
        }else if(strcmp(word,words->word) < 0){ //if input word < current word alphabetically
            wordMap* newWord = malloc(sizeof(wordMap)); //create a new word
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
        words->word = word;
        words->next = NULL;
    } 
    
}

//function to initialize wfd field in file struct
void initWFD(struct fileStruct* file){
	struct wordMap* crnt = file->words;
	while(crnt != NULL){
		crnt->wfd = (double)(crnt->freq)/(double)(file->numTokens);
		crnt = crnt->next;
	}
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

//function to calculate kld
double compute_kld(double wfd, double mean){
	return wfd * log2(wfd/mean);
}

//calculates the JSD for a specified portion of the WFD//
void* analysisPhase(void* args){
	struct aArgs* a = (struct aArgs*)args; 
	struct jsdStruct** array = a->array;
	int start = a->start;
	int end = a->end;
	//iterate through array
	for(int i=start; i<end; i++){
		double kld1 = 0;
		double kld2 = 0;
		struct jsdStruct* crntStruct = array[i];
		struct wordMap* word1 = crntStruct->file1->words;
		struct wordMap* word2 = crntStruct->file2->words;
		while(word1 != NULL && word2 != NULL){
			//compare words to see if same
			if(strcmp(word1->word, word2->word) == 0){
				//compute mean
				double mean = .5 * (word1->wfd + word2->wfd);
				//compute kld for each file
				kld1 += compute_kld(word1->wfd, mean);
				kld2 += compute_kld(word2->wfd, mean);
				//increment same word count
				crntStruct->combined++;
				//increment both words
				word1 = word1->next;
				word2 = word2->next;
			}
			else if(strcmp(word1->word, word2->word) < 0){
				//word1 comes before word2
				//compute mean
				double mean = .5 * word1->wfd;
				//compute kld1 since kld2 will be 0
				kld1 += compute_kld(word1->wfd, mean);
				//increment word1 only
				word1 = word1->next;
			}
			else{
				//word2 comes before word1
				//compute mean
				double mean = .5 * word2->wfd;
				//compute kld2 since kld1 will be 0
				kld2 += compute_kld(word2->wfd, mean);
				//increment word2 only
				word2 = word2->next;
			}
		}
		//check to see if files aren't same size
		struct wordMap* crnt;
		if(word1 == NULL && word2 != NULL){
			//compute mean
			double mean = .5 * word2->wfd;
			//compute kld2
			kld2 += compute_kld(word2->wfd, mean);
			//increment word2 only
			word2 = word2->next;
		}
		else if(word1 != NULL && word2 == NULL){
			//compute mean
			double mean = .5 * word1->wfd;
			//compute kld2
			kld1 += compute_kld(word1->wfd, mean);
			//increment word2 only
			word1 = word1->next;
		}
		//compute jsd
		double jsd = sqrt(.5 * kld1  + .5 * kld2);
		crntStruct->jsd = jsd;
	}

    return NULL;
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
    int err = stat(name, &data);
    if(err){
        //perror(name);
        //this means not a file
        return 0;
    }
    
	return S_ISREG(data.st_mode);
}

int isDir(char *name) {
	struct stat data;
    int err = stat(name, &data);
    if(err){
        //perror(name);
        //this means not a directoy
        return 0;
    }
	return S_ISDIR(data.st_mode);
}

//file handler
void* fileHandler(void* args){
    threadArgs* tArgs = (threadArgs*)args;
    fqueue* fileQueue = tArgs->fileQueue;
    dqueue* dirQueue = tArgs->dirQueue;
    fileStruct* fileHead = tArgs->fileHead;
    int* numFiles = tArgs->numFiles;

    //while loop keeps trying to dequeue from file queue
    //repeat until the queue is empty and the directory threads have stopped
    //might be && not ||
    //while(fileQueue->count != 0 || dirQueue->active != 0){
    
    do{
        char* fileName = fDequeue(fileQueue);

        // No file, so SKIP, happens when more threads needed than files
        if(fileName == NULL){
            return NULL; 
        }

        //Need to lock when appending to shared linked list of files
        pthread_mutex_lock(&tArgs->lock);

        //get tail of file list
        fileStruct* file = fileHead;
        fileStruct* prev = NULL;
        while(file != NULL){
            prev = file;
            file = file->next;
        }

        //create new fileStruct 
        file = malloc(sizeof (fileStruct));
        
        //append new file it to the list
        prev->next = file;

        file->fileName = fileName;
        file->numTokens = 0;
        file->next = NULL;
        file->words = malloc(sizeof (wordMap));
        file->words->word = NULL; //initialize word

	//increment file counter
	(*numFiles)++;

        pthread_mutex_unlock(&tArgs->lock);

        //tokenize words in the file
        tokenize(file);        
        initWFD(file);
       // free(fileName);
    }while(fileQueue->count != 0 || dirQueue->active != 0);

    return 0;
}

//directory handler
void* dirHandler(void* args){
    threadArgs* tArgs = (threadArgs*)args;
    fqueue* fileQueue = tArgs->fileQueue;
    dqueue* dirQueue = tArgs->dirQueue;
    char* suffix = tArgs->suffix;

    //while loop keeps trying to dequeue from directory queue
    while(dirQueue->count != 0 || dirQueue->active != 0){
        char* dirName = dDequeue(dirQueue);

        //empty queue?
        if(dirName == NULL){
            return NULL;
        }

        DIR *dirp = opendir(dirName);
        if(!dirp){
            //handle directory error
            perror(dirName);
            //EXIT_FAILURE =;
            closedir(dirp);
            continue;
        }

        //Concat path everytime you go inside a new directory
        char* parentPath = malloc(strlen(dirName) +2);
        strcpy(parentPath,dirName);
        strcat(parentPath,"/");

        struct dirent *de;
        while((de = readdir(dirp))){
            //ignore entries whose name begins with a period
            if(de->d_name[0] == '.'){
                continue; 
            }
           
            char* newPath = malloc(strlen(parentPath) + strlen(de->d_name) +1);
            strcpy(newPath,parentPath);
            strcat(newPath,de->d_name);

            if(de->d_type == DT_DIR){
                //enqueue subdirectory
                char* newDir = malloc(strlen(newPath)+1);
                strcpy(newDir,newPath);
                dEnqueue(dirQueue,newDir);
                free(newDir);
            }else if(de->d_type == DT_REG){
                //compare suffix
                int pathLength = strlen(newPath);
                int suffixLen = strlen(suffix);
                 //start of string to compare to
                int startLen = pathLength - suffixLen;

                //if suffix matches, enqueue
                if(strncmp(newPath+startLen,suffix,suffixLen)==0){
                    //enqueue file
                    char* newFile = malloc(strlen(newPath)+1);
                    strcpy(newFile,newPath);
                    fEnqueue(fileQueue,newFile);
                    free(newFile);
                }
            }
            free(newPath);
        }
        free(parentPath);
        free(dirName);
        closedir(dirp);
    }
    
    return 0;
}


//init jsd struct array with all file pairs
void initPairs(fileStruct* f, struct jsdStruct** array){
	struct fileStruct* crnt = f->next;
	int count = 0;
	while(crnt != NULL){
		struct fileStruct* tmp = crnt->next;
		while(tmp != NULL){
			array[count] = malloc(sizeof(struct jsdStruct));
			array[count]->file1 = crnt;
			array[count]->file2 = tmp;
            		array[count]->combined = 0;
			array[count]->jsd = 0; 
			count++;
			tmp = tmp->next;
		}
		crnt = crnt->next;
	}
}

int cmpfunc (const void * a, const void * b){
	return ((struct jsdStruct*)a)->combined - ((struct jsdStruct*)b)->combined;
}

int main(int argc, char** argv){
    //Parameters initialized with default values
    int dirThreads = 1;
    int fileThreads = 1;
    int aThreads = 1;
    int suffixSize = 4;
    char* fileSuffix = malloc((suffixSize * sizeof(char)+1)); //size 5 for ".txt" default
    strcpy(fileSuffix, ".txt");

    //Handle arguments, regular and optional
    for(int i = 1; i < argc; i++){
        //handle optional argument
        if(argv[i][0] == '-'){
            //get the second part of the parameter ex. -dN (get the N part)
            int wordIndex = 0; //index of the new string
            char* argRemainder = malloc(strlen(argv[i])+1);
            
            //Get the string from user input, ex. "-stest" puts string "test" into tempSuffix
            for(int j = 2; j < strlen(argv[i]); j++){
                argRemainder[wordIndex] = argv[i][j];
                wordIndex++;
            }
            argRemainder[wordIndex] = '\0';
            
            //handles missing arguments for threads
            if(strlen(argRemainder) == 0 && argv[i][1] != 's'){
                fprintf(stderr,"Error: Invalid or missing argument\n");
                free(argRemainder);
                free(fileSuffix);
                return EXIT_FAILURE;
            }

            //argument type must be positive number for threads
            if(argv[i][1] != 's'){
                int nonDigit = 0;
                for(int i = 0; i < strlen(argRemainder);i++){
                    if(!isdigit(argRemainder[i])){
                        nonDigit = 1;
                        break;
                    }
                }
                //if arg is not a digit or is not positive
                if(nonDigit || atoi(argRemainder) <= 0){
                    fprintf(stderr,"Error: threads must be a positive integer\n");
                    free(fileSuffix);
                    free(argRemainder);
                    return EXIT_FAILURE;
                }
            }
        
            if(argv[i][1] == 'd'){
                dirThreads = atoi(argRemainder); //convert char to integer
            }else if(argv[i][1] == 'f'){
                fileThreads = atoi(argRemainder);
            }else if(argv[i][1] == 'a'){
                aThreads = atoi(argRemainder);
            }else if(argv[i][1] == 's'){
                //Copy argRemainder into fileSuffix
                //if it is null, then it is the empty string
                memset(fileSuffix, '\0', strlen(fileSuffix));
                for(int j = 0; j < wordIndex; j++){
                    if(suffixSize == j){
                        fileSuffix = (char*) realloc(fileSuffix,suffixSize*2);
                        strcpy(fileSuffix,argRemainder);
                    }
                    fileSuffix[j] = argRemainder[j];
                }             
            }else{
                //"Not one of the optional argument types" ex "-x or -z"
                fprintf(stderr,"Error: Missing or invalid argument\n");
                free(fileSuffix);
                free(argRemainder);
                return EXIT_FAILURE;
            }
            free(argRemainder);
        }
    }
    
    //create and initialize queues
    dqueue* dirQueue = malloc(sizeof(dqueue));
    fqueue* fileQueue = malloc(sizeof(fqueue));

/*
   Consider "main" as a directory thread because it is adding from the argument list.
    dInit(dirQueue,dirThreads,dirThreads+1);
*/

    dInit(dirQueue,dirThreads,dirThreads);
    fInit(fileQueue,fileThreads);
    
    //init file counter
    int a = 0;

     //struct that contains both queues
    threadArgs* tArgs = malloc(sizeof (threadArgs));
    tArgs->dirQueue = dirQueue;
    tArgs->fileQueue = fileQueue;
    tArgs->suffix = fileSuffix;
    tArgs->numFiles = &a;
    pthread_mutex_init(&tArgs->lock, NULL);

    //Creates the head of the fileStruct (dummyHead) set to null
    fileStruct* fileHead = malloc(sizeof(fileStruct));
    fileHead->next = NULL;
    tArgs->fileHead = fileHead;

    //start file threads
    pthread_t* fTids = malloc(sizeof(pthread_t) * fileThreads);
    for(int i = 0; i < fileThreads; i++){
        int err;
        err = pthread_create(&fTids[i],NULL,fileHandler,(void*)tArgs);
        if(err != 0){
            // errno = err;
            perror("pthread_create");
        }
    }

    //enqueue file and directory queues
    for(int i = 1; i < argc; i++){
        if(isFile(argv[i])){
            fEnqueue(fileQueue,argv[i]);
        }else if(isDir(argv[i])){
            dEnqueue(dirQueue,argv[i]);
        }
    }

    //start directory threads
    pthread_t* dTids = malloc(sizeof(pthread_t) * dirThreads);

    for(int i = 0; i < dirThreads; i++){
        int err;
        err = pthread_create(&dTids[i],NULL,dirHandler,(void*)tArgs);
        if(err != 0){
            // errno = err;
            perror("pthread_create");
        }
    }

    //Once main thread adds all argument directories, remove it from the active thread list
    //dirQueue->active--;

    //join directory threads
    for(int i = 0; i < dirThreads; i++){
        pthread_join(dTids[i],NULL);        
    }

    //if directory threads are done, file queue can close
    if(dirQueue->active == 0){
        fClose(fileQueue);
    }
    
    //join file threads
    for(int i = 0; i < fileThreads;i++){
        pthread_join(fTids[i],NULL);
    }

    //free queues
    dDestroy(dirQueue);
    fDestroy(fileQueue);
    free(dirQueue);
    free(fileQueue);
    
	//allocate pair array
	int numFiles = *tArgs->numFiles;
	int pairNum = .5 * (numFiles) * (numFiles-1);
	struct jsdStruct** result = malloc(sizeof(struct jsdStruct*) * pairNum);
	//initialize pair array
	initPairs(fileHead, result);

	//divide work based on number of threads
	int perThread = pairNum / aThreads;

	//start analysis threads
    	pthread_t* aTids = malloc(sizeof(pthread_t) * aThreads);
	int index = 0;
/*	 struct aArgs* args = malloc(sizeof(struct aArgs));
	args->array = result;
	args->start = index;
	index += perThread;
	args->end = index;

	*analysisPhase((void*)args);*/
	for(int i=0; i<aThreads; i++){
	        struct aArgs* args = malloc(sizeof(struct aArgs));
		args->array = result;
		args->start = index;
		index += perThread;
		args->end = index; 
		int err;
       		err = pthread_create(&aTids[i],NULL,analysisPhase,(void*)args);
        	if(err != 0){
            		// errno = err;
            		perror("pthread_create");
       		 }
	}

	//join analysis threads
    	for(int i = 0; i < aThreads; i++){
        	pthread_join(aTids[i],NULL);        
   	 }

	//sort the results
	qsort(result, pairNum, sizeof(struct jsdStruct*), cmpfunc);

	//print results
	for(int i=0; i<pairNum; i++){
		printf("%s %s %d %f\n", result[i]->file1->fileName, result[i]->file2->fileName, result[i]->combined, result[i]->jsd);
	}
	//test to make sure working
	printf("%d\n", pairNum);
	for(int i=0; i<pairNum; i++){
		printf("%s %s %f % d\n", result[i]->file1->fileName, result[i]->file2->fileName, result[i]->jsd, result[i]->combined);
	}

    //testing input
    printf("dir threads: %d\n",dirThreads);
    printf("file threads: %d\n",fileThreads);
    printf("analysis threads: %d\n",aThreads);
    printf("suffix is: %s\n", fileSuffix);

    //test printing output
    fileStruct* filePrint = fileHead->next;
    while(filePrint!=NULL){
        wordMap* words = filePrint->words;
        while(words != NULL){
            printf("%s, wfd: %f\n",words->word, words->wfd);
            words = words->next;
        }
        filePrint = filePrint->next;
    }


    //free all files
    fileStruct* filePtr = fileHead->next;
    while(filePtr != NULL){
        wordMap* wordPtr = filePtr->words; 
        while(wordPtr != NULL){
            wordMap* tempWord = wordPtr;
            wordPtr = wordPtr->next;
            free(tempWord->word);
            free(tempWord);
        }
        fileStruct* tempFile = filePtr;
        filePtr = filePtr->next;
        free(tempFile);
    }


    //free thread arguments
    free(fTids);
    free(dTids);
    free(fileHead);
    free(tArgs);
    free(fileSuffix);

    return EXIT_SUCCESS;

}
