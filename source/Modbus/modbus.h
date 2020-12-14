
//modbus.h

#define MODBUS_READ       0x03
#define MODBUS_INPUT      0x04
#define MODBUS_WRITE      0x06
#define MODBUS_WRITE_MUL  0x10
#define MODBUS_CALIB      0x40

void HandleUART(void);          // UART2    Sensor
void SendUART(void);
void RecvUart1(void);
void HandleUART1(void);         // UART1    PC(wifi)
void HandleUART3(void);         // UART3    LoRa

void Send_To_Lora(const char *packet); 
void Send_To_PC(const char *packet);
void Send_String_To_PC(const char *packet);
void Send_Number_To_PC(int number);
void Send_CSV_Number_To_PC(int number);
void Send_CSV_U16Num_To_PC(U16 number);
void Send_FNumber_To_PC(float number, int point_val);
void Send_CSV_String_To_PC(const char *packet);
void Send_Char_To_PC(char *char_data);
void Send_String_To_PC_WithOutCR(const char *packet);

///////////////// HRCHO ADD START ///////////////////////////
char *metal_strtok(char *_Str, const char *_Delim);
void Send_CSV_FloatNum_To_PC(U32 number);
void Send_CSV_U32Num_To_PC(U32 number);
void Send_CSV_U32HexNum_To_PC(U32 number);

void Send_CSV_Flo32_To_PC(Flo32 number);

void LoRa_Mode_Set(U8 set_mode);
void RecvUart3_icast(void);

Flo32 floatizeMe2 (U32 myNumba );

void Flash_data_update(U8 update_id, U8 *p);

#define ICAST_CREG_PAGE_ADDR_CONFIG     0x1000
#define ICAST_CREG_PAGE_ADDR_SETUP      0x2000
#define ICAST_CREG_PAGE_ADDR_RS485      0x3000
#define ICAST_CREG_PAGE_ADDR_LORA       0x4000
#define ICAST_CREG_PAGE_ADDR_SENSORSET  0x5000
#define ICAST_CREG_PAGE_ADDR_TEMPHUMI   0x6000
#define ICAST_CREG_PAGE_ADDR_ADIN       0x7000
#define ICAST_CREG_PAGE_ADDR_SENSOR     0x8000
#define ICAST_CREG_PAGE_ADDR_EMERG      0x9000
#define ICAST_CREG_PAGE_ADDR_VER        0xa000

#define ICAST_CREG_SENSORSET_ADDR_INTERVAL  0x0001      // 0*2 +1
#define ICAST_CREG_SENSORSET_ADDR_MAP       0x0007      // 
#define ICAST_CREG_SENSORSET_ADDR_EUI       0x0047      // 35*2 +1
#define ICAST_CREG_SENSORSET_ADDR_KEY       0x0057      // 35*2 + 8*2 + 1

#define ICAST_CREG_SENSOR_INTERVAL_WORD_LENGTH  1
#define ICAST_CREG_SENSOR_MAP_WORD_LENGTH  16
#define ICAST_CREG_SENSOR_EUI_WORD_LENGTH  8+1    // add CR LF
#define ICAST_CREG_SENSOR_KEY_WORD_LENGTH  16 +1  // add CR LF

// #define ICAST_SENSOR_MAP_TOTAL_CNT      ICAST_CREG_SENSOR_MAP_WORD_LENGTH

#define ICAST_CARRYRETURN_ASCII_CODE  0x0D
#define ICAST_LINEFEED_ASCII_CODE     0x0A

#define ASCII_NUM_ZERO                  0x30

#define Sensor_SCAN_CMD_READ  3
#define Sensor_SCAN_CMD_WRITE 6
#define Sensor_SCAN_CMD_MULTY_WRITE 0x10

