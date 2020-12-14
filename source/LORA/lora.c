

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "main.h"

extern SysReg gSysReg;
extern CREG REG;
extern RegData SensorRead;

U16 LoraDataCnt = 0;
U8 LoraDataEnd1 = 0;
U8 LoraDataEnd2 = 0;
char LoraRxData[128];

void InitLora(U8 status)
{
    char cCmd[64]={0,};
    U8 i=0,j=0;
    switch(status){
      case S_LORA_SLEEP :
        sprintf(cCmd, "at+get_config=device:status");
        Send_To_Lora(cCmd);
        break;
      case S_LORA_WAKEUP :
        Send_LoRa_Command(eLora_CMD_AT_join);
        break;
      case S_LORA_APPEUI :
        sprintf(cCmd, "at+set_config=lora:app_eui:");
        for(int i=0;i<16;i++){
          cCmd[ICAST_LORA_Clen_lora_app_eui+i] = REG.MeterSet.LORA_EUI[i];
        }
        Send_To_Lora(cCmd);
        break;
      case S_LORA_APPKEY :
        sprintf(cCmd, "at+set_config=lora:app_key:");
        for(int i=0;i<32;i++){
          cCmd[ICAST_LORA_Clen_lora_app_key+i] = REG.MeterSet.LORA_KEY[i];
        }
        Send_To_Lora(cCmd);
        break;
      case S_LORA_P2P_MODE :
        sprintf(cCmd, "at+set_config=lora:work_mode:1");
        Send_To_Lora(cCmd);
        break;
      case S_LORA_P2P_CONDITION :
        i = REG.LoraSet.Device_SF_val;
        j = REG.LoraSet.Device_MaxEIRP;
      // at+set_config=lorap2p:<frequency>:<Spreadfact>:<Bandwidth>:<Codingrate>:<Preamlen>:<Powerdbm>
      // 922100000,12,0,1,8,14    (922100000,10,0,1,8,14)
        sprintf(cCmd, "at+set_config=lorap2p:%d,%d,%d,%d,%d,%d",Call_Lora_Freq_Val(REG.LoraSet.Device_FREQ),Call_Lora_Spreadfact_Val(i),
                                                            Call_Lora_bandwidth_Val(i),Call_Lora_Codeingrate_Val(i),
                                                            Call_Lora_preamlen_Val(i),Call_Lora_power_Val(j));
        Send_To_Lora(cCmd);
        break;
      case S_LORA_RX_MODE :
        sprintf(cCmd, "at+set_config=lorap2p:transfer_mode:1");
        Send_To_Lora(cCmd);
        break;
      case S_LORA_TX_MODE :
        sprintf(cCmd, "at+set_config=lorap2p:transfer_mode:2");
        Send_To_Lora(cCmd);
        break;
      case S_LORA_P2P_RESTART:
        sprintf(cCmd, "at+set_config=device:restart");
        Send_To_Lora(cCmd);
        break;
    }
}

uint8 Get_Lora_Status(void)
{
  uint8 rtn_val=0;
  rtn_val = gSysReg.lora.Lora_Packet_Send_Status;
  return rtn_val;
}
uint8 Get_LoraWait_Status(void) // 0 : idle, 1:busy
{
  uint8 rtn_val=0;
  if(gSysReg.lora.Lora_Packet_Response_Wait_counter) rtn_val=1;
  return rtn_val;
}

void Set_LoraWait_Time(U16 WaitCount)   // mSec counter
{
  gSysReg.lora.Lora_Packet_Response_Wait_counter = WaitCount;
  return;
}

#define DAY_SEC_COUNT 60*60*24    // 60sec * 60min * 24 hour
void MakeLoRaSendPacket(void)
{
  uint32 sec_count;
  
  if(gSysReg.Time.RunTime > DAY_SEC_COUNT){
    gSysReg.Time.Day++;
    gSysReg.Time.RunTime -= DAY_SEC_COUNT;
  } 

  sec_count = gSysReg.Time.RunTime;
  gSysReg.Time.Hour = sec_count/(60*60); sec_count -= (gSysReg.Time.Hour*60*60);
  gSysReg.Time.Min = sec_count/60; sec_count -= (gSysReg.Time.Min*60);
  gSysReg.Time.Sec = sec_count;
  // sprintf(gSysReg.Uart.Make_Sensor_Packet_A,"at+send=lora:1:0d7f%04x%02x%02x%02x%02x%02x",
  // gSysReg.Time.Day,gSysReg.Time.Hour,gSysReg.Time.Min,gSysReg.Time.Sec, 0x0d,0x0a);
  sprintf(gSysReg.Uart.Make_Sensor_Packet_A,"at+send=lora:1:0d7f%04x%02x%02x%02x%04x%02x%02x%02x%02x%02x%02x",
  gSysReg.Time.Day,gSysReg.Time.Hour,gSysReg.Time.Min,gSysReg.Time.Sec, 
  POWER_NODE_TTL_LEVEL_CHANGE,gSysReg.lora.SSR_control[eSSR_CH0],gSysReg.lora.SSR_control[eSSR_CH1],gSysReg.lora.SSR_control[eSSR_CH2],gSysReg.lora.SSR_control[eSSR_CH3],
  0x0d,0x0a);
  
  return;
}

void monitor_Time_Set(void)
{
  uint32 sec_count;
}

void current_Time_check(void) // runtime chane to day, hour, min, sec
{
  uint32 sec_count;
  if(gSysReg.Time.RunTime > DAY_SEC_COUNT){
    gSysReg.Time.Day++;
    gSysReg.Time.RunTime -= DAY_SEC_COUNT;
  } 

  sec_count = gSysReg.Time.RunTime;
  gSysReg.Time.Hour = sec_count/(60*60); sec_count -= (gSysReg.Time.Hour*60*60);
  gSysReg.Time.Min = sec_count/60; sec_count -= (gSysReg.Time.Min*60);
  gSysReg.Time.Sec = sec_count;
}

void MakeLoRaMonitor_Return_Message(void)
{
  monitor_Time_Set();
  gSysReg.lora.PacketType = Pkt_RTN_From_Mon;
  gSysReg.lora.Destination_ID = gSysReg.lora.Device_ID;
  // sprintf(gSysReg.Uart.Make_Sensor_Packet_A,"at+send=lorap2p:%02x%02x%02x%02x%04x%02x%02x%02x%02x%02x%04x%04x%02x%02x",gSysReg.lora.PacketType,REG.LoraSet.Device_Channel,gSysReg.lora.Destination_ID,
  // LORA_RCV_PACKET_DATE,gSysReg.lora.Year,gSysReg.lora.Month,gSysReg.lora.Day,gSysReg.lora.Hour,gSysReg.lora.Minute,gSysReg.lora.Sec,
  // gSysReg.lora.UPLink_CNT,gSysReg.lora.DOWNLink_CNT,0x0d,0x0a);

  sprintf(gSysReg.Uart.Make_Sensor_Packet_A,"at+send=lorap2p:%02x%02x%02x%02x%04x%02x%02x%02x%02x%02x%04x%04x%02x%02x",gSysReg.lora.PacketType,REG.LoraSet.Device_Channel,gSysReg.lora.Destination_ID,
  LORA_RCV_PACKET_DATE,gSysReg.lora.Year,gSysReg.lora.Month,gSysReg.lora.Day,gSysReg.lora.Hour,gSysReg.lora.Minute,gSysReg.lora.Sec,
  gSysReg.lora.UPLink_CNT,gSysReg.lora.DOWNLink_CNT,0x0d,0x0a);

}

