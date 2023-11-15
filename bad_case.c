#include "bad_case.h"

int test0301(int ap, int bp, int cp){
    int ai = ap;
    int bi = bp;
    int ci = cp;
    int sum = ai
            + bi /* sum of ai and bi */
            + ci
            ;
    return sum;
}
