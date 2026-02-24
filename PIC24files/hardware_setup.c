#include "hardware_setup.h"
#include <string.h>

//--------DE (RF0)------
//mikroC bit aliases for PIC24
sbit RS485_DE_LAT at LATF0_bit;
sbit RS485_DE_TRIS at TRISF0_bit;

//----MAPPING UART1 to RP pins-------
static void rs485_pps_init_uart1(void)
{

   //Configuring the pins using the method from Brazil
   OSCCON = 0x46;
   OSCCON = 0x57;
   OSCCONbits.IOLOCK = 0;

   RPINR18bits.U1RXR = 32;  //RX
   RPOR15bits.RP31R = 3;   //TX

   OSCCON = 0x46;
   OSCCON = 0x57;
   OSCCONbits.IOLOCK = 1;

}

//--------------Direction Control------
void RS485_SetTxMode(void)  //transmitting
{
 RS485_DE_LAT = RS485_DE_TX_LEVEL;
 if (RS485_DE_SETTLE_US) Delay_us(RS485_DE_SETTLE_US);
}

void RS485_SetRxMode(void)  //receiving
{
 if (RS485_DE_SETTLE_US) Delay_us(RS485_DE_SETTLE_US);
 RS485_DE_LAT = RS485_DE_RX_LEVEL;
}

//---------UART Helpers---------
unsigned char RS485_ByteAvailable(void)
{
 return(U1STAbits.URXDA != 0);
}

unsigned char RS485_ReadByte(void)
{
 while(!RS485_ByteAvailable()) {}
 return(unsigned char)UART1_Read();
}

void RS485_Init(unsigned long baud)
{
 //configuring DE pin
 RS485_DE_TRIS = 0;  //output
 RS485_SetRxMode();  //default: listen
 
 //pps mapping must be done before UART init
 rs485_pps_init_uart1();
 
 UART1_Init(baud);
 Delay_ms(50);
}

void RS485_SendBytes(unsigned char *buf, unsigned int len)
{
 unsigned int i;
 
 RS485_SetTxMode();
 
 for(i = 0; i < len; i++)
 {
       UART1_Write(buf[i]);
 }
 while (U1STAbits.TRMT == 0) {/*waiting until register empty*/}
 
 RS485_SetRxMode();
}

void Board_Init(void)
{
 RS485_Init(RS485_DEFAULT_BAUD);

}