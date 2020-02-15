- set your uart's baudrate to 9600, too high may not work

- replace all the tos_mmheap_alloc in tos_shell.c with malloc

  replace all the tos_mmheap_free in tos_shell.c with free

  That means we would not use tos_mmheap in STM8