#include <assert.h>
#include "bad_case.h"

int main() {
    int a = 1;
    int b = 2;
    int c = 3;

    int res = test0301(a, b, c);

    assert(res == 6);
	
	return 0;
}