void MakeLoRaP2PAgingPacket(void)
{
  current_Time_check();
  gSysReg.lora.PacketType = Pkt_Sensor2Mon;
  gSysReg.lora.Destination_ID = TARGET_ALL_MONITOR;   // already received device id from LoRa message

  sprintf(gSysReg.Uart.Make_Sensor_Packet_A,"at+send=lorap2p:%02x%02x%02x%02x%04x%02x%02x%02x%04x%04x%02x%02x",gSysReg.lora.PacketType,REG.LoraSet.Device_Channel,gSysReg.lora.Destination_ID,
  LORA_RCV_PACKET_DATE,gSysReg.Time.Day,gSysReg.Time.Hour,gSysReg.Time.Min,gSysReg.Time.Sec, 
  gSysReg.lora.UPLink_CNT,gSysReg.lora.DOWNLink_CNT,0x0d,0x0a);
}

void MakeLoRaP2PAgingPacket2(void)
{
  current_Time_check();
  gSysReg.lora.PacketType = Pkt_Sensor2Mon;
  gSysReg.lora.Destination_ID = TARGET_ALL_MONITOR;   // already received device id from LoRa message

  sprintf(gSysReg.Uart.Make_Sensor_Packet_A,"at+send=lorap2p:%02x%02x%02x%02x%04x%02x%02x%02x%02x%02x%02x%04x%02x%04x%02x%02x",gSysReg.lora.PacketType,REG.LoraSet.Device_Channel,gSysReg.lora.Destination_ID,
        LORA_RCV_PACKET_DATE,gSysReg.lora.Year,gSysReg.lora.Month,gSysReg.lora.Day,gSysReg.lora.Hour,gSysReg.lora.Minute,gSysReg.lora.Sec, 
        LORA_RCV_PACKET_CNT_UPLINK, gSysReg.lora.UPLink_CNT,LORA_RCV_PACKET_CNT_DOWNLINK,gSysReg.lora.DOWNLink_CNT,0x0d,0x0a);
}


// void MakeLoRaP2PAgingPacket(void)
// {
//   current_Time_check();
//   gSysReg.lora.PacketType = Pkt_Sensor2Mon;
//   gSysReg.lora.Destination_ID = TARGET_ALL_MONITOR;  // 0x00 : listen to all device 
//   // sprintf(gSysReg.Uart.Make_Sensor_Packet_A,"at+send=lorap2p:%2d%04x%02x%02x%02x%04x%04x%02x%02x",REG.LoraSet.Device_Channel,
//   sprintf(gSysReg.Uart.Make_Sensor_Packet_A,"at+send=lorap2p:%02x%02x%02x%02x%04x%02x%02x%02x%04x%04x%02x%02x",gSysReg.lora.PacketType,REG.LoraSet.Device_Channel,gSysReg.lora.Destination_ID,
//   LORA_RCV_PACKET_DATE,gSysReg.Time.Day,gSysReg.Time.Hour,gSysReg.Time.Min,gSysReg.Time.Sec, 
//   gSysReg.lora.UPLink_CNT,gSysReg.lora.DOWNLink_CNT,0x0d,0x0a);
// }

// void At_CmdFromLora(char cmd, U8 sendTo)
// {
//     int i;
//     const char *at_set_config="at+set_config";
//     const char *at_get_config="at+get_config";
//     const char *at_loraap_s_status="at+loraap=status";
//     const char *at_loraap_s_wakeup="at+loraap=status:wakeup";
//     const char *at_loraap_s_request="at+loraap=status:request";
//     const char *at_loraap_s_sleep="at+loraap=status:sleep";
    
//     char cCmd [128] = {0,};
    
//     LoraRxData[LoraDataCnt] = cmd;
//     if(cmd == ICAST_CARRYRETURN_ASCII_CODE) LoraDataEnd1= LoraDataCnt;
//     else if(cmd == ICAST_LINEFEED_ASCII_CODE) LoraDataEnd2 = LoraDataCnt;

//     if(LoraDataEnd1 == (LoraDataEnd2 - 1))
//     {
//         if((strncmp(at_set_config, LoraRxData, ICAST_LORA_Clen_set_config) == 0) || (strncmp(at_get_config, LoraRxData, ICAST_LORA_Clen_get_config) == 0))
//         {
//             sprintf(cCmd, "%s",LoraRxData);
//             Send_To_PC(cCmd);
//         }
//         else if(strncmp(at_loraap_s_status, LoraRxData, ICAST_LORA_Clen_status) == 0)
//         {
//             if(strncmp(at_loraap_s_wakeup, LoraRxData, ICAST_LORA_Clen_status_wakeup) == 0){
//               gSysReg.lora.LoraStatus = S_LORA_WAKEUP;
//             } 
//             else if(strncmp(at_loraap_s_request, LoraRxData, ICAST_LORA_Clen_status_request) == 0)
//             {
//                 gSysReg.lora.LoraStatus = S_LORA_REQUEST;
//             }
//             else if(strncmp(at_loraap_s_sleep, LoraRxData, ICAST_LORA_Clen_status_sleep) == 0) gSysReg.lora.LoraStatus = S_LORA_SLEEP;
//             else
//                 gSysReg.lora.LoraStatus = S_LORA_SLEEP;
//         }
           
//         LoraDataEnd1 = 0;
//         LoraDataEnd2 = 0;
//         LoraDataCnt = 0;

//         for(i=0;i<128;i+=8){
//         LoraRxData[i] = NULL;LoraRxData[i+1] = NULL;LoraRxData[i+2] = NULL;LoraRxData[i+3] = NULL;
//         LoraRxData[i+4] = NULL;LoraRxData[i+5] = NULL;LoraRxData[i+6] = NULL;LoraRxData[i+7] = NULL;
//         }
//     }
//     else
//     {
//         LoraDataCnt++;
//     }
// }

