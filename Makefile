main: main.o dqueue.o fqueue.o
	gcc -g -std=gnu99 -Wvla -Wall -fsanitize=address,undefined -pthread -o main dqueue.o fqueue.o main.o 

main.o: main.c main.h
	gcc -c -g -std=gnu99 -Wvla -Wall -fsanitize=address,undefined main.c -pthread

test: test.o  dqueue.o
	gcc -g -std=gnu99 -Wvla -Wall -fsanitize=address,undefined -o test test.o dqueue.o -pthread

test.o: test.c dqueue.h
	gcc -c -g -std=gnu99 -Wvla -Wall -fsanitize=address,undefined test.c -pthread

dqueue.o: dqueue.c dqueue.h
	gcc -c -g -std=gnu99 -Wvla -Wall -fsanitize=address,undefined dqueue.c -pthread

fqueue.o: fqueue.c fqueue.h
	gcc -c -g -std=gnu99 -Wvla -Wall -fsanitize=address,undefined fqueue.c -pthread

clean:
	rm -f *.o test
