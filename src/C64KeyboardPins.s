
.section .text

.globl initKeyboardGPIOPins
initKeyboardGPIOPins:
	push {lr}
        bl GetGpioAddress
        ldr r1,=0b1001001000000000
        str r1,[r0,#8]   @; set GPIO pin 22(b6) to input and 23(b9),24(b12),25(b15) function to output
	pop {pc}


.globl serialDelay
serialDelay:
	mov r2,#0x10
	wait1$:
	sub r2,#1
	cmp r2,#0
	bne wait1$
	mov pc,lr

