#include "hardware_setup.h"
#include "modbus_handler.h"

//unsigned char send_test[] = {0xAA,0xBB,0xFF,0x55,0xAA,0xFF};
//unsigned char test;

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