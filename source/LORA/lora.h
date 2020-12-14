
void Send_LORA_Packet(void);
void InitLora(U8 status);
void At_CmdFromLora_icast2(char cmd, U8 sendTo);
void Send_LoRa_Command(U8 CMD_type);

U32 Call_Lora_power_Val(U8 sel_index);   // defined by SF value
U32 Call_Lora_preamlen_Val(U8 sel_index);   // defined by SF value
U32 Call_Lora_Codeingrate_Val(U8 sel_index);   // defined by SF value
U32 Call_Lora_bandwidth_Val(U8 sel_index);   // defined by SF value
U32 Call_Lora_Spreadfact_Val(U8 sel_index);
U32 Call_Lora_Freq_Val(U8 sel_index);


// *** LORA RECEIVE PACKET INDEX NUM
#define MONITOR_PACKET_INDEX            0x00            // Receive packet byte position 
#define MONITOR_PACKET_DATE             0x01            // Receive packet byte position

enum {
        LORA_RCV_PACKET_INDEX=0,        
        LORA_RCV_PACKET_DATE,                   // 1
        LORA_RCV_PACKET_ID_TEMPERATURE,         // 2
        LORA_RCV_PACKET_ID_HUMIDITY,            // 3
        LORA_RCV_PACKET_ID_DO,                  // 4
        LORA_RCV_PACKET_ID_PH,                  // 5
        LORA_RCV_PACKET_ID_CONDITIVITY,         // 6
        LORA_RCV_PACKET_ID_TURBIDITY,           // 7

        LORA_RCV_PACKET_CNT_UPLINK =0xf0,
        LORA_RCV_PACKET_CNT_DOWNLINK,
        
};

enum {              // LORA_FREQ_VAL
    LORA_FREQ_9219=0,                 // 921900000,
    LORA_FREQ_9221,                   // 922100000
    LORA_FREQ_9223,                   // 922300000
    LORA_FREQ_9225,                   // 922500000
    LORA_FREQ_9227,                   // 922700000
    LORA_FREQ_9229,                   // 922900000
    LORA_FREQ_9231,                   // 923100000
    LORA_FREQ_9233,                   // 923300000
    LORA_FREQ_MAX_NUM,
// (921.9, 922.1, 922.3, 922.5, 922.7, 922.9, 923.1, 923.3
};

enum {              // Spreadfact(12 ~ 7), 125Khz, 
    LORA_SF_12=0,         // 0 LoRa: SF12 / 125kHz 250 bit/sec
    LORA_SF_11,           // 1 LoRa: SF11 / 125kHz 440 bit/sec
    LORA_SF_10,           // 2 LoRa: SF10 / 125kHz 980 bit/sec
    LORA_SF_09,           // 3 LoRa: SF9 / 125kHz 1760 bit/sec
    LORA_SF_08,           // 4 LoRa: SF8 / 125kHz 3125 bit/sec
    LORA_SF_07,           // 5 LoRa: SF7 / 125kHz 5470 bit/sec
    LORA_SF_MAX_NUM,
};

enum {          // MaxEIRP
    LORA_MaxEIRP_0dB=0,
    LORA_MaxEIRP_2dB,
    LORA_MaxEIRP_4dB,
    LORA_MaxEIRP_6dB,
    LORA_MaxEIRP_8dB,
    LORA_MaxEIRP_10dB,
    LORA_MaxEIRP_12dB,
    LORA_MaxEIRP_14dB,
    LORA_MaxEIRP_MAX_NUM,
};

enum{           // DataRate,   Maximum Transmission Load by Region
    LORA_DataRate_0_M59_N51=0,
    LORA_DataRate_1_M59_N51,
    LORA_DataRate_2_M59_N51,
    LORA_DataRate_3_M123_N115,
    LORA_DataRate_4_M250_N242,
    LORA_DataRate_5_M250_N242,
    LORA_DataRate_MAX_NUM,
};



