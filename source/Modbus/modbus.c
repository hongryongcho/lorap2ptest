
/******************************************************************************/
//modbus.c 

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "main.h"

extern SysReg gSysReg;
extern CREG REG;
extern RegData SensorRead;

//----- Parity ------
#define PRTY_NONE     0
#define PRTY_ODD      1
#define PRTY_EVEN     2

//----- Bits ---------
#define RSFIVE_BITS   0
#define RSSIX_BITS    1
#define RSSEVEN_BITS  2
#define RSEIGHT_BITS  3
#define RSSTOP_1BIT   0
#define RSSTOP_2BIT   1

#define _HoldLine    1
#define _UnHoldLine  0

AM2302_DHT22_REG_Data AM2302_Humidity_RCV_Data_Copy(U8 *rcv_data)
{
  AM2302_DHT22_REG_Data temp_data;
  U16 reg_data;
  temp_data = SensorRead.AM2302_DHT22_Humidity;
  for(int i=0;i<(ICAST_AM2302_DHT22_HUMINITY_LENGTH);i++)
  {
    reg_data = (U16)(rcv_data[i*2+0]<<8 | rcv_data[i*2+1]);
    switch(i)
    {
      case 0 : temp_data.Humidity     = reg_data;   break;    // addr 3
      case 1 : temp_data.Temperaute   = reg_data;   break;    // addr 5
      default :
      break;
    }
  }
  return temp_data;
}


///////////////////////////ICAST START////////////////////////////////

///////////////////////////ICAST END////////////////////////////////

void Send_To_Lora(const char *packet) {
  int i, lstrlen = strlen(packet);
  char endData[2]={0x0d,0x0a};
  
  for(i=0; i < lstrlen; i++) {
     if((gSysReg.Flag.ICAST_DBG_ON_LORA_TX_MONITOR)&&(gSysReg.Flag.ICAST_CSV_MONITOR_ENABLE==0)){
       USART_SendData(USART1, packet[i]);
     }
    USART_SendData(USART3, packet[i]);
    while( USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET );
  }
  for(i=0; i < 2; i++) {
     if((gSysReg.Flag.ICAST_DBG_ON_LORA_TX_MONITOR)&&(gSysReg.Flag.ICAST_CSV_MONITOR_ENABLE==0)){
       USART_SendData(USART1, endData[i]);
     }
    USART_SendData(USART3, endData[i]);
    while( USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET );
  }
}

void Send_To_PC(const char *packet) {
  int i, lstrlen = strlen(packet);
  char endData[2]={0x0d,0x0a};
  for(i=0; i < lstrlen; i++) {
    USART_SendData(USART1, packet[i]);
    while( USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET );
  }
  for(i=0; i < 2; i++) {
    USART_SendData(USART1, endData[i]);
    while( USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET );
  }
}

void Send_Char_To_PC(char *char_data)
{
    USART_SendData(USART1, char_data[0]);
    while( USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET );
}

void Send_String_To_PC(const char *packet) {
  int i, lstrlen = strlen(packet);
  for(i=0; i < lstrlen; i++) {
    USART_SendData(USART1, packet[i]);
    while( USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET );
  }
}
void Send_String_To_PC_WithOutCR(const char *packet) {
  int i, lstrlen = strlen(packet);
  for(i=0; i < lstrlen-1; i++) {
    USART_SendData(USART1, packet[i]);
    while( USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET );
  }
}

