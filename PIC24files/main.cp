#line 1 "C:/Users/danie/Desktop/PIC_Programming/Project_1/main.c"
#line 1 "c:/users/danie/desktop/pic_programming/project_1/hardware_setup.h"
#line 20 "c:/users/danie/desktop/pic_programming/project_1/hardware_setup.h"
void Board_Init(void);

void RS485_Init(unsigned long baud);
void RS485_SetTxMode(void);
void RS485_SetRxMode(void);

void RS485_SendBytes(unsigned char *buf, unsigned int len);

unsigned char RS485_ByteAvailable(void);
unsigned char RS485_ReadByte(void);
void RS485_WriteByte(unsigned char b);
#line 1 "c:/users/danie/desktop/pic_programming/project_1/modbus_handler.h"
#line 17 "c:/users/danie/desktop/pic_programming/project_1/modbus_handler.h"
void MB_Init(unsigned char slave_id, unsigned long baud);


void MB_Task(void);


unsigned int MB_HregRead(unsigned int addr);
void MB_HregWrite(unsigned int addr, unsigned int value);


extern volatile unsigned int mb_hregs[ 64 ];
#line 7 "C:/Users/danie/Desktop/PIC_Programming/Project_1/main.c"
void main() {
 Board_Init();
 MB_Init(2, 19200);

 while (1)
 {

 MB_Task();
 mb_hregs[0] = 0xAAAA;
 mb_hregs[1] = 0x1234;

 }
}
