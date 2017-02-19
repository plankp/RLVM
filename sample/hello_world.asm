	# A typical "Hello, world!" program
	.STACK 1
	.SECTION text
main:	LDC r0, _LC0		# Change to _LC1 will also work
	call puts
	MOV r0, 0
	HALT r0

	.SECTION data
_LC0:	db 0x48,
	db 0x65,
	db 0x6c,
	db 0x6c,
	db 0x6f,
	db 0x2c,
	db 0x20,
	db 0x77,
	db 0x6f,
	db 0x72,
	db 0x6c,
	db 0x64,
	db 0x21,
	db 0x00
_LC1:	"Hello, world!"
