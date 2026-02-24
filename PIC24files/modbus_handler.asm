
modbus_handler_mb_crc16:

;modbus_handler.c,26 :: 		static unsigned int mb_crc16(unsigned char *buf, unsigned int len)
;modbus_handler.c,28 :: 		unsigned int crc = 0xFFFF;
; crc start address is: 2 (W1)
	MOV	#65535, W1
;modbus_handler.c,32 :: 		for(pos = 0; pos < len; pos++)
; pos start address is: 6 (W3)
	CLR	W3
; crc end address is: 2 (W1)
; pos end address is: 6 (W3)
L_modbus_handler_mb_crc160:
; pos start address is: 6 (W3)
; crc start address is: 2 (W1)
	CP	W3, W11
	BRA LTU	L_modbus_handler_mb_crc1659
	GOTO	L_modbus_handler_mb_crc161
L_modbus_handler_mb_crc1659:
;modbus_handler.c,34 :: 		crc^= (unsigned int)buf[pos];
	ADD	W10, W3, W0
	ZE	[W0], W0
; crc start address is: 8 (W4)
	XOR	W1, W0, W4
; crc end address is: 2 (W1)
;modbus_handler.c,35 :: 		for(i=0; i <8 ; i++)
; i start address is: 4 (W2)
	CLR	W2
; crc end address is: 8 (W4)
; i end address is: 4 (W2)
; pos end address is: 6 (W3)
L_modbus_handler_mb_crc163:
; i start address is: 4 (W2)
; crc start address is: 8 (W4)
; pos start address is: 6 (W3)
	CP.B	W2, #8
	BRA LTU	L_modbus_handler_mb_crc1660
	GOTO	L_modbus_handler_mb_crc164
L_modbus_handler_mb_crc1660:
;modbus_handler.c,37 :: 		if (crc & 0x0001) crc = (crc >> 1) ^ 0xA001;
	BTSS	W4, #0
	GOTO	L_modbus_handler_mb_crc166
	LSR	W4, #1, W1
; crc end address is: 8 (W4)
	MOV	#40961, W0
; crc start address is: 8 (W4)
	XOR	W1, W0, W4
	GOTO	L_modbus_handler_mb_crc167
L_modbus_handler_mb_crc166:
;modbus_handler.c,38 :: 		else      crc >>= 1;
	LSR	W4, #1, W0
; crc end address is: 8 (W4)
; crc start address is: 2 (W1)
	MOV	W0, W1
; crc end address is: 2 (W1)
	MOV	W1, W4
L_modbus_handler_mb_crc167:
;modbus_handler.c,35 :: 		for(i=0; i <8 ; i++)
; crc start address is: 8 (W4)
	INC.B	W2
;modbus_handler.c,39 :: 		}
; i end address is: 4 (W2)
	GOTO	L_modbus_handler_mb_crc163
L_modbus_handler_mb_crc164:
;modbus_handler.c,32 :: 		for(pos = 0; pos < len; pos++)
	INC	W3
;modbus_handler.c,40 :: 		}
	MOV	W4, W1
; crc end address is: 8 (W4)
; pos end address is: 6 (W3)
	GOTO	L_modbus_handler_mb_crc160
L_modbus_handler_mb_crc161:
;modbus_handler.c,41 :: 		return crc;
; crc start address is: 2 (W1)
	MOV	W1, W0
; crc end address is: 2 (W1)
;modbus_handler.c,42 :: 		}
L_end_mb_crc16:
	RETURN
; end of modbus_handler_mb_crc16

modbus_handler_be16:

;modbus_handler.c,45 :: 		static unsigned int be16(unsigned char hi, unsigned char lo)
;modbus_handler.c,47 :: 		return (((unsigned int)hi) <<8) | (unsigned int)lo;
	ZE	W10, W0
	SL	W0, #8, W1
	ZE	W11, W0
	IOR	W1, W0, W0
;modbus_handler.c,48 :: 		}
L_end_be16:
	RETURN
; end of modbus_handler_be16

modbus_handler_put_be16:

;modbus_handler.c,50 :: 		static void put_be16(unsigned char *p, unsigned int v)
;modbus_handler.c,52 :: 		p[0] = (unsigned char)(v>>8);
	LSR	W11, #8, W0
	MOV.B	W0, [W10]
;modbus_handler.c,53 :: 		p[1] = (unsigned char)(v & 0xFF);
	ADD	W10, #1, W1
	MOV	#255, W0
	AND	W11, W0, W0
	MOV.B	W0, [W1]
;modbus_handler.c,54 :: 		}
L_end_put_be16:
	RETURN
; end of modbus_handler_put_be16

modbus_handler_compute_gap_threshold_ms:

