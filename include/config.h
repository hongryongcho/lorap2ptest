#ifndef _CONFIG_H_
#define _CONFIG_H_

#include "stm32f10x.h"
#include "Types.h"

#define SEL_MODEL REG.Setup.ProductID_L    // algae : 0, water farm : 1, soil : 2

#define BAUD_115200       11520
#define BAUD_57600        57600
#define BAUD_38400        38400
#define BAUD_28800        28800
#define BAUD_19200        19200
#define BAUD_14400        14400
#define BAUD_9600         9600
#define BAUD_4800         4800
#define BAUD_2400         2400
#define BAUD_1200         1200

#define MAX_BUF_SIZE		1460
#define KEEP_ALIVE_TIME	30	// 30sec
#define ON	1
#define OFF	0
#define HIGH		1
#define LOW		0
#define __GNUC__
#define TX_RX_MAX_BUF_SIZE	2048

#define METER_CALIB_V   'V'  
#define METER_CALIB_I   'I'     
#define METER_CALIB_T   'T'   
#define METER_CALIB_O   'O'   
#define METER_CALIB_S   'S'   
#define METER_CALIB_R   'R'  
#define DO_OPTOD 11

#define UART1_TX_BUFFER_CNT 256
#define UART2_TX_BUFFER_CNT 256
#define UART3_TX_BUFFER_CNT 256

#define UART1_RX_BUFFER_CNT 256
#define UART2_RX_BUFFER_CNT 256
#define UART3_RX_BUFFER_CNT 256

#define MAX_MAP_CNT 16

#define FLASH_MAGIC_NUM3    0xa5a5
#define FLASH_MAGIC_NUM2    0x5a5a
#define FLASH_MAGIC_NUM1    0x2705

#define PORT_LOW  0
#define PORT_HIGH 1
#define PHOTO_INT_UPSIDE    1
#define PHOTO_INT_DOWNSIDE  0

enum{
  eSSR_CH0=0,
  eSSR_CH1,
  eSSR_CH2,
  eSSR_CH3,
  eSSR_CH_MAX,
};

enum
{
    eMODEL_AIR = 0,
    eMODEL_WATER_FARM ,
    eMODEL_SOIL,
    eMODEL_TOTAL_CNT,
};

enum
{
    eMoPos_UPside = 0,
    eMoPos_Middle ,
    eMoPos_DOWNside,
    eMoPos_Wrong,
};

enum
{
    S_DUMMY1 = 1,
    S_PH,
    S_DO_TEMP,
    S_SALINITY,
    S_CHLOROPHYLL,
    S_TURBIDITY,
    S_MAX,
};

enum
{
    S_LORA_WAKEUP = 1,
    S_LORA_REQUEST,
    S_LORA_SLEEP,

    S_LORA_APPEUI,
    S_LORA_APPKEY,

    S_LORA_P2P_MODE,
    S_LORA_P2P_CONDITION,
    S_LORA_RX_MODE,
    S_LORA_TX_MODE,

    S_LORA_P2P_RESTART,

    S_LORA_MAX,
};

enum{
  eFlash_Update_APPEUI =0,
  eFlash_Update_APPKEY,

};

enum{
  eSensor_No_1=0,
  eSensor_No_2,
  eSensor_No_3,
  eSensor_No_4,
  eSensor_No_5,
  eSensor_No_6,
  eSensor_No_7,
  eSensor_No_8,
  eSensor_No_9,
  eSensor_No_10,
  eSensor_No_11,
  eSensor_No_12,
  eSensor_No_13,
  eSensor_No_14,
  eSensor_No_15,
  eSensor_No_16,
  eSensor_No_MAX,
};


enum{
  eADC_ch0=0,
  eADC_ch1,
  eADC_ch10,
  eADC_ch11,
  eADC_ch12,
  eADC_ch13,
  eADC_ch5_Battery,
  eADC_ch6_Thermistor,
  eADC_ch7_EMF,
  eADC_MAX_channel,
};


