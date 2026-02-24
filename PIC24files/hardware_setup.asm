
hardware_setup_rs485_pps_init_uart1:

;hardware_setup.c,10 :: 		static void rs485_pps_init_uart1(void)
;hardware_setup.c,14 :: 		OSCCON = 0x46;
	MOV	#70, W0
	MOV	WREG, OSCCON
;hardware_setup.c,15 :: 		OSCCON = 0x57;
	MOV	#87, W0
	MOV	WREG, OSCCON
;hardware_setup.c,16 :: 		OSCCONbits.IOLOCK = 0;
	BCLR.B	OSCCONbits, #6
;hardware_setup.c,18 :: 		RPINR18bits.U1RXR = 32;  //RX
	MOV.B	#32, W0
	MOV.B	W0, W1
	MOV	#lo_addr(RPINR18bits), W0
	XOR.B	W1, [W0], W1
	MOV.B	#63, W0
	AND.B	W1, W0, W1
	MOV	#lo_addr(RPINR18bits), W0
	XOR.B	W1, [W0], W1
	MOV	#lo_addr(RPINR18bits), W0
	MOV.B	W1, [W0]
;hardware_setup.c,19 :: 		RPOR15bits.RP31R = 3;   //TX
	MOV	#768, W0
	MOV	W0, W1
	MOV	#lo_addr(RPOR15bits), W0
	XOR	W1, [W0], W1
	MOV	#16128, W0
	AND	W1, W0, W1
	MOV	#lo_addr(RPOR15bits), W0
	XOR	W1, [W0], W1
	MOV	W1, RPOR15bits
;hardware_setup.c,21 :: 		OSCCON = 0x46;
	MOV	#70, W0
	MOV	WREG, OSCCON
;hardware_setup.c,22 :: 		OSCCON = 0x57;
	MOV	#87, W0
	MOV	WREG, OSCCON
;hardware_setup.c,23 :: 		OSCCONbits.IOLOCK = 1;
	BSET.B	OSCCONbits, #6
;hardware_setup.c,25 :: 		}
L_end_rs485_pps_init_uart1:
	RETURN
; end of hardware_setup_rs485_pps_init_uart1

_RS485_SetTxMode:

;hardware_setup.c,28 :: 		void RS485_SetTxMode(void)  //transmitting
;hardware_setup.c,30 :: 		RS485_DE_LAT = RS485_DE_TX_LEVEL;
	BSET	LATF0_bit, BitPos(LATF0_bit+0)
;hardware_setup.c,31 :: 		if (RS485_DE_SETTLE_US) Delay_us(RS485_DE_SETTLE_US);
	MOV	#20, W7
L_RS485_SetTxMode1:
	DEC	W7
	BRA NZ	L_RS485_SetTxMode1
;hardware_setup.c,32 :: 		}
L_end_RS485_SetTxMode:
	RETURN
; end of _RS485_SetTxMode

_RS485_SetRxMode:

;hardware_setup.c,34 :: 		void RS485_SetRxMode(void)  //receiving
;hardware_setup.c,36 :: 		if (RS485_DE_SETTLE_US) Delay_us(RS485_DE_SETTLE_US);
	MOV	#20, W7
L_RS485_SetRxMode4:
	DEC	W7
	BRA NZ	L_RS485_SetRxMode4
;hardware_setup.c,37 :: 		RS485_DE_LAT = RS485_DE_RX_LEVEL;
	BCLR	LATF0_bit, BitPos(LATF0_bit+0)
;hardware_setup.c,38 :: 		}
L_end_RS485_SetRxMode:
	RETURN
; end of _RS485_SetRxMode

_RS485_ByteAvailable:

;hardware_setup.c,41 :: 		unsigned char RS485_ByteAvailable(void)
;hardware_setup.c,43 :: 		return(U1STAbits.URXDA != 0);
	CLR.B	W0
	BTSC	U1STAbits, #0
	INC.B	W0
	CP.B	W0, #0
	CLR.B	W0
	BRA Z	L__RS485_ByteAvailable19
	INC.B	W0