void Send_Number_To_PC(int number){
    char s1[10];       // ��ȯ�� ���ڿ��� ������ �迭
    sprintf(s1, "%d", number);    // %d�� �����Ͽ� ������ ���ڿ��� ����
    Send_String_To_PC(s1);
}
void Send_CSV_Number_To_PC(int number){
    char s1[10];       // ��ȯ�� ���ڿ��� ������ �迭
    sprintf(s1, "%d,", number);    // %d�� �����Ͽ� ������ ���ڿ��� ����
    Send_String_To_PC(s1);
}
void Send_CSV_U16Num_To_PC(U16 number){
    char s1[10];       // ��ȯ�� ���ڿ��� ������ �迭
    sprintf(s1, "%d,", number);    // %d�� �����Ͽ� ������ ���ڿ��� ����
    Send_String_To_PC(s1);
}
void Send_CSV_U32Num_To_PC(U32 number){
    char s1[10];       // ��ȯ�� ���ڿ��� ������ �迭
    sprintf(s1, "%d,", number);    // %d�� �����Ͽ� ������ ���ڿ��� ����
    Send_String_To_PC(s1);
}
void Send_CSV_FloatNum_To_PC(U32 number){
  char s1[10];       // ��ȯ�� ���ڿ��� ������ �迭
  Flo32 f_val;
    f_val = floatizeMe2(number);
    sprintf(s1, "%f,", f_val);    // %d�� �����Ͽ� ������ ���ڿ��� ����
    Send_String_To_PC(s1);
}
void Send_CSV_Flo32_To_PC(Flo32 number){
  char s1[10];       // ��ȯ�� ���ڿ��� ������ �迭
    sprintf(s1, "%f,", number);    // %d�� �����Ͽ� ������ ���ڿ��� ����
    Send_String_To_PC(s1);
}
void Send_CSV_U32HexNum_To_PC(U32 number){
  char s1[10];       // ��ȯ�� ���ڿ��� ������ �迭
    sprintf(s1, "%x,", number);    // %d�� �����Ͽ� ������ ���ڿ��� ����
    Send_String_To_PC(s1);
}
void Send_CSV_String_To_PC(const char *packet) {
  int i, lstrlen = strlen(packet);
  for(i=0; i < lstrlen; i++) {
    USART_SendData(USART1, packet[i]);
    while( USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET );
  }
  Send_String_To_PC(",");
}

Flo32 icast_pow(U32 val, S32 exp)
{
  Flo32 rtn_val=1;
  if(exp<1)
  {
    for(int i=0;i>=exp;i--)
    {
      rtn_val = rtn_val/val;
    }
   return rtn_val;
  }

  for(int i=0;i<exp;i++)
  {
    rtn_val = rtn_val*val;
  }
  return rtn_val;
}

Flo32 floatizeMe2 (U32 myNumba )
{
    U32  sign = myNumba >>31;
    S32  exponent = ((myNumba >> 23) & 0x00ff);
    U32  mantissa = myNumba << 9;
    Flo64  value = 0;
    Flo64 mantissa2;
    U8 loop_cnt=0;
    exponent = exponent - 127;
    
    value = 0.5;
    mantissa2 = 0.0;
    while (mantissa)
    {
      loop_cnt++;
        if (mantissa & 0x80000000){
          mantissa2 += value;
        }
        mantissa <<= 1;
      value = value * 0.5;
    }

    value = (Flo32)((Flo32)(1+mantissa2)*(Flo32)(icast_pow(2, exponent)));
    if(sign) value = -value;
    return (Flo32)value;
}

void Send_FNumber_To_PC(float number, int point_val){
    int display_val;       // ��ȯ�� ���ڿ��� ������ �迭
    int cal_val;
    char s1[10];       // ��ȯ�� ���ڿ��� ������ �迭
    display_val = (int)(number);
    sprintf(s1, "%d.", display_val);    // %d�� �����Ͽ� ������ ���ڿ��� ����
    Send_String_To_PC(s1);

    cal_val = display_val;
    for(int i=0;i<point_val;i++)
    {
      number*=10;
      cal_val*=10;
    }
    display_val = (int)(number-cal_val);
    sprintf(s1, "%d", display_val);    // %d�� �����Ͽ� ������ ���ڿ��� ����
    Send_String_To_PC(s1);
}

