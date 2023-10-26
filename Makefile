CC:=gcc
CFLAGS:=
LDFLAGS:=

all: test

test: test.o BranchChecker_avl.o
	$(CC) $(CFLAGS) test.o BranchChecker_avl.o -o test $(LDFLAGS)

BranchChecker_avl.o: src/BranchChecker_avl.c
	$(CC) $(CFLAGS) -o BranchChecker_avl.o -c src/BranchChecker_avl.c

test.o: test.c
	$(CC) $(CFLAGS) -o test.o -c test.c

clean:
	rm -f *.gcov
	rm -f *.gcda *.gcno
	rm -f *.o
	rm -f test