;modbus_handler.c,66 :: 		static unsigned int compute_gap_threshold_ms(unsigned long baud)
;modbus_handler.c,68 :: 		unsigned long ms = (39UL * 1000UL) / baud; //integer floor
	MOV.D	W10, W2
	MOV	#39000, W0
	MOV	#0, W1
	CLR	W4
	CALL	__Divide_32x32
;modbus_handler.c,69 :: 		ms += 2; //margin
	ADD	W0, #2, W0
	ADDC	W1, #0, W1
; ms start address is: 4 (W2)
	MOV.D	W0, W2
;modbus_handler.c,70 :: 		if (ms < 4) ms = 4;
	CP	W0, #4
	CPB	W1, #0
	BRA LTU	L_modbus_handler_compute_gap_threshold_ms64
	GOTO	L_modbus_handler_compute_gap_threshold_ms50
L_modbus_handler_compute_gap_threshold_ms64:
; ms end address is: 4 (W2)
; ms start address is: 0 (W0)
	MOV	#4, W0
	MOV	#0, W1
; ms end address is: 0 (W0)
	MOV	W1, W2
	MOV	W0, W1
	GOTO	L_modbus_handler_compute_gap_threshold_ms8
L_modbus_handler_compute_gap_threshold_ms50:
	MOV	W2, W1
	MOV	W3, W2
L_modbus_handler_compute_gap_threshold_ms8:
;modbus_handler.c,71 :: 		if (ms > 20) ms = 20;
; ms start address is: 2 (W1)
	CP	W1, #20
	CPB	W2, #0
	BRA GTU	L_modbus_handler_compute_gap_threshold_ms65
	GOTO	L_modbus_handler_compute_gap_threshold_ms51
L_modbus_handler_compute_gap_threshold_ms65:
	MOV	#20, W1
	MOV	#0, W2
; ms end address is: 2 (W1)
	GOTO	L_modbus_handler_compute_gap_threshold_ms9
L_modbus_handler_compute_gap_threshold_ms51:
L_modbus_handler_compute_gap_threshold_ms9:
;modbus_handler.c,72 :: 		return (unsigned int)ms;
; ms start address is: 2 (W1)
	MOV	W1, W0
; ms end address is: 2 (W1)
;modbus_handler.c,73 :: 		}
L_end_compute_gap_threshold_ms:
	RETURN
; end of modbus_handler_compute_gap_threshold_ms

modbus_handler_gap_tick_1ms:

;modbus_handler.c,77 :: 		static void gap_tick_1ms(void)
;modbus_handler.c,79 :: 		if(receiving)
	MOV	#lo_addr(modbus_handler_receiving), W0
	CP0.B	[W0]
	BRA NZ	L_modbus_handler_gap_tick_1ms67
	GOTO	L_modbus_handler_gap_tick_1ms10
L_modbus_handler_gap_tick_1ms67:
;modbus_handler.c,81 :: 		gap_ticks++;
	MOV	#1, W1
	MOV	#lo_addr(modbus_handler_gap_ticks), W0
	ADD	W1, [W0], [W0]
;modbus_handler.c,82 :: 		}
L_modbus_handler_gap_tick_1ms10:
;modbus_handler.c,83 :: 		}
L_end_gap_tick_1ms:
	RETURN
; end of modbus_handler_gap_tick_1ms

_MB_HregRead:

;modbus_handler.c,86 :: 		unsigned int MB_HregRead(unsigned int addr)
;modbus_handler.c,88 :: 		if (addr >= MB_HREG_COUNT) return 0;
	MOV	#64, W0
	CP	W10, W0
	BRA GEU	L__MB_HregRead69
	GOTO	L_MB_HregRead11
L__MB_HregRead69:
	CLR	W0
	GOTO	L_end_MB_HregRead
L_MB_HregRead11:
;modbus_handler.c,89 :: 		return mb_hregs[addr];
	SL	W10, #1, W1
	MOV	#lo_addr(_mb_hregs), W0
	ADD	W0, W1, W0
	MOV	[W0], W0
;modbus_handler.c,90 :: 		}
L_end_MB_HregRead:
	RETURN
; end of _MB_HregRead

_MB_HregWrite:

;modbus_handler.c,92 :: 		void MB_HregWrite(unsigned int addr, unsigned int value)
;modbus_handler.c,94 :: 		if(addr >= MB_HREG_COUNT) return;
	MOV	#64, W0
	CP	W10, W0
	BRA GEU	L__MB_HregWrite71
	GOTO	L_MB_HregWrite12
L__MB_HregWrite71:
	GOTO	L_end_MB_HregWrite
L_MB_HregWrite12:
;modbus_handler.c,95 :: 		mb_hregs[addr] = value;
	SL	W10, #1, W1
	MOV	#lo_addr(_mb_hregs), W0
	ADD	W0, W1, W0
	MOV	W11, [W0]
;modbus_handler.c,96 :: 		}
L_end_MB_HregWrite:
	RETURN