//register ---------------------------------------------------------------------
#define MOTOR_MOVE_CW   1
#define MOTOR_MOVE_CCW  0
typedef struct ICAST_Motor
{
  U8 Running_Flag;          // motor running flag
  U8 Start_flag;            // motor start phase flag it is start for phase locking
  int motor_phase;          // phase 0 ~ 3 (2 phase driving)
  U16 motor_direction;      // CW or CCW direction register
  int curent_position;      // current motor position register
  int target_position;      // motor target position at moving
  U16 Accelation_Ratio;     // Accelation ratio it is percent value 
  U16 DeAccelation_Ratio;   // Deaccelation ratio it is percent value 
  U16 Break_Step_Count;     // motor breaking phase count for reduce speed and anti-over moving
  U16 MAX_PPS_Speed;        // motor max pps (pulse per second) value
  U16 MIN_PPS_Speed;        // motor min pps (use to start and stop condition)
  U16 Current_PPS_Speed;    // current motor speed register it is range in MIN and MAX pps speed
  U16 Motor_IDLE_Count;       // motor stop count for phase unlocking
  U16 Motor_Running_Count;    // motor running count during start to stop
  int M_High_Limit_Position;  // mechnical limit position for protect the over running at add direction (CW)
  int M_Low_Limit_Position;   // mechnical limit position for protect the over running at sub direction (CCW)
  int Phase_Locking_Mode;   // motor driving phase locking flag, 1 : Locking, 0 : unlocking
}ICAST_Motor;

typedef struct ICAST_monitor
{
  U8 Sensor_ID[eSensor_No_MAX];
  U16 Sensor_ 
}ICAST_monitor;

typedef struct AM2302_DHT22_REG_Data
{
  U16 Temperaute;   // 16bit T data 0.1 degree unit
  U16 Humidity;     // RH 0.1 percent unit
  uint8_t AM2302_Data[10];
}AM2302_DHT22_REG_Data;

typedef struct TypeConfig //(1)
{			          
  U16   Reset;
  U16   Factory_inite;

}TypeConfig;

typedef struct TypeSetup // (2)
{			          
  U16   ProductID_H;
  U16   ProductID_L;
}TypeSetup;

typedef struct TypeLora  //(4)
{			          
  U16   Mode;
  U16   ID;
  U16   COMPort;
  U32   BaudRate;
  U16   DataBit;
  U16   StopBit;
  U16   ParityBit;
  U16   Flow;

  U8    Device_FREQ;        // FREQ index 0: 921.9Mhz ~ index 7 : 923.3Mhz
  U8    Device_Channel;     // channel 0 ~ 200 : sensor, 201 ~ 255 : monitor
  U8    Device_SF_val;      // SF 7(low) ~ 12(strong)
  U8    Device_MaxEIRP;     // EIRP (power) value

}TypeLora;

typedef struct TypeSensorSet //(5)
{			          
  U16   Sensor_Scan_Interval;
  U16   Lora_Send_Interval;
  U8    LORA_EUI[16];          // 35 + 8   =()
  U8    LORA_KEY[32];         // 35 + 8 + 16
}TypeSensorSet;

typedef struct TypeDHT220 //(6)
{			          
  U16   RH_Data;
  U16   T_Data;
  U8   CheckSum;
}TypeDHT220;


typedef struct TypeVer   //(A) 
{			          
  U16 	FwVer[10];			
}TypeVer;

typedef struct TypeICAST
{
  U16 fFlash_length;
  U16 Motor_MAX_PPS_Speed;
  U16 Motor_MIN_PPS_Speed;
  U16 Motor_Accelation_Ratio;
  U16 Motor_DeAccelation_Ratio;
  U16 Motor_Break_Step_Count;
  int Mechanical_High_Limit_Position;
  int Mechanical_Low_Limit_Position;
  int Motor_Phase_Lock_Mode;
  U16 AirPump_OPEN_Time;
  U16 Lora_UpLink_WaitTime;  // mSec unit
}TypeICAST;

// GREG
typedef struct CREG //(14)
{			           
  TypeConfig      Config;       // 2 words
  TypeSetup       Setup;        // 5 words
  TypeLora        LoraSet;      // 8 words
  TypeSensorSet   MeterSet;     // ? + 8(EUI) + 16(KEY)
  TypeVer         Ver;
  TypeICAST       gICAST;
}CREG;

enum{
    eICAST_CREG_Config=0,
    eICAST_CREG_Setup,
    eICAST_CREG_RS485,
    eICAST_CREG_LoraSet,
    eICAST_CREG_MeterSet,
    eICAST_CREG_TempHumi,
    eICAST_CREG_AdIn,
    eICAST_CREG_Meter,
    eICAST_CREG_EmergStatus,
    eICAST_CREG_Ver,
    eICAST_CREG_gICAST,
    eICAST_MAX_CREG_Num,
    };

typedef struct
{
  TypeDHT220      DHT220;
  AM2302_DHT22_REG_Data       AM2302_DHT22_Humidity;
}RegData;

