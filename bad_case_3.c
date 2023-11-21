#include "bad_case.h"

UInt64 test0703(void){
   long a_l = 100L;
   long b_l = 100L; // Changed 'l' to 'L' to comply with rule MISRA_C_2012_07_03
   UInt64 c_ul = 200L; // This line is compliant
   UInt64 d_ul = 350L; // This line is compliant
   UInt64 sum = (UInt64)a_l + (UInt64)b_l + c_ul + d_ul;
   return sum;
}
