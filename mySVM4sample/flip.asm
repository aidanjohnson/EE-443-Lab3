			.def _flipfunc
_flipfunc:
			MV	A6,A0
			MV  A6,A1
			NOP 2
count:
			LDH *B4++,A8
			SUB A0,1,A0
	[A0]	B	count
			NOP 5
loop:
			LDH *A4++,A7
			NOP 2
			STH A7,*B4--
			SUB A1,1,A1
	[A1]	B	loop
			NOP 5
			B   B3
			NOP 5
			.end
