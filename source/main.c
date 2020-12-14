/*******************************************************************************
* Project            : Process sensor data
* File Name          : main.c
* Version            : V1.0
* Date               : 2016/01/05 
* Description        : Main program body
*******************************************************************************/
#include <string.h>
#include <stdlib.h>
#include "main.h"

#define ICAST_DBG_ON
#ifdef ICAST_DBG_ON
  #define ICAST_GPIO_DEBUG_ON 0
  #define ICAST_FLASH_REG_SAVE_DEBUG_ON 1
#endif

/* Private typedef -----------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*                              Defines                                       */
/*----------------------------------------------------------------------------*/
// #define DataFlashAddr 0x0801FC00
// #define BackFlashAddr 0x0801F800
#define DataFlashAddr 0x0801F800
#define BackFlashAddr 0x0801F000
// #define FLASH_BUFFER_MAX 256
#define FLASH_BUFFER_MAX 512    // change to double size

/* Private function prototypes -----------------------------------------------*/
void RCC_Configuration(void);
void NVIC_Configuration(void);
void GPIO_Configuration(void);
void Timer_Configuration(void);

void Dly1mSec(int time);

// void ADC_ConfigDMA();

void WatchDogSet(void);
void ResetTypeCheck(void);
void SystemReset(void);
void writeFlash(uint32_t startAddress, uint16_t *buffer, uint16_t size);
void readFlash(uint32_t startAddress, uint16_t *buffer, uint16_t size);
void SysInit(void);

void FlashBackup(int mode);
void FlashSetupLoad_icast(void);

void Print_CSV_Header_Name(void);
void Print_CSV_Header_Value(void);

extern void Send_FNumber_To_PC(float number, int point_val);
extern void Send_CSV_String_To_PC(const char *packet);
extern void Send_CSV_Number_To_PC(int number);
extern void Send_CSV_U16Num_To_PC(U16 number);
extern void Send_Char_To_PC(char *char_data);
extern void Send_String_To_PC_WithOutCR(const char *packet);

extern void Send_CSV_FloatNum_To_PC(U32 number);
extern void Send_CSV_U32Num_To_PC(U32 number);
extern void LoRa_Mode_Set(U8 set_mode);

/* Private variables ---------------------------------------------------------*/

const char FirmwareVersion[]= "V1.00";

CREG REG;
RegData SensorRead;
SysReg gSysReg;

__IO uint32_t Timer_10mSec_Conter=0;
int ADC2ConvertedValue;
volatile uint16_t  ADC_DualConvertedValueTab[9]; //ADC0,ADC1~

void GPIO_Level_Change(uint8 port_sel, uint8 Level)
{
  if(Level == PORT_LOW){
    switch(port_sel)
    {
      case eICAST_GPOUT_P_SSR_CON :  GPIO_ResetBits(GPIOA,P_SSR_CON );     break;
      case eICAST_GPOUT_SSR_STATUS : GPIO_ResetBits(GPIOA,P_MOTOR_STATUS );     break; 
      case eICAST_GPOUT_MCU_LED1 :   GPIO_ResetBits(GPIOC, MCU_LED_1);     break;
      case eICAST_GPOUT_MCU_LED2 :   GPIO_ResetBits(GPIOC, MCU_LED_2);     break;
      case eICAST_GPOUT_MCU_LED3 :   GPIO_ResetBits(GPIOC, MCU_LED_3);     break;
      case eICAST_GPOUT_MCU_LED4 :   GPIO_ResetBits(GPIOC, MCU_LED_4);     break;
      case eICAST_GPOUT_LORA_RESET:  GPIO_ResetBits(GPIOB, LORA_RESET);     break;
      
      default : break;
    }
  }
  else{
    switch(port_sel)
    {
      case eICAST_GPOUT_P_SSR_CON :  GPIO_SetBits(GPIOA,P_SSR_CON );       break;
      case eICAST_GPOUT_SSR_STATUS : GPIO_SetBits(GPIOA,P_MOTOR_STATUS );     break; 
      case eICAST_GPOUT_MCU_LED1 :   GPIO_SetBits(GPIOC, MCU_LED_1);     break;
      case eICAST_GPOUT_MCU_LED2 :   GPIO_SetBits(GPIOC, MCU_LED_2);     break;
      case eICAST_GPOUT_MCU_LED3 :   GPIO_SetBits(GPIOC, MCU_LED_3);     break;
      case eICAST_GPOUT_MCU_LED4 :   GPIO_SetBits(GPIOC, MCU_LED_4);     break;
      case eICAST_GPOUT_LORA_RESET:  GPIO_SetBits(GPIOB, LORA_RESET);     break;
      default : break;
    }
  }
}
uint8 GPIO_Level_Scan(uint8 port_sel)
{
  uint8 rtn_val=0;
  switch(port_sel)
  {
    case eICAST_GPIN_SSR_M_CON : rtn_val = GPIO_ReadInputDataBit(GPIOA,P_SSR_M_CON);      break;
    case eICAST_GPIN_TP2 :    rtn_val = GPIO_ReadInputDataBit(GPIOB,MCU_TP_B_6);      break;
    case eICAST_GPIN_SW_A : rtn_val = GPIO_ReadInputDataBit(GPIOB,SW_IN_B_12);        break;
    case eICAST_GPIN_SW_B : rtn_val = GPIO_ReadInputDataBit(GPIOB,SW_IN_B_13);        break;
    default : break;
  }
  return rtn_val;
}


