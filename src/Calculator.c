#include <stdio.h>
#include "Calculator.h"

int Calculator(int a, char c, int b)  
{  
    float res;  
    int new_b = b;  // COMPLIANT: Create a new variable to make modification
    switch(c)  
    {  
        case '+':  
            res = a + new_b; 
            break;  

        case '-':  
            res = a - new_b;
            break;  

        case '*':  
            res = a * new_b; 
            break;    

        case '/':  
            if (new_b == 0)   // if b == 0, take another number  
            {  
                new_b = 1;
            }  
            res = a / new_b;
            break;  

        default:  
            printf (" Something is wrong!! Please check the options ");               
    }  
    int res_int = (int)res;
    return res_int;  // COMPLIANT: return integer result instead of float
}  
