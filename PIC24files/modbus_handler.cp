#line 1 "C:/Users/danie/Desktop/PIC_Programming/Project_1/modbus_handler.c"
#line 1 "c:/users/danie/desktop/pic_programming/project_1/modbus_handler.h"
#line 17 "c:/users/danie/desktop/pic_programming/project_1/modbus_handler.h"
void MB_Init(unsigned char slave_id, unsigned long baud);


void MB_Task(void);


unsigned int MB_HregRead(unsigned int addr);
void MB_HregWrite(unsigned int addr, unsigned int value);


extern volatile unsigned int mb_hregs[ 64 ];
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
#line 6 "C:/Users/danie/Desktop/PIC_Programming/Project_1/modbus_handler.c"
static unsigned char mb_slave_id = 2;
static unsigned long mb_baud = 19200;


static unsigned char rx_buf[ 256 ];
static unsigned int rx_len = 0;


static unsigned char tx_buf[ 256 ];


static unsigned int gap_ticks = 0;
static unsigned int gap_threshold = 0;
static unsigned char receiving;


volatile unsigned int mb_hregs[ 64 ];


volatile bit mb_coils[ 32 ];



static unsigned int mb_crc16(unsigned char *buf, unsigned int len)
{
 unsigned int crc = 0xFFFF;
 unsigned int pos;
 unsigned char i;

 for(pos = 0; pos < len; pos++)
 {
 crc^= (unsigned int)buf[pos];
 for(i=0; i <8 ; i++)
 {
 if (crc & 0x0001) crc = (crc >> 1) ^ 0xA001;
 else crc >>= 1;
 }
 }
 return crc;
}


static unsigned int be16(unsigned char hi, unsigned char lo)
{
 return (((unsigned int)hi) <<8) | (unsigned int)lo;
}

static void put_be16(unsigned char *p, unsigned int v)
{
 p[0] = (unsigned char)(v>>8);
 p[1] = (unsigned char)(v & 0xFF);
}
#line 69 "C:/Users/danie/Desktop/PIC_Programming/Project_1/modbus_handler.c"
static unsigned int compute_gap_threshold_ms(unsigned long baud)
{
 unsigned long ms = (39UL * 1000UL) / baud;
 ms += 2;
 if (ms < 4) ms = 4;
 if (ms > 20) ms = 20;
 return (unsigned int)ms;
}



static void gap_tick_1ms(void)
{
 if(receiving)
 {
 gap_ticks++;
 }
}


unsigned int MB_HregRead(unsigned int addr)
{
 if (addr >=  64 ) return 0;
 return mb_hregs[addr];
}

void MB_HregWrite(unsigned int addr, unsigned int value)
{
 if(addr >=  64 ) return;
 mb_hregs[addr] = value;
}


static unsigned int build_exception(unsigned char addr, unsigned char func,
 unsigned char excode)
{
 tx_buf[0] = addr;
 tx_buf[1] = func | 0x80;
 tx_buf[2] = excode;

 {
 unsigned int crc = mb_crc16(tx_buf, 3);
 tx_buf[3] = (unsigned char)(crc & 0xFF);
 tx_buf[4] = (unsigned char)(crc >> 8);
 }
 return 5;
}



static unsigned int handle_fc03(unsigned char *req, unsigned int req_len)
{
 unsigned int start;
 unsigned int qty;
 unsigned int i;
 unsigned int idx;

 start = (req[2] << 8) | req[3];
 qty = (req[4] <<8) | req[5];



 if (req_len != 8) return build_exception(req[0], req[1],  0x03 );
 if ((start + qty) >  64 ) return build_exception(req[0], req[1],  0x02 );

 tx_buf[0] = req[0];
 tx_buf[1] = 0x03;
 tx_buf[2] = (unsigned char)(qty *2);

 idx = 3;
 for (i=0; i<qty; i++)
 {
 unsigned int v = mb_hregs[start + i];
 tx_buf[idx++] = (unsigned char)(v>>8);
 tx_buf[idx++] = (unsigned char)(v & 0xFF);
 }

 {
 unsigned int crc = mb_crc16(tx_buf, idx);
 tx_buf[idx++] = (unsigned char)(crc & 0xFF);
 tx_buf[idx++] = (unsigned char)(crc >>8);
 }
 return idx;
}


