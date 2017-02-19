	# Same thing in C's <stdio.h>
	#
	# extern r0 puts (r0)
	.SECTION text
	.GLOBAL puts
puts:	LDC r1, STDOUT
	FWRTS r0, r1, r0
	MOV r0, 0xA
	FWRTB r0, r1, r0
