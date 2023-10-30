CC:=gcc
CFLAGS:=
LDFLAGS:=

all: test

test: test.o Calculator.o
	$(CC) $(CFLAGS) test.o Calculator.o -o test $(LDFLAGS)

Calculator.o: src/Calculator.c
	$(CC) $(CFLAGS) -o Calculator.o -c src/Calculator.c

test.o: test.c
	$(CC) $(CFLAGS) -o test.o -c test.c

clean:
	rm -f *.gcov
	rm -f *.gcda *.gcno
	rm -f *.o
	rm -f test
