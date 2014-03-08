/*
 * This file is released under the GPL license.
 */
// Converts a hexadecimal string to integer
//   0    - Conversion is successful
//   1    - String is empty
//   2    - String has more than 8 bytes
//   4    - Conversion is in process but abnormally terminated by 
//          illegal hexadecimal character


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int xtoi(const char* xs, unsigned int* result)
{
    size_t szlen = strlen(xs);
    int i, xv, fact;

    if (szlen > 0)
    {
        //Converting more than 32bit hexadecimal value?
        if (szlen > 8) 
            return 2; 

        // Begin conversion here
        *result = 0;
        fact = 1;

        for(i=szlen-1; i>=0 ;i--)
        {
            if (isxdigit(*(xs+i)))
            {
                if (*(xs+i)>=97)
                {
                    xv = ( *(xs+i) - 97) + 10;
                }
                else if ( *(xs+i) >= 65)
                {
                    xv = (*(xs+i) - 65) + 10;
                }
                else
                {
                    xv = *(xs+i) - 48;
                }
                *result += (xv * fact);
                fact *= 16;
            }
            else
            {
                return 4;
            }
        }
    }
    return 1;
}

