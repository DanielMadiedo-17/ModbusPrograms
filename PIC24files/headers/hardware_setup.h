#ifndef HARDWARE_SETUP_H
#define HARDWARE_SETUP_H

#include <stdint.h>
#include <stdbool.h>

/*
 * modular plc - PIC24FJ256GB110
 * DE -> RF0
 * TX -> RP32 (RF12)
 * RX -> RP31 (RF13)
 * UART1
*/

#define RS485_DEFAULT_BAUD (19200UL)

#define RS485_DE_TX_LEVEL   1
#define RS485_DE_RX_LEVEL   0

#define RS485_DE_SETTLE_US  5

void Board_Init(void);

void RS485_Init(uint32_t baud);
void RS485_SetTxMode(void);
void RS485_SetRxMode(void);

void RS485_SendBytes(const uint8_t *data, uint16_t len);

bool RS485_ByteAvailable(void);
uint8_t RS485_ReadByte(void);
void RS485_WriteByte(uint8_t b);

#endif //HARDWARE_SETUP_H
