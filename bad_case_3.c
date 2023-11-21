#include "bad_case.h"

UInt64 test0703(void){
   long a_l = 100L;
   long b_l = 100L;  /* Compliant with MISRA_C_2012_07_03 */
   UInt64 c_ul = 200UL;  /* Compliant with MISRA_C_2012_07_03 */
   UInt64 d_ul = 350UL;  /* Compliant with MISRA_C_2012_07_03 */
   UInt64 sum = (UInt64)a_l + (UInt64)b_l + c_ul + d_ul;
   return sum;
}