U16 DataCnt = 0;
U8 DataEnd1 = 0;
U8 DataEnd2 = 0;
char RxData[256];
void Flash_data_update(U8 update_id, U8 *p)
{
  switch(update_id)
  {
    case eFlash_Update_APPEUI :     
      for(int i=0;i<16;i++) REG.MeterSet.LORA_EUI[i] = p[i];
      // memcpy(REG.MeterSet.LORA_EUI, &p,16);
     break;
    case eFlash_Update_APPKEY :      break;
    default :       break;
  }
}


void icast_motor_test(int command, int motor_data)
{
  switch(command)
  {
    case 1 :   break;    // target position move
    case 2 :   break;    // Accelation ratio change
    case 3 :   break;    // Break step count change
    case 4 :   break;    // max speed change
    case 5 :   break;    // min speed change
    case 6 :   break;    // Mechnical limit high change
    case 7 :   break;    // Mechnical limit low change
    case 8 :   break;    // motor phase locking flag
    case 9 :   FlashSetupSave_icast();                          break;    // flash memory save 
    case 10 :     break;    // flash memory save 
    case 11 :     break;    
    case 12 :     break;    
    case 13 :     break;    
    
    default :
      break;
  }
}
void icast_system_Set(int command,int set_data)
{
  switch(command)
  {
    case 0 : FlashSetupSave_icast(); 
      break;
    case 1 : GPIO_Level_Change(eICAST_GPOUT_SSR_STATUS,set_data);
      break;
    case 2 : 
            gSysReg.Flag.ICAST_DBG_ON_LORA_TX_MONITOR = 1;
            gSysReg.Flag.ICAST_DBG_ON_LORA_RX_MONITOR = 1;
      break;
    case 3 : GPIO_Level_Change(eICAST_GPOUT_LORA_RESET,set_data);
      break;
    case 4 : REG.gICAST.Lora_UpLink_WaitTime = set_data;
      break;
    case 5 :      // req CSV log header name
            gSysReg.Flag.ICAST_CSV_MONITOR_ENABLE = (U8)set_data;
            if(set_data ==1) {
              Print_CSV_Header_Name();
            }
      break;
    case 6 :      // req CSV log data value
            gSysReg.Flag.CSV_OUTPUT_Flag =1;
      break;
    case 7 :
              REG.MeterSet.Sensor_Scan_Interval = (U16)set_data; 
              gSysReg.Time.Sensor_Scan_Count = REG.MeterSet.Sensor_Scan_Interval;
              FlashSetupSave_icast(); 
      break;
    case 8 :      // LoRa debug command 
              Send_LoRa_Command((U8)set_data);
      break;
    case 9 : REG.LoraSet.Device_Channel = (U8)set_data;     break;
    case 10 : REG.LoraSet.Device_FREQ = (U8)set_data;       break;
    case 11 : REG.LoraSet.Device_SF_val = (U8)set_data;     break;
    case 12 : REG.LoraSet.Device_MaxEIRP = (U8)set_data;    break;
    
    default :
      break;
  }

}

          
void At_CmdParser(char cmd, U8 sendTo)
{
    int i;
    U8 tempAdd;
    U32 data;
    const char *at_set_config="at+set_config";
    const char *at_get_config="at+get_config";
    const char *at_loraap_s_status="at+loraap=status";
    const char *at_loraap_s_wakeup="at+loraap=status:wakeup";
    const char *at_loraap_s_request="at+loraap=status:request";//joined";
    const char *at_loraap_s_sleep="at+loraap=status:sleep";
    const char *at_set_config_appkey="at+set_config=lora:app_key:";
    const char *at_set_config_appeui="at+set_config=lora:app_eui:";
    const char *at_set_config_lora_send_interval="at+set_config=lora:send_interval:";
    const char *at_get_config_lora_send_interval="at+get_config=lora:send_interval:";
    
    const char *at_version="at+version";
    const char *at_help="at+help";
    const char *at_run="at+run";
    const char *at_send="at+send";
    const char *at_join="at+join";
    // const char *at_get_config="at+get_config";
    // const char *at_set_config="at+set_config";
    const char *it_set_config="it+set=system";

    const char *it_set_sensor_map="it+set=sensor:map";
    const char *it_get_sensor_map="it+get=sensor:map";
    const char *it_set_sensor_offset="it+set=sensor:offset:";
    const char *it_get_sensor_offset="it+get=sensor:offset:";
    const char *it_set_sensor_sensitivity="it+set=sensor:sensitivity:";
    const char *it_get_sensor_sensitivity="it+get=sensor:sensitivity:";

    const char *test_get_Sensor_Read="it+get=sensor:sensorread:";
    const char *test_set_Sensor_Set="it+set=sensor:sensorset:";
    const char *test_set_csvdebug_mode="it+set=csvdebug:";
    const char *test_set_sensordebug_mode="it+set=sensordebug:";
    const char *test_set_loradebug_mode="it+set=loradebug:";
    const char *test_set_motor_mode="it+set=motor:";

    U8 temp[16];
    char cCmd [256] = {0,};
    char cCmd1 [64] = {0,};
    char cCmd2 [64] = {0,};
    char *pString;

    RxData[DataCnt] = cmd;
    if(cmd == ICAST_CARRYRETURN_ASCII_CODE) DataEnd1= DataCnt;
    else if(cmd == ICAST_LINEFEED_ASCII_CODE) DataEnd2 = DataCnt;

    if(DataEnd1 == (DataEnd2 - 1))
    {
        if((strncmp(at_set_config, RxData, ICAST_LORA_Clen_set_config) == 0) || (strncmp(at_get_config, RxData, ICAST_LORA_Clen_get_config) == 0))    // at+get_config or at+set_config
        {
            sprintf(cCmd, "%s",RxData);
            if(sendTo == ICAST_UART_CH_PC)            // PC -> LoRa   // main loop defined the send to LoRa command string
              Send_To_Lora(cCmd);
            else if(sendTo == ICAST_UART_CH_SENSOR)   // PC -> Sensor // current source cod didn't used the sensor command 
              Send_To_PC(cCmd);

            if(strncmp(at_set_config_appeui, RxData, ICAST_LORA_Clen_lora_app_eui) == 0)    // at+set_config=lora:app_eui: (26char), LoRa EUI 0x5047 
            {
                memcpy(cCmd1, RxData,DataEnd2);
                for(int i=0;i<16;i++) {
                  REG.MeterSet.LORA_EUI[i] = cCmd1[ICAST_LORA_Clen_lora_app_eui+i];
                }
            }
            else if(strncmp(at_set_config_appkey, RxData, ICAST_LORA_Clen_lora_app_key) == 0)
            {
                memcpy(cCmd1, RxData,DataEnd2);
                for(int i=0;i<32;i++) {
                  REG.MeterSet.LORA_KEY[i] = cCmd1[ICAST_LORA_Clen_lora_app_key+i];
                }
            }
            else if(strncmp(at_set_config_lora_send_interval, RxData, ICAST_LORA_Clen_lora_set_send_interval) == 0)
            {
              U16 rcv_data=0;
              metal_strtok(&RxData[ICAST_LORA_Clen_lora_set_send_interval-1],":");

              pString = metal_strtok(NULL,"\r");
              sscanf(pString,"%d",&rcv_data);

              REG.MeterSet.Sensor_Scan_Interval = rcv_data; 
              gSysReg.Time.Sensor_Scan_Count = REG.MeterSet.Sensor_Scan_Interval;
            }
            else if(strncmp(at_get_config_lora_send_interval, RxData, ICAST_LORA_Clen_lora_get_send_interval) == 0)
            {
                strncpy(cCmd2,RxData,ICAST_LORA_Clen_lora_get_send_interval);
                sprintf(cCmd1, "%s%d",cCmd2,REG.MeterSet.Sensor_Scan_Interval);
                Send_To_PC(cCmd1);
            }
        }
        else if(strncmp(at_loraap_s_status, RxData, ICAST_LORA_Clen_status) == 0)     // status value return to LoRA 
        {
            if(strncmp(at_loraap_s_wakeup, RxData, ICAST_LORA_Clen_status_wakeup) == 0){
              gSysReg.lora.LoraStatus  = S_LORA_WAKEUP;
            }
            else if(strncmp(at_loraap_s_request, RxData, ICAST_LORA_Clen_status_request) == 0){
              gSysReg.lora.LoraStatus = S_LORA_REQUEST;
            } 
            else if(strncmp(at_loraap_s_sleep, RxData, ICAST_LORA_Clen_status_sleep) == 0) {
              gSysReg.lora.LoraStatus = S_LORA_SLEEP;
            }
            else
              gSysReg.lora.LoraStatus = S_LORA_SLEEP;

            strncpy(cCmd2,RxData,ICAST_LORA_Clen_status_request);
            sprintf(cCmd1, "%s lora status : %d",cCmd2,gSysReg.lora.LoraStatus);
            Send_To_PC(cCmd1);
        }
        else if(strncmp(it_get_sensor_map, RxData, ICAST_LORA_Clen_get_sensor_map) == 0)
        {
        }
        else if(strncmp(it_set_sensor_map, RxData, ICAST_LORA_Clen_set_sensor_map) == 0)
        {
        }
        else if((strncmp(it_get_sensor_offset, RxData, ICAST_LORA_Clen_get_sensor_offset) == 0) || (strncmp(it_set_sensor_offset, RxData, ICAST_LORA_Clen_set_sensor_offset) == 0))
        {
          if(strncmp(it_set_sensor_offset, RxData, ICAST_LORA_Clen_set_sensor_offset) == 0)
          {
          }
          else if(strncmp(it_get_sensor_offset, RxData, ICAST_LORA_Clen_get_sensor_offset) == 0)
          {
          }
        }
        else if((strncmp(it_get_sensor_sensitivity, RxData, ICAST_LORA_Clen_get_sensor_sensitivity) == 0) || (strncmp(it_set_sensor_sensitivity, RxData, ICAST_LORA_Clen_set_sensor_sensitivity) == 0))
        {
        }
        else if(strncmp(test_get_Sensor_Read, RxData, ICAST_LORA_Clen_get_sensor_test_cmd) == 0)
        {
        }
        else if(strncmp(test_set_Sensor_Set, RxData, ICAST_LORA_Clen_set_sensor_test_cmd) == 0)
        {
        }
        else if(strncmp(test_set_csvdebug_mode, RxData, ICAST_DBG_Clen_set_csv) == 0)
        {

          int on_off_flag=0;

          strncpy(cCmd2,RxData,ICAST_DBG_Clen_set_csv);
          metal_strtok(&RxData[ICAST_DBG_Clen_set_csv-1],":");
          pString = metal_strtok(NULL,"\r");
          sscanf(pString,"%d",&on_off_flag);

          if(on_off_flag==1)
          {
            Print_CSV_Header_Name();
            gSysReg.Flag.ICAST_CSV_MONITOR_ENABLE =1;
          } 
          else gSysReg.Flag.ICAST_CSV_MONITOR_ENABLE=0;
        }
        else if(strncmp(test_set_sensordebug_mode, RxData, ICAST_DBG_Clen_set_sensor) == 0)
        {
        }
        else if(strncmp(test_set_loradebug_mode, RxData, ICAST_DBG_Clen_set_lora) == 0)
        {

          int on_off_flag=0;

          strncpy(cCmd2,RxData,ICAST_DBG_Clen_set_lora);
          metal_strtok(&RxData[ICAST_DBG_Clen_set_lora-1],":");
          pString = metal_strtok(NULL,"\r");
          sscanf(pString,"%d",&on_off_flag);

          if(on_off_flag==1) 
          {
            gSysReg.Flag.ICAST_DBG_ON_LORA_TX_MONITOR = 1;
            gSysReg.Flag.ICAST_DBG_ON_LORA_RX_MONITOR = 1;
          }
          else 
          {
            gSysReg.Flag.ICAST_DBG_ON_LORA_TX_MONITOR = 0;
            gSysReg.Flag.ICAST_DBG_ON_LORA_RX_MONITOR = 0;
          }
        }
        else if(strncmp(test_set_motor_mode, RxData, ICAST_DBG_Clen_set_motor) == 0)
        {
          int command=0;
          int motor_data=0;

          strncpy(cCmd2,RxData,ICAST_DBG_Clen_set_motor);
          metal_strtok(&RxData[ICAST_DBG_Clen_set_motor-1],":");
          pString = metal_strtok(NULL,":");
          sscanf(pString,"%d",&command);
          pString = metal_strtok(NULL,":");
          sscanf(pString,"%d",&motor_data);

          icast_motor_test(command,motor_data);
        }
        
        else if(strncmp(at_version, RxData, ICAST_LORA_Clen_version) == 0)      {
          Send_LoRa_Command(eLora_CMD_AT_version);
        }
        else if(strncmp(at_help, RxData, ICAST_LORA_Clen_help) == 0)      {
          Send_LoRa_Command(eLora_CMD_AT_help);
        }
        else if(strncmp(at_run, RxData, ICAST_LORA_Clen_run) == 0)      {
          Send_LoRa_Command(eLora_CMD_AT_run);
        }
        else if(strncmp(at_send, RxData, ICAST_LORA_Clen_send) == 0)      {
        }
        else if(strncmp(at_join, RxData, ICAST_LORA_Clen_join) == 0)      {
          LoRa_Mode_Set(eLoRa_Status_JOIN);
        }
        else if(strncmp(at_get_config, RxData, ICAST_LORA_Clen_get_config) == 0)
        {
        }
        else if(strncmp(at_set_config, RxData, ICAST_LORA_Clen_set_config) == 0)
        {
          // pc_cmd_set_config();
        }
        else if(strncmp(it_set_config, RxData, ICAST_LORA_Clen_it_set) == 0)
        {
          int command=0;
          int set_data=0;

          strncpy(cCmd2,RxData,ICAST_DBG_Clen_set_motor);
          metal_strtok(&RxData[ICAST_DBG_Clen_set_motor-1],":");
          pString = metal_strtok(NULL,":");
          sscanf(pString,"%d",&command);
          pString = metal_strtok(NULL,":");
          sscanf(pString,"%d",&set_data);

          icast_system_Set(command,set_data);
        }

        // Send_To_PC("TEST");

        DataEnd1 = 0;
        DataEnd2 = 0;
        DataCnt = 0;

        gSysReg.Flag.Is_Sensor_BoardData = 0;

        for(i=0;i<256;i+=8){
          RxData[i] = NULL;RxData[i+1] = NULL;RxData[i+2] = NULL;RxData[i+3] = NULL;
          RxData[i+4] = NULL;RxData[i+5] = NULL;RxData[i+6] = NULL;RxData[i+7] = NULL;
        }
    }
    else
    {
        DataCnt++;
    }
}

