#include <assert.h>
#include "bad_case.h"

/*******************************************/
/* Assuming that the function test0301 is defined like this:
int test0301(int var1, int var2, int var3);

The issue here is that the function call in main does not match the 
function prototype (which is not provided here), which violates
MISRA C:2012 Rule 8.2. We need to ensure that the function is declared
with correct prototype before calling it in main. */

/* Correct function prototype declaration */
int test0301(int var1, int var2, int var3);

int main(void) { /* main must return int and specify void if it takes no parameters */
    int a = 1;
    int b = 2;
    int c = 3;
    
    /* Since test0301 accepts three parameters, the call is now compliant */
    int res = test0301(a, b, c);
    
    assert(res == 6);
    
    return 0;
}
/*******************************************/