typedef struct 
{
  U16 Day;
  U8  Hour;
  U8  Min;
  U8  Sec;
  U32 RunTime;

  U16 Sensor_Scan_Count;    // CSV log out interval time it takes flash interval time
  U16 SS_Loop_skip_val;
  U16 Current_SS_count;

  U16 LoRa_Aging_Counter;

  U8 Icast_Sensor_Scan_Status;
  
  U8 Timer_HeartBeat_Cnt;
  U8 heartbeat_flag;

  /* data */
}icast_TimeReg;

typedef struct 
{
  U8 PowerReadReady;
  U8 IWDG_EnableFlag;
  
  U8 ICAST_CSV_MONITOR_ENABLE;
  U8 CSV_OUTPUT_Flag;

  U8 ICAST_DBG_ON_LORA_TX_MONITOR ;
  U8 ICAST_DBG_ON_LORA_RX_MONITOR ;

  U8 Flash_Save_Flag;
  U8 Is_Sensor_BoardData;

  U8 LORA_P2P_Aging_Flag;
  U8 LORA_P2P_Packet_Send_Flag;

}icast_Flag;

typedef struct 
{
  char UART1TxBuffer[UART1_TX_BUFFER_CNT];
  char UART1RxBuffer[UART1_RX_BUFFER_CNT];
  u32  UART1RxHead;
  u32  UART1RxTail;
  char UART2RxBuffer[UART2_RX_BUFFER_CNT];
  u32  UART2RxHead;
  u32  UART2RxTail;
  char UART2TxBuffer[UART2_TX_BUFFER_CNT];
  char UART3RxBuffer[UART3_RX_BUFFER_CNT];
  u32  UART3RxHead;
  u32  UART3RxTail;
  char UART3TxBuffer[UART3_TX_BUFFER_CNT];

  U16 RS485_3BaudRate;
  U16 RS485_3DataBit;
  U16 RS485_3StopBit;
  U16 RS485_3ParityBit;

  U8 TxCnt;
  unsigned char TxOn;

  char Make_Sensor_Packet_A[80];    // at+send=lora:1: (15)+4*2*6(48)+4(0d0a) ==> 15+48+4 => 67
  char Make_Sensor_Packet_B[80];
  char Make_Sensor_Packet_C[80];
  char Make_Temperature_Packet[80];
  U8 Packet_Ready;

}icast_uart;

typedef struct 
{
  U8 Lora_Enable;
  U8 LoraStatus;
  U8 Lora_Packet_Send_Status;
  U8 Lora_Connection;
  U8 LoRa_Return_flag;
  
  int Lora_Rcv_PortNum;
  int Lora_RSSI_Val;
  int Lora_SNR;


  U16 Lora_Packet_Max_Wait_Time;    // 6Sec
  U16 Lora_Packet_data_Send_interval_Time;    // 3Sec
  U16 Lora_Packet_Response_Wait_counter;
  U16 Lora_Join_Retry_count;
  U16 Lora_Sample_p2p_Tx_Timer;

  int rcv_lora_port;
  int rcv_RSSI;
  int rcv_SNR;
  int rcv_LengthOfData;
  U8 rcv_data[32];
  U8 SSR_control[eSSR_CH_MAX];

  U16 UPLink_CNT;
  U16 DOWNLink_CNT;

  U8 PacketType;        // 0x01 ~ 0x04
  U8 Device_ID;         // common : 0x00
  U8 Destination_ID;    // common : 0x00

  U16 Year;
  U8 Month;
  U8 Day;
  U8 Hour;
  U8 Minute;
  U8 Sec;

}icast_lora;

enum{
  Pkt_Sensor2Mon=0,
  Pkt_RTN_From_Mon,
  Pkt_Mon2Sensor,
  Pkt_RTN_From_Sen,
};
enum{
  TARGET_ALL_SENSOR=0,
  TARGET_ALL_MONITOR,
  TARGET_ANYONE,
  TARGET_NOTHING,
};

typedef struct 
{
  volatile icast_TimeReg Time;
  volatile icast_Flag Flag;
  icast_uart Uart;
  volatile icast_lora lora;
  /* data */
}SysReg;





//PORT A
#define P_SSR_M_CON     GPIO_Pin_0
#define MCU_NC_A_1      GPIO_Pin_1
#define MCU_TP_A_2      GPIO_Pin_2
#define MCU_TP_A_2      GPIO_Pin_3
#define P_SSR_CON       GPIO_Pin_4
#define P_MOTOR_STATUS  GPIO_Pin_5
#define MCU_NC_A_6      GPIO_Pin_6
#define MCU_NC_A_7      GPIO_Pin_7
#define USART1_TX       GPIO_Pin_9
#define USART1_RX       GPIO_Pin_10
#define TMS             GPIO_Pin_13
#define TCK             GPIO_Pin_14
#define TDI             GPIO_Pin_15

