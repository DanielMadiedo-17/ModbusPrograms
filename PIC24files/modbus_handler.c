#include "modbus_handler.h"
#include "hardware_setup.h"

//modbus state

static unsigned char mb_slave_id = 2;
static unsigned long mb_baud = 19200;

//Receiver assembly
static unsigned char rx_buf[MB_MAX_FRAME];
static unsigned int  rx_len = 0;

//Response buffer
static unsigned char tx_buf[MB_MAX_FRAME];

//a simple gap detector
static unsigned int gap_ticks = 0;
static unsigned int gap_threshold = 0;
static unsigned char receiving;

//Holding registers
volatile unsigned int mb_hregs[MB_HREG_COUNT];

//Coils
volatile char mb_coils[MB_COIL_COUNT];


//------------CRC16 Modbus--------
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
            else      crc >>= 1;
           }
         }
 return crc;
}

//for handling bits
static unsigned int be16(unsigned char hi, unsigned char lo)
{
 return (((unsigned int)hi) <<8) | (unsigned int)lo;
}

static void put_be16(unsigned char *p, unsigned int v)
{
 p[0] = (unsigned char)(v>>8);
 p[1] = (unsigned char)(v & 0xFF);
}

//------------Gap timing------------
//Modbus RTU gap = 3.5 char times.
//Using a coarse tick of 1 ms and computing a safe threshold/
//Char time ~ 11 bits/ baud  seconds; 3.5 char ~ 38.5 bits
// threshold_ms =~ (39/baud)*1000, plus margin
//
//example @19200 baud: (39/19200)*1000 = 2.03 -> we use 4 ms
//example @9600 baud: 4.06 ms -> we use 7-8 ms
//

static unsigned int compute_gap_threshold_ms(unsigned long baud)
{
 unsigned long ms = (39UL * 1000UL) / baud; //integer floor
 ms += 2; //margin
 if (ms < 4) ms = 4;
 if (ms > 20) ms = 20;
 return (unsigned int)ms;
}

//MB_task() needs to be called often enough so that one iteration ~= 1ms
//for now, we are using a simple Delay_ms(1) inside the task when receiving
static void gap_tick_1ms(void)
{
 if(receiving)
   {
    gap_ticks++;
    }
}

//-----------Register helpers-------
unsigned int MB_HregRead(unsigned int addr)
{
 if (addr >= MB_HREG_COUNT) return 0;
 return mb_hregs[addr];
}

void MB_HregWrite(unsigned int addr, unsigned int value)
{
 if(addr >= MB_HREG_COUNT) return;
 mb_hregs[addr] = value;
}

//-----------Coil Helpers-------
static unsigned char coil_get(unsigned int idx)
{
    if (idx >= MB_COIL_COUNT) return 0;
    return (mb_coils[idx] != 0) ? 1 : 0;
}

static void coil_set(unsigned int idx, unsigned char v)
{
    if (idx >= MB_COIL_COUNT) return;
    mb_coils[idx] = (v != 0) ? 1 : 0;
}

//-----------Exception response builder-------
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