; end of _MB_HregWrite

modbus_handler_build_exception:

;modbus_handler.c,100 :: 		unsigned char excode)
;modbus_handler.c,102 :: 		tx_buf[0] = addr;
	PUSH	W10
	PUSH	W11
	MOV	#lo_addr(modbus_handler_tx_buf), W0
	MOV.B	W10, [W0]
;modbus_handler.c,103 :: 		tx_buf[1] = func | 0x80;
	MOV.B	#128, W1
	MOV	#lo_addr(modbus_handler_tx_buf+1), W0
	IOR.B	W11, W1, [W0]
;modbus_handler.c,104 :: 		tx_buf[2] = excode;
	MOV	#lo_addr(modbus_handler_tx_buf+2), W0
	MOV.B	W12, [W0]
;modbus_handler.c,107 :: 		unsigned int crc = mb_crc16(tx_buf, 3);
	MOV	#3, W11
	MOV	#lo_addr(modbus_handler_tx_buf), W10
	CALL	modbus_handler_mb_crc16
;modbus_handler.c,108 :: 		tx_buf[3] = (unsigned char)(crc & 0xFF);
	MOV.B	#255, W2
	MOV	#lo_addr(modbus_handler_tx_buf+3), W1
	AND.B	W0, W2, [W1]
	MOV	#lo_addr(modbus_handler_tx_buf+3), W2
	MOV	#lo_addr(modbus_handler_tx_buf+3), W1
	MOV.B	[W1], [W2]
;modbus_handler.c,109 :: 		tx_buf[4] = (unsigned char)(crc >> 8);
	LSR	W0, #8, W1
	MOV	#lo_addr(modbus_handler_tx_buf+4), W0
	MOV.B	W1, [W0]
;modbus_handler.c,111 :: 		return 5;
	MOV	#5, W0
;modbus_handler.c,112 :: 		}
;modbus_handler.c,111 :: 		return 5;
;modbus_handler.c,112 :: 		}
L_end_build_exception:
	POP	W11
	POP	W10
	RETURN
; end of modbus_handler_build_exception

modbus_handler_handle_fc03:

;modbus_handler.c,115 :: 		static unsigned int handle_fc03(unsigned char *req, unsigned int req_len)
;modbus_handler.c,122 :: 		start = (req[2] << 8) | req[3];
	PUSH	W10
	PUSH	W11
	PUSH	W12
	ADD	W10, #2, W0
	MOV.B	[W0], W0
	ZE	W0, W0
	SL	W0, #8, W1
	ADD	W10, #3, W0
	ZE	[W0], W0
; start start address is: 8 (W4)
	IOR	W1, W0, W4
;modbus_handler.c,123 :: 		qty = (req[4] <<8) | req[5];
	ADD	W10, #4, W0
	MOV.B	[W0], W0
	ZE	W0, W0
	SL	W0, #8, W1
	ADD	W10, #5, W0
	ZE	[W0], W0
; qty start address is: 6 (W3)
	IOR	W1, W0, W3
;modbus_handler.c,127 :: 		if (req_len != 8) return build_exception(req[0], req[1], MB_EX_ILLEGAL_VALUE);
	CP	W11, #8
	BRA NZ	L_modbus_handler_handle_fc0374
	GOTO	L_modbus_handler_handle_fc0313
L_modbus_handler_handle_fc0374:
; qty end address is: 6 (W3)
; start end address is: 8 (W4)
	ADD	W10, #1, W0
	MOV.B	#3, W12
	MOV.B	[W0], W11
	MOV.B	[W10], W10
	CALL	modbus_handler_build_exception
	GOTO	L_end_handle_fc03
L_modbus_handler_handle_fc0313:
;modbus_handler.c,128 :: 		if ((start + qty) > MB_HREG_COUNT) return build_exception(req[0], req[1], MB_EX_ILLEGAL_ADDRESS);
; start start address is: 8 (W4)
; qty start address is: 6 (W3)
	ADD	W4, W3, W1
	MOV	#64, W0
	CP	W1, W0
	BRA GTU	L_modbus_handler_handle_fc0375
	GOTO	L_modbus_handler_handle_fc0314
L_modbus_handler_handle_fc0375:
; qty end address is: 6 (W3)
; start end address is: 8 (W4)
	ADD	W10, #1, W0
	MOV.B	#2, W12
	MOV.B	[W0], W11
	MOV.B	[W10], W10
	CALL	modbus_handler_build_exception
	GOTO	L_end_handle_fc03
L_modbus_handler_handle_fc0314:
;modbus_handler.c,130 :: 		tx_buf[0] = req[0];
; start start address is: 8 (W4)
; qty start address is: 6 (W3)
	MOV.B	[W10], W1
	MOV	#lo_addr(modbus_handler_tx_buf), W0
	MOV.B	W1, [W0]
