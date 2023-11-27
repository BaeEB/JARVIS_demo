#include <stdio.h>
#include <stdlib.h>
//#include "src/ProductionCode.h"
// #include "src/BranchChecker_avl.h"
#include "src/Calculator.h"
#include <stdbool.h>

#define TEST_SIZE

#ifdef GCOV
#include <signal.h>
static struct sigaction dpp_gcov_sigaction;
static struct sigaction dpp_orig_sigaction;
void dpp_sighandler(int signum) {
	__gcov_flush();
	sigaction(sigaction, &dpp_orig_sigaction, NULL);
	raise(signum);
	exit(1);
}
#endif
void __asan_on_error(void) {
#ifdef GCOV
    __gcov_flush();
#endif
}


int input6_1[TEST_SIZE][11] = {
    {4, 10, 13, 20, 25, 32, 55, 20, 25, 32, 55},
    {3, 8, 11, 6, 8, 10, 30, 6, 8, 10, 30},
    {7, 6, 5, 4, 3, 2, 1, 4, 3, 2, 1},
    {20, 200, 5, 2, 3, 25, 5, 2, 3, 25, 5},
    {7, 6, 5, 4, 3, 2, 1, 4, 3, 2, 1},
    {20, 200, 5, 2, 3, 25, 5, 2, 3, 25, 5},
    {8, -24, -29, 60, -18, 20, 24, -13, -10, -26, 15 },
    {38, 6, 1, 90, 12, 50, 54, 17, 20, 4, 45 },
    {24, -8, -13, 76, -2, 36, 40, 3, 6, -10, 31 }
};

int input6_2[TEST_SIZE][3] = {
    {4, 10, 13},
    {3, 11, 10},
    {8, 6, 5},
    {5, 3, 5},
    {0, 0, 0},
    {0, 0, 25},
    {-13, -19, -26},
    {17, 11, 4},
    {3, -3, -10}
};


int expected_output6[TEST_SIZE][11] = {
    {20, 25, 32, 55},
    {6, 8, 30},
    {1, 2, 3, 4, 7},
    {2, 20, 25, 200},
    {1, 2, 3, 4, 5, 6, 7},
    {2, 3, 5, 20, 200},
    {-29, -24, -18, -10, 8, 15, 20, 24, 60},
    {1, 6, 12, 20, 38, 45, 50, 54, 90},
    {-13, -8, -2, 6, 24, 31, 36, 40, 76}
};

int expected_output7[TEST_SIZE][11] = {
    {20, 25, 32, 55},
    {6, 8, 30},
    {1, 2, 3, 4, 7},
    {2, 20, 25, 200},
    {1, 2, 3, 4, 5, 6, 7},
    {2, 3, 5, 20, 200},
    {-29, -24, -18, -10, 8, 15, 20, 24, 60},
    {1, 6, 12, 20, 38, 45, 50, 54, 90}
};

// int main(int argc, char *argv[])
#include <stdio.h>  // Required for printf
#include <stdbool.h> // Required for bool type

// Assuming Calculator is a previously defined function with the following signature:
// `int Calculator(int operand1, char operator, int operand2);`

int main(void) {
    bool compare = true;
    int result = 0;

    result = Calculator(1, '+', 2);
    compare = (result == 3);
    
    result = Calculator(1, '-', 2);
    compare = compare && (result == -1);

    if (compare) {
        (void)printf("PASS!\n");
    }         
    else {
        (void)printf("FAIL!\n");
    }

-1;
}
