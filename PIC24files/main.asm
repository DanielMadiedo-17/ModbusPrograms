
_main:
	MOV	#2048, W15
	MOV	#6142, W0
	MOV	WREG, 32
	MOV	#1, W0
	MOV	WREG, 52
	MOV	#4, W0
	IOR	68

;main.c,7 :: 		void main() {
;main.c,8 :: 		Board_Init();
	PUSH	W10
	PUSH	W11
	PUSH	W12
	CALL	_Board_Init
;main.c,9 :: 		MB_Init(2, 19200);
	MOV	#19200, W11
	MOV	#0, W12
	MOV.B	#2, W10
	CALL	_MB_Init
;main.c,11 :: 		while (1)
L_main0:
;main.c,14 :: 		MB_Task();
	CALL	_MB_Task
;main.c,15 :: 		mb_hregs[0] = 0xAAAA;
	MOV	#43690, W0
	MOV	W0, _mb_hregs
;main.c,16 :: 		mb_hregs[1] = 0x1234;
	MOV	#4660, W0
	MOV	W0, _mb_hregs+2
;main.c,18 :: 		}
	GOTO	L_main0
;main.c,19 :: 		}
L_end_main:
	POP	W12
	POP	W11
	POP	W10
L__main_end_loop:
	BRA	L__main_end_loop
; end of _main
