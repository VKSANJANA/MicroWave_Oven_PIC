#include <xc.h>
#include "main.h"

extern int sec,min; //to use variable of another file

void __interrupt() isr(void)
{
    static unsigned int count = 0;
    
    if (TMR2IF == 1)
    {
        if (++count == 1250)//1250 for 1sec
        {
            count = 0;
            if(sec >= 1){
              sec--;   
            }
            else if((sec == 0) && (min > 0)){
                min--;
                sec = 59;
            }
            
        }
        
        TMR2IF = 0;
    }
}