//------------------------------------------------------------------------------
int main(void)      // at+recv=1,-91,10,4:11111111<CR><LF>   LORA downlink example
{
  int i = 0;

  RCC_Configuration();
  NVIC_Configuration();
  SysTick_Config(18000);        //250uS(72mHZ)
  GPIO_Configuration();
  // ADC_ConfigDMA();
  USART1_Init();
  ResetTypeCheck();
  Timer_Configuration();    // TIM2 and TIM3
  WatchDogSet(); 
  SysInit();
  FlashSetupLoad_icast();

  // USART2_Init();   // TP pin
  USART3_Init();
  
  GPIO_Level_Change(eICAST_GPOUT_LORA_RESET,PORT_HIGH);
  LoRa_Mode_Set(eLoRa_Status_Sleep);

  LoRa_Mode_Set(eLoRa_Status_Send_P2P);

////////////// hrcho test code start /////////////////
#if(ICAST_GPIO_DEBUG_ON)
  for(i=0;i<600;i++)
  {
   GPIO_Level_Change(eICAST_GPOUT_P_SSR_CON,1);
   GPIO_Level_Change(eICAST_GPOUT_SSR_STATUS,1);
   GPIO_Level_Change(eICAST_GPOUT_MCU_LED1,1);
   GPIO_Level_Change(eICAST_GPOUT_MCU_LED2,1);
   GPIO_Level_Change(eICAST_GPOUT_MCU_LED3,1);
   GPIO_Level_Change(eICAST_GPOUT_MCU_LED4,1);
   GPIO_Level_Change(eICAST_GPOUT_LORA_RESET,1);
   Send_String_To_PC("Debug Port15_1  High\r\n"); 
   Delay_ms(500);

   GPIO_Level_Change(eICAST_GPOUT_P_SSR_CON,0);
   GPIO_Level_Change(eICAST_GPOUT_SSR_STATUS,0);
   GPIO_Level_Change(eICAST_GPOUT_MCU_LED1,0);
   GPIO_Level_Change(eICAST_GPOUT_MCU_LED2,0);
   GPIO_Level_Change(eICAST_GPOUT_MCU_LED3,0);
   GPIO_Level_Change(eICAST_GPOUT_MCU_LED4,0);
   
   GPIO_Level_Change(eICAST_GPOUT_LORA_RESET,0);
   Send_String_To_PC("Debug Port15_1 Low\r\n"); 
   Delay_ms(500);
  }
  while(1);
#endif
////////////// hrcho test code end /////////////////
  
  if(gSysReg.Flag.ICAST_CSV_MONITOR_ENABLE)  
  {
    Print_CSV_Header_Name();
  }

  while (TRUE)   
  {
    if(gSysReg.Flag.PowerReadReady)     // always 1 value
    {
      if(gSysReg.Flag.ICAST_CSV_MONITOR_ENABLE)
      {
        if(gSysReg.Flag.CSV_OUTPUT_Flag == 1){
          gSysReg.Flag.CSV_OUTPUT_Flag=0;
          Print_CSV_Header_Value();
        }
      }
    }

    RecvUart1();          // pc command patch
    RecvUart3_icast();    // lora data receive routine
    Send_LORA_Packet();   // send command to lora
  }  
}


// GREG
void SysInit(void)
{
  int k;
  int sum=0;
  ICAST_Global_Value_Init();
  GPIO_Level_Change(eICAST_GPOUT_LORA_RESET,PORT_LOW);
}


////////////////////// HRCHO ADD START///////////////////////////

enum{
  ICST_HN_Project_Name=0,
  ICST_HN_Test_Time,        
  ICST_HN_MODULE_SN,
  ICST_HN_FW_VERSION,
  ICST_HN_Timer_10mSec,
  ICST_HN_MODULE_MAGICnum1,
  ICST_HN_MODULE_MAGICnum2,
  ICST_HN_MODULE_MAGICnum3,

  // to do start
  ICST_HN_SETUP_PROD_ID_HIGH,
  ICST_HN_SETUP_PROD_ID_LOW,
  ICST_HN_SETUP_TIME_YEAR,
  ICST_HN_SETUP_TIME_MONTH,
  ICST_HN_SETUP_TIME_DAYS,
  ICST_HN_SETUP_TIME_HOURS,
  ICST_HN_SETUP_TIME_MINUTS,
  ICST_HN_SETUP_TIME_SEC,
  
  ICST_HN_FLASH_BYTE_COUNT,

  ICST_HN_LORA_MODE,
  ICST_HN_LORA_ID,
  ICST_HN_LORA_COMPort,
  ICST_HN_LORA_BAUDRATE,
  ICST_HN_LORA_DATABIT,
  ICST_HN_LORA_STOPBIT,
  ICST_HN_LORA_PARITYBIT,
  ICST_HN_LORA_FLOW,
  
