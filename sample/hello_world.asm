	# A typical "Hello, world!" program
	.STACK 1
	.SECTION text
main:	LDC r0, _LC0
	call puts
	MOV r0, 0
	HALT r0

	.SECTION data
_LC0:	0x48, 0x65, 0x6c, 0x6c, 0x6f, 0x2c, 0x20,
	0x77, 0x6f, 0x72, 0x6c, 0x64, 0x21
