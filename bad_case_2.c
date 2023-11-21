#include "bad_case.h"

int test1603(int x)
{
    switch(x)
    {
    case 1:
    case 2:
        x++;
        break;
    case 3:    
        x--;
        break; /* Compliant: Add break to terminate the case */
    default:
        break; /* Compliant: Add break to ensure proper termination of the default switch-clause */
    }

    return x;
}
