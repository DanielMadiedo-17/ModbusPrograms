#ifndef HARDWARE_SETUP_H
#define HARDWARE_SETUP_H


/*
 * modular plc - PIC24FJ256GB110
 * DE -> RF0
 * TX -> RPI32 (RF12)
 * RX -> RP31 (RF13)
 * UART1
*/

#define RS485_DEFAULT_BAUD 19200

#define RS485_DE_TX_LEVEL   1
#define RS485_DE_RX_LEVEL   0

#define RS485_DE_SETTLE_US  5

void Board_Init(void);

void RS485_Init(unsigned long baud);
void RS485_SetTxMode(void);
void RS485_SetRxMode(void);

void RS485_SendBytes(unsigned char *buf, unsigned int len);

unsigned char RS485_ByteAvailable(void);
unsigned char RS485_ReadByte(void);
void RS485_WriteByte(unsigned char b);

#endif //HARDWARE_SETUP_H