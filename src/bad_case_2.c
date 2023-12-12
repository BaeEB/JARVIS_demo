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
        // Fall through - intentional, no further action needed
    default:
        break;
    }

    return x;
}
