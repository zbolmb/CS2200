		ADD $a0, $zero, $zero
		ADDI $t0, $zero, 10
loop:	ADDI $a0, $a0, 1
		BNE $a0, $t0, loop

loop2:

