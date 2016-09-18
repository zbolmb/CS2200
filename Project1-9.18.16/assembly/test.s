main:	ADDI $zero, $zero, 5                ! try to add 5 to $zero
	ADDI $s0, $zero, 0                  ! put $zero into $s0 to see if $zero changed
	BRnp fail
	halt

success:
        LEA $k0, success                    ! clear error number
        HALT   

fail:
        ADDI $t0, $zero, -1                 ! set $t registers to signal error
        ADDI $t1, $zero, -1
        ADDI $t2, $zero, -1
        HALT
