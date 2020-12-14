/******************** (C) COPYRIGHT 2009 STMicroelectronics ********************
* File Name          : main.h
* Author             : IMS Systems Lab
* Version            : V1.0
* Date First Issued  : 11/Nov/2009
* Description        : Header for main.c module
********************************************************************************

********************************************************************************
* THE PRESENT SOFTWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*
* THIS SOURCE CODE IS PROTECTED BY A LICENSE.
* FOR MORE INFORMATION PLEASE CAREFULLY READ THE LICENSE AGREEMENT FILE LOCATED
* IN THE ROOT DIRECTORY OF THIS FIRMWARE PACKAGE.
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "stdio.h"
#include "config.h"
#include "util.h"
#include "rtc_32f1.h"
#include "Delay.h"
#include "modbus\modbus.h"
#include "LORA\lora.h"

void Make_DHT220_Sensor_Data(void);
void ICAST_Global_Value_Init(void);
void GPIO_Level_Change(uint8 port_sel, uint8 Level);

#define SENSOR_NODE     1
#define MONITOR_DEVICE  2

//    #define DEVICE_TYPE     MONITOR_DEVICE
 #define DEVICE_TYPE     SENSOR_NODE

#define ICAST_SENSOR_MAP_TOTAL_CNT      16
#define ICAST_SENSITIVITY_OFFSET_TOTAL_CNT  6

#define POWER_NODE_TTL_LEVEL_CHANGE   0xA0
#define POWER_NODE_SSR_LEVEL_CHANGE   0xA1

#endif /* __MAIN_H */

/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/