void LoRa_Mode_Set(U8 set_mode)     // lora status changing and wait time set for OK Set message
{
  gSysReg.lora.Lora_Packet_Send_Status = set_mode;
  switch(set_mode)
  {
    case eLoRa_Status_Sleep :
    case eLoRa_Disable :
    case eLoRa_Status_Wake_Up :
    case eLoRa_Status_Wake_Up_wait :
    case eLoRa_Status_IDLE :
      Set_LoraWait_Time(gSysReg.lora.Lora_Packet_data_Send_interval_Time);
      break;
    case eLoRa_Status_JOIN :
      Set_LoraWait_Time(gSysReg.lora.Lora_Packet_Max_Wait_Time);
      break;
    case eLoRa_Status_JOIN_wait :
      Set_LoraWait_Time(gSysReg.lora.Lora_Packet_Max_Wait_Time*2);
      break;

    case eLoRa_Status_Send_A :
    case eLoRa_Status_Send_B :
    case eLoRa_Status_Send_C :
    case eLoRa_Status_Send_D :
      Set_LoraWait_Time(REG.gICAST.Lora_UpLink_WaitTime);
      break;
    case eLoRa_Status_Send_A_wait :
    case eLoRa_Status_Send_B_wait :
    case eLoRa_Status_Send_C_wait :
    case eLoRa_Status_Send_D_wait :
      Set_LoraWait_Time(gSysReg.lora.Lora_Packet_Max_Wait_Time+3000);
      break;

    case eLoRa_Status_Send_SOIL_wait :
    case eLoRa_Status_Send_APPKEY_wait  :
    case eLoRa_Status_Send_APPEUI_wait :
      Set_LoraWait_Time(gSysReg.lora.Lora_Packet_Max_Wait_Time);
      break;
    case eLoRa_Status_Send_P2P :
      break;
    case eLoRa_Status_P2P_ModeSet :             break;
    case eLoRa_Status_P2P_ModeSet_wait :    Set_LoraWait_Time(gSysReg.lora.Lora_Packet_Max_Wait_Time+3000);     // OK wait time
      break;
    case eLoRa_Status_P2P_ConditionSet :
      break;
    case eLoRa_Status_P2P_Aging_Send :     Set_LoraWait_Time(gSysReg.lora.Lora_Packet_Max_Wait_Time+3000);     // aging loop time set
      break;
    case eLoRa_Status_P2P_Tx_Mode_Set_wait :
    case eLoRa_Status_P2P_Rx_Mode_Set_wait :
    case eLoRa_Status_P2P_Aging_Send_wait :
    case eLoRa_Status_P2P_ConditionSet_wait :
      Set_LoraWait_Time(gSysReg.lora.Lora_Packet_Max_Wait_Time);     // wait Rx mode 
      break;
    case eLoRa_Status_Aging_Sleep :
      gSysReg.Time.LoRa_Aging_Counter = 10;
      break;

//// **************** LORA Monitor mode set start *********************/////
    case eLoRa_Status_MON_Tx_MessageSending :
    case eLoRa_Status_MON_Tx_Mode_Set :
    case eLoRa_Status_MON_Rx_Mode_Set :
      Set_LoraWait_Time(gSysReg.lora.Lora_Packet_Max_Wait_Time);     // wait Rx mode 
      break;
//// **************** LORA Monitor mode set end *********************/////
    default :
      break;
  }
}

void LoRa_OKPacket_RCV_Status_Check(void)
{
  gSysReg.lora.Lora_Join_Retry_count =0;
  switch(gSysReg.lora.Lora_Packet_Send_Status)
  {
    case eLoRa_Disable :
      LoRa_Mode_Set(eLoRa_Status_JOIN);
      break;
    case eLoRa_Enable :
    case eLoRa_Status_Sleep :
      break;
    case eLoRa_Status_Sleep_wait :
      break;

    case eLoRa_Status_Wake_Up_wait :    // Received Version info
      break;
    case eLoRa_Status_JOIN_wait:
      gSysReg.lora.Lora_Packet_Send_Status = eLoRa_Status_Send_A;
      break;

    case eLoRa_Status_JOIN:
    case eLoRa_Status_IDLE:
      break;
    case eLoRa_Status_Send_A_wait :
      if(gSysReg.Uart.Packet_Ready==1){
        LoRa_Mode_Set(eLoRa_Status_Send_A);
        }
      else gSysReg.lora.Lora_Packet_Send_Status +=1;
      break;
    case eLoRa_Status_Send_B_wait:
      if(gSysReg.Uart.Packet_Ready==2)
        gSysReg.lora.Lora_Packet_Send_Status = eLoRa_Status_Send_D;
      else gSysReg.lora.Lora_Packet_Send_Status +=1;
      break;
    case eLoRa_Status_Send_C_wait:
      gSysReg.lora.Lora_Packet_Send_Status +=1;
      break;
    case eLoRa_Status_Send_D_wait   :
      LoRa_Mode_Set(eLoRa_Status_IDLE);
      break;

    case eLoRa_Status_Send_APPEUI_wait : 
      gSysReg.lora.Lora_Packet_Send_Status = eLoRa_Status_Send_APPKEY;
      break;
    case eLoRa_Status_Send_APPKEY_wait : 
      gSysReg.lora.Lora_Packet_Send_Status = eLoRa_Status_JOIN;
      break;
    
    case eLoRa_Status_Send_APPEUI :
    case eLoRa_Status_Send_APPKEY :
    case eLoRa_Status_Wake_Up :     // =10,
    case eLoRa_Status_Send_A :      // =20,
    case eLoRa_Status_Send_B:
    case eLoRa_Status_Send_C:
    case eLoRa_Status_Send_D:
      break;
    case eLoRa_Status_Send_P2P :
      break;

    case eLoRa_Status_P2P_ModeSet_wait :
      LoRa_Mode_Set(eLoRa_Status_P2P_ConditionSet);
      break;
    case eLoRa_Status_P2P_ConditionSet_wait :     // p2p mode -> p2p ocndition -> rx mode set -> aging mode check & timer check -> Tx mode set
      if(DEVICE_TYPE == MONITOR_DEVICE)     LoRa_Mode_Set(eLoRa_Status_MON);
      else if(DEVICE_TYPE == SENSOR_NODE)   LoRa_Mode_Set(eLoRa_Status_P2P_Tx_Mode_Set);
      break;
    case eLoRa_Status_P2P_Aging_Send_wait :
      gSysReg.lora.UPLink_CNT++;
      LoRa_Mode_Set(eLoRa_Status_P2P_Rx_Mode_Set);    // Rx mode set after tx sended
      break;
    case eLoRa_Status_P2P_Rx_Mode_Set_wait :        // Rx mode completed

      break;
    case eLoRa_Status_P2P_Tx_Mode_Set_wait :
      LoRa_Mode_Set(eLoRa_Status_P2P_Aging_Send);   // aging test packet send
      break;
    
    // case eLoRa_Status_MON   :    LoRa_Mode_Set(eLoRa_Status_MON_Rx_Mode_Set);   break;   // =70,
    case eLoRa_Status_MON_Wait          :     // 
    case eLoRa_Status_MON_Tx_Mode_Set   :     // 
    case eLoRa_Status_MON_Rx_Mode_Set   :     // 
    case eLoRa_Status_MON_Rx_Mode_wait  :     // 
      break;
    case eLoRa_Status_MON_Tx_Mode_wait  :     // 
      if(gSysReg.lora.LoRa_Return_flag == 1) {       // receive return message
        LoRa_Mode_Set(eLoRa_Status_MON_Tx_MessageSending);
        Send_To_PC("Tx mode change");
        }
      else {        // case : control command send

      }
      break;
    case eLoRa_Status_MON_Tx_MessageSending_wait :
      LoRa_Mode_Set(eLoRa_Status_MON_Rx_Mode_Set);
      break;
  }
}

uint8 charTOuint8(char charVal)
{
  uint8 rtn_val;
  switch(charVal)
  {
    case '0':   rtn_val =0;        break;
    case '1':   rtn_val =1;        break;
    case '2':   rtn_val =2;        break;
    case '3':   rtn_val =3;        break;
    case '4':   rtn_val =4;        break;
    case '5':   rtn_val =5;        break;
    case '6':   rtn_val =6;        break;
    case '7':   rtn_val =7;        break;
    case '8':   rtn_val =8;        break;
    case '9':   rtn_val =9;        break;
    case 'A':   rtn_val =10;        break;
    case 'a':   rtn_val =10;        break;
    case 'B':   rtn_val =11;        break;
    case 'b':   rtn_val =11;        break;
    case 'C':   rtn_val =12;        break;
    case 'c':   rtn_val =12;        break;
    case 'D':   rtn_val =13;        break;
    case 'd':   rtn_val =13;        break;
    case 'E':   rtn_val =14;        break;
    case 'e':   rtn_val =14;        break;
    case 'F':   rtn_val =15;        break;
    case 'f':   rtn_val =15;        break;
    default :
      break;
  }
  return rtn_val;
}

