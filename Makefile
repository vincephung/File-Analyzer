test: test.o  dstack.o
	gcc -g -std=gnu99 -Wvla -Wall -fsanitize=address,undefined -o test test.o dstack.o -pthread

test.o: test.c dstack.h
	gcc -c -g -std=gnu99 -Wvla -Wall -fsanitize=address,undefined test.c -pthread

dstack.o: dstack.c dstack.h
	gcc -c -g -std=gnu99 -Wvla -Wall -fsanitize=address,undefined dstack.c -pthread

clean:
	rm -f *.o test
