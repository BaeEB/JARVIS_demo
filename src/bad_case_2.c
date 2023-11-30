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
        // Fall through to default is intentional here - no break is needed
    default:
        // Default case intentionally empty
        break;
    }

    return x;
}
