#ifndef MODBUS_HANDER_H
#define MODBUS_HANDLER_H

//mikroC native types

//--------Configuration--------
#define MB_MAX_FRAME       256
#define MB_HREG_COUNT       64  //Can be altered to add/reduce holding regs
#define MB_COIL_COUNT       32  // can be altered to add/reduce

//modbus exception codes
#define MB_EX_ILLEGAL_FUNCTION 0x01
#define MB_EX_ILLEGAL_ADDRESS  0x02
#define MB_EX_ILLEGAL_VALUE    0x03

//-----Public API----
void MB_Init(unsigned char slave_id, unsigned long baud);

//call this frequently in main loop (non-blocking)
void MB_Task(void);

//Register map access (an optional helper)
unsigned int MB_HregRead(unsigned int addr);
void         MB_HregWrite(unsigned int addr, unsigned int value);

//expose register array ( for direct access)
extern volatile unsigned int mb_hregs[MB_HREG_COUNT];

#endif