compare: compare.o dqueue.o fqueue.o
	gcc -g -std=gnu99 -Wvla -Wall -fsanitize=address,undefined -pthread -o compare dqueue.o fqueue.o compare.o -lm

compare.o: compare.c compare.h
	gcc -c -g -std=gnu99 -Wvla -Wall -fsanitize=address,undefined compare.c -pthread

dqueue.o: dqueue.c dqueue.h
	gcc -c -g -std=gnu99 -Wvla -Wall -fsanitize=address,undefined dqueue.c -pthread

fqueue.o: fqueue.c fqueue.h
	gcc -c -g -std=gnu99 -Wvla -Wall -fsanitize=address,undefined fqueue.c -pthread

clean:
	rm -f *.o test
