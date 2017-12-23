.section .data
        .global Executable
Executable:
    .quad .+8
    .incbin "data"
data_end:

    .global ExecLen
ExecLen:
    .int data_end - Executable - 8