void SSR_Status_Change(uint8 channel, uint8 level)
{
  switch(channel)
  {
    case eSSR_CH0 :  GPIO_Level_Change(eICAST_GPOUT_SSR_STATUS,level);     break;
    case eSSR_CH1 :  GPIO_Level_Change(eICAST_GPOUT_P_SSR_CON,level);     break;
    case eSSR_CH2 :  GPIO_Level_Change(eICAST_GPOUT_MCU_LED1,level);     break;
    case eSSR_CH3 :  GPIO_Level_Change(eICAST_GPOUT_MCU_LED2,level);     break;
    default : break;
  }
}


void HandleUART(void)     // UART2
{ 
  gSysReg.Uart.UART2RxBuffer[gSysReg.Uart.UART2RxHead++] = USART2->DR;
  gSysReg.Uart.UART2RxHead &= 0xff;
  if(gSysReg.Uart.UART2RxHead == gSysReg.Uart.UART2RxTail)
  {
    gSysReg.Uart.UART2RxTail = ((gSysReg.Uart.UART2RxTail+1) & 0x1ff);
  } 
}

void SendUART(void) 
{   
  if(gSysReg.Uart.TxOn < gSysReg.Uart.TxCnt)
  { 
    USART2->DR = gSysReg.Uart.UART2TxBuffer[gSysReg.Uart.TxOn++];
  }
  else 
  {  
    USART_ITConfig(USART2,  USART_IT_TXE, DISABLE);
  }
}
////////////////////////////////////////////////////////////

