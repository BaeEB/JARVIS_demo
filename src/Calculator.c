#include <stdio.h>
#include "Calculator.h"

#include <stdio.h>  // Required for printf

int Calculator(int a, char c, int b)
{  
    int res_int;  // Use int to store result
    switch(c)
    {
        case '+':
            res_int = a + b;  // add two numbers
            break;
            
        case '-':
            res_int = a - b;  // subtract two numbers
            break;
            
        case '*':
            res_int = a * b;  // multiply two numbers
            break;
            
        case '/':
            if (b == 0)  // if b == 0, take another number
            {  
                b = 1;
            }  
            res_int = a / b;  // divide two numbers
            break;
            
 /* use default to print default message if any condition is not satisfied */
            res_int = 0; // Set default result to 0
            printf(" Something is wrong!! Please check the options ");
            break; // Add break to terminate default case properly
    }
    
    return res_int; // return the integer result
}