  ICST_HN_LORA_CHANNEL,
  ICST_HN_LORA_SF_val,
  ICST_HN_LORA_FREQ,

  ICST_HN_EUI,
  ICST_HN_KEY,

  ICAST_HN_LORA_CONNECTION,
  ICAST_HN_LORA_RCV_PORT,
  ICAST_HN_LORA_RSSI,
  ICAST_HN_LORA_SNR,
  ICAST_HN_LORA_UPLINK_CNT,
  ICAST_HN_LORA_DOWNLINK_CNT,
  ICAST_HN_SSR_CH_STATUS,

  ICST_HN_DHT22_RH_DATA,
  ICST_HN_DHT22_T_DATA,
  ICST_HN_DHT22_CHECKSUM,

  
  ICST_HN_MAX_NUM,
};

void Print_CSV_Header_Name(void)
{
  for(int i=0;i<=ICST_HN_MAX_NUM;i++)
  {
    switch(i)
    {
      case ICST_HN_Project_Name :       Send_CSV_String_To_PC("Project_Name"); break;
      case ICST_HN_Test_Time:           Send_CSV_String_To_PC("Test_Time"); break;
      case ICST_HN_MODULE_SN:           Send_CSV_String_To_PC("MODULE_SN"); break;
      case ICST_HN_FW_VERSION :         Send_CSV_String_To_PC("FW_VERSION"); break;
      case ICST_HN_Timer_10mSec :       Send_CSV_String_To_PC("Running_Time"); break;

      case ICST_HN_MODULE_MAGICnum1:    Send_CSV_String_To_PC("MAGIC num 1"); break;
      case ICST_HN_MODULE_MAGICnum2:    Send_CSV_String_To_PC("MAGIC num 2"); break;
      case ICST_HN_MODULE_MAGICnum3:    Send_CSV_String_To_PC("MAGIC num 3"); break;

      case ICST_HN_SETUP_PROD_ID_HIGH : Send_CSV_String_To_PC("REG_PROD_ID_H"); break;
      case ICST_HN_SETUP_PROD_ID_LOW :  Send_CSV_String_To_PC("REG_PROD_ID_L"); break;
      case ICST_HN_SETUP_TIME_YEAR  :    Send_CSV_String_To_PC("REG_Time_Year"); break;
      case ICST_HN_SETUP_TIME_MONTH :   Send_CSV_String_To_PC("REG_Time_Month"); break;
      case ICST_HN_SETUP_TIME_DAYS:     Send_CSV_String_To_PC("REG_Time_Day"); break;
      case ICST_HN_SETUP_TIME_HOURS:    Send_CSV_String_To_PC("REG_Time_Hour"); break;
      case ICST_HN_SETUP_TIME_MINUTS:   Send_CSV_String_To_PC("REG_Time_Min"); break;
      case ICST_HN_SETUP_TIME_SEC:      Send_CSV_String_To_PC("REG_Time_Sec"); break;

      case ICST_HN_FLASH_BYTE_COUNT :   Send_CSV_String_To_PC("Total Flash Byte Count"); break;

      case ICST_HN_LORA_MODE :          Send_CSV_String_To_PC("REG_Lora_Mode"); break;
      case ICST_HN_LORA_ID :            Send_CSV_String_To_PC("REG_Lora_ID"); break;
      case ICST_HN_LORA_COMPort :       Send_CSV_String_To_PC("REG_Lora_ComPort"); break;
      case ICST_HN_LORA_BAUDRATE :      Send_CSV_String_To_PC("REG_Lora_BaudRate"); break;
      case ICST_HN_LORA_DATABIT :       Send_CSV_String_To_PC("REG_Lora_DataBit"); break;
      case ICST_HN_LORA_STOPBIT :       Send_CSV_String_To_PC("REG_Lora_StopBit"); break;
      case ICST_HN_LORA_PARITYBIT :     Send_CSV_String_To_PC("REG_Lora_ParityBit"); break;
      case ICST_HN_LORA_FLOW :          Send_CSV_String_To_PC("REG_Lora_Flow"); break;

      case ICST_HN_LORA_CHANNEL :       Send_CSV_String_To_PC("REG_Lora_CHANNEL"); break;
      case ICST_HN_LORA_SF_val :        Send_CSV_String_To_PC("REG_Lora_SF_val"); break;
      case ICST_HN_LORA_FREQ :          Send_CSV_String_To_PC("REG_Lora_FREQ"); break;

      case ICST_HN_EUI :                Send_CSV_String_To_PC("EUI");  break;
      case ICST_HN_KEY :                Send_CSV_String_To_PC("KEY");  break;

      case ICAST_HN_LORA_RCV_PORT :     Send_CSV_String_To_PC("LORA_RCV_Port");  break;
      case ICAST_HN_LORA_RSSI :         Send_CSV_String_To_PC("LORA_RSSI");  break;
      case ICAST_HN_LORA_SNR :          Send_CSV_String_To_PC("LORA_SNR");  break;

      case ICAST_HN_LORA_CONNECTION :   Send_CSV_String_To_PC("LORA Contact");  break;
      case ICAST_HN_LORA_UPLINK_CNT:    Send_CSV_String_To_PC("LORA UP CNT");  break;
      case ICAST_HN_LORA_DOWNLINK_CNT:  Send_CSV_String_To_PC("LORA DOWN CNT");  break;
      case ICAST_HN_SSR_CH_STATUS :
        for(int i=eSSR_CH0;i<eSSR_CH_MAX;i++){
          Send_String_To_PC("SSR CH");
          Send_CSV_Number_To_PC(i); 
        }
      break;

      case ICST_HN_DHT22_RH_DATA :      Send_CSV_String_To_PC("DHT220 RH"); break;
      case ICST_HN_DHT22_T_DATA :       Send_CSV_String_To_PC("DHT220 Temperature"); break;
      case ICST_HN_DHT22_CHECKSUM :     Send_CSV_String_To_PC("DHT220 Checksum"); break;

      case ICST_HN_MAX_NUM :            Send_String_To_PC("\r"); break;
    }
  }
}