;modbus_handler.c,131 :: 		tx_buf[1] = 0x03;
	MOV	#lo_addr(modbus_handler_tx_buf+1), W1
	MOV.B	#3, W0
	MOV.B	W0, [W1]
;modbus_handler.c,132 :: 		tx_buf[2] = (unsigned char)(qty *2);
	SL	W3, #1, W1
	MOV	#lo_addr(modbus_handler_tx_buf+2), W0
	MOV.B	W1, [W0]
;modbus_handler.c,134 :: 		idx = 3;
; idx start address is: 10 (W5)
	MOV	#3, W5
;modbus_handler.c,135 :: 		for (i=0; i<qty; i++)
; i start address is: 4 (W2)
	CLR	W2
; qty end address is: 6 (W3)
; idx end address is: 10 (W5)
; i end address is: 4 (W2)
L_modbus_handler_handle_fc0315:
; i start address is: 4 (W2)
; idx start address is: 10 (W5)
; qty start address is: 6 (W3)
; start start address is: 8 (W4)
; start end address is: 8 (W4)
	CP	W2, W3
	BRA LTU	L_modbus_handler_handle_fc0376
	GOTO	L_modbus_handler_handle_fc0316
L_modbus_handler_handle_fc0376:
; start end address is: 8 (W4)
;modbus_handler.c,137 :: 		unsigned int v = mb_hregs[start + i];
; start start address is: 8 (W4)
	ADD	W4, W2, W0
	SL	W0, #1, W1
	MOV	#lo_addr(_mb_hregs), W0
	ADD	W0, W1, W0
	MOV	[W0], W0
; v start address is: 12 (W6)
	MOV	W0, W6
;modbus_handler.c,138 :: 		tx_buf[idx++] = (unsigned char)(v>>8);
	MOV	#lo_addr(modbus_handler_tx_buf), W0
	ADD	W0, W5, W1
	LSR	W6, #8, W0
	MOV.B	W0, [W1]
	ADD	W5, #1, W1
	MOV	W1, W5
;modbus_handler.c,139 :: 		tx_buf[idx++] = (unsigned char)(v & 0xFF);
	MOV	#lo_addr(modbus_handler_tx_buf), W0
	ADD	W0, W1, W1
	MOV	#255, W0
	AND	W6, W0, W0
; v end address is: 12 (W6)
	MOV.B	W0, [W1]
	INC	W5
;modbus_handler.c,135 :: 		for (i=0; i<qty; i++)
	INC	W2
;modbus_handler.c,140 :: 		}
; qty end address is: 6 (W3)
; start end address is: 8 (W4)
; i end address is: 4 (W2)
	GOTO	L_modbus_handler_handle_fc0315
L_modbus_handler_handle_fc0316:
;modbus_handler.c,143 :: 		unsigned int crc = mb_crc16(tx_buf, idx);
	PUSH.D	W10
	MOV	W5, W11
	MOV	#lo_addr(modbus_handler_tx_buf), W10
	CALL	modbus_handler_mb_crc16
	POP.D	W10
; crc start address is: 6 (W3)
	MOV	W0, W3
;modbus_handler.c,144 :: 		tx_buf[idx++] = (unsigned char)(crc & 0xFF);
	MOV	#lo_addr(modbus_handler_tx_buf), W1
	ADD	W1, W5, W2
	MOV	#255, W1
	AND	W0, W1, W0
	MOV.B	W0, [W2]
	ADD	W5, #1, W1
; idx end address is: 10 (W5)
; idx start address is: 4 (W2)
	MOV	W1, W2
;modbus_handler.c,145 :: 		tx_buf[idx++] = (unsigned char)(crc >>8);
	MOV	#lo_addr(modbus_handler_tx_buf), W0
	ADD	W0, W1, W1
	LSR	W3, #8, W0
; crc end address is: 6 (W3)
	MOV.B	W0, [W1]
	ADD	W2, #1, W0
; idx end address is: 4 (W2)
;modbus_handler.c,147 :: 		return idx;
;modbus_handler.c,148 :: 		}
;modbus_handler.c,147 :: 		return idx;
;modbus_handler.c,148 :: 		}
L_end_handle_fc03:
	POP	W12
	POP	W11
	POP	W10
	RETURN
; end of modbus_handler_handle_fc03

modbus_handler_handle_fc06:

;modbus_handler.c,150 :: 		static unsigned int handle_fc06(unsigned char *req, unsigned int req_len)
;modbus_handler.c,158 :: 		if (req_len != 8) return build_exception(req[0], req[1], MB_EX_ILLEGAL_VALUE);
	PUSH	W10
	PUSH	W11
	PUSH	W12
	CP	W11, #8
	BRA NZ	L_modbus_handler_handle_fc0678
	GOTO	L_modbus_handler_handle_fc0618
