#include "bad_case.h"

int test0301(int ap, int bp, int cp){
	int ai = ap;
	int bi = bp;
	int ci = cp;
	int sum = ai
             + bi /* Removed illegal comment sequences to comply with MISRA_C_2012_03_01 */
             + ci
             ;
    return sum;
}
