	# THIS FILE MUST GIVEN WITH SUM.ASM

	.SECTION TEXT
	.STACK 1
START:	MOV R0, 1
	MOV R1, 2
	CALL SUM
	LDC R2, STDOUT
	FWRTQ R5, R2, R0
	MOV R0, 0XA
	FWRTB R5, R2, R0
	HALT R31