L_modbus_handler_handle_fc0678:
	ADD	W10, #1, W0
	MOV.B	#3, W12
	MOV.B	[W0], W11
	MOV.B	[W10], W10
	CALL	modbus_handler_build_exception
	GOTO	L_end_handle_fc06
L_modbus_handler_handle_fc0618:
;modbus_handler.c,160 :: 		reg = be16(req[2], req[3]); //be16 converts the  two bytes of the array into start register
	ADD	W10, #3, W1
	ADD	W10, #2, W0
	PUSH	W10
	MOV.B	[W1], W11
	MOV.B	[W0], W10
	CALL	modbus_handler_be16
	POP	W10
; reg start address is: 4 (W2)
	MOV	W0, W2
;modbus_handler.c,161 :: 		val = be16(req[4], req[5]); //be16 coverts
	ADD	W10, #5, W1
	ADD	W10, #4, W0
	PUSH	W10
	MOV.B	[W1], W11
	MOV.B	[W0], W10
	CALL	modbus_handler_be16
	POP	W10
; val start address is: 6 (W3)
	MOV	W0, W3
;modbus_handler.c,163 :: 		if (reg >= MB_HREG_COUNT)return build_exception(req[0], req[1], MB_EX_ILLEGAL_ADDRESS);
	MOV	#64, W0
	CP	W2, W0
	BRA GEU	L_modbus_handler_handle_fc0679
	GOTO	L_modbus_handler_handle_fc0619
L_modbus_handler_handle_fc0679:
; reg end address is: 4 (W2)
; val end address is: 6 (W3)
	ADD	W10, #1, W0
	MOV.B	#2, W12
	MOV.B	[W0], W11
	MOV.B	[W10], W10
	CALL	modbus_handler_build_exception
	GOTO	L_end_handle_fc06
L_modbus_handler_handle_fc0619:
;modbus_handler.c,165 :: 		mb_hregs[reg] = val;
; val start address is: 6 (W3)
; reg start address is: 4 (W2)
	SL	W2, #1, W1
; reg end address is: 4 (W2)
	MOV	#lo_addr(_mb_hregs), W0
	ADD	W0, W1, W0
	MOV	W3, [W0]
; val end address is: 6 (W3)
;modbus_handler.c,167 :: 		for(i = 0; i<6; i++) // copying the request into the tx buffer
; i start address is: 4 (W2)
	CLR	W2
; i end address is: 4 (W2)
L_modbus_handler_handle_fc0620:
; i start address is: 4 (W2)
	CP	W2, #6
	BRA LTU	L_modbus_handler_handle_fc0680
	GOTO	L_modbus_handler_handle_fc0621
L_modbus_handler_handle_fc0680:
;modbus_handler.c,169 :: 		tx_buf[i] = req[i];
	MOV	#lo_addr(modbus_handler_tx_buf), W0
	ADD	W0, W2, W1
	ADD	W10, W2, W0
	MOV.B	[W0], [W1]
;modbus_handler.c,167 :: 		for(i = 0; i<6; i++) // copying the request into the tx buffer
	INC	W2
;modbus_handler.c,170 :: 		}
; i end address is: 4 (W2)
	GOTO	L_modbus_handler_handle_fc0620
L_modbus_handler_handle_fc0621:
;modbus_handler.c,173 :: 		unsigned int crc = mb_crc16(tx_buf, 6);
	PUSH.D	W10
	MOV	#6, W11
	MOV	#lo_addr(modbus_handler_tx_buf), W10
	CALL	modbus_handler_mb_crc16
	POP.D	W10
;modbus_handler.c,174 :: 		tx_buf[6] = (unsigned char)(crc & 0xFF); // CRC LO
	MOV.B	#255, W2
	MOV	#lo_addr(modbus_handler_tx_buf+6), W1
	AND.B	W0, W2, [W1]
	MOV	#lo_addr(modbus_handler_tx_buf+6), W2
	MOV	#lo_addr(modbus_handler_tx_buf+6), W1
	MOV.B	[W1], [W2]
;modbus_handler.c,175 :: 		tx_buf[7] = (unsigned char)(crc >> 8);   // CRC HI
	LSR	W0, #8, W1
	MOV	#lo_addr(modbus_handler_tx_buf+7), W0
	MOV.B	W1, [W0]
;modbus_handler.c,177 :: 		return 8;
	MOV	#8, W0
;modbus_handler.c,179 :: 		}
;modbus_handler.c,177 :: 		return 8;
;modbus_handler.c,179 :: 		}
L_end_handle_fc06:
	POP	W12
	POP	W11
	POP	W10
	RETURN
; end of modbus_handler_handle_fc06

modbus_handler_process_frame:
	LNK	#0