uint8 make_number(char charH, char charL)
{
  uint8 rtn_Val;
  rtn_Val = charTOuint8(charH);
  rtn_Val<<=4;
  rtn_Val +=charTOuint8(charL);
  return rtn_Val;
}

U32 Call_Lora_Freq_Val(U8 sel_index)
{ 
  U32 rtn_val=0;
  switch(sel_index)
  {
    case LORA_FREQ_9219  :   rtn_val = 921900000; break;
    case LORA_FREQ_9221  :   rtn_val = 922100000; break;
    case LORA_FREQ_9223  :   rtn_val = 922300000; break;
    case LORA_FREQ_9225  :   rtn_val = 922500000; break;
    case LORA_FREQ_9227  :   rtn_val = 922700000; break;
    case LORA_FREQ_9229  :   rtn_val = 922900000; break;
    case LORA_FREQ_9231  :   rtn_val = 923100000; break;
    case LORA_FREQ_9233  :   rtn_val = 923300000; break;
  }
    return rtn_val;
}
U32 Call_Lora_Spreadfact_Val(U8 sel_index)
{
  U32 rtn_val=0;
  switch(sel_index)
  {
    case LORA_SF_12 : rtn_val = 12; break;     // 0 LoRa: SF12 / 125kHz 250 bit/sec
    case LORA_SF_11 : rtn_val = 11; break;     // 1 LoRa: SF11 / 125kHz 440 bit/sec
    case LORA_SF_10 : rtn_val = 10; break;     // 2 LoRa: SF10 / 125kHz 980 bit/sec
    case LORA_SF_09 : rtn_val = 9; break;     // 3 LoRa: SF9 / 125kHz 1760 bit/sec
    case LORA_SF_08 : rtn_val = 8; break;     // 4 LoRa: SF8 / 125kHz 3125 bit/sec
    case LORA_SF_07 : rtn_val = 7; break;     // 5 LoRa: SF7 / 125kHz 5470 bit/sec
    default :
      break;
  }
  return rtn_val;
}

U32 Call_Lora_bandwidth_Val(U8 sel_index)   // defined by SF value
{
  U32 rtn_val=0;
  switch(sel_index)
  {
    case LORA_SF_12 : rtn_val = 0; break;     // 0 LoRa: SF12 / 125kHz(0:125kHz,1:250kHz,2:500kHz)
    case LORA_SF_11 : rtn_val = 0; break;     // 1 LoRa: SF11 / 125kHz(0:125kHz,1:250kHz,2:500kHz)
    case LORA_SF_10 : rtn_val = 0; break;     // 2 LoRa: SF10 / 125kHz(0:125kHz,1:250kHz,2:500kHz)
    case LORA_SF_09 : rtn_val = 0; break;     // 3 LoRa: SF9 / 125kHz (0:125kHz,1:250kHz,2:500kHz)
    case LORA_SF_08 : rtn_val = 0; break;     // 4 LoRa: SF8 / 125kHz (0:125kHz,1:250kHz,2:500kHz)
    case LORA_SF_07 : rtn_val = 0; break;     // 5 LoRa: SF7 / 125kHz (0:125kHz,1:250kHz,2:500kHz)
    default :
      break;
  }
  return rtn_val;
}

U32 Call_Lora_Codeingrate_Val(U8 sel_index)   // defined by SF value
{
  U32 rtn_val=0;
  switch(sel_index)
  {
    case LORA_SF_12 : rtn_val = 1; break;     // 0 LoRa: SF12 codeingrate 1:3/5, 2:4/6, 3:4/7, 4:4/8
    case LORA_SF_11 : rtn_val = 1; break;     // 1 LoRa: SF11 codeingrate 1:3/5, 2:4/6, 3:4/7, 4:4/8
    case LORA_SF_10 : rtn_val = 1; break;     // 2 LoRa: SF10 codeingrate 1:3/5, 2:4/6, 3:4/7, 4:4/8
    case LORA_SF_09 : rtn_val = 1; break;     // 3 LoRa: SF9  codeingrate 1:3/5, 2:4/6, 3:4/7, 4:4/8
    case LORA_SF_08 : rtn_val = 1; break;     // 4 LoRa: SF8  codeingrate 1:3/5, 2:4/6, 3:4/7, 4:4/8
    case LORA_SF_07 : rtn_val = 1; break;     // 5 LoRa: SF7  codeingrate 1:3/5, 2:4/6, 3:4/7, 4:4/8
    default :
      break;
  }
  return rtn_val;
}

U32 Call_Lora_preamlen_Val(U8 sel_index)   // defined by SF value
{
  U32 rtn_val=0;
  switch(sel_index)
  {
    case LORA_SF_12 : rtn_val = 8; break;     // 0 LoRa: SF12 preamlen default : 8
    case LORA_SF_11 : rtn_val = 8; break;     // 1 LoRa: SF11 preamlen default : 8
    case LORA_SF_10 : rtn_val = 8; break;     // 2 LoRa: SF10 preamlen default : 8
    case LORA_SF_09 : rtn_val = 8; break;     // 3 LoRa: SF9  preamlen default : 8
    case LORA_SF_08 : rtn_val = 8; break;     // 4 LoRa: SF8  preamlen default : 8
    case LORA_SF_07 : rtn_val = 8; break;     // 5 LoRa: SF7  preamlen default : 8
    default :
      break;
  }
  return rtn_val;
}
U32 Call_Lora_power_Val(U8 sel_index)   // defined by SF value
{
  U32 rtn_val=0;
  switch(sel_index)
  {
    case LORA_MaxEIRP_0dB : rtn_val = 0;      // MaxEIORP is considered to be +14dBm
    case LORA_MaxEIRP_2dB : rtn_val = 2;      // MaxEIORP is considered to be +14dBm
    case LORA_MaxEIRP_4dB : rtn_val = 4;      // MaxEIORP is considered to be +14dBm
    case LORA_MaxEIRP_6dB : rtn_val = 6;      // MaxEIORP is considered to be +14dBm
    case LORA_MaxEIRP_8dB : rtn_val = 8;      // MaxEIORP is considered to be +14dBm
    case LORA_MaxEIRP_10dB: rtn_val = 10;      // MaxEIORP is considered to be +14dBm
    case LORA_MaxEIRP_12dB: rtn_val = 12;      // MaxEIORP is considered to be +14dBm
    case LORA_MaxEIRP_14dB: rtn_val = 14;      // MaxEIORP is considered to be +14dBm
    default :
      break;
  }
  return rtn_val;
}