//PORT B
#define MCU_NC_B_0      GPIO_Pin_0
#define MCU_NC_B_1      GPIO_Pin_1
#define MCU_NC_B_3      GPIO_Pin_3
#define MCU_NC_B_4      GPIO_Pin_4
#define MCU_NC_B_5      GPIO_Pin_5
#define MCU_TP_B_6      GPIO_Pin_6
#define LORA_RESET      GPIO_Pin_7
#define MCU_NC_B_8      GPIO_Pin_8
#define MCU_NC_B_9      GPIO_Pin_9
#define USART3_TX       GPIO_Pin_10
#define USART3_RX       GPIO_Pin_11

#define SW_IN_B_12      GPIO_Pin_12
#define SW_IN_B_13      GPIO_Pin_13
#define MCU_NC_B_14     GPIO_Pin_14
#define MCU_NC_B_15     GPIO_Pin_15

//PORT C
#define MCU_NC_C_0      GPIO_Pin_0 
#define MCU_NC_C_1      GPIO_Pin_1 
#define MCU_NC_C_2      GPIO_Pin_2
#define MCU_NC_C_3      GPIO_Pin_3
#define MCU_NC_C_4      GPIO_Pin_4
#define MCU_NC_C_5      GPIO_Pin_5

#define MCU_LED_4       GPIO_Pin_6
#define MCU_LED_3       GPIO_Pin_7
#define MCU_LED_2       GPIO_Pin_8
#define MCU_LED_1       GPIO_Pin_9

#define MCU_NC_C_10     GPIO_Pin_10
#define MCU_NC_C_11     GPIO_Pin_11
#define MCU_NC_C_12     GPIO_Pin_12

//PORT D
#define OSC_IN          GPIO_Pin_0 
#define OSC_OUT         GPIO_Pin_1
#define MCU_NC_D_2      GPIO_Pin_2

#define TEMP_GPIO_PORT  GPIOC
#define HUMI_GPIO_PORT  GPIOC

enum{
    eICAST_GPOUT_P_SSR_CON=0,
    eICAST_GPOUT_SSR_STATUS,
    eICAST_GPOUT_MCU_LED1,
    eICAST_GPOUT_MCU_LED2,
    eICAST_GPOUT_MCU_LED3,
    eICAST_GPOUT_MCU_LED4,
    eICAST_GPOUT_LORA_RESET,
    eICAST_GPOUT_MAX_NUM,
};    // output

enum{
    eICAST_GPIN_SSR_M_CON=0,
    eICAST_GPIN_TP2,
    eICAST_GPIN_SW_A,
    eICAST_GPIN_SW_B,
    eICAST_GPIN_MAX_NUM,
};    // input

// enum
// {
//     ICST_HN_Project_Name=0,
//     ICST_HN_Test_Time,
//     ICST_HN_MODULE_SN,
//     ICST_HN_FW_VERSION,
//     ICST_HN_Timer_10mSec,

//     ICST_HN_MODULE_MAGICnum1,
//     ICST_HN_MODULE_MAGICnum2,
//     ICST_HN_MODULE_MAGICnum3,

//     // to do start
//     ICST_HN_CFG_RESET,
//     ICST_HN_CFG_FACTORY_INIT,
//     ICST_HN_SETUP_PROD_ID_HIGH,
//     ICST_HN_SETUP_PROD_ID_LOW,
//     ICST_HN_SETUP_TIME_HIGH,
//     ICST_HN_SETUP_TIME_MIDDLE,
//     ICST_HN_SETUP_TIME_LOW,
//     ICST_HN_FLASH_BYTE_COUNT,

//     ICST_HN_RS485_MODE,
//     ICST_HN_RS485_ID,
//     ICST_HN_RS485_COMPort,
//     ICST_HN_RS485_BAUDRATE,
//     ICST_HN_RS485_DATABIT,
//     ICST_HN_RS485_STOPBIT,
//     ICST_HN_RS485_PARITYBIT,
//     ICST_HN_RS485_FLOW,
    
//     ICST_HN_LORA_MODE,
//     ICST_HN_LORA_ID,
//     ICST_HN_LORA_COMPort,
//     ICST_HN_LORA_BAUDRATE,
//     ICST_HN_LORA_DATABIT,
//     ICST_HN_LORA_STOPBIT,
//     ICST_HN_LORA_PARITYBIT,
//     ICST_HN_LORA_FLOW,

