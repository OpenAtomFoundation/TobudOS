.global irq_entry
.global trap_entry

.extern rv32_exception_entry

.align 2
irq_entry:
    j rv32_exception_entry

.align 6
trap_entry:
    j rv32_exception_entry
