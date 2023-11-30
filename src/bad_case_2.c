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
        /* The break statement is required to prevent fall-through from case 3 to default.
           Since there is an empty default clause, the break statement still needs to be added
           to comply with MISRA rules even though it doesn't change execution. */
        break;      
    default:
        /* The default clause is empty, no operation here.
           Included a break for compliance with the MISRA rule. */
        break;
    }

    return x;
}
