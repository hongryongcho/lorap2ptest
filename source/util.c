
/******************************************************************************/
//util.c 

#include "stm32f10x.h"
#include "config.h"
#include "util.h"
#include <stdio.h>
#include <stdarg.h>

extern SysReg gSysReg;

void Dly100us(int arg)
{
  Int32U Dly = (Int32U)arg;
  while(Dly--)
  {
    for(volatile int i = LOOP_DLY_100US; i; i--);
  }
}
void delay_int_count(volatile unsigned int nTime)
{
  for(;nTime>0;nTime--); 
  
}
void Dly1mSec(int time)
{
      Tm1msDly = time;// *1000;
      while(Tm1msDly);
}

void SPI1_Init(void)
{
  SPI_InitTypeDef   SPI_InitStructure;
  
  /* SPI Config -------------------------------------------------------------*/
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  //SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;    //clk=27ns 
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;  //clk=56ns
  //SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;  //clk=111ns 
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_Init(SPI1, &SPI_InitStructure);
  
  /* Enable SPI */
  SPI_Cmd(SPI1, ENABLE);
}

uint8_t SPI1_SendByte(uint8_t byte)
{
  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);        
  SPI_I2S_SendData(SPI1, byte);        
  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);         
  return SPI_I2S_ReceiveData(SPI1);
}

void USART1_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    // Configure the USART1_Tx as Alternate function Push-Pull
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin   =  USART1_TX;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
  
    // Configure the USART1_Rx as input floating
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Pin   = USART1_RX;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
     
    USART_InitTypeDef USART_InitStructure;
 
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No ;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    /* Configure the USARTx */
    USART_Init(USART1, &USART_InitStructure);
    /* Enable the USARTx */
    USART_Cmd(USART1, ENABLE);
    
    //interrupt config
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
   
}
void USART1_RM_Init(void)
{
    #if 0
    GPIO_InitTypeDef GPIO_InitStructure;
    
    // Configure the USART1_Tx as Alternate function Push-Pull
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin   =  USART1_TX;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
  
    // Configure the USART1_Rx as input floating
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Pin   = USART1_RX;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
     
    USART_InitTypeDef USART_InitStructure;
 
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No ;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    /* Configure the USARTx */
    USART_Init(USART1, &USART_InitStructure);
    /* Enable the USARTx */
    USART_Cmd(USART1, ENABLE);
    
    //interrupt config
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    #endif
   
}
int BaudRateTbl[9] = {BAUD_1200, BAUD_2400, BAUD_4800, BAUD_9600, BAUD_14400, BAUD_19200, BAUD_28800, BAUD_57600, BAUD_115200};
int DataBitTbl[2] = {USART_WordLength_8b,USART_WordLength_9b}; //8bit,9bit
int StopBitTbl[2]= {USART_StopBits_1,USART_StopBits_2};  //1stop, 2stop
int ParityBitTbl[3] = {USART_Parity_No,USART_Parity_Odd,USART_Parity_Even}; //no,odd,even

void USART3_Init(void)
{
    int baud,data,stop,parity;
    GPIO_InitTypeDef GPIO_InitStructure;
    
    // Configure the USART2_Tx as Alternate function Push-Pull
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin   =  USART3_TX;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
  
    // Configure the USART2_Rx as input floating
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Pin   = USART3_RX;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    USART_InitTypeDef USART_InitStructure;
    if(gSysReg.Uart.RS485_3BaudRate<1 || gSysReg.Uart.RS485_3BaudRate>9)gSysReg.Uart.RS485_3BaudRate=4;//9600
    baud = BaudRateTbl[gSysReg.Uart.RS485_3BaudRate-1];
    if(gSysReg.Uart.RS485_3ParityBit>2)gSysReg.Uart.RS485_3ParityBit=0;
    parity = ParityBitTbl[gSysReg.Uart.RS485_3ParityBit];
    if(gSysReg.Uart.RS485_3StopBit<1 || gSysReg.Uart.RS485_3StopBit>2)gSysReg.Uart.RS485_3StopBit=1;
    stop = StopBitTbl[gSysReg.Uart.RS485_3StopBit-1];
    if(gSysReg.Uart.RS485_3DataBit<8 || gSysReg.Uart.RS485_3DataBit>9)gSysReg.Uart.RS485_3DataBit=8;
    if(gSysReg.Uart.RS485_3ParityBit !=0)gSysReg.Uart.RS485_3DataBit=9; //V2.68
    data = DataBitTbl[gSysReg.Uart.RS485_3DataBit-8];

    USART_InitStructure.USART_BaudRate = 115200;//9600;//baud;    // lora 115200 bps
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No ;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = (USART_Mode_Rx | USART_Mode_Tx);
    /* Configure the USARTx */
    USART_Init(USART3, &USART_InitStructure);
    /* Enable the USARTx */
    USART_Cmd(USART3, ENABLE);
    
    //interrupt config
    USART_ITConfig(USART3, (USART_IT_RXNE), ENABLE); //| USART_IT_TXE
    //printf("Uart3(%d,%d,%d,%d)\n",baud,data,stop,parity);
}

void Delay_us( u8 time_us )
{
  register u8 i;
  register u8 j;
  for( i=0;i<time_us;i++ )
  {
    for( j=0;j<5;j++ )          // 25CLK
    {
      asm("nop");       //1CLK
      asm("nop");       //1CLK
      asm("nop");       //1CLK
      asm("nop");       //1CLK
      asm("nop");       //1CLK
    }
  }                              // 25CLK*0.04us=1us
}

void Delay_ms( u16 time_ms )
{
  register u16 i;
  for( i=0;i<time_ms;i++ )
  {
    Delay_us(250);
    Delay_us(250);
    Delay_us(250);
    Delay_us(250);
  }
}

void Uart5RX_SR(void)
{


}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line)
{
    /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

    /* Infinite loop */
    while (1)
    {
    }
}
#endif