//---------function handlers-----
//READING HOLDING REGISTERS
static unsigned int handle_fc03(unsigned char *req, unsigned int req_len)
{
 unsigned int start; //what holding register to start reading
 unsigned int qty;   //How many registers to read
 unsigned int i;
 unsigned int idx;
 
 start = (req[2] << 8) | req[3];
 qty = (req[4] <<8) | req[5];
 
 //request length needs to be 8 bytes
 //addr func startHi startLo qtyHi qtyLo crcLo crcHI
 if (req_len != 8) return build_exception(req[0], req[1], MB_EX_ILLEGAL_VALUE);
 if ((start + qty) > MB_HREG_COUNT) return build_exception(req[0], req[1], MB_EX_ILLEGAL_ADDRESS);
 
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

//WRITE A SINGLE REGISTER
static unsigned int handle_fc06(unsigned char *req, unsigned int req_len)
{
 unsigned int reg;
 unsigned int val;
 unsigned int i;
 
//request length needs to be 8 bits
 //addr func startHi startLo qtyHi qtyLo crcLo crcHI
 if (req_len != 8) return build_exception(req[0], req[1], MB_EX_ILLEGAL_VALUE);

 reg = be16(req[2], req[3]); //be16 converts the  two bytes of the array into start register
 val = be16(req[4], req[5]); //be16 coverts  the two bytes of array into whats being written
 
 if (reg >= MB_HREG_COUNT)return build_exception(req[0], req[1], MB_EX_ILLEGAL_ADDRESS);
 
 mb_hregs[reg] = val;
 
 for(i = 0; i<6; i++) // copying the request into the tx buffer
 {
  tx_buf[i] = req[i];
 }
 //appending the crc
 {
  unsigned int crc = mb_crc16(tx_buf, 6);
  tx_buf[6] = (unsigned char)(crc & 0xFF); // CRC LO
  tx_buf[7] = (unsigned char)(crc >> 8);   // CRC HI
 }
 return 8;
 
}

 //WRITING MULTIPLE REGISTERS (FC10 missing)
 
 //Reading multiple coils
 static unsigned int handle_fc01(unsigned char *req, unsigned int req_len)
 {
   unsigned int start; //which coil to start reading at
   unsigned int qty;   //how many coils to read
   unsigned int i;
   unsigned int idx;
   unsigned int byteCount;
   unsigned int coilIndex;
   unsigned char outByte;
   unsigned char bitPos;

   
   //request length needs to be 8 bytes
   //addr func startHi startLo qtyHi qtyLo crcLo crcHI
   if (req_len != 8) return build_exception(req[0], req[1], MB_EX_ILLEGAL_VALUE);
   
   start = (req[2] << 8) | req[3];
   qty = (req[4] <<8) | req[5];
   
   if (qty < 1 || qty > 2000) return build_exception(req[0], req[1], MB_EX_ILLEGAL_VALUE);
   if ((start + qty) > MB_COIL_COUNT) return build_exception(req[0], req[1], MB_EX_ILLEGAL_ADDRESS);
   
   byteCount = (qty + 7) / 8;
   
   tx_buf[0] = req[0];
   tx_buf[1] = 0x01;
   tx_buf[2] = (unsigned char)byteCount;
   
   idx = 3;
   coilIndex = start;
   
   //packing coils LSB first
   for(i = 0; i < byteCount; i++)
      {
       outByte = 0;
       for(bitPos = 0; bitPos<8; bitPos++)
       {
          if((coilIndex-start) >= qty) break; //no more coils requested
          
          if(mb_coils[coilIndex] != 0)
             outByte |= (1u <<bitPos);
             
          coilIndex++;
       }
       tx_buf[idx++] = outByte;
      }
      
      //crc
      {
       unsigned int crc = mb_crc16(tx_buf, idx);
       tx_buf[idx++] = (unsigned char)(crc & 0xFF);
       tx_buf[idx++] = (unsigned char)(crc >>8);
      }
      return idx;
 }

//-----------Frame Processing------------
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
  //case 0x10: resp_len = handle_fc10(frame,len); break;
  default:
          resp_len = build_exception(addr, func, MB_EX_ILLEGAL_FUNCTION);
          break;
 }

 //Broadcast: no reply per modbus spec
 if(!is_broadcast && resp_len > 0)
 {
  RS485_SendBytes(tx_buf, resp_len);
 }
}

//----------Public API------
void MB_Init(unsigned char slave_id, unsigned long baud)
{
 unsigned int i;
 
 mb_slave_id = slave_id;
 mb_baud = baud;
 
 //initalize holding registers
 for (i = 0; i < MB_HREG_COUNT; i++)
     mb_hregs[i] = 0;
     
//initalize coils
for (i = 0; i < MB_COIL_COUNT; i++) mb_coils[i] = 0;

 //configure threshold
 gap_threshold = compute_gap_threshold_ms(baud);
 
 //ensuring RS485 is initalized at the same baud
 RS485_Init(baud);
 
 rx_len = 0;
 receiving = 0;
 gap_ticks = 0;
}

void MB_Task(void)
{
 //Pull all available bytes from UART into rx_buf
 while (RS485_ByteAvailable())
 {
  unsigned char b = RS485_ReadByte();
  
  if (rx_len < MB_MAX_FRAME)
     rx_buf[rx_len++] = b;
  else
      rx_len = 0; //overflow -> reset

  //start receiving/ reset gap timer
  receiving = 1;
  gap_ticks = 0;
 }
 
  //if we have started receivng, tick the gap timer
  if(receiving)
  {
   //This creates the 1ms delay in the MB_Task, if loop
   //cant tolerate we will move to an external isr
   Delay_ms(1);
   gap_tick_1ms();
   
   //if silence gap is reached, frame is complete
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