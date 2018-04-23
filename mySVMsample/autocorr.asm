		.def    _autoCorr

.text

;-------------------------------------------------------------------- Purpose of this function
; This function takes the input signal x[n] and computes y[k]
; (i.e. k-th sample of the autocorrelation function of x[n])

;-------------------------------------------------------------------- Note
; All pointers and variables must be 16-bits in size

;-------------------------------------------------------------------- Registers passed in as arguments
; A4 = Pointer to x[0]
; B4 = k
; A6 = Pointer to y[k]
; B6 = length(x)-1-k

;-------------------------------------------------------------------- Temporary Registers
; A0 = Pointer to x[n+k]
; B0 = length(x)-1-k
; A1 = Placeholder for x[n]*x[n+k]
; A2 = Placeholder for (x[0]*x[0+k] + x[1]*x[1+k] + x[2]*x[2+k] ...)
; A3 = Placeholder for x[n]
; A7 = Placeholder for x[n+k]

_autoCorr:
		ADD A4,0,A0
		MV B6,B0
		NOP 5
		MV B4,A1
		NOP 5
		ZERO .L1 A9
		NOP 5
		ZERO .L1 A2
		NOP 5
		[!A1] B .S2 loop
		NOP 5
count:
		LDH .D1 *A4++,A3
		NOP 5
		SUB  A1,1,A1
		[A1] B .S2 count
		NOP 5
loop:
		LDH .D1 *A4++,A3
		NOP 5
		LDH .D1 *A0++,A7
		NOP 5
		MPY .M1 A3,A7,A9 ; Multiply the two samples.
		NOP 5
		ADD .L1 A2,A9,A2 ; Accumulate the result of multiplication.
		NOP 5
		SUB .S2 B0,1,B0 ; Count down
		NOP 5
		[B0] B .S2 loop

		NOP 5
		STH .D1 A2,*A6 ; Store the sum of products to the output.
		NOP 5
		B .S2 B3
		NOP 5

		.end