void Send_LORA_Packet(void)   // from main loop (check the lora status and sending message)
{
  switch(Get_Lora_Status())
  {
    case eLoRa_Disable :
      if(Get_LoraWait_Status()==0){
        gSysReg.lora.Lora_Join_Retry_count++;
        Set_LoraWait_Time(gSysReg.lora.Lora_Packet_data_Send_interval_Time);
        Send_Number_To_PC(gSysReg.lora.Lora_Join_Retry_count);
        Send_To_PC("LORA working check");
        Send_LoRa_Command(eLora_CMD_AT_join);
      }
      break;
    case eLoRa_Status_Sleep :                       // MCU start status
      if(gSysReg.lora.Lora_Enable ==ENABLE){
        LoRa_Mode_Set(eLoRa_Status_Wake_Up);
      } 
      break;
    case eLoRa_Status_Wake_Up :   
      if(Get_LoraWait_Status()==0){
        Send_LoRa_Command(eLora_CMD_AT_version);
        LoRa_Mode_Set(eLoRa_Status_Wake_Up_wait);
      }
      break;
    case eLoRa_Status_Wake_Up_wait :
      if(Get_LoraWait_Status()==0){
      LoRa_Mode_Set(eLoRa_Status_Send_APPEUI);
//      LoRa_Mode_Set(eLoRa_Status_JOIN);
      }
      break;
    case eLora_CMD_AT_run:
      Send_To_PC("LORA Run");
      LoRa_Mode_Set(eLora_CMD_AT_run);
      break;

    case eLoRa_Status_IDLE :      
      if(Get_LoraWait_Status()==0) gSysReg.lora.Lora_Packet_Send_Status = eLoRa_Status_Send_A;
      break;
    case eLoRa_Status_JOIN :    InitLora(S_LORA_WAKEUP); LoRa_Mode_Set(eLoRa_Status_JOIN_wait); break;
    case eLoRa_Status_JOIN_wait : 
      if(Get_LoraWait_Status()==0) {
        gSysReg.lora.Lora_Join_Retry_count++;
        if(gSysReg.lora.Lora_Join_Retry_count>10) 
        { 
          LoRa_Mode_Set(eLoRa_Disable);
          Send_To_PC("CAN NOT Contact to LORA!! at join wait");
        } 
        else LoRa_Mode_Set(eLoRa_Status_JOIN);
      }
      break;
    case eLoRa_Status_Send_A :  
      if(Get_LoraWait_Status()==0){
        MakeLoRaSendPacket();
        Send_To_Lora(gSysReg.Uart.Make_Sensor_Packet_A); 
        LoRa_Mode_Set(eLoRa_Status_Send_A_wait);  
      }
      break;
    case eLoRa_Status_Send_B :  Send_To_Lora(gSysReg.Uart.Make_Sensor_Packet_B); LoRa_Mode_Set(eLoRa_Status_Send_B_wait);   break;
    case eLoRa_Status_Send_C :  Send_To_Lora(gSysReg.Uart.Make_Sensor_Packet_C); LoRa_Mode_Set(eLoRa_Status_Send_C_wait);   break;
    case eLoRa_Status_Send_D :  Send_To_Lora(gSysReg.Uart.Make_Temperature_Packet); LoRa_Mode_Set(eLoRa_Status_Send_D_wait);   break;
    case eLoRa_Status_Send_A_wait : 
    case eLoRa_Status_Send_B_wait :   
    case eLoRa_Status_Send_C_wait :   
    case eLoRa_Status_Send_D_wait :  
      if(Get_LoraWait_Status()==0) {
        gSysReg.lora.Lora_Join_Retry_count++;
        if(gSysReg.lora.Lora_Join_Retry_count>10) 
        {
          LoRa_Mode_Set(eLoRa_Disable);
          Send_To_PC("CAN NOT Contact to LORA!!at data send") ;
        }
        else LoRa_Mode_Set(eLoRa_Status_Send_A_wait);
      }
      break;
    case eLoRa_Status_Send_SOIL_A :     Send_To_Lora(gSysReg.Uart.Make_Sensor_Packet_A); LoRa_Mode_Set(eLoRa_Status_Send_SOIL_wait);  break;
    case eLoRa_Status_Send_SOIL_wait :    break;

    case eLoRa_Status_Send_APPEUI       :  InitLora(S_LORA_APPEUI); LoRa_Mode_Set(eLoRa_Status_Send_APPEUI_wait);     break;
    case eLoRa_Status_Send_APPKEY       :  InitLora(S_LORA_APPKEY); LoRa_Mode_Set(eLoRa_Status_Send_APPKEY_wait);     break;
    case eLoRa_Status_Send_APPEUI_wait  :
    case eLoRa_Status_Send_APPKEY_wait  :
      if(Get_LoraWait_Status()==0) {
        gSysReg.lora.Lora_Join_Retry_count++;
        if(gSysReg.lora.Lora_Join_Retry_count>10) 
        {
          LoRa_Mode_Set(eLoRa_Disable);
          Send_To_PC("CAN NOT OK Received from LORA module") ;
        }
        else LoRa_Mode_Set(eLoRa_Status_Send_APPEUI);
      }
      break;
    case eLoRa_Status_Send_P2P :
      // if(gSysReg.lora.Lora_Sample_p2p_Tx_Timer==0){
      //   Send_LoRa_Command(eLora_CMD_AT_P2P_TxSample_Data);
      //   gSysReg.lora.Lora_Sample_p2p_Tx_Timer=5000;
      //   }
      if(gSysReg.Flag.LORA_P2P_Aging_Flag==1)
      LoRa_Mode_Set(eLoRa_Status_P2P_ModeSet);
      break;
    case eLoRa_Status_P2P_ModeSet :         InitLora(S_LORA_P2P_MODE);  LoRa_Mode_Set(eLoRa_Status_P2P_ModeSet_wait); break;
    case eLoRa_Status_P2P_ModeSet_wait   :    
      if(Get_LoraWait_Status()==0) {
         LoRa_Mode_Set(eLoRa_Status_P2P_ConditionSet);
        // InitLora(S_LORA_P2P_RESTART);
        // Send_To_PC("LORA P2P mode Set and didn't received return value so restart") ;
        // LoRa_Mode_Set(eLoRa_Status_Send_P2P); 
      }
      break;
    case eLoRa_Status_P2P_ConditionSet :
      InitLora(S_LORA_P2P_CONDITION); LoRa_Mode_Set(eLoRa_Status_P2P_ConditionSet_wait);
      break;

    case eLoRa_Status_P2P_ConditionSet_wait :
      if(Get_LoraWait_Status()==0) {
        InitLora(S_LORA_P2P_RESTART);
        Send_To_PC("LORA P2P condition Set and didn't received return value so restart") ;
        LoRa_Mode_Set(eLoRa_Status_Send_P2P); 
      }
      break;
    case eLoRa_Status_P2P_Aging_Send  :       
        // MakeLoRaP2PAgingPacket();
        MakeLoRaP2PAgingPacket2();
        Send_To_Lora(gSysReg.Uart.Make_Sensor_Packet_A); 
        LoRa_Mode_Set(eLoRa_Status_P2P_Aging_Send_wait);  
      break;
    case eLoRa_Status_P2P_Aging_Send_wait  :
      if(Get_LoraWait_Status()==0){
        InitLora(S_LORA_P2P_RESTART);
        Send_To_PC("LORA P2P Tx return fail, goto Restart") ;
        LoRa_Mode_Set(eLoRa_Status_Send_P2P);  
      }
      break;

    case eLoRa_Status_P2P_Rx_Mode_Set : 
      InitLora(S_LORA_RX_MODE); LoRa_Mode_Set(eLoRa_Status_P2P_Rx_Mode_Set_wait);
      gSysReg.Time.LoRa_Aging_Counter = 5;
      break;
    case eLoRa_Status_P2P_Tx_Mode_Set :     // function : Send_LORA_Packet
      InitLora(S_LORA_TX_MODE); LoRa_Mode_Set(eLoRa_Status_P2P_Tx_Mode_Set_wait);
      break;

    case eLoRa_Status_P2P_Rx_Mode_Set_wait :
      if(gSysReg.Time.LoRa_Aging_Counter ==0) {
        gSysReg.Time.LoRa_Aging_Counter = 5;
        LoRa_Mode_Set(eLoRa_Status_P2P_Tx_Mode_Set);
      }
      break;
    case eLoRa_Status_P2P_Tx_Mode_Set_wait :
      if(Get_LoraWait_Status()==0){
        InitLora(S_LORA_P2P_RESTART);
        Send_To_PC("LORA P2P Rx, Tx changing Fail go to restart") ;
        LoRa_Mode_Set(eLoRa_Status_Send_P2P);  
      }
      break;
    
    case eLoRa_Status_P2P_CMD_Send  :         break;
    case eLoRa_Status_P2P_CMD_Send_wait  :    break;
    case eLoRa_Status_Aging_Sleep :
      if(gSysReg.Time.LoRa_Aging_Counter ==0)
        LoRa_Mode_Set(eLoRa_Status_P2P_Tx_Mode_Set);  
      break;
    
////// ***************** LORA MONITOR Packet check routine start *********************////////////
    case eLoRa_Status_MON   :    LoRa_Mode_Set(eLoRa_Status_MON_Rx_Mode_Set);   break;   // =70,
    case eLoRa_Status_MON_Rx_Mode_Set : 
      InitLora(S_LORA_RX_MODE); LoRa_Mode_Set(eLoRa_Status_MON_Rx_Mode_wait);
      gSysReg.Time.LoRa_Aging_Counter = 100;
      break;
    case eLoRa_Status_MON_Tx_Mode_Set :
      InitLora(S_LORA_TX_MODE); LoRa_Mode_Set(eLoRa_Status_MON_Tx_Mode_wait);
      break;
    case eLoRa_Status_MON_Rx_Mode_wait :
      if(gSysReg.Time.LoRa_Aging_Counter ==0) {
        gSysReg.Time.LoRa_Aging_Counter = 100;
        Send_To_PC("LORA Monitor Rx message wait during 100 sec") ;
        }
      break;
    case eLoRa_Status_MON_Tx_Mode_wait :
      if(gSysReg.Time.LoRa_Aging_Counter ==0) {
        gSysReg.Time.LoRa_Aging_Counter = 100;
        Send_To_PC("LORA Monitor Tx message wait during 100 sec") ;
        }  
      break;
    case eLoRa_Status_MON_Tx_MessageSending :
      if(gSysReg.lora.LoRa_Return_flag == ENABLE){
        gSysReg.lora.LoRa_Return_flag =0;
        MakeLoRaMonitor_Return_Message();
        // Delay_ms(100);
        LoRa_Mode_Set(eLoRa_Status_MON_Tx_MessageSending_wait);
        Send_To_Lora(gSysReg.Uart.Make_Sensor_Packet_A); 
        Send_To_PC("LoRa message return") ;
      }
      gSysReg.lora.LoRa_Return_flag =0;
      break;
    case eLoRa_Status_MON_Tx_MessageSending_wait :
      if(Get_LoraWait_Status()==0) {
        InitLora(S_LORA_P2P_RESTART);
        Send_To_PC("LORA MON message sned but did't catched return value so restart") ;
        LoRa_Mode_Set(eLoRa_Status_Send_P2P); 
      }
      break;
////// ***************** LORA MONITOR Packet check routine end  *********************////////////
    default : break;
  }
  return;
}