void Print_CSV_Header_Value(void)
{
  for(int i=0;i<=ICST_HN_MAX_NUM;i++)
  {
    switch(i)
    {
      case ICST_HN_Project_Name :             Send_CSV_String_To_PC("ICAST Sensor SCAN"); break;
      case ICST_HN_Test_Time:                 Send_CSV_String_To_PC("Test_Time"); break;
      case ICST_HN_MODULE_SN:                 Send_CSV_String_To_PC("MODULE_SN"); break;
      case ICST_HN_FW_VERSION :               Send_CSV_String_To_PC(&FirmwareVersion[0]); break;
      case ICST_HN_Timer_10mSec :             Send_CSV_U32Num_To_PC(Timer_10mSec_Conter); break;
      case ICST_HN_MODULE_MAGICnum1:          Send_CSV_Number_To_PC(FLASH_MAGIC_NUM3); break;
      case ICST_HN_MODULE_MAGICnum2:          Send_CSV_Number_To_PC(FLASH_MAGIC_NUM2); break;
      case ICST_HN_MODULE_MAGICnum3:          Send_CSV_Number_To_PC(FLASH_MAGIC_NUM1); break;

      case ICST_HN_SETUP_PROD_ID_HIGH :       Send_CSV_Number_To_PC(REG.Setup.ProductID_H); break;
      case ICST_HN_SETUP_PROD_ID_LOW :        Send_CSV_Number_To_PC(REG.Setup.ProductID_L); break;
      
      case ICST_HN_SETUP_TIME_YEAR :          Send_CSV_Number_To_PC(gSysReg.lora.Year); break;
      case ICST_HN_SETUP_TIME_MONTH :         Send_CSV_Number_To_PC(gSysReg.lora.Month); break;
      case ICST_HN_SETUP_TIME_DAYS:           Send_CSV_Number_To_PC(gSysReg.lora.Day); break;
      case ICST_HN_SETUP_TIME_HOURS:          Send_CSV_Number_To_PC(gSysReg.lora.Hour); break;
      case ICST_HN_SETUP_TIME_MINUTS:         Send_CSV_Number_To_PC(gSysReg.lora.Minute); break;
      case ICST_HN_SETUP_TIME_SEC:            Send_CSV_Number_To_PC(gSysReg.lora.Sec); break;
      case ICST_HN_FLASH_BYTE_COUNT :         Send_CSV_Number_To_PC(REG.gICAST.fFlash_length); break;

      case ICST_HN_LORA_MODE :                Send_CSV_Number_To_PC(REG.LoraSet.Mode); break;
      case ICST_HN_LORA_ID :                  Send_CSV_Number_To_PC(REG.LoraSet.ID); break;
      case ICST_HN_LORA_COMPort :             Send_CSV_Number_To_PC(REG.LoraSet.COMPort); break;
      case ICST_HN_LORA_BAUDRATE :            Send_CSV_Number_To_PC(REG.LoraSet.BaudRate); break;
      case ICST_HN_LORA_DATABIT :             Send_CSV_Number_To_PC(REG.LoraSet.DataBit); break;
      case ICST_HN_LORA_STOPBIT :             Send_CSV_Number_To_PC(REG.LoraSet.StopBit); break;
      case ICST_HN_LORA_PARITYBIT :           Send_CSV_Number_To_PC(REG.LoraSet.ParityBit); break;
      case ICST_HN_LORA_FLOW :                Send_CSV_Number_To_PC(REG.LoraSet.Flow); break;

      case ICST_HN_LORA_CHANNEL :             Send_CSV_Number_To_PC(REG.LoraSet.Device_Channel); break;
      case ICST_HN_LORA_SF_val :              Send_CSV_Number_To_PC(REG.LoraSet.Device_SF_val); break;
      case ICST_HN_LORA_FREQ :                Send_CSV_Number_To_PC(REG.LoraSet.Device_FREQ); break;

      case ICST_HN_EUI :
        for(int i=0;i<16;i++){
          Send_Char_To_PC(&REG.MeterSet.LORA_EUI[i]);
        }
        Send_String_To_PC(","); 
        break;
      case ICST_HN_KEY :
        for(int i=0;i<32;i++){
          Send_Char_To_PC(&REG.MeterSet.LORA_KEY[i]); 
        }
        Send_String_To_PC(","); 
        break;
      
      case ICAST_HN_LORA_RCV_PORT :  Send_CSV_Number_To_PC(gSysReg.lora.rcv_lora_port); break;
      case ICAST_HN_LORA_RSSI :      Send_CSV_Number_To_PC(gSysReg.lora.rcv_RSSI); break;
      case ICAST_HN_LORA_SNR :       Send_CSV_Number_To_PC(gSysReg.lora.rcv_SNR); break;
      
      case ICAST_HN_LORA_CONNECTION :  Send_CSV_Number_To_PC(gSysReg.lora.Lora_Connection); break;
      case ICAST_HN_LORA_UPLINK_CNT:   Send_CSV_Number_To_PC(gSysReg.lora.UPLink_CNT); break;
      case ICAST_HN_LORA_DOWNLINK_CNT: Send_CSV_Number_To_PC(gSysReg.lora.DOWNLink_CNT); break;
      case ICAST_HN_SSR_CH_STATUS :
        for(int i=eSSR_CH0;i<eSSR_CH_MAX;i++){
          Send_CSV_Number_To_PC(gSysReg.lora.SSR_control[i]); 
        }
        break;

      case ICST_HN_DHT22_RH_DATA :            Send_CSV_Number_To_PC(SensorRead.DHT220.RH_Data); break;
      case ICST_HN_DHT22_T_DATA :             Send_CSV_Number_To_PC(SensorRead.DHT220.T_Data); break;
      case ICST_HN_DHT22_CHECKSUM :           Send_CSV_Number_To_PC(SensorRead.DHT220.CheckSum); break;

      case ICST_HN_MAX_NUM :      Send_String_To_PC("\r");                  break;
      default : break;
    }
  }
}

