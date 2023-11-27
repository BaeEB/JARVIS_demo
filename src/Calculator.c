#include <stdio.h>
#include "Calculator.h"

int Calculator(int a, char c, int b)
{
    float res;
    int res_int;
    switch(c)
    {
        case '+':
            res = a + b; // add two numbers
            break;

        case '-':
            res = a - b; // subtract two numbers
            break;

        case '*':
            res = a * b; // multiply two numbers
            break;

        case '/':
            if (b == 0)   // if b == 0, use 1 as the divisor
            {
                b = 1; // Although this addresses the direct modification, it might be better to handle this case outside the function
            }
            res = a / b; // divide two numbers
            break;

 /* default message if any condition is not satisfied */
            // printf (" Something is wrong!! Please check the options ");
            res = 0.0f; // Provide a default value for res
            break;
    }
    res_int = (int)res; // Explicit cast from float to int
    return res_int;
}