L__RS485_ByteAvailable19:
;hardware_setup.c,44 :: 		}
L_end_RS485_ByteAvailable:
	RETURN
; end of _RS485_ByteAvailable

_RS485_ReadByte:

;hardware_setup.c,46 :: 		unsigned char RS485_ReadByte(void)
;hardware_setup.c,48 :: 		while(!RS485_ByteAvailable()) {}
L_RS485_ReadByte6:
	CALL	_RS485_ByteAvailable
	CP0.B	W0
	BRA Z	L__RS485_ReadByte21
	GOTO	L_RS485_ReadByte7
L__RS485_ReadByte21:
	GOTO	L_RS485_ReadByte6
L_RS485_ReadByte7:
;hardware_setup.c,49 :: 		return(unsigned char)UART1_Read();
	CALL	_UART1_Read
;hardware_setup.c,50 :: 		}
L_end_RS485_ReadByte:
	RETURN
; end of _RS485_ReadByte

_RS485_Init:

;hardware_setup.c,52 :: 		void RS485_Init(unsigned long baud)
;hardware_setup.c,55 :: 		RS485_DE_TRIS = 0;  //output
	BCLR	TRISF0_bit, BitPos(TRISF0_bit+0)
;hardware_setup.c,56 :: 		RS485_SetRxMode();  //default: listen
	CALL	_RS485_SetRxMode
;hardware_setup.c,59 :: 		rs485_pps_init_uart1();
	CALL	hardware_setup_rs485_pps_init_uart1
;hardware_setup.c,61 :: 		UART1_Init(baud);
	CALL	_UART1_Init
;hardware_setup.c,62 :: 		Delay_ms(50);
	MOV	#4, W8
	MOV	#3392, W7
L_RS485_Init8:
	DEC	W7
	BRA NZ	L_RS485_Init8
	DEC	W8
	BRA NZ	L_RS485_Init8
;hardware_setup.c,63 :: 		}
L_end_RS485_Init:
	RETURN
; end of _RS485_Init

_RS485_SendBytes:

;hardware_setup.c,65 :: 		void RS485_SendBytes(unsigned char *buf, unsigned int len)
;hardware_setup.c,69 :: 		RS485_SetTxMode();
	CALL	_RS485_SetTxMode
;hardware_setup.c,71 :: 		for(i = 0; i < len; i++)
; i start address is: 2 (W1)
	CLR	W1
; i end address is: 2 (W1)
L_RS485_SendBytes10:
; i start address is: 2 (W1)
	CP	W1, W11
	BRA LTU	L__RS485_SendBytes24
	GOTO	L_RS485_SendBytes11
L__RS485_SendBytes24:
;hardware_setup.c,73 :: 		UART1_Write(buf[i]);
	ADD	W10, W1, W0
	PUSH	W10
	ZE	[W0], W10
	CALL	_UART1_Write
	POP	W10
;hardware_setup.c,71 :: 		for(i = 0; i < len; i++)
	INC	W1
;hardware_setup.c,74 :: 		}
; i end address is: 2 (W1)
	GOTO	L_RS485_SendBytes10
L_RS485_SendBytes11:
;hardware_setup.c,75 :: 		while (U1STAbits.TRMT == 0) {/*waiting until register empty*/}
L_RS485_SendBytes13:
	BTSC	U1STAbits, #8
	GOTO	L_RS485_SendBytes14
	GOTO	L_RS485_SendBytes13
L_RS485_SendBytes14:
;hardware_setup.c,77 :: 		RS485_SetRxMode();
	CALL	_RS485_SetRxMode
;hardware_setup.c,78 :: 		}
L_end_RS485_SendBytes:
	RETURN
; end of _RS485_SendBytes

_Board_Init:

;hardware_setup.c,80 :: 		void Board_Init(void)
;hardware_setup.c,82 :: 		RS485_Init(RS485_DEFAULT_BAUD);
	PUSH	W10
	PUSH	W11
	MOV	#19200, W10
	MOV	#0, W11
	CALL	_RS485_Init
;hardware_setup.c,84 :: 		}
L_end_Board_Init:
	POP	W11
	POP	W10
	RETURN
; end of _Board_Init
