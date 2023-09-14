CC:=gcc
CFLAGS:=
LDFLAGS:=

all: test

test: test.o bad_case.o
	$(CC) $(CFLAGS) test.o bad_case.o -o test $(LDFLAGS)

bad_case.o: bad_case.c
	$(CC) $(CFLAGS) -o bad_case.o -c bad_case.c

test.o: test.c
	$(CC) $(CFLAGS) -o test.o -c test.c

clean:
	rm -f *.o
	rm -f test