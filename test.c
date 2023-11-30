#include <assert.h>
#include "src/bad_case.h"

int main() {
    short test00 = test0902(0,0,92);
    short test01 = test0902(0,1,91);
    short test10 = test0902(1,0,90);
    short test11 = test0902(1,1,89);
    short test20 = test0902(2,0,88);
    short test21 = test0902(2,1,87);

    assert(test00 == (short)92);
    assert(test01 == (short)91);
    assert(test10 == (short)90);
    assert(test11 == (short)89);
    assert(test20 == (short)88);
    assert(test21 == (short)87);

    assert(test1603(1) == 2);
    assert(test1603(2) == 3);
    assert(test1603(3) == 2);
    assert(test1603(4) == 4);
    
    UInt64 res = test0703();
    assert(res == (UInt64)750L);
    return 0;
}