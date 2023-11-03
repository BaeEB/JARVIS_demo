#include "Calculator.h"

int Calculator(int a, char c, int b)  
{  
    float res;  
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
            if (b == 0)   // if b == 0, take another number  
            {  
                b = 1;
            }  
            res = a / b; // divide two numbers  
            break;  
            
        default:  
            // do something in case of invalid operator  
            break;
    }  
    int res_int = (int)res;
    return res_int;  
}
