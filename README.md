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