#define ICAST_BH485_SCAN_START_ADDRESS 0
#define ICAST_BH485_SCAN_LENGTH 33
// #define ICAST_AQUABO_SCAN_START_ADDRESS 83           // start addr : 0x53
// #define ICAST_AQUABO_SCAN_LENGTH 18                  // 0x53 to 0x63 : 17
#define ICAST_AQUABO_SCAN_START_ADDRESS 82           // start addr : 0x52
#define ICAST_AQUABO_SCAN_LENGTH 19                  // 0x53 to 0x63 : 17
#define ICAST_AQUABO_INIT_START_ADDRESS 163  // 163 = 0xa3, init need for read a4 address
#define ICAST_AQUABO_INIT_LENGTH 2
#define ICAST_AQUABO_SCANENABLE_ADDRESS 0x01  // 163 = 0xa3, init need for read a4 address
#define ICAST_AQUABO_SCANENABLE_LENGTH 1
#define ICAST_HANBAEK_CHLOROPHYLL_ADDRESS 5  // address number : 5 to 25 
#define ICAST_HANBAEK_CHLOROPHYLL_ADDRESS2 13  // address number : 5 to 25 
#define ICAST_HANBAEK_CHLOROPHYLL_ADDRESS3 13  // address number : 5 to 25 
#define ICAST_HANBAEK_CHLOROPHYLL_LENGTH 6
#define ICAST_HANBAEK_TURBIDITY_ADDRESS 0x05  // address number : 5 to 13 
#define ICAST_HANBAEK_TURBIDITY_LENGTH 2
#define ICAST_SOIL_JXCT_HUMINITY_LENGTH 2
#define ICAST_AM2302_DHT22_HUMINITY_LENGTH 2
#define ICAST_URSALINK_NPK_LENGTH          3

#define ICAST_SOIL_JXCT_HUMINITY_ADDRESS 0x02  // address number : 5 to 13 
#define ICAST_SOIL_NPK_URSALINK_ADDRESS  0x1E   // 1e : Nitrogen, 1f : phosphorus, 20 : potassium

typedef enum
{
    eICAST_SEN_SLAVE_ADDR_BH485_PH =2,      // 2
    eICAST_SEN_SLAVE_ADDR_BH485_DO,         // 3
    eICAST_SEN_SLAVE_ADDR_BH485_SALINITY,   // 4
    eICAST_SEN_SLAVE_ADDR_AQUABO_DO=11,     // 11
    eICAST_SEN_SLAVE_ADDR_SOIL_JXCT_HUMINITY=12,   // 12
    eICAST_SEN_SLAVE_ADDR_NPK_URSALINK,     //       13

    eICAST_SEN_SLAVE_ADDR_HABACK_TURBIDITY=30,     // 30  
    eICAST_SEN_SLAVE_ADDR_HABACK_CHLOROPHYLL=40,   // 40

}ICAST_SENSOR_RS485_ID_NUM;


typedef enum
{
    ICAST_UART_CH_PC = 1,
    ICAST_UART_CH_SENSOR,
    ICAST_UART_CH_LORA,
}ICAST_UART_CH_NAME;

typedef enum
{
    ICAST_LORA_Clen_OK = 2,
    ICAST_LORA_Clen_Initialize_ok=17,

    ICAST_LORA_Clen_version =10,
    ICAST_LORA_Clen_help =7,
    ICAST_LORA_Clen_run =6,
    ICAST_LORA_Clen_send =7,
    ICAST_LORA_Clen_join =7,
    ICAST_LORA_Clen_set_config = 13,
    ICAST_LORA_Clen_get_config = 13,
    ICAST_LORA_Clen_it_set=13,

    ICAST_LORA_Clen_status = 16,
    ICAST_LORA_Clen_status_wakeup = 23,
    ICAST_LORA_Clen_status_request = 24,
    ICAST_LORA_Clen_status_sleep = 22,
    ICAST_LORA_Clen_lora_app_key = 27,
    ICAST_LORA_Clen_lora_app_eui = 27,
    ICAST_LORA_Clen_lora_set_send_interval = 33,
    ICAST_LORA_Clen_lora_get_send_interval = 33,

    ICAST_LORA_Clen_set_sensor_map = 17,
    ICAST_LORA_Clen_get_sensor_map = 17,
    ICAST_LORA_Clen_set_sensor_offset = 21,
    ICAST_LORA_Clen_get_sensor_offset = 21,
    ICAST_LORA_Clen_set_sensor_sensitivity = 26,
    ICAST_LORA_Clen_get_sensor_sensitivity = 26,

    ICAST_LORA_Clen_get_sensor_test_cmd = 25,
    ICAST_LORA_Clen_set_sensor_test_cmd = 24,
    ICAST_DBG_Clen_set_csv = 16,
    ICAST_DBG_Clen_set_sensor = 19,
    ICAST_DBG_Clen_set_lora=17,
    ICAST_DBG_Clen_set_motor=13,
}ICAST_LORA_CMD_LENGTH;

