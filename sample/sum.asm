	# CALCULATES THE SUM OF R0 AND R1

	.SECTION TEXT
	.STACK 1
	.ESTACK 0
START:	MOV R0, 1
	MOV R1, 2
	CALL SUM
	LDC R2, STDOUT
	FWRTQ R5, R2, R0
	MOV R0, 0xA
	FWRTB R5, R2, R0
	HALT R31
	.GLOBAL SUM
SUM:	ADD R0, R0, R1
	RET
