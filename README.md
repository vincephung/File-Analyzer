Vincent Phung vcp34
William McFarland wmm53

Testing collection phase:
	-Handling input:
                To make sure that we handled user input correctly, we used a variety of test cases where the user entered invalid input such as -x or an invalid argument type (negative int) for threads.
	-Testing Queues:
                The first thing to check in this phase were both queues. To make sure that our file/directory queues were working properly, we first had to make sure that enqueue and dequeue were working properly. The file queue was bounded, so we made sure that the queue was locking when the capacity was reached and locking when the queue size was 0. We also had to test that the locks were unlocking after an enqueue/dequeue. The test cases we used were just simply enqueuing and dequeuing strings.
	-Testing tokenizer:
                To test computing the WFD, we had to make sure that tokenizer was working correctly and that the WFD structure was valid. We used test cases that included non-alphanumeric characters, variations of words that contained upper and lowercase characters etc.
    -Testing threads:
                To make sure that multiple threads were actually running, we had print statements inside of the thread function handlers that would print the working thread’s id. We used various amounts of threads to make sure that the program would work with any number of threads, ex. Including arguments such as -d4 -f2.

Testing analysis phase:
	-Testing correct WFD:
		-used assert() to make sure the sum of all the WFDs for each file was equal to 1 before continuing to analysis Phase
	-Testing initPairs():
		-Made sure the array of jsdStruct pointers was allocated and freed properly by utilizing AddressSanitizer, UBSan, and gdb
	-Testing Analysis threads:
		-used gdb to make sure the right number of threads were being started and joined
		-tried file different numbers and analysis thread numbers that led to even and uneven allocation of file comparisons among threads
		-used > and cmp on the terminal to make sure output was consistent regardless of the number of threads
	-Testing jsd computation:
		-created small scale test cases that could easily be done by hand and compared manual calculation with programm output
		-made sure to test edge cases where files had no similarity, one of the files were blank, both files were blank, or both files were exactly the same
		-tested comparisons between large txt files found on the internet and made sure program output was consistent
	-Memory Leaks:
		-all memory leak testing utilized AddressSanitizer and UBSan
