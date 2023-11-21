#include "bad_case.h"

UInt64 test0703(void){
   long a_l = 100L;
   long b_l = 100L; // Corrected: Replaced '100l' with '100L' to comply with MISRA_C_2012_07_03
   UInt64 c_ul = 200UL; // Corrected: Added 'U' to indicate unsigned long suffix
   UInt64 d_ul = 350UL; // Corrected: Added 'U' to indicate unsigned long suffix
   UInt64 sum = (UInt64)a_l + (UInt64)b_l + c_ul + d_ul;
   return sum;
}