void DownLink_MonitorPacket_Parsing(char *str_data)      // power node data parsing
{
  int i,j, lstrlen = strlen(str_data);
  for(i=0,j=0; i < lstrlen; i+=2,j++) {
    gSysReg.lora.rcv_data[j] = make_number(str_data[i],str_data[i+1]);
  }
  gSysReg.lora.rcv_data[j] = 0xff;  j++;
  gSysReg.lora.rcv_data[j] = 0x5A;  j++;
  gSysReg.lora.rcv_data[j] = 0xA5;

  if(DEVICE_TYPE == SENSOR_NODE)
  {
    gSysReg.lora.PacketType     = gSysReg.lora.rcv_data[0];
    gSysReg.lora.Device_ID      = gSysReg.lora.rcv_data[1];
    gSysReg.lora.Destination_ID = gSysReg.lora.rcv_data[2];
    switch(gSysReg.lora.PacketType)
    {
      case Pkt_Sensor2Mon :      
        return;
        break;
      case Pkt_RTN_From_Mon :    
        if(gSysReg.lora.Destination_ID == REG.LoraSet.Device_Channel) {
          Send_To_PC("RCV Return signal from monitor");
          LoRa_Mode_Set(eLoRa_Status_Aging_Sleep);  
        }
        return;
        break;
      case Pkt_Mon2Sensor :      
        if(gSysReg.lora.Destination_ID == REG.LoraSet.Device_Channel) {
          Send_To_PC("Monitor command received");
        }
        return;
        break;
      case Pkt_RTN_From_Sen :    
        return;
        break;
      default :     break;
    }
  }
  else if(DEVICE_TYPE == MONITOR_DEVICE)
  {
    gSysReg.lora.PacketType     = gSysReg.lora.rcv_data[0];
    gSysReg.lora.Device_ID      = gSysReg.lora.rcv_data[1];
    gSysReg.lora.Destination_ID = gSysReg.lora.rcv_data[2];
    switch(gSysReg.lora.PacketType)
    {
      case Pkt_Sensor2Mon :     
        Send_To_PC("RCV Sensor data "); 
        gSysReg.lora.LoRa_Return_flag = 1;
        break;
      case Pkt_RTN_From_Mon :    
        Send_To_PC("other Monitor Return received");
        break;
      case Pkt_Mon2Sensor :      
        Send_To_PC("other Monitor Command received");
        break;
      case Pkt_RTN_From_Sen :    break;
      default :     break;
    }
  }

  for(i=3;i<lstrlen/2;)
  {
    switch(gSysReg.lora.rcv_data[i])
    {
      case POWER_NODE_TTL_LEVEL_CHANGE :
        SSR_Status_Change(eSSR_CH0,gSysReg.lora.rcv_data[i+1]); gSysReg.lora.SSR_control[eSSR_CH0]=gSysReg.lora.rcv_data[i+1] ;
        SSR_Status_Change(eSSR_CH1,gSysReg.lora.rcv_data[i+2]); gSysReg.lora.SSR_control[eSSR_CH1]=gSysReg.lora.rcv_data[i+2] ;
        SSR_Status_Change(eSSR_CH2,gSysReg.lora.rcv_data[i+3]); gSysReg.lora.SSR_control[eSSR_CH2]=gSysReg.lora.rcv_data[i+3] ;
        SSR_Status_Change(eSSR_CH3,gSysReg.lora.rcv_data[i+4]); gSysReg.lora.SSR_control[eSSR_CH3]=gSysReg.lora.rcv_data[i+4] ;
        i+=5;
        break;
      case POWER_NODE_SSR_LEVEL_CHANGE :
        SSR_Status_Change(eSSR_CH0,gSysReg.lora.rcv_data[i+1]); gSysReg.lora.SSR_control[eSSR_CH0]=gSysReg.lora.rcv_data[i+1] ;
        SSR_Status_Change(eSSR_CH1,gSysReg.lora.rcv_data[i+2]); gSysReg.lora.SSR_control[eSSR_CH1]=gSysReg.lora.rcv_data[i+2] ;
        SSR_Status_Change(eSSR_CH2,gSysReg.lora.rcv_data[i+3]); gSysReg.lora.SSR_control[eSSR_CH2]=gSysReg.lora.rcv_data[i+3] ;
        SSR_Status_Change(eSSR_CH3,gSysReg.lora.rcv_data[i+4]); gSysReg.lora.SSR_control[eSSR_CH3]=gSysReg.lora.rcv_data[i+4] ;
        i+=5;
        break;
      case LORA_RCV_PACKET_DATE :   // cmd 1
      Send_To_PC("Time Scan");
        gSysReg.lora.Year  = gSysReg.lora.rcv_data[i+1];  gSysReg.lora.Year<<=8;
        gSysReg.lora.Year += gSysReg.lora.rcv_data[i+2]; 
        gSysReg.lora.Month = gSysReg.lora.rcv_data[i+3]; 
        gSysReg.lora.Day   = gSysReg.lora.rcv_data[i+4]; 
        gSysReg.lora.Hour  = gSysReg.lora.rcv_data[i+5]; 
        gSysReg.lora.Minute= gSysReg.lora.rcv_data[i+6]; 
        gSysReg.lora.Sec   = gSysReg.lora.rcv_data[i+7];  i+=8;
        break;
      case LORA_RCV_PACKET_ID_TEMPERATURE :       // cmd 2
        Send_To_PC("Temp Scan");
        i++;
        break;
      case LORA_RCV_PACKET_ID_HUMIDITY    :       // 3
      case LORA_RCV_PACKET_ID_DO          :       // 4
      case LORA_RCV_PACKET_ID_PH          :       // 5
      case LORA_RCV_PACKET_ID_CONDITIVITY :       // 6
      case LORA_RCV_PACKET_ID_TURBIDITY   :       // 7
        Send_To_PC("Sensor val");
        i++;
        break;
      case LORA_RCV_PACKET_CNT_UPLINK :   // =0xf0,
        Send_To_PC("Uplink cnt scan");
        i+=3;
        break;
      case LORA_RCV_PACKET_CNT_DOWNLINK : //  ,
        Send_To_PC("Downlink cnt scan");
        i+=3;
        break;
      default :
        Send_To_PC("WrongValue Scan");
        i++;
        break;
    }
  }
}