void ICAST_Global_Value_Init(void)
{
  gSysReg.Time.Sensor_Scan_Count = 300;
  gSysReg.Time.SS_Loop_skip_val = 30;    // 30mSec loop time in skip scan condition
  gSysReg.Time.Current_SS_count =0;
  gSysReg.Time.heartbeat_flag=0;
  gSysReg.Time.Timer_HeartBeat_Cnt=0;
  gSysReg.Time.Day =0;
  gSysReg.Time.Hour =0;
  gSysReg.Time.Min =0;
  gSysReg.Time.Sec =0;
  gSysReg.Time.LoRa_Aging_Counter = 0;

  gSysReg.Flag.PowerReadReady=1;
  gSysReg.Flag.ICAST_CSV_MONITOR_ENABLE = 0;
  gSysReg.Flag.CSV_OUTPUT_Flag=0;
  gSysReg.Flag.LORA_P2P_Aging_Flag=1;
  gSysReg.Flag.LORA_P2P_Packet_Send_Flag =0;

  gSysReg.Flag.ICAST_DBG_ON_LORA_TX_MONITOR=0 ;
  gSysReg.Flag.ICAST_DBG_ON_LORA_RX_MONITOR=0 ;

  gSysReg.Flag.Is_Sensor_BoardData=0;  

  gSysReg.Uart.UART1RxHead=0;
  gSysReg.Uart.UART1RxTail=0;
  gSysReg.Uart.UART2RxHead=0;
  gSysReg.Uart.UART2RxTail=0;
  gSysReg.Uart.UART3RxHead=0;
  gSysReg.Uart.UART3RxTail=0;
  
   // gSysReg.lora.Lora_Packet_Send_Status = eLoRa_Status_JOIN;
  gSysReg.lora.Lora_Packet_Send_Status = eLoRa_Status_Send_APPEUI;
  gSysReg.lora.Lora_Connection = DISABLE;
      
  gSysReg.lora.Lora_Enable =DISABLE;
  gSysReg.lora.Lora_Packet_Max_Wait_Time = 6000;    
  gSysReg.lora.Lora_Packet_data_Send_interval_Time = 6000;    
  gSysReg.lora.Lora_Packet_Response_Wait_counter=0;
  gSysReg.lora.Lora_Join_Retry_count=0;
  gSysReg.lora.Lora_Sample_p2p_Tx_Timer =0;
  gSysReg.lora.Lora_Rcv_PortNum =0;
  gSysReg.lora.Lora_RSSI_Val =0;
  gSysReg.lora.Lora_SNR =0;

  gSysReg.lora.Year = 2020;
  gSysReg.lora.Month = 1;
  gSysReg.lora.Day = 1;
  gSysReg.lora.Hour =12;
  gSysReg.lora.Minute =0;
  gSysReg.lora.Sec=0;


  gSysReg.lora.UPLink_CNT =0;
  gSysReg.lora.DOWNLink_CNT =0;
  

  sprintf(gSysReg.Uart.Make_Sensor_Packet_A,"at+send=lora:1:df1234%02x%02x", 0x0d,0x0a);

  gSysReg.Uart.Packet_Ready =1;   // lora send packet is only A

}
void ICAST_Factory_Value_Loading(void)
{
  uint16 flash_length;
  U16* mp;

  REG.gICAST.fFlash_length = sizeof(REG);
  REG.gICAST.Lora_UpLink_WaitTime = 1000;

  REG.Config.Factory_inite=0;
  REG.Config.Reset =0;

  REG.Setup.ProductID_H = 0x0100;
  REG.Setup.ProductID_L = eMODEL_WATER_FARM;      // 0 : ALGAE, 1 : Water Farm, 2 : SOIL
  
  REG.LoraSet.BaudRate = 115200;
  REG.LoraSet.COMPort =0;
  REG.LoraSet.DataBit=8;
  REG.LoraSet.Flow=0;
  REG.LoraSet.ID=0;
  REG.LoraSet.Mode=0;
  REG.LoraSet.ParityBit=0;
  REG.LoraSet.StopBit=1;

  REG.LoraSet.Device_Channel=0;
  REG.LoraSet.Device_FREQ=LORA_FREQ_9221;
  REG.LoraSet.Device_SF_val=LORA_SF_12;
  REG.LoraSet.Device_MaxEIRP=LORA_MaxEIRP_0dB;

  REG.MeterSet.Lora_Send_Interval = 1*60*10;  // min * SEC * 10(0.1sec unit)
  REG.MeterSet.Sensor_Scan_Interval = 3*100;   // SEC * 100(0.01sec unit)
  
  SensorRead.DHT220.RH_Data=0;
  SensorRead.DHT220.T_Data=0;
  SensorRead.DHT220.CheckSum=0;
  
  mp =(U16 *)&FirmwareVersion;
  memcpy(REG.Ver.FwVer,mp,6);
  
}

