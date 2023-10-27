#include "bad_case.h"

int test0301(int ap, int bp, int cp){
	int ai = ap;
	int bi = bp;
	int ci = cp;
	int sum = ai   /* MISRA_C_2012_03_01 */
            + bi 
            + ci
            ;
    return sum;
}
