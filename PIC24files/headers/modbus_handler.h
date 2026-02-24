#ifndef MODBUS_HANDLER_H
#define MODBUS_HANDLER_H

#define MB_MAX_FRAME   256
#define MB_HREG_COUNT   64 //number of registers, can be altered


//exception codes
#define MB_EX_ILLEGAL_FUNCTION  0x01
#define MB_EX_ILLEGAL_ADDRESS   0x02
#define MB_EX_ILLEGAL_VALUE     0x03

//Public API
void MB_Init(unsigned char slave_id, unsigned long baud);

//needs to be called frequently in main loop.
void MB_Task(void);

//register map access
unsigned int MB_HRegRead(unsigned int addr);
void         MB_HRegWrite(unsigned int addr, unsigned int value);

extern volatile unsigned int mb_hregs[MB_HREG_COUNT];

#endif