// extern char UART1RxBuffer[UART1_RX_BUFFER_CNT];
extern u32  UART1RxHead;
extern u32  UART1RxTail;
void HandleUART1(void) 
{ 
  gSysReg.Uart.UART1RxBuffer[gSysReg.Uart.UART1RxHead++] = USART1->DR;
  gSysReg.Uart.UART1RxHead &= 0xff;
  if(gSysReg.Uart.UART1RxHead == gSysReg.Uart.UART1RxTail)
  {
    gSysReg.Uart.UART1RxTail = ((gSysReg.Uart.UART1RxTail+1) & 0x1ff); 
  } 
}

void HandleUART3(void) 
{ 
  gSysReg.Uart.UART3RxBuffer[gSysReg.Uart.UART3RxHead++] = USART3->DR;
  gSysReg.Uart.UART3RxHead &= 0xff;
  if(gSysReg.Uart.UART3RxHead == gSysReg.Uart.UART3RxTail)
  {
    gSysReg.Uart.UART3RxTail = ((gSysReg.Uart.UART3RxTail+1) & 0x1ff);
  } 
  if((gSysReg.Flag.ICAST_DBG_ON_LORA_RX_MONITOR)&&(gSysReg.Flag.ICAST_CSV_MONITOR_ENABLE==0)){
    Send_Char_To_PC(&gSysReg.Uart.UART3RxBuffer[gSysReg.Uart.UART3RxHead-1]);   // hrcho add 0824 
  }
}

