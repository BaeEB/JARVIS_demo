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
		break; /* Added break statement for compliance with MISRA C:2012 Rule 16.3 */
	default:
		break; /* Added break statement for compliance with MISRA C:2012 Rule 16.3 */
	}

	return x;
}