static unsigned int handle_fc06(unsigned char *req, unsigned int req_len)
{
 unsigned int reg;
 unsigned int val;
 unsigned int i;



 if (req_len != 8) return build_exception(req[0], req[1],  0x03 );

 reg = be16(req[2], req[3]);
 val = be16(req[4], req[5]);

 if (reg >=  64 )return build_exception(req[0], req[1],  0x02 );

 mb_hregs[reg] = val;

 for(i = 0; i<6; i++)
 {
 tx_buf[i] = req[i];
 }

 {
 unsigned int crc = mb_crc16(tx_buf, 6);
 tx_buf[6] = (unsigned char)(crc & 0xFF);
 tx_buf[7] = (unsigned char)(crc >> 8);
 }
 return 8;

}




 static unsigned int handle_fc01(unsigned char *req, unsigned int req_len)
 {
 unsigned int start;
 unsigned int qty;
 unsigned int i;
 unsigned int idx;

 start = (req[2] << 8) | req[3];
 qty = (req[4] <<8) | req[5];



 if (req_len != 8) return build_exception(req[0], req[1],  0x03 );
 if ((start + qty) >  32 ) return build_exception(req[0], req[1],  0x02 );

 tx_buf[0] = req[0];
 tx_buf[1] = 0x01;
 tx_buf[2] = qty*2;

 idx = 3;

 for(i = 0; i < qty; i++)
 {
 unsigned int v = mb_coils[start + i];
 tx_buf[idx++] = (unsigned char)(v>>8);
 tx_buf[idx++] = (unsigned char)(v & 0xFF);
 }
 {
 unsigned int crc = mb_crc16(tx_buf, idx);
 tx_buf[idx++] = (unsigned char)(crc & 0xFF);
 tx_buf[idx++] = (unsigned char)(crc >>8);
 }
 return idx;
 }


static void process_frame(unsigned char *frame, unsigned int len)
{
 unsigned char addr;
 unsigned char func;
 unsigned int crc_rx;
 unsigned int crc_calc;
 unsigned int resp_len = 0;
 bit is_broadcast;

 if (len < 4) return;

 addr = frame[0];
 func = frame[1];

 is_broadcast = (addr == 0) ? 1 : 0;
 if (!is_broadcast && addr != mb_slave_id) return;

 crc_rx = (unsigned int)frame[len -2] | ((unsigned int)frame[len-1] << 8);
 crc_calc = mb_crc16(frame, len - 2);
 if (crc_rx != crc_calc) return;

 switch(func)
 {
 case 0x03: resp_len = handle_fc03(frame, len); break;
 case 0x06: resp_len = handle_fc06(frame,len); break;
 case 0x01: resp_len = handle_fc01(frame, len); break;

 default:
 resp_len = build_exception(addr, func,  0x01 );
 break;
 }


 if(!is_broadcast && resp_len > 0)
 {
 RS485_SendBytes(tx_buf, resp_len);
 }
}


void MB_Init(unsigned char slave_id, unsigned long baud)
{
 unsigned int i;

 mb_slave_id = slave_id;
 mb_baud = baud;


 for (i = 0; i <  64 ; i++)
 mb_hregs[i] = 0;


 gap_threshold = compute_gap_threshold_ms(baud);


 RS485_Init(baud);

 rx_len = 0;
 receiving = 0;
 gap_ticks = 0;
}

void MB_Task(void)
{

 while (RS485_ByteAvailable())
 {
 unsigned char b = RS485_ReadByte();

 if (rx_len <  256 )
 rx_buf[rx_len++] = b;
 else
 rx_len = 0;


 receiving = 1;
 gap_ticks = 0;
 }


 if(receiving)
 {


 Delay_ms(1);
 gap_tick_1ms();


 if(gap_ticks >= gap_threshold)
 {
 unsigned int len = rx_len;

 receiving = 0;
 rx_len = 0;
 gap_ticks = 0;

 if (len >= 4)
 process_frame(rx_buf, len);
 }

 }


}
