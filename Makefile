CC:=gcc
CFLAGS:=
LDFLAGS:=

all: test

test: test.o bad_case_1.o bad_case_2.o bad_case_3.o
	$(CC) $(CFLAGS) test.o bad_case_1.o bad_case_2.o bad_case_3.o -o test $(LDFLAGS)

bad_case_1.o: bad_case_1.c 
	$(CC) $(CFLAGS) -o bad_case_1.o -c bad_case_1.c

bad_case_2.o: bad_case_2.c 
	$(CC) $(CFLAGS) -o bad_case_2.o -c bad_case_2.c

bad_case_3.o: bad_case_3.c 
	$(CC) $(CFLAGS) -o bad_case_3.o -c bad_case_3.c

test.o: test.c
	$(CC) $(CFLAGS) -o test.o -c test.c

clean:
	rm -f *.o
	rm -f test