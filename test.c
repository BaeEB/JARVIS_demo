#include <stdio.h>
#include <stdlib.h>
//#include "src/ProductionCode.h"
// #include "src/BranchChecker_avl.h"
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


int main(int argc, char *argv[]) {
#ifdef GCOV
	  {
		  dpp_gcov_sigaction.sa_handler = dpp_sighandler;
		  sigemptyset(&dpp_gcov_sigaction.sa_mask);
		  dpp_gcov_sigaction.sa_flags = 0;
		  sigaction(SIGSEGV, &dpp_gcov_sigaction, &dpp_orig_sigaction);
		  sigaction(SIGFPE, &dpp_gcov_sigaction, &dpp_orig_sigaction);
		  sigaction(SIGABRT, &dpp_gcov_sigaction, &dpp_orig_sigaction);
	  }
#endif
    int test_case = atoi(argv[1]);
    int test_index = atoi(argv[2]); //e.q. ./test 2 3
    bool compare = true;

    switch(test_case) {
        case 6: ;
            struct Node* root = NULL;

            for (int i = 0; i < 11; i++) {
                // root = Insert(root, input6_1[test_index][i]);
            }

            for (int i = 0; i < 3; i++) {
                // root = Delete(root, input6_2[test_index][i]);
            }

            // int* actual_output6 = getInorder(root);

            for (int i = 0; i < 11; i++) {
                // printf("%d, ", actual_output6[i]);
            }
            int i = 0;
            bool compare = true;
            for (int i = 0; i < 11; i++) {
                // printf("Actual: %d  Expected: %d\n", actual_output6[i], expected_output6[test_index][i]);
                // if (actual_output6[i] != expected_output6[test_index][i]) {

                //     compare = false;
                //     break;
                // }
            }
            if (compare) {
                printf("PASSED\n");
            }
            else {
                printf("FAILED\n");
            }
            return compare == true ? 0 : 1;

    }
    return 0;


}
