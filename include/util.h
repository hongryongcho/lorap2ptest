/*
*
@file		util.h
@brief	
*/

#ifndef _UTIL_H
#define _UTIL_H

void LED1_onoff(uint8_t on_off);
void LED2_onoff(uint8_t on_off);

void USART1_Init(void);
void USART1_RM_Init(void);
// void USART2_Init(void);
void USART3_Init(void);

void Delay_us(uint8 time_us);
void Delay_ms(uint16 time_ms);
#if 1
int putchar(int ch);
int getchar(void);
#endif
void SPI1_Init(void);
uint8_t SPI1_SendByte(uint8_t byte);
void Dly100us(int arg);
void delay_int_count(volatile unsigned int nTime);
void Dly1mSec(int time);

extern u32 Tm1msDly;
#define TM_DELAY_SetTime2(time)	  (Tm1msDly = (time))
#define TM_DELAY_Time2()	  (Tm1msDly)

#define LOOP_DLY_100US  545 //450


#endif
