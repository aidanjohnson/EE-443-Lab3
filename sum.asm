	    .def	  _sumfunc
_sumfunc:
		MV   .L1  A4,A1
	    SUB  .S1  A1,1,A1

LOOP:
		ADD  .L1  A4,A1,A4
        SUB  .S1  A1,1,A1
  [A1]  B      .S2  LOOP
        NOP	  5
        B    .S2  B3
        NOP	  5 ; otherwise will steal the cycle for some operations.
        .end
