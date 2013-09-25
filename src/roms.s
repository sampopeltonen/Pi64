.section .rodata


.globl characterROM
characterROM:
	.incbin "./rom/chargen"

.globl kernalROM
kernalROM:
	.incbin "./rom/kernal"

.globl basicROM
basicROM:
	.incbin "./rom/basic"

