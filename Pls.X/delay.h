//#include <libpic30.h>

// clock-frequecy in Hz with suffix LL (64-bit-long), eg. 32000000LL for 32MHz
//#define FOSC  32000000ULL  
#define _XTAL_FREQ 8000000
/*
 delay_us(x) and delay_ms(x) for C30
 */
 #ifndef __DELAY_H
 #define FCY       (FOSC/2)  // MCU is running at FCY MIPS
 #define delay_us(x) __delay_us(x) // delays x us
 #define delay_ms(x) __delay_ms(x)  // delays x ms
 #define __DELAY_H 1
 #endif
 
