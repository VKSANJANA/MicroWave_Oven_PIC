/*
 * File:   main.c
 * Author: sanjana
 *
 * Created on 13 December, 2021, 9:24 AM
 */


#include <xc.h>
#include "main.h"
#include "clcd.h"
#include "matrix_keypad.h"
#include "timers.h"

#pragma config WDTE = OFF // Watchdog Timer Enable bit (WDT disabled)

int sec = 0,min = 0,temp = 0;
unsigned char reset_flag ; //resets the value at the start
unsigned char operation_mode , convection_mode_op ;

static void init_config(void) {
    //initialization of clcd module
    init_clcd();
    //initialization of matrix-keypad module
    init_matrix_keypad( );
    //initialization of timer module
    init_timer2( );
    
    //interrupt peripheral 
    PEIE = 1;
    GIE = 1;
    
    
    //setting the fan as output port
    FAN_DDR = 0;
    FAN = OFF; //OFF
    
    //buzzer as output port
    BUZZER_DDR = 0;
    BUZZER = OFF; //OFF
    

}

void main(void) {
    unsigned char key ;
    init_config();
    power_on_screen();
    clear_clcd();
    operation_mode = COOKING_MODE_DISPLAY;
    while (1) {
        //write your application code here
        key=read_matrix_keypad(STATE);   //edge triggering
        if(operation_mode == MICRO_MODE || operation_mode == GRILL_MODE || operation_mode == CONVECTION_MODE){
            ;
        }
        else if(key == 1 && operation_mode != TIME_DISPLAY){
            operation_mode = MICRO_MODE;
            reset_flag = MODE_RESET ;
            clear_clcd();
            clcd_print(" Power = 900W ",LINE2(0));
            __delay_ms(3000);
            clear_clcd();
        }
        else if(key == 2 && operation_mode != TIME_DISPLAY){
            operation_mode = GRILL_MODE;
            reset_flag = MODE_RESET;
            clear_clcd();
        }
        else if(key == 3 ){
            operation_mode = CONVECTION_MODE;
            reset_flag = RESET_TEMP;
            convection_mode_op = SET_TEMP; 
            clear_clcd();
        }
        else if(key == 4){
            if(operation_mode == COOKING_MODE_DISPLAY){
                min=0;
                sec=30;
                TMR2ON = ON;
                FAN = ON;
                clear_clcd();
                operation_mode = TIME_DISPLAY;
            }
            else if(operation_mode == TIME_DISPLAY){
                //increment time by 30sec
                sec += 30;
                if(sec > 59){
                    min++;
                    sec -= 60;
                }
            }
            //if task is paused then execute this
            else if(operation_mode == PAUSE){
                TMR2ON = ON;
                FAN = ON;
                operation_mode = START;
            }
        }
        else if(key == 5){
            operation_mode = PAUSE;
        }
        else if(key == 6){
            operation_mode = STOP;
            clear_clcd();
            __delay_ms(500);
        }
        switch(operation_mode){
            
            case COOKING_MODE_DISPLAY:
                cooking_mode_display();
                break;
            case MICRO_MODE:
                set_time(key);
                break;
            case GRILL_MODE:
                set_time(key);
                break;
            case CONVECTION_MODE:
                //TO CALL SET TEMP FUNCTION
                if(convection_mode_op == SET_TEMP){
                    set_temp(key);
                    if(convection_mode_op == SET_TIME){
                        continue;
                    }
                    
                }
                else if(convection_mode_op == SET_TIME){
                    set_time(key);
                }
                break;
            case START:
                time_display_screen();
                break;
            case TIME_DISPLAY:
                time_display_screen();
                break;
            case PAUSE:
                pause();
                break;
            case STOP:
                stop();
                break;
    }
       
          reset_flag = RESET_NOTHING;
     }
    }

void clear_clcd(void){
    clcd_write(CLEAR_DISP_SCREEN, INST_MODE);
    __delay_us(100);
}

void power_on_screen(void){
    
    for(int i=0;i<16;i++){
        clcd_putch(BLOCK,LINE1(i));
    }
    clcd_print("  Powering ON  ",LINE2(0));
    clcd_print(" Microwave Oven ",LINE3(0));
    for(int i=0;i<16;i++){
        clcd_putch(BLOCK,LINE4(i));
    }
    //2 sec delay
    __delay_ms(4000);
}

void cooking_mode_display(void){
    
    clcd_print("1.Micro",LINE1(0));
    clcd_print("2.Grill",LINE2(0));
    clcd_print("3.Convection",LINE3(0));
    clcd_print("4.Start",LINE4(0));
}