void FlashSetupSave_icast(void)
{
    int k;
    int p=0;

    U16 *buf; 

    Send_String_To_PC(" Magic 2"); 
    // memset(buf, 0, FLASH_BUFFER_MAX + 1);
    // Send_String_To_PC(" Magic 2-1"); 
    buf =(U16 *)&REG;
    Send_String_To_PC(" Magic 3"); 
    // memcpy(buf,&REG,REG.gICAST.fFlash_length); 
    memcpy(buf,&REG,REG.gICAST.fFlash_length);
    Send_String_To_PC(" Magic 4"); 
    buf[FLASH_BUFFER_MAX-3] = FLASH_MAGIC_NUM3;
    buf[FLASH_BUFFER_MAX-2] = FLASH_MAGIC_NUM2;
    buf[FLASH_BUFFER_MAX-1] = FLASH_MAGIC_NUM1;
    writeFlash(DataFlashAddr,buf,FLASH_BUFFER_MAX);
    Send_String_To_PC(" Magic 5"); 
}

void FlashSetupLoad_icast(void)
{
  int i, k;
  int p=0;
  U16* mp;
  U16 buf[FLASH_BUFFER_MAX]; 
  
  memset(buf, 0, FLASH_BUFFER_MAX  +1);
  readFlash(DataFlashAddr,buf,FLASH_BUFFER_MAX);
  if(buf[FLASH_BUFFER_MAX-3] != FLASH_MAGIC_NUM3 || buf[FLASH_BUFFER_MAX-2] != FLASH_MAGIC_NUM2 || buf[FLASH_BUFFER_MAX-1] != FLASH_MAGIC_NUM1)
  {
    Send_String_To_PC(" Magic number wrong :"); 
    Send_Number_To_PC(buf[FLASH_BUFFER_MAX-1]);
    
    ICAST_Factory_Value_Loading();
    Send_String_To_PC(" Magic 1"); 
    gSysReg.Flag.Flash_Save_Flag = 0;
    FlashSetupSave_icast();
    Send_String_To_PC("\r\n"); 
  } 
  else
  {
    Send_String_To_PC(" Magic number good :"); 
    Send_Number_To_PC(buf[FLASH_BUFFER_MAX-1]);
    Send_String_To_PC("\r\n"); 
    ICAST_Factory_Value_Loading();    // default value loading
    
    memcpy(&REG,&buf[0],REG.gICAST.fFlash_length);

    gSysReg.Time.Sensor_Scan_Count = REG.MeterSet.Sensor_Scan_Interval;
    SEL_MODEL = REG.Setup.ProductID_L;
  }
}

/////////////////////// HRCHO ADD END///////////////////////////

void writeFlash(uint32_t startAddress, uint16_t *buffer, uint16_t size)
{
    u32 i;
    FLASH_Unlock();//unlock flash writing
    FLASH_ClearFlag(FLASH_FLAG_EOP|FLASH_FLAG_PGERR|FLASH_FLAG_WRPRTERR);
    FLASH_ErasePage(startAddress);//erase the entire page before you can write as I //mentioned
    for(i=0; i<size; i++)
      FLASH_ProgramHalfWord((startAddress + (i*2)),buffer[i]);
    FLASH_Lock();//lock the flash for writing
}
void readFlash(uint32_t startAddress, uint16_t *buffer, uint16_t size)
{
    u32 i;    
    for(i=0; i<size; i++)
      buffer[i] = *(uint16_t *)(startAddress + (i*2));
}

