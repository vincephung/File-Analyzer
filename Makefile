test: test.o  fqueue.o
	gcc -g -std=gnu99 -Wvla -Wall -fsanitize=address,undefined -o test test.o fqueue.o -pthread

test.o: test.c fqueue.h
	gcc -c -g -std=gnu99 -Wvla -Wall -fsanitize=address,undefined test.c -pthread

dstack.o: dstack.c dstack.h
	gcc -c -g -std=gnu99 -Wvla -Wall -fsanitize=address,undefined dstack.c -pthread

fqueue.o: fqueue.c fqueue.h
	gcc -c -g -std=gnu99 -Wvla -Wall -fsanitize=address,undefined fqueue.c -pthread


clean:
	rm -f *.o test
