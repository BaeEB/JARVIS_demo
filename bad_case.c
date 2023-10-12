#include "bad_case.h"

int test0301(int ap, int bp, int cp){
    int ai = ap;
    int bi = bp;
    int ci = cp;
    int sum = ai   /* start the comment line */
        + bi 
        + ci
            /* end commenting here */
        ;
    return sum;
}