;modbus_handler.c,184 :: 		static void process_frame(unsigned char *frame, unsigned int len)
;modbus_handler.c,190 :: 		unsigned int resp_len = 0;
	PUSH	W10
	PUSH	W11
	PUSH	W12
;modbus_handler.c,193 :: 		if (len < 4) return;
	CP	W11, #4
	BRA LTU	L_modbus_handler_process_frame82
	GOTO	L_modbus_handler_process_frame23
L_modbus_handler_process_frame82:
	GOTO	L_end_process_frame
L_modbus_handler_process_frame23:
;modbus_handler.c,195 :: 		addr = frame[0];
; addr start address is: 10 (W5)
	MOV.B	[W10], W5
;modbus_handler.c,196 :: 		func = frame[1];
	ADD	W10, #1, W0
; func start address is: 12 (W6)
	MOV.B	[W0], W6
;modbus_handler.c,198 :: 		is_broadcast = (addr == 0) ? 1 : 0;
	MOV.B	[W10], W0
	CP.B	W0, #0
	BRA Z	L_modbus_handler_process_frame83
	GOTO	L_modbus_handler_process_frame24
L_modbus_handler_process_frame83:
; ?FLOC__modbus_handler_process_frame?T174 start address is: 2 (W1)
	MOV.B	#1, W1
; ?FLOC__modbus_handler_process_frame?T174 end address is: 2 (W1)
	GOTO	L_modbus_handler_process_frame25
L_modbus_handler_process_frame24:
; ?FLOC__modbus_handler_process_frame?T174 start address is: 2 (W1)
	CLR	W1
; ?FLOC__modbus_handler_process_frame?T174 end address is: 2 (W1)
L_modbus_handler_process_frame25:
; ?FLOC__modbus_handler_process_frame?T174 start address is: 2 (W1)
	ADD	W14, #0, W0
	BSET	[W0], #0
	BTSS	W1, #0
	BCLR	[W0], #0
; ?FLOC__modbus_handler_process_frame?T174 end address is: 2 (W1)
;modbus_handler.c,199 :: 		if (!is_broadcast && addr != mb_slave_id) return;
	ADD	W14, #0, W0
	BTSC	[W0], #0
	GOTO	L_modbus_handler_process_frame55
	MOV	#lo_addr(modbus_handler_mb_slave_id), W0
	CP.B	W5, [W0]
	BRA NZ	L_modbus_handler_process_frame84
	GOTO	L_modbus_handler_process_frame54
L_modbus_handler_process_frame84:
; addr end address is: 10 (W5)
; func end address is: 12 (W6)
L_modbus_handler_process_frame53:
	GOTO	L_end_process_frame
L_modbus_handler_process_frame55:
; func start address is: 12 (W6)
; addr start address is: 10 (W5)
L_modbus_handler_process_frame54:
;modbus_handler.c,201 :: 		crc_rx = (unsigned int)frame[len -2] | ((unsigned int)frame[len-1] << 8);
	SUB	W11, #2, W2
	ADD	W10, W2, W0
	ZE	[W0], W1
	SUB	W11, #1, W0
	ADD	W10, W0, W0
	ZE	[W0], W0
	SL	W0, #8, W0
; crc_rx start address is: 14 (W7)
	IOR	W1, W0, W7
;modbus_handler.c,202 :: 		crc_calc = mb_crc16(frame, len - 2);
	PUSH	W11
	MOV	W2, W11
	CALL	modbus_handler_mb_crc16
	POP	W11
;modbus_handler.c,203 :: 		if (crc_rx != crc_calc) return;
	CP	W7, W0
	BRA NZ	L_modbus_handler_process_frame85
	GOTO	L_modbus_handler_process_frame29
L_modbus_handler_process_frame85:
; addr end address is: 10 (W5)
; func end address is: 12 (W6)
; crc_rx end address is: 14 (W7)
	GOTO	L_end_process_frame
L_modbus_handler_process_frame29:
;modbus_handler.c,205 :: 		switch(func)
; func start address is: 12 (W6)
; addr start address is: 10 (W5)
	GOTO	L_modbus_handler_process_frame30
; addr end address is: 10 (W5)
; func end address is: 12 (W6)
;modbus_handler.c,207 :: 		case 0x03: resp_len = handle_fc03(frame, len); break;
L_modbus_handler_process_frame32:
	CALL	modbus_handler_handle_fc03
; resp_len start address is: 2 (W1)
	MOV	W0, W1
; resp_len end address is: 2 (W1)
	GOTO	L_modbus_handler_process_frame31
;modbus_handler.c,208 :: 		case 0x06: resp_len = handle_fc06(frame,len); break;
L_modbus_handler_process_frame33:
	CALL	modbus_handler_handle_fc06
; resp_len start address is: 2 (W1)
	MOV	W0, W1
