#include "bad_case.h"

UInt64 test0703(void){
   long a_l = 100L;
   long b_l = 100L; /* Fixed */
   UInt64 c_ul = 200L;
   UInt64 d_ul = 350L;
   UInt64 sum = (UInt64)a_l + (UInt64)b_l + c_ul + d_ul;
   return sum;
}
