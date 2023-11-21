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
        // Fall-through is intentional to default case
    default:
        break; // Compliant with MISRA_C_2012_16_03
    }

    return x;
}
