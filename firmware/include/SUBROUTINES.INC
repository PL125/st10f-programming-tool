IDENTIFY PROC NEAR
		MOV R15,IDMANUF
		CALL SEND
		MOV R15,IDCHIP
		CALL SEND
		RET
IDENTIFY ENDP


; Vstup: R8:R7 global counter, R10 block counter
; Vystup: R14 navratovy kod (0 pokracuje sa, 1 ukoncit spracovanie)
CHECK_COUNT PROC NEAR
		; Defaultny navratovy kod je "pokracovat"
		MOV R14,#0
		; Dekrementujeme globalny citac
		SUB R7,#2
		SUBC R8,#0
		CMP R8,#0
		JMPR CC_NE,CHECK_COUNT_BLOCK
		CMP R7,#0
		JMPR CC_NE,CHECK_COUNT_BLOCK
		; Uspesne koncime, mame precitany potrebny pocet bajtov
		MOV R14,#1
		JMP CHECK_RET_SEND
CHECK_COUNT_BLOCK:
		; Dekrementujeme citac 1024 bajtoveho bloku
		SUB R10,#2
		CMP R10,#0
		JMPR CC_NE,CHECK_RET
CHECK_RET_SEND:
		; Odosleme nulu
		MOV R15,#0
		CALL SEND
		; Resetujeme citac 1024 bajtoveho bloku
		MOV R10,#1024
		; Posleme hodnotu globalneho citaca
		MOV R15,R7
		CALL SEND
		MOV R15,R8
		CALL SEND		
CHECK_RET:
		RET
CHECK_COUNT ENDP

;-------------------------------------------------------------------------------
; Safe receive
;-------------------------------------------------------------------------------
; !! Posiela do PC kod chyby
REC_SAFE PROC NEAR
		; Vystup: R14 navratovy kod, R15 data (slovo)
		CALL REC
		CMP R14,#0
		JMPR CC_NE,REC_SAFE_RETURN_NO_SEND
		CALL ACK_WORD
		; Send result of safe receiving
REC_SAFE_RETURN:
		PUSH R15
		MOV R15,R14
		CALL SEND
		POP R15
REC_SAFE_RETURN_NO_SEND:
		RET
REC_SAFE ENDP
	

REC_DWORD_SAFE PROC NEAR
		; Vystup: R14 navratovy kod, R8:R7 data (dvojslovo)
		PUSH R15
		CALL REC_SAFE
		CMP R14,#0
		JMPR CC_NE,REC_DWORD_ERROR
		MOV R7,R15
		CALL REC_SAFE
		CMP R14,#0
		JMPR CC_NE,REC_DWORD_ERROR
		MOV R8,R15
REC_DWORD_ERROR:
		POP R15
		RET
REC_DWORD_SAFE ENDP
		
		
ACK_WORD PROC NEAR
		PUSH R12
		PUSH R13
		MOV R13,#SEND
		MOV R12,#REC
		CALL ACK_DATA
		POP R13
		POP R12
		RET
ACK_WORD ENDP
	
	
ACK_DATA PROC NEAR
		; Vstup: R15 data (slovo), R13 (pointer na SEND), R12 (pointer na REC)
		; Vystup: R14 navratovy kod, R15 data (slovo)
		PUSH R0
		PUSH R15
		MOV R14,#RET_BAD_ECHO
		MOV R0,R15
		CALL [R13] ; SEND
		CALL [R12] ; RECEIVE
		; Serial buffer overrun?
		CMP R14,#0
		JMPR CC_NE,ACK_DATA_RETURN ; Return code is already in R14
		; Good echo from PC?
		CMP R15,R0
		JMPR CC_NE,ACK_DATA_RETURN ; Return code is already in R14
		; Everything is OK
		MOV R14,#0
ACK_DATA_RETURN:
		POP R15
		POP R0
		RET
ACK_DATA ENDP
	
;-------------------------------------------------------------------------------
; Send and receive
;-------------------------------------------------------------------------------
SEND PROC NEAR
		; Posle slovo.
		; Vstup: R15
		; Zachovava: R15
		PUSH R15
		CALL SEND_BYTE
		SHR R15,#8
		CALL SEND_BYTE
		POP R15
		RET
SEND ENDP
	
	
SEND_BYTE PROC NEAR
		; Posle nizsi bajt zo slova.
		; Vstup: R15
		; Zachovava: R15
SEND_BYTE_1:
		JNB S0TIC.7,SEND_BYTE_1
		BCLR S0TIC.7
		MOV S0TBUF,R15 ; bity 9 az 15 su nevyznamne
		RET
SEND_BYTE ENDP
	
	
REC PROC NEAR
		; Vystup: R15 data, R14 navratovy kod
		PUSH R0
		CALL REC_BYTE  ; Receive low byte
		CMP R14,#0
		JMPR CC_NE,REC_OK
		MOV R0,R15    ; high byte bude vzdy 0
		CALL REC_BYTE
		SHL R15,#8
		OR R15,R0
REC_OK:
		POP R0
		RET
REC ENDP


REC_BYTE PROC NEAR
		; Vystup: R15 data (low byte), R14 navratovy kod
REC_BYTE_1:
		JNB S0RIC.7,REC_BYTE_1
		JB S0CON.10,REC_BYTE_ERR
		MOV R15,S0RBUF ; nevyznamne bity pre dany operacny rezim serioveho rozhrania budu precitane ako 0
		BCLR S0RIC.7
		; Successfully received word
		MOV R14,#0
		JMP REC_BYTE_OK
REC_BYTE_ERR:
		BCLR S0CON.10
		MOV R14,#RET_SERIAL_OVERRUN
REC_BYTE_OK:
		RET
REC_BYTE ENDP
