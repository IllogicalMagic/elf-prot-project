    .global Executable
Executable:
    .quad .+8
    .incbin "data"
data_end:

    .global Exec_len
Exec_len:
    .int data_end - Executable - 8
