#include "bad_case.h"

UInt64 test0703(void){
   long a_l = 100L;
   long b_l = 100L; // Corrected suffix for compliance
   UInt64 c_ul = 200LU; // Added U for unsigned literal and used uppercase L for compliance 
   UInt64 d_ul = 350LU; // Added U for unsigned literal and used uppercase L for compliance
   UInt64 sum = (UInt64)a_l + (UInt64)b_l + c_ul + d_ul;
   return sum;
}