// LORA data header Index
// water condition
#define ID_BH485_DO            0x0d00
#define ID_BH485_PH            0x0d01
#define ID_BH485_SALINITY      0x0d02
#define ID_HABAEK_CHLOROPHYLL  0x0d03
#define ID_HABAEK_TURBIDITY    0x0d04
#define ID_AQUABO_DO           0x0d05

// soil condition
#define ID_JXCT_SOIL_HUMIDITY  0x0d13
#define ID_URSALINK_NPK_N      0x0d10
#define ID_URSALINK_NPK_P      0x0d11
#define ID_URSALINK_NPK_K      0x0d12

#define ID_DHT22_AIR_HUMIDITY  0x0d14

#define ID_RUNNING_TIME        0x0d7f

// downlink
#define ID_SSR_CTL              0xA0
#define ID_TTL_CTL              0xA1


typedef enum
{
    eLora_CMD_None=0,
    eLora_CMD_AT_version,
    eLora_CMD_AT_version_wait,
    eLora_CMD_AT_help,
    eLora_CMD_AT_run,
    eLora_CMD_AT_send,         // uart:<index>:<data>, lora:<port>:<data>, lorap2p:<data>
    eLora_CMD_AT_join,
    eLora_CMD_AT_get_config,   // device:status, 
    eLora_CMD_AT_set_config,   // device:restart, device:boot, device:sleep:<status>, device:uartL:<index>:<baud_Rate>
                              // device:uart_mode:<index>:<mode>, 
    eLora_CMD_AT_status,
    eLora_CMD_AT_work_mode,         // 0 : LORAWAN, 1 : LORAP2P set
    eLora_CMD_AT_join_mode,         // 0 : OTAA, 1 : ABP mode set
    eLora_CMD_AT_sleep,             // sleep mode set
    eLora_CMD_AT_P2P_Txmode,
    eLora_CMD_AT_Restart,


    eLora_CMD_AT_P2P_TxSample_Data,
    
    // Lora_CMD_AT_,
    // Lora_CMD_AT_,
    // Lora_CMD_AT_,
    // Lora_CMD_AT_,
    // Lora_CMD_AT_,
    // Lora_CMD_AT_,
    // Lora_CMD_AT_,
    
    // Lora_CMD_AT_sleep,
    // Lora_CMD_AT_reset,
    // Lora_CMD_AT_reload,
    // Lora_CMD_AT_mode,
    // Lora_CMD_AT_recv_ex,
    // Lora_CMD_AT_set_config,
    // Lora_CMD_AT_get_config,
    // Lora_CMD_AT_band,
    // Lora_CMD_AT_join,
    // Lora_CMD_AT_signal,
    // Lora_CMD_AT_dr,
    // Lora_CMD_AT_link_cnt,
    // Lora_CMD_AT_abp_info,
    // Lora_CMD_AT_send,
    // Lora_CMD_AT_recv,
    // Lora_CMD_AT_rf_config,
    // Lora_CMD_AT_txc,
    // Lora_CMD_AT_rxc,
    // Lora_CMD_AT_tx_stop,
    // Lora_CMD_AT_rx_stop,
    // Lora_CMD_AT_status,
    // Lora_CMD_AT_uart,
    // Lora_CMD_AT_rd_reg,
    // Lora_CMD_AT_wr_reg,
    // Lora_CMD_AT_gpio,
    // Lora_CMD_AT_rd_adc,
    // Lora_CMD_AT_rd_iic,
    // Lora_CMD_AT_wr_iic,
}LORA_RAK811_Command_List;


///////////////// HRCHO ADD END ///////////////////////////