void set_time(unsigned char key){
    static int wait , blink, key_count, blink_pos;
   
    if(reset_flag == MODE_RESET){
        wait=15,blink=0;
        min=0;
        sec=0;
        key = ALL_RELEASED;
        key_count = 0;
        blink_pos=0;
        clcd_print("SET TIME (MM:SS)",LINE1(0));
        clcd_print("TIME- ",LINE2(0));
        clcd_print("*:CLEAR  #:ENTER",LINE4(0));
      
        
    }
    
    if(key != '*' && key!= '#' && key != ALL_RELEASED){
        key_count++;
        if(key_count <= 2){
            sec = sec*10 + key;
            blink_pos = 0;
        } 
        else if(key_count > 2 && key_count <= 4){
            min = min*10 + key;
            blink_pos = 1;
        }
    }
    
    if(key == '*'){
        
        if(key_count <= 2){
            sec = 0;
            key_count = 0;
        }
        
        else if(key_count > 2 && key_count <= 4){
            min=0;
            key_count = 2;
        }
    } 
    
    if(key == '#'){
        clear_clcd();
        operation_mode = TIME_DISPLAY;
        FAN = ON; // turn on fan
        TMR2ON = ON; //timer on
        
    }
   
    if(wait++ == 15){
        wait = 0;
        blink=!blink;
        //displaying seconds -convert it into ascii
        clcd_putch(sec/10+'0',LINE2(9));
        clcd_putch(sec%10+'0',LINE2(10));
        
        //displaying minutes - convert it into asscii
        clcd_putch(min/10+'0',LINE2(7));
        clcd_putch(min%10+'0',LINE2(6));
        

       
    }
    if(blink){
        
        if(blink_pos == 0){
                clcd_print("   ",LINE2(9));  //to blink seconds
        }
        
        else{
            
                clcd_print("   ",LINE2(6));  //to blink minutes
        }
    } 
    clcd_print(":",LINE2(8));
    
}

void door_status_check(void){
    //rb3 pressed - door open
    if(DOOR == OPEN){
        TMR2ON = OFF;
        FAN = OFF;
        BUZZER = ON;
        clear_clcd();
        clcd_print("Door Status:Open",LINE1(0));
        clcd_print("Close the door ",LINE3(0));
        //wait till door is closed
        while(DOOR == OPEN){
            ;
        }
        TMR2ON = ON;
        FAN = ON;
        BUZZER = OFF;
        clear_clcd();
    }
}

void time_display_screen(void){
    
    door_status_check();
    clcd_print(" TIME = ",LINE1(0));
    //displaying seconds -convert it into ascii
    clcd_putch(sec/10+'0',LINE1(11));
    clcd_putch(sec%10+'0',LINE1(12));
    
    clcd_print(":",LINE1(10));
        
    //displaying minutes - convert it into asscii
    clcd_putch(min/10+'0',LINE1(8));
    clcd_putch(min%10+'0',LINE1(9));
    
    clcd_print("4.Start/Resume  ",LINE2(0));
    clcd_print("5.Pause  ",LINE3(0));
    clcd_print("6.Stop   ",LINE4(0));
    
    
    if((sec == 0) && (min == 0)){
        clear_clcd( );
        TMR2ON = OFF; //timer off
        FAN = OFF; // turn off fan
        clcd_print("TIME UP! ",LINE2(1));
        clcd_print("COOKING DONE! ",LINE3(1));
        BUZZER = ON;
        __delay_ms(3000);
        BUZZER = OFF;
        clear_clcd();
        operation_mode = COOKING_MODE_DISPLAY;
       
    }
    
}

void set_temp(unsigned char key){
    static int wait , blink, key_count;
    static char temp_arr[4];
    if(reset_flag == RESET_TEMP){
        key = ALL_RELEASED;
        temp = 0;
        wait = 20;
        blink = 0;
        key_count = 0;
        //display
        clcd_print("SET TEMP.( C)",LINE1(0));
        clcd_putch(DEGREE,LINE1(10));
        clcd_print("TEMP : ",LINE2(1));
        clcd_print("*:CLEAR  #:ENTER",LINE4(0));
    }
    
    
    
    //read temp
    if(key != '*' && key!= '#' && key != ALL_RELEASED){
        key_count++;
        if(key_count <= 3){
            temp = temp*10 + key;
        } 
    }
    
    //to clear
    else if(key == '*'){
        temp = 0;
        key_count = 0;
     }
    
     else if (key == '#')
    {
       clear_clcd();
       clcd_print(" Pre-Heating  ",LINE1(0));
       clcd_print("Time Rem.=",LINE3(0));
       //set pre-heating time for 3min = 180sec
       sec=10;
       TMR2ON = ON;
       while(sec != 0){
           clcd_putch(sec/100 + '0',LINE3(10));
           clcd_putch((sec/10)%10 + '0' ,LINE3(11));
           clcd_putch(sec%10 + '0' , LINE3(12));
       }
       
       //after pre-heating
       clear_clcd();
       TMR2ON = OFF;
       convection_mode_op = SET_TIME;
       reset_flag = MODE_RESET;
     
      
    }
        
        
    
    //display temp= 231
    //temp[0]=2 temp[1]=3 temp[2]=1 temp[4]=null_character
        temp_arr[0] = (temp / 100) + '0';
        temp_arr[1] = (temp / 10)%10 + '0';
        temp_arr[2] = (temp % 10) + '0';
        temp_arr[3] = '\0';
    
        if(wait++ == 20){
            wait = 0;
            blink = !blink;
            clcd_print(temp_arr,LINE2(8));
        }
        
        if(blink){
            clcd_print("   ",LINE2(8));
        }
       
    
    
}

void stop(void){
    TMR2ON = OFF;
    FAN = OFF;
    operation_mode = COOKING_MODE_DISPLAY;
    
}

void pause(void){
    TMR2ON = OFF;
    FAN = OFF;
    
    
}