//     ICST_HN_SENSOR_Interval,
//     ICST_HN_SENSOR_I_SCALE,
//     ICST_HN_SENSOR_V_SCALE,

 
//     // to do end
//     ICST_HN_DHT22_RH_DATA,
//     ICST_HN_DHT22_T_DATA,
//     ICST_HN_DHT22_CHECKSUM,

//     ICST_HN_BATT_LEVEL,
//     ICST_HN_CO2_ADC_THERMISTOR,
//     ICST_HN_CO2_EMF,
    
//     ICST_HN_CHANNEL_NUM,

//     ICST_HN_SENSOR_PKT_12to15,
//     ICST_HN_SENSOR_PKT_16to19,
//     ICST_HN_SENSOR_PKT_20to23,
//     ICST_HN_SENSOR_WTEMP,

//     ICST_HN_SEN_OFFSET,
//     ICST_HN_EUI,
//     ICST_HN_KEY,

//     ICST_HN_TEMPHUMI_TEMPHUMI,
//     ICST_HN_TEMPHUMI_HUMI,
//     ICST_HN_TEMPHUMI_TEMP2,
//     ICST_HN_ADIN_CH0,
//     ICST_HN_ADIN_CH1,
//     ICST_HN_ADIN_CH2,
//     ICST_HN_ADIN_CH3,
//     ICST_HN_ADIN_CH4,
//     ICST_HN_ADIN_CH5,

//     ICST_HN_EMERGENCY,
    
//     ICST_HN_CR_LF,
//     ICST_HN_MAX_NUM,
// // }enum_ICAST_DATA_HEADER_NAME;
// };

enum{
    ICAST_MBUS_STATUS_IDLE_AND_WORKING=0,
    ICAST_MBUS_STATUS_START,
    ICAST_MBUS_STATUS_END,
};

typedef enum {
  eLoRa_Disable=0,
  eLoRa_Enable,
  
  eLoRa_Status_Sleep,
  eLoRa_Status_Sleep_wait,
  // eLoRa_Status_Version,
  // eLoRa_Status_Version_wait,
  
  eLoRa_Status_Wake_Up=10,
  eLoRa_Status_Wake_Up_wait,
  eLoRa_Status_JOIN,
  eLoRa_Status_JOIN_wait,
  eLoRa_Status_IDLE,

  eLoRa_Status_Send_A=20,         // Fish Farm (DO, ) 
  eLoRa_Status_Send_A_wait,
  eLoRa_Status_Send_B,
  eLoRa_Status_Send_B_wait,
  eLoRa_Status_Send_C,
  eLoRa_Status_Send_C_wait,
  eLoRa_Status_Send_D,
  eLoRa_Status_Send_D_wait,      // max 4 packet

  eLoRa_Status_Send_SOIL_A = 30,
  eLoRa_Status_Send_SOIL_wait,
 
  eLoRa_Status_Send_APPEUI=40,
  eLoRa_Status_Send_APPEUI_wait,
  eLoRa_Status_Send_APPKEY,
  eLoRa_Status_Send_APPKEY_wait,

  eLoRa_Status_Send_P2P=50,
  eLoRa_Status_P2P_ModeSet,
  eLoRa_Status_P2P_ModeSet_wait,
  eLoRa_Status_P2P_ConditionSet,
  eLoRa_Status_P2P_ConditionSet_wait,
  eLoRa_Status_P2P_Tx_Mode_Set,
  eLoRa_Status_P2P_Tx_Mode_Set_wait,
  eLoRa_Status_P2P_Rx_Mode_Set,
  eLoRa_Status_P2P_Rx_Mode_Set_wait,
  eLoRa_Status_P2P_Aging_Send,
  eLoRa_Status_P2P_Aging_Send_wait,
  eLoRa_Status_P2P_CMD_Send,
  eLoRa_Status_P2P_CMD_Send_wait,
  eLoRa_Status_Aging_Sleep,

  eLoRa_Status_MON=70,
  eLoRa_Status_MON_Wait,
  eLoRa_Status_MON_Tx_Mode_Set,
  eLoRa_Status_MON_Tx_Mode_wait,
  eLoRa_Status_MON_Tx_MessageSending,
  eLoRa_Status_MON_Tx_MessageSending_wait,
  eLoRa_Status_MON_Rx_Mode_Set,
  eLoRa_Status_MON_Rx_Mode_wait,
  
  
  
  

}Lora_packet_send_interface;


#endif


