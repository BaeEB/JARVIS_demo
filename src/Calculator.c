#include <stdio.h>
#include "Calculator.h"

        int Calculator(int a, char c, int b)  
        {  
            float res;  
            switch(c)  
            {  
                case '+':  
                    res = a + b;  
                    break;  
                 
                case '-':  
                    res = a - b; 
                    break;  
                     
                case '*':  
                    res = a * b;   
                    break;               
          
                case '/':  
                    if (b == 0)   
                    {  
                        b = 1;
                    }  
                    res = a / (float)b;   
                    break;  
                default:      
                    printf (" Something is wrong!! Please check the options ");               
            }  
            return (int)res;  
        }  