void ResetTypeCheck(void)
{
    if (RCC_GetFlagStatus(RCC_FLAG_IWDGRST) != RESET)
    {
        //printf("IWDGRST flag set ...\n");
        /* IWDGRST flag set */
        /* Clear reset flags */
        RCC_ClearFlag();
    }
    else
    {
        //printf("IWDGRST flag is not set ...\n");
        /* IWDGRST flag is not set */
    }
}

void WatchDogSet(void)
{
    /* Enable write access to IWDG_PR and IWDG_RLR registers */
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
    /* IWDG counter clock: 40KHz(LSI) / 32 = 1.25 KHz */
    IWDG_SetPrescaler(IWDG_Prescaler_32);
    /* Set counter reload value to 349 */
    IWDG_SetReload(349); // 349 = 0x15D   //12bit  0.8x1250=1000ms
    /* Reload IWDG counter */
    IWDG_ReloadCounter();
    /* Enable IWDG (the LSI oscillator will be enabled by hardware) */

    IWDG_Enable();
    gSysReg.Flag.IWDG_EnableFlag=1;
}

void SystemReset(void)
{
   gSysReg.Flag.IWDG_EnableFlag=0;   //reset
   while(1);  
}  

void Current_Time_Val_Check(U8 current_sec)
{
  int temp_val;
  if(current_sec>=60) {
    gSysReg.lora.Minute++;
    gSysReg.lora.Sec=0;
    if(gSysReg.lora.Minute>=60) {
      gSysReg.lora.Hour++;
      gSysReg.lora.Minute=0;
      if(gSysReg.lora.Hour>=24){
        gSysReg.lora.Day++;
        gSysReg.lora.Hour=0;
        switch(gSysReg.lora.Month){
          case 1 :
          case 3 :
          case 5 :
          case 7 :
          case 8 :
          case 10 :
          case 12 :
            temp_val =31;
            break;
          case 4 :
          case 6 :
          case 9 :
          case 11 :
            temp_val =30;
            break;
          case 2 :
            temp_val =28;
            break;
        }
        if(gSysReg.lora.Day>temp_val){
          gSysReg.lora.Month++;
          gSysReg.lora.Day=1;
          if(gSysReg.lora.Month>12) {
            gSysReg.lora.Year++;
            gSysReg.lora.Month=1;
          }
        } 
      }
    }
  }
}

void Timer2_ISR(void) //10ms
{
  if(gSysReg.Flag.IWDG_EnableFlag)IWDG_ReloadCounter();   //Reload IWDG counter
  if(gSysReg.Time.Current_SS_count++ >= gSysReg.Time.Sensor_Scan_Count)  // 10m x 300 = 3sec
  { 
    gSysReg.Time.Current_SS_count = 0;
    gSysReg.Flag.CSV_OUTPUT_Flag =1;
  } 
  Timer_10mSec_Conter++;

  if(gSysReg.Time.Timer_HeartBeat_Cnt!=0) gSysReg.Time.Timer_HeartBeat_Cnt--;
  else
  {
    if(gSysReg.Time.heartbeat_flag==0){
      gSysReg.Time.heartbeat_flag =1;
      gSysReg.Time.Timer_HeartBeat_Cnt=49;
      gSysReg.Time.RunTime++;
      Current_Time_Val_Check(++gSysReg.lora.Sec);
      if(gSysReg.Time.LoRa_Aging_Counter !=0) gSysReg.Time.LoRa_Aging_Counter--;

    }
    else{
      gSysReg.Time.heartbeat_flag =0;
      gSysReg.Time.Timer_HeartBeat_Cnt=49;
    }
  }
}


void Timer3_ISR(void) // step_motor driver
{
  // Timer3 interrupt sevice routine
}


/////////////////////////Start HRCHO//////////////////////////

void Make_DHT220_Sensor_Data(void)
{
  U16 reg_data;
  for(int i=0;i<(ICAST_AM2302_DHT22_HUMINITY_LENGTH);i++)
  {
    reg_data = (U16)(SensorRead.AM2302_DHT22_Humidity.AM2302_Data[i*2+0]<<8 | SensorRead.AM2302_DHT22_Humidity.AM2302_Data[i*2+1]);
    switch(i)
    {
      case 0 : SensorRead.DHT220.RH_Data     = reg_data;   break;    // addr 3
      case 1 : SensorRead.DHT220.T_Data      = reg_data;   break;    // addr 5
      default :
      break;
    }
    SensorRead.DHT220.CheckSum = SensorRead.AM2302_DHT22_Humidity.AM2302_Data[4];
  }
}

/////////////////////////End HRCHO//////////////////////////

void GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  // pin PB3 and PB4 using to GPIO
  CLEAR_BIT(AFIO->MAPR,AFIO_MAPR_SWJ_CFG);            
  SET_BIT(AFIO->MAPR, AFIO_MAPR_SWJ_CFG_JTAGDISABLE);

  //portA output
  RCC_APB2PeriphResetCmd(RCC_APB2Periph_GPIOA, DISABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
  GPIO_InitStructure.GPIO_Pin   =  P_SSR_CON | P_MOTOR_STATUS; 
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  GPIO_ResetBits(GPIOA,P_SSR_CON );
  GPIO_ResetBits(GPIOA,P_MOTOR_STATUS );

  //portA input
  GPIO_InitStructure.GPIO_Pin =  P_SSR_M_CON;  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  //ADC GPIOA
  // GPIO_InitStructure.GPIO_Pin = ;   
  // GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  // GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  //portB 
  RCC_APB2PeriphResetCmd(RCC_APB2Periph_GPIOB, DISABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
  GPIO_InitStructure.GPIO_Pin =  LORA_RESET;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin =  MCU_TP_B_6|SW_IN_B_12|SW_IN_B_13;  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  GPIO_ResetBits(GPIOB,LORA_RESET );
  
  //portC 
  RCC_APB2PeriphResetCmd(RCC_APB2Periph_GPIOC, DISABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
  // GPIO_InitStructure.GPIO_Pin = ;   
  // GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  // GPIO_Init(GPIOC, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = MCU_LED_4|MCU_LED_3|MCU_LED_2|MCU_LED_1 ;   
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  // GPIO_InitStructure.GPIO_Pin =  ;   
  // GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  // GPIO_Init(GPIOC, &GPIO_InitStructure);
  GPIO_ResetBits(GPIOC,MCU_LED_1 );
  GPIO_ResetBits(GPIOC,MCU_LED_2 );
  GPIO_ResetBits(GPIOC,MCU_LED_3 );
  GPIO_ResetBits(GPIOC,MCU_LED_4 );

  //portD   
  // RCC_APB2PeriphResetCmd(RCC_APB2Periph_GPIOD, DISABLE);
  // RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
  // GPIO_InitStructure.GPIO_Pin = ;   
  // GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  // GPIO_Init(GPIOD, &GPIO_InitStructure);

}


/*******************************************************************************
* Function Name  : RCC_Configuration
* Description    : Configures the different system clocks.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void RCC_Configuration(void)
{
ErrorStatus HSEStartUpStatus;

 /* RCC system reset(for debug purpose) */
  RCC_DeInit();

  /* Enable HSE */
  RCC_HSEConfig(RCC_HSE_ON);

  /* Wait till HSE is ready */
  HSEStartUpStatus = RCC_WaitForHSEStartUp();

  if(HSEStartUpStatus == SUCCESS)
  {
    /* Enable Prefetch Buffer */
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

    /* Flash 2 wait state */
    FLASH_SetLatency(FLASH_Latency_2);

    /* HCLK = SYSCLK */
    RCC_HCLKConfig(RCC_SYSCLK_Div1);

    /* PCLK2 = HCLK */
    RCC_PCLK2Config(RCC_HCLK_Div1);

    /* PCLK1 = HCLK/2 */
    RCC_PCLK1Config(RCC_HCLK_Div2);

    /* PLLCLK = 8MHz * 9 = 72 MHz */
    RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);
    //RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_3);  //24mhz

    /* Enable PLL */
    RCC_PLLCmd(ENABLE);

    /* Wait till PLL is ready */
    while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
    {
    }
    /* Select PLL as system clock source */
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

    /* Wait till PLL is used as system clock source */
    while(RCC_GetSYSCLKSource() != 0x08)
    {
    }
  }
  /*clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 | RCC_APB1Periph_SPI2 | RCC_APB1Periph_TIM3 |
                         RCC_APB1Periph_USART2 | RCC_APB1Periph_USART3 | RCC_APB1Periph_BKP |
                           RCC_APB1Periph_PWR, ENABLE);

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_SPI1 | 
                         RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | 
                         RCC_APB2Periph_GPIOD | RCC_APB2Periph_AFIO  | 
                         RCC_APB2Periph_USART1 | RCC_APB2Periph_ADC1 , ENABLE);
}

void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;

#ifdef  VECT_TAB_RAM
  /* Set the Vector Table base location at 0x20000000 */
  NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0);

#else  /* VECT_TAB_FLASH  */
  /* Configure one bit for preemption priority */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

  /* Enable the USART1 Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
 
  /* Enable the USART2 Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  /* Enable the USART2 Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  /* Enable the TIM2 global Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  /* Enable the TIM3 global Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  /* Set the Vector Table base location at 0x08000000 */
  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);

  /* 1 bit for pre-emption priority, 3 bits for subpriority */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
 
#endif
}

void Timer_Configuration(void) //timer2
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  /* Time base configuration */
  TIM_TimeBaseStructure.TIM_Period = 10000;  //10ms
  TIM_TimeBaseStructure.TIM_Prescaler = 0;
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
  /* Prescaler configuration */
  TIM_PrescalerConfig(TIM2, 71, TIM_PSCReloadMode_Immediate);   // 1mSec timer

  /* Time base configuration */
  TIM_TimeBaseStructure.TIM_Period = 1000;  //1ms
  TIM_TimeBaseStructure.TIM_Prescaler = 0;
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
  /* Prescaler configuration */
  TIM_PrescalerConfig(TIM3, 71, TIM_PSCReloadMode_Immediate);   // 1mSec timer

  /* TIM enable counter */
  TIM_Cmd(TIM2, ENABLE);
  /* TIM IT enable */
  TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

  /* TIM enable counter */
  TIM_Cmd(TIM3, ENABLE);
  /* TIM IT enable */
  TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);

}
 