; resp_len end address is: 2 (W1)
	GOTO	L_modbus_handler_process_frame31
;modbus_handler.c,210 :: 		default:
L_modbus_handler_process_frame34:
;modbus_handler.c,211 :: 		resp_len = build_exception(addr, func, MB_EX_ILLEGAL_FUNCTION);
; func start address is: 12 (W6)
; addr start address is: 10 (W5)
	MOV.B	#1, W12
	MOV.B	W6, W11
; func end address is: 12 (W6)
	MOV.B	W5, W10
; addr end address is: 10 (W5)
	CALL	modbus_handler_build_exception
; resp_len start address is: 2 (W1)
	MOV	W0, W1
;modbus_handler.c,212 :: 		break;
; resp_len end address is: 2 (W1)
	GOTO	L_modbus_handler_process_frame31
;modbus_handler.c,213 :: 		}
L_modbus_handler_process_frame30:
; func start address is: 12 (W6)
; addr start address is: 10 (W5)
	CP.B	W6, #3
	BRA NZ	L_modbus_handler_process_frame86
	GOTO	L_modbus_handler_process_frame32
L_modbus_handler_process_frame86:
	CP.B	W6, #6
	BRA NZ	L_modbus_handler_process_frame87
	GOTO	L_modbus_handler_process_frame33
L_modbus_handler_process_frame87:
	GOTO	L_modbus_handler_process_frame34
; addr end address is: 10 (W5)
; func end address is: 12 (W6)
L_modbus_handler_process_frame31:
;modbus_handler.c,216 :: 		if(!is_broadcast && resp_len > 0)
; resp_len start address is: 2 (W1)
	ADD	W14, #0, W0
	BTSC	[W0], #0
	GOTO	L_modbus_handler_process_frame57
	CP	W1, #0
	BRA GTU	L_modbus_handler_process_frame88
	GOTO	L_modbus_handler_process_frame56
L_modbus_handler_process_frame88:
L_modbus_handler_process_frame52:
;modbus_handler.c,218 :: 		RS485_SendBytes(tx_buf, resp_len);
	MOV	W1, W11
; resp_len end address is: 2 (W1)
	MOV	#lo_addr(modbus_handler_tx_buf), W10
	CALL	_RS485_SendBytes
;modbus_handler.c,216 :: 		if(!is_broadcast && resp_len > 0)
L_modbus_handler_process_frame57:
L_modbus_handler_process_frame56:
;modbus_handler.c,220 :: 		}
L_end_process_frame:
	POP	W12
	POP	W11
	POP	W10
	ULNK
	RETURN
; end of modbus_handler_process_frame

_MB_Init:

;modbus_handler.c,223 :: 		void MB_Init(unsigned char slave_id, unsigned long baud)
;modbus_handler.c,227 :: 		mb_slave_id = slave_id;
	MOV	#lo_addr(modbus_handler_mb_slave_id), W0
	MOV.B	W10, [W0]
;modbus_handler.c,228 :: 		mb_baud = baud;
	MOV	W11, modbus_handler_mb_baud
	MOV	W12, modbus_handler_mb_baud+2
;modbus_handler.c,231 :: 		for (i = 0; i < MB_HREG_COUNT; i++)
; i start address is: 4 (W2)
	CLR	W2
; i end address is: 4 (W2)
L_MB_Init38:
; i start address is: 4 (W2)
	MOV	#64, W0
	CP	W2, W0
	BRA LTU	L__MB_Init90
	GOTO	L_MB_Init39
L__MB_Init90:
;modbus_handler.c,232 :: 		mb_hregs[i] = 0;
	SL	W2, #1, W1
	MOV	#lo_addr(_mb_hregs), W0
	ADD	W0, W1, W1
	CLR	W0
	MOV	W0, [W1]
;modbus_handler.c,231 :: 		for (i = 0; i < MB_HREG_COUNT; i++)
	INC	W2
;modbus_handler.c,232 :: 		mb_hregs[i] = 0;
; i end address is: 4 (W2)
	GOTO	L_MB_Init38
L_MB_Init39:
;modbus_handler.c,235 :: 		gap_threshold = compute_gap_threshold_ms(baud);
	PUSH	W11
	PUSH	W12
	PUSH	W10
	MOV	W11, W10
	MOV	W12, W11
	CALL	modbus_handler_compute_gap_threshold_ms
	POP	W10
	POP	W12
	POP	W11
	MOV	W0, modbus_handler_gap_threshold
;modbus_handler.c,238 :: 		RS485_Init(baud);
	PUSH	W11
	PUSH	W12
	PUSH	W10
	MOV	W11, W10
	MOV	W12, W11
	CALL	_RS485_Init
	POP	W10
	POP	W12
	POP	W11
;modbus_handler.c,240 :: 		rx_len = 0;
	CLR	W0
	MOV	W0, modbus_handler_rx_len
