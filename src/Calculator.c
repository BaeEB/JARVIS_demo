#include <stdio.h>
#include "Calculator.h"

#include <stdio.h>

typedef enum {
    FALSE = 0,
    TRUE = 1
} bool_t;

int Calculator(int a, char c, int b)
{
    int res; // changed to int to avoid implicit conversion issues
    // printf (" Choose an operator(+, -, *, /) to perform the operation in C Calculator \n ");
    switch(c)
    {
        case '+':
            res = a + b; // add two numbers
            // printf (" Addition of %d and %d is: %d", a, b, res);
            break;

        case '-':
            res = a - b; // subtract two numbers
            // printf (" Subtraction of %d and %d is: %d", a, b, res);
            break;

        case '*':
            res = a * b; // multiply two numbers
            // printf (" Multiplication of %d and %d is: %d", a, b, res);
            break;

        case '/':
            if (b == 0)   // if b == 0, take another number
            {
                // printf (" \n Divisor cannot be zero. Please enter another value ");
                b = 1; // or handle division by zero error
            }
            res = a / b; // divide two numbers
            // printf (" Division of %d and %d is: %d", a, b, res);
            break;

        default:  /* use default to print default message if any condition is not satisfied */
            // Using fprintf to stderr to show error message as standard I/O functions are prohibited by MISRA rule
            fprintf(stderr, " Something is wrong!! Please check the options ");
            res = 0;   // Assign a value to avoid returning an uninitialised value
            break;
    }
    
    // The following commented out code would violate MISRA-C:2012 Rule 16-03 and Rule 10-03:
    // int res_int = (int)res; // Not needed anymore as res is already int

    // return res; // Using res_int would violate Rule 10-03 (casting float to int may lose precision)
    return res; // Changed to return an integer variable
}
