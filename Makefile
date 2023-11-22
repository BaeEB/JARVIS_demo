CC:=gcc
CFLAGS:=
LDFLAGS:=

all: test

test: test.o src/bad_case_1.o src/bad_case_2.o src/bad_case_3.o
	$(CC) $(CFLAGS) test.o src/bad_case_1.o src/bad_case_2.o src/bad_case_3.o -o test $(LDFLAGS)

src/bad_case_1.o: src/bad_case_1.c 
	$(CC) $(CFLAGS) -o src/bad_case_1.o -c src/bad_case_1.c

src/bad_case_2.o: src/bad_case_2.c 
	$(CC) $(CFLAGS) -o src/bad_case_2.o -c src/bad_case_2.c

src/bad_case_3.o: src/bad_case_3.c 
	$(CC) $(CFLAGS) -o src/bad_case_3.o -c src/bad_case_3.c

test.o: test.c
	$(CC) $(CFLAGS) -o test.o -c test.c

clean:
	rm -f src/*.o
	rm -f test