#line 1 "C:/Users/danie/Desktop/PIC_Programming/Project_1/hardware_setup.c"
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
#line 1 "c:/users/public/documents/mikroelektronika/mikroc pro for dspic/include/string.h"





void * memchr(void *p, char n, unsigned int v);
int memcmp(void *s1, void *s2, int n);
void * memcpy(void * d1, void * s1, int n);
void * memmove(void * to, void * from, int n);
void * memset(void * p1, char character, int n);
char * strcat(char * to, char * from);
char * strchr(char * ptr, char chr);
int strcmp(char * s1, char * s2);
char * strcpy(char * to, char * from);
int strlen(char * s);
char * strncat(char * to, char * from, int size);
char * strncpy(char * to, char * from, int size);
int strspn(char * str1, char * str2);
char strcspn(char * s1, char * s2);
int strncmp(char * s1, char * s2, char len);
char * strpbrk(char * s1, char * s2);
char * strrchr(char *ptr, char chr);
char * strstr(char * s1, char * s2);
char * strtok(char * s1, char * s2);
#line 6 "C:/Users/danie/Desktop/PIC_Programming/Project_1/hardware_setup.c"
sbit RS485_DE_LAT at LATF0_bit;
sbit RS485_DE_TRIS at TRISF0_bit;


static void rs485_pps_init_uart1(void)
{


 OSCCON = 0x46;
 OSCCON = 0x57;
 OSCCONbits.IOLOCK = 0;

 RPINR18bits.U1RXR = 32;
 RPOR15bits.RP31R = 3;

 OSCCON = 0x46;
 OSCCON = 0x57;
 OSCCONbits.IOLOCK = 1;

}


void RS485_SetTxMode(void)
{
 RS485_DE_LAT =  1 ;
 if ( 5 ) Delay_us( 5 );
}

void RS485_SetRxMode(void)
{
 if ( 5 ) Delay_us( 5 );
 RS485_DE_LAT =  0 ;
}


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

 RS485_DE_TRIS = 0;
 RS485_SetRxMode();


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
 while (U1STAbits.TRMT == 0) { }

 RS485_SetRxMode();
}

void Board_Init(void)
{
 RS485_Init( 19200 );

}
