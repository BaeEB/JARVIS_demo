#include "bad_case.h"

UInt64 test0703(void){
   long a_l = 100L;     // Compliant
   long b_l = 100L;     // Corrected from 100l to 100L to be compliant
   UInt64 c_ul = 200UL; // Corrected from 200L to 200UL to indicate unsigned long
   UInt64 d_ul = 350UL; // Corrected from 350L to 350UL to indicate unsigned long
   UInt64 sum = (UInt64)a_l + (UInt64)b_l + c_ul + d_ul;
   return sum;
}
