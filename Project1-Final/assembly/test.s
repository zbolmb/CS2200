        ! Set up our stack pointer
        LEA $sp, stackpointer
        LDR $sp, 0($sp)
        ADDI $fp, $fp, 0                    ! set topmost frame pointer to zero

        ! Call bitcount(firstnum)
        LEA $s0, firstnum                   ! load address of firstnum into $s0
        LDR $a0, 0($s0)                     ! load firstnum into argument $a0
        LEA $at, bitcount                   ! load address of bitcount
        JALR $at, $ra                       ! ...and jump!
        STR $v0, 0($s0)                     ! put the answer into memory
        ADD $s0, $v0, $zero                 ! now put answer into $s0

        ! Call bitcount(secondnum)
        LEA $s1, secondnum                  ! load address of secondnum into $s1
        LDR $a0, 0($s1)                     ! load secondnum into argument $a0
        LEA $at, bitcount                   ! load address of bitcount
        JALR $at, $ra                       ! ...and jump!
        STR $v0, 0($s1)                     ! put the answer into memory
        ADD $s1, $v0, $zero                 ! now put answer into $s1

        ! Call bitcount(thirdnum)
        LEA $s2, thirdnum                   ! load address of thirdnum into $s2
        LDR $a0, 0($s2)                     ! load thirdnum into argument $a0
        LEA $at, bitcount                   ! load address of bitcount
        JALR $at, $ra                       ! ...and jump!
        STR $v0, 0($s2)                     ! put the answer into memory
        ADD $s2, $v0, $zero                 ! now put answer into $s2
        HALT

bitcount:
        ! [set up our stack frame]
        ADDI $sp, $sp, -1                   ! allocate space for old frame pointer
        STR $fp, 0($sp)
        ADDI $fp, $sp, 0                    ! initialize new frame pointer
        ADDI $sp, $sp, -3                   ! allocate space for any S registers we use
        STR $s0, -1($fp)
        STR $s1, -2($fp)

        ! [do our work]
        ADD $s1, $a0, $zero                 ! $s1 = $a0 (the number we will be shifting)
        ADD $s0, $zero, $zero               ! count($s0) = 0
loop:   ADDI $t2, $zero, 1                  ! $t2 = 1
        NAND $t2, $s1, $t2
        NAND $t2, $t2, $t2                  ! $t2 = num & 1
        ADD $s0, $s0, $t2                   ! count += (num & 1)
        SHFRL $s1, $s1, 1                   ! num >>= 1
        BRnp loop
        ADD $v0, $s0, $zero                 ! return value = $s0

        ! [tear down and return]
        LDR $s0, -1($fp)                    ! restore the saved S registers
        LDR $s1, -2($fp)
        ADDI $sp, $fp, 0                    ! clear all locals and saved S off the stack
        LDR $fp, 0($sp)                     ! put back old frame pointer
        ADDI $sp, $sp, 1                    ! clear space used by old FP
        RET                                 ! return

        ! Initial value of the stack pointer
stackpointer:
        .word 0xFFFFFF

        ! Our test inputs
firstnum:
        .word 0xFFFF                        ! 16 1's
secondnum:
        .word 0x0000                        ! 0 1's
thirdnum:
        .word 0x4231                        ! 5 1's