;modbus_handler.c,241 :: 		receiving = 0;
	MOV	#lo_addr(modbus_handler_receiving), W1
	CLR	W0
	MOV.B	W0, [W1]
;modbus_handler.c,242 :: 		gap_ticks = 0;
	CLR	W0
	MOV	W0, modbus_handler_gap_ticks
;modbus_handler.c,243 :: 		}
L_end_MB_Init:
	RETURN
; end of _MB_Init

_MB_Task:

;modbus_handler.c,245 :: 		void MB_Task(void)
;modbus_handler.c,248 :: 		while (RS485_ByteAvailable())
	PUSH	W10
	PUSH	W11
L_MB_Task41:
	CALL	_RS485_ByteAvailable
	CP0.B	W0
	BRA NZ	L__MB_Task92
	GOTO	L_MB_Task42
L__MB_Task92:
;modbus_handler.c,250 :: 		unsigned char b = RS485_ReadByte();
	CALL	_RS485_ReadByte
; b start address is: 4 (W2)
	MOV.B	W0, W2
;modbus_handler.c,252 :: 		if (rx_len < MB_MAX_FRAME)
	MOV	modbus_handler_rx_len, W1
	MOV	#256, W0
	CP	W1, W0
	BRA LTU	L__MB_Task93
	GOTO	L_MB_Task43
L__MB_Task93:
;modbus_handler.c,253 :: 		rx_buf[rx_len++] = b;
	MOV	#lo_addr(modbus_handler_rx_buf), W1
	MOV	#lo_addr(modbus_handler_rx_len), W0
	ADD	W1, [W0], W0
	MOV.B	W2, [W0]
; b end address is: 4 (W2)
	MOV	#1, W1
	MOV	#lo_addr(modbus_handler_rx_len), W0
	ADD	W1, [W0], [W0]
	GOTO	L_MB_Task44
L_MB_Task43:
;modbus_handler.c,255 :: 		rx_len = 0; //overflow -> reset
	CLR	W0
	MOV	W0, modbus_handler_rx_len
L_MB_Task44:
;modbus_handler.c,258 :: 		receiving = 1;
	MOV	#lo_addr(modbus_handler_receiving), W1
	MOV.B	#1, W0
	MOV.B	W0, [W1]
;modbus_handler.c,259 :: 		gap_ticks = 0;
	CLR	W0
	MOV	W0, modbus_handler_gap_ticks
;modbus_handler.c,260 :: 		}
	GOTO	L_MB_Task41
L_MB_Task42:
;modbus_handler.c,263 :: 		if(receiving)
	MOV	#lo_addr(modbus_handler_receiving), W0
	CP0.B	[W0]
	BRA NZ	L__MB_Task94
	GOTO	L_MB_Task45
L__MB_Task94:
;modbus_handler.c,267 :: 		Delay_ms(1);
	MOV	#4000, W7
L_MB_Task46:
	DEC	W7
	BRA NZ	L_MB_Task46
;modbus_handler.c,268 :: 		gap_tick_1ms();
	CALL	modbus_handler_gap_tick_1ms
;modbus_handler.c,271 :: 		if(gap_ticks >= gap_threshold)
	MOV	modbus_handler_gap_ticks, W1
	MOV	#lo_addr(modbus_handler_gap_threshold), W0
	CP	W1, [W0]
	BRA GEU	L__MB_Task95
	GOTO	L_MB_Task48
L__MB_Task95:
;modbus_handler.c,273 :: 		unsigned int len = rx_len;
; len start address is: 4 (W2)
	MOV	modbus_handler_rx_len, W2
;modbus_handler.c,275 :: 		receiving = 0;
	MOV	#lo_addr(modbus_handler_receiving), W1
	CLR	W0
	MOV.B	W0, [W1]
;modbus_handler.c,276 :: 		rx_len = 0;
	CLR	W0
	MOV	W0, modbus_handler_rx_len
;modbus_handler.c,277 :: 		gap_ticks = 0;
	CLR	W0
	MOV	W0, modbus_handler_gap_ticks
;modbus_handler.c,279 :: 		if (len >= 4)
	CP	W2, #4
	BRA GEU	L__MB_Task96
	GOTO	L_MB_Task49
L__MB_Task96:
;modbus_handler.c,280 :: 		process_frame(rx_buf, len);
	MOV	W2, W11
; len end address is: 4 (W2)
	MOV	#lo_addr(modbus_handler_rx_buf), W10
	CALL	modbus_handler_process_frame
L_MB_Task49:
;modbus_handler.c,281 :: 		}
L_MB_Task48:
;modbus_handler.c,283 :: 		}
L_MB_Task45:
;modbus_handler.c,286 :: 		}
L_end_MB_Task:
	POP	W11
	POP	W10
	RETURN
; end of _MB_Task
