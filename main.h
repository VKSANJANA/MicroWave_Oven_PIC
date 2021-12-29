/* 
 * File:   main.h
 * Author: sanjana
 *
 * Created on 13 December, 2021, 9:28 AM
 */

//contains declaration of function prototype

#ifndef MAIN_H
#define	MAIN_H
#define COOKING_MODE_DISPLAY 0x01
#define MICRO_MODE           0x02
#define GRILL_MODE           0x03
#define CONVECTION_MODE      0x04
#define TIME_DISPLAY         0x05
#define START                0x06
#define PAUSE                0x07
#define STOP                 0x08
#define SET_TEMP             0x09
#define SET_TIME             0x0A

#define RESET_NOTHING        0x0F
#define MODE_RESET           0x10
#define RESET_TEMP           0x20
#define RESET_TIME           0x30

#define FAN                  RC2
#define FAN_DDR              TRISC2
#define BUZZER               RC1
#define BUZZER_DDR           TRISC1
#define DOOR                 RB3
#define DOOR_DDR             TRISB3

#define ON                   1
#define OFF                  0
#define OPEN                 0

void power_on_screen(void);
void cooking_mode_display(void);
void clcd_write(unsigned char byte,unsigned char mode);
void clear_clcd(void);
void set_time(unsigned char key);
void time_display_screen(void);
void set_temp(unsigned char key);
void pause(void);
void stop(void);


#endif	/* MAIN_H */