// void DownLink_Data_Parsing(char *str_data)      // power node data parsing
// {
//   int i,j, lstrlen = strlen(str_data);
//   for(i=0,j=0; i < lstrlen; i+=2,j++) {
//     gSysReg.lora.rcv_data[j] = make_number(str_data[i],str_data[i+1]);
//   }
//   gSysReg.lora.rcv_data[j] = 0xff;  j++;
//   gSysReg.lora.rcv_data[j] = 0x5A;  j++;
//   gSysReg.lora.rcv_data[j] = 0xA5;

//   for(i=0;i<lstrlen/2;)
//   {
//     switch(gSysReg.lora.rcv_data[i])
//     {
//       case POWER_NODE_TTL_LEVEL_CHANGE :
//         SSR_Status_Change(eSSR_CH0,gSysReg.lora.rcv_data[i+1]); gSysReg.lora.SSR_control[eSSR_CH0]=gSysReg.lora.rcv_data[i+1] ;
//         SSR_Status_Change(eSSR_CH1,gSysReg.lora.rcv_data[i+2]); gSysReg.lora.SSR_control[eSSR_CH1]=gSysReg.lora.rcv_data[i+2] ;
//         SSR_Status_Change(eSSR_CH2,gSysReg.lora.rcv_data[i+3]); gSysReg.lora.SSR_control[eSSR_CH2]=gSysReg.lora.rcv_data[i+3] ;
//         SSR_Status_Change(eSSR_CH3,gSysReg.lora.rcv_data[i+4]); gSysReg.lora.SSR_control[eSSR_CH3]=gSysReg.lora.rcv_data[i+4] ;
//         i+=5;
//         break;
//       case POWER_NODE_SSR_LEVEL_CHANGE :
//         SSR_Status_Change(eSSR_CH0,gSysReg.lora.rcv_data[i+1]); gSysReg.lora.SSR_control[eSSR_CH0]=gSysReg.lora.rcv_data[i+1] ;
//         SSR_Status_Change(eSSR_CH1,gSysReg.lora.rcv_data[i+2]); gSysReg.lora.SSR_control[eSSR_CH1]=gSysReg.lora.rcv_data[i+2] ;
//         SSR_Status_Change(eSSR_CH2,gSysReg.lora.rcv_data[i+3]); gSysReg.lora.SSR_control[eSSR_CH2]=gSysReg.lora.rcv_data[i+3] ;
//         SSR_Status_Change(eSSR_CH3,gSysReg.lora.rcv_data[i+4]); gSysReg.lora.SSR_control[eSSR_CH3]=gSysReg.lora.rcv_data[i+4] ;
//         i+=5;
//         break;
//       default :
//       Send_To_PC("WrongValue Scan");
//       i++;
//         break;
//     }
//   }
// }


