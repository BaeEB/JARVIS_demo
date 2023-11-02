#include <stdio.h>
#include "Calculator.h"

int Calculator(int a, char c, int b)  
{  
    float res;  
    // printf (" Choose an operator(+, -, *, /) to perform the operation in C Calculator \n ");  
    if (c == '/' )  
    {  
        printf (" You have selected: Division");  
    }  
    else if (c == '*')  
    {  
        printf (" You have selected: Multiplication");  
     }  
       
    else if (c == '-')  
    {  
        printf (" You have selected: Subtraction");  
     }  
        else if (c == '+')  
    {  
        printf (" You have selected: Addition");  
     }     
      
    switch(c)  
    {  
        case '+':  
            res = a + b; // add two numbers  
            // printf (" Addition of %d and %d is: %.2f", a, b, res);  
            break;  
          
        case '-':  
            res = a - b; // subtract two numbers  
            // printf (" Subtraction of %d and %d is: %.2f", a, b, res);  
            break;  
              
        case '*':  
            res = a * b; // multiply two numbers  
            // printf (" Multiplication of %d and %d is: %.2f", a, b, res);  
            break;            
          
        case '/':  
            if (b == 0)   // if b == 0, take another number  
            {  
                printf (" \n Divisor cannot be zero. Please enter another value ");  
                b = 1;
            }  
            res = a / b; // divide two numbers  
            // printf (" Division of %d and %d is: %.2f", a, b, res);  
            break;  
        default:  /* use default to print default message if any condition is not satisfied */  
            printf (" Something is wrong!! Please check the options ");               
    }  
    int res_int = (int)res;
    return res;  
}  