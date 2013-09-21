.globl _start
_start:
	/* set up the stack pointer. Stack grows downwards. */
	/* mov sp,#0x8000 */
	mov sp,#0x00200000

	bl start_l1cache
	bl init_mmu
/*
* Jump to main program
*/
	bl vicmain

/* 
* Setup the screen.
*/
.globl setupScreen
setupScreen:
	push {r4,lr}
	/*mov r0,#1024
        mov r1,#768
        mov r2,#16*/
        bl InitialiseFrameBuffer

/* 
* Check for a failed frame buffer.
*/
        teq r0,#0
        bne noError$

	bl ledOn
	b hang

        noError$:

        fbInfoAddr .req r4
        mov fbInfoAddr,r0


/*
* Let our drawing method know where we are drawing to.
*/
        bl SetGraphicsAddress
	.unreq fbInfoAddr
	pop {r4,pc}


.globl start_l1cache
start_l1cache:
mov r0, #0
mcr p15, 0, r0, c7, c7, 0 ;@ invalidate caches
mcr p15, 0, r0, c8, c7, 0 ;@ invalidate tlb
mrc p15, 0, r0, c1, c0, 0
orr r0,r0,#0x1000 ;@ instruction
orr r0,r0,#0x0004 ;@ data
mcr p15, 0, r0, c1, c0, 0
bx lr

/*
.globl stop_l1cache
stop_l1cache:
mrc p15, 0, r0, c1, c0, 0
bic r0,r0,#0x1000 ;@ instruction
bic r0,r0,#0x0004 ;@ data
mcr p15, 0, r0, c1, c0, 0
bx lr
*/

.globl start_mmu
start_mmu:
    mov r2,#0
    mcr p15,0,r2,c7,c7,0 ;@ invalidate caches
    mcr p15,0,r2,c8,c7,0 ;@ invalidate tlb

    mvn r2,#0
    mcr p15,0,r2,c3,c0,0 ;@ domain

    mcr p15,0,r0,c2,c0,0 ;@ tlb base
    mcr p15,0,r0,c2,c0,1 ;@ tlb base

    mrc p15,0,r2,c1,c0,0
    orr r2,r2,r1
    mcr p15,0,r2,c1,c0,0

    bx lr


.globl PUT32
PUT32:
    str r1,[r0]
    bx lr

.globl GET32
GET32:
    ldr r0,[r0]
    bx lr

.globl ledOn
ledOn:
	push {lr}
        mov r0,#16
        mov r1,#1
        bl SetGpioFunction
        mov r0,#16
        mov r1,#0
        bl SetGpio
	pop {pc}

.globl hang
hang: b hang