void At_CmdFromLora_icast2(char cmd, U8 sendTo)
{
    int i;
    const char *init_ok="Initialization OK";
    const char *Join_ok="OK Join Success";
    const char *Current_work_mode="Current work_mode";
    const char *word_OK="OK";
    const char *word_ERROR="ERROR:";
    const char *Receive="at+recv";//  LoRa Received :: at+recv=1,-92,9,4:12345678<CR><LF>
    const char *at_set_config="at+set_config";
    const char *at_get_config="at+get_config";
    const char *at_loraap_s_status="at+loraap=status";
    const char *at_loraap_s_wakeup="at+loraap=status:wakeup";
    const char *at_loraap_s_request="at+loraap=status:request";
    const char *at_loraap_s_sleep="at+loraap=status:sleep";
    
    char cCmd [128] = {0,};
    char *pString;
    
    LoraRxData[LoraDataCnt] = cmd;
    if(cmd == ICAST_CARRYRETURN_ASCII_CODE) LoraDataEnd1= LoraDataCnt;
    else if(cmd == ICAST_LINEFEED_ASCII_CODE) LoraDataEnd2 = LoraDataCnt;

    if(LoraDataEnd1 == (LoraDataEnd2 - 1))
    {
        if(strncmp(Join_ok, LoraRxData, 15) == 0)
        {
          LoRa_Mode_Set(eLoRa_Status_Send_A);
          // Send_To_PC("OK Join Success");
          gSysReg.lora.Lora_Connection = ENABLE;
        }
        else if(strncmp(word_OK, LoraRxData, ICAST_LORA_Clen_OK) == 0)
        {
          LoRa_OKPacket_RCV_Status_Check();
          strncpy(cCmd,LoraRxData,strlen(LoraRxData));
          Send_String_To_PC(cCmd);
        }
        else if(strncmp(init_ok, LoraRxData,ICAST_LORA_Clen_Initialize_ok)==0)
        {
          Send_To_PC("icast LORA initizlie OK");
          gSysReg.lora.Lora_Enable=ENABLE;
        }
        // else if(strncmp(Current_work_mode, LoraRxData,17)==0)
        // {
        //   Send_To_PC("current work mode");
        // }
        else if(strncmp(Receive, LoraRxData, 7) == 0)    // Receive // at+recv=0,-94,8,0    // at+recv=1,-92,9,4:12345678<CR><LF>
        {
          int scan_val=0;
          gSysReg.lora.DOWNLink_CNT++;
          metal_strtok(&LoraRxData[7-1],"=");
          // pString = metal_strtok(NULL,",");
          // sscanf(pString,"%d",&gSysReg.lora.rcv_lora_port);
          pString = metal_strtok(NULL,",");
          sscanf(pString,"%d",&gSysReg.lora.rcv_RSSI);
          pString = metal_strtok(NULL,",");
          sscanf(pString,"%d",&gSysReg.lora.rcv_SNR);
          pString = metal_strtok(NULL,":");
          sscanf(pString,"%d",&gSysReg.lora.rcv_LengthOfData);
          pString = metal_strtok(NULL,"\r");
          sscanf(pString,"%s",&cCmd);
          
//          // pString = metal_strtok(NULL,":");
//          // sscanf(pString,"%d",&set_data);

//          // Send_String_To_PC("LoRa Receive Port: ");
//          // Send_Number_To_PC(gSysReg.lora.rcv_lora_port);
          Send_String_To_PC(" RSSI: ");
          Send_Number_To_PC(gSysReg.lora.rcv_RSSI);
          Send_String_To_PC(" SNR: ");
          Send_Number_To_PC(gSysReg.lora.rcv_SNR);
          Send_String_To_PC(" Length: ");
          Send_Number_To_PC(gSysReg.lora.rcv_LengthOfData);
          Send_String_To_PC(" rcv data: ");
          Send_To_PC(cCmd);
          // gSysReg.lora.Lora_DownLink_Count++;
          DownLink_MonitorPacket_Parsing(cCmd);

          if(DEVICE_TYPE == MONITOR_DEVICE){
            if(gSysReg.lora.LoRa_Return_flag == 1)
            LoRa_Mode_Set(eLoRa_Status_MON_Tx_Mode_Set);
            Send_To_PC("Tx mode set");
          }
          else if(DEVICE_TYPE == SENSOR_NODE){
            // LoRa_Mode_Set(eLoRa_Status_Aging_Sleep);
          }
        }
        
        else if((strncmp(at_set_config, LoraRxData, ICAST_LORA_Clen_set_config) == 0) || (strncmp(at_get_config, LoraRxData, ICAST_LORA_Clen_get_config) == 0))
        {
          sprintf(cCmd, "%s",LoraRxData);
          if(sendTo == ICAST_UART_CH_LORA)            Send_To_Lora(cCmd);
          else if(sendTo == ICAST_UART_CH_PC)         Send_To_PC(cCmd);
        }
        else if(strncmp(at_loraap_s_status, LoraRxData, ICAST_LORA_Clen_status) == 0)
        {
            if(strncmp(at_loraap_s_wakeup, LoraRxData, ICAST_LORA_Clen_status_wakeup) == 0){
              gSysReg.lora.LoraStatus = S_LORA_WAKEUP;
            } 
            else if(strncmp(at_loraap_s_request, LoraRxData, ICAST_LORA_Clen_status_request) == 0)
            {
                gSysReg.lora.LoraStatus = S_LORA_REQUEST;
            }
            else if(strncmp(at_loraap_s_sleep, LoraRxData, ICAST_LORA_Clen_status_sleep) == 0)
            {
              gSysReg.lora.LoraStatus = S_LORA_SLEEP;
            }
            else
                gSysReg.lora.LoraStatus = S_LORA_SLEEP;
        }
        else if(strncmp(word_ERROR, LoraRxData, 6) == 0)
        {
          int error_num=0;

          metal_strtok(&LoraRxData[6-1],":");
          pString = metal_strtok(NULL,":");
          sscanf(pString,"%d",&error_num);
          Send_String_To_PC("LoRa ERROR Receive : ");
          Send_Number_To_PC(error_num);
          Send_String_To_PC("\r\n");

          if(error_num ==1) LoRa_OKPacket_RCV_Status_Check();   // unsupported AT command.
        }
        else{
           Send_String_To_PC("LoRa Received :: ");
           strncpy(cCmd,LoraRxData,strlen(LoraRxData));
           Send_String_To_PC(cCmd);
        }
           
        LoraDataEnd1 = 0;
        LoraDataEnd2 = 0;
        LoraDataCnt = 0;

        for(i=0;i<128;i+=8){
        LoraRxData[i] = NULL;LoraRxData[i+1] = NULL;LoraRxData[i+2] = NULL;LoraRxData[i+3] = NULL;
        LoraRxData[i+4] = NULL;LoraRxData[i+5] = NULL;LoraRxData[i+6] = NULL;LoraRxData[i+7] = NULL;
        }
    }
    else
    {
        LoraDataCnt++;
    }
}



void Send_LoRa_Command(U8 CMD_type)
{
  char cCmd[64]={0,};
  switch(CMD_type)
  {
    case eLora_CMD_AT_version : sprintf(cCmd, "at+version");       break;     // 1
    case eLora_CMD_AT_help :    sprintf(cCmd, "at+help");   break;               // 3
    case eLora_CMD_AT_run :     sprintf(cCmd, "at+run");   break;                 // 4
    case eLora_CMD_AT_send :    break;                                        // 5
    case eLora_CMD_AT_join :    sprintf(cCmd, "at+join");   break;               // 6
    case eLora_CMD_AT_get_config :    break;                                  // 7
    case eLora_CMD_AT_set_config :    break;                                  // 8
   
    case eLora_CMD_AT_status :      sprintf(cCmd, "at+get_config=lora:status");   break;     // 9
    case eLora_CMD_AT_work_mode :   sprintf(cCmd, "at+set_config=lora:work_mode:1");   break;     // 10  // p2p mode set
    case eLora_CMD_AT_join_mode :   sprintf(cCmd, "at+set_config=lora:join_mode:1");   break;     // 11  // ABP mode set
    case eLora_CMD_AT_sleep :       sprintf(cCmd, "at+set_config=device:sleep:1");   break;     // 12  // ABP mode set
    case eLora_CMD_AT_P2P_Txmode :  sprintf(cCmd, "at+set_config=lorap2p:transfer_mode:1");   break;     // 13 1: Rx mode, 2 : Tx mode
    case eLora_CMD_AT_Restart :     sprintf(cCmd, "at+set_config=device:restart");   break;     // 14 restart
    case eLora_CMD_AT_P2P_TxSample_Data :sprintf(cCmd, "at+send=lorap2p:0102030405060708");   break;     // 15 
    
    default :
    break;
  }
  Send_To_Lora(cCmd);
}