void RecvUart1(void)
{         
  char RxD; 
  while(gSysReg.Uart.UART1RxTail != gSysReg.Uart.UART1RxHead)
  {  
    RxD = gSysReg.Uart.UART1RxBuffer[gSysReg.Uart.UART1RxTail++];
    gSysReg.Uart.UART1RxTail &= 0xff;
    if((RxD == 'i' || RxD == 'a') && gSysReg.Flag.Is_Sensor_BoardData == 0)
    {
      gSysReg.Flag.Is_Sensor_BoardData = 1;
    }
    if(gSysReg.Flag.Is_Sensor_BoardData == 1)
    {
      At_CmdParser(RxD, ICAST_UART_CH_PC);
    }
  }  
}

void RecvUart3_icast(void)
{         
  char RxD; 
  while(gSysReg.Uart.UART3RxTail != gSysReg.Uart.UART3RxHead)
  {  
    RxD = gSysReg.Uart.UART3RxBuffer[gSysReg.Uart.UART3RxTail++];
    gSysReg.Uart.UART3RxTail &= 0xff;
    At_CmdFromLora_icast2(RxD, ICAST_UART_CH_LORA);
  }
}


char *metal_strtok(char *_Str, const char *_Delim)
{
  static char* pStr;
  const char *pDelim;
  if(_Str == NULL)
  {
    _Str=pStr;
  }
  else
  {
    pStr = _Str;
  }
  
  while(*pStr)
  {
    pDelim = _Delim;
    while(*pDelim)
    {
      if(*pStr == *pDelim)
      {
        *pStr = NULL;
        pStr++;
        return _Str;
      }
      pDelim++;
    }
    pStr++;
  }
  return _Str;
}