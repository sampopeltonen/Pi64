/*
* cathoderay.s
* by Sampo Peltonen
*/
.section .data
.align 1
.globl VICforeColour
VICforeColour:
        .hword 0x0

.globl VICbgColour
VICbgColour:
        .hword 0x0

.section .text

/* 
* Draws VIC octet (8 pixels) to current beam position
* using foreColour and bgColour and advances beam position.
*/
.globl drawPixelOctet
drawPixelOctet:
        push {r4,r5,lr}
        px .req r0
        py .req r1
        octet .req r2

        addr .req r4
        ldr addr,=graphicsAddress
        ldr addr,[addr]

        width .req r3
        ldr width,[addr,#0]
        ldr addr,[addr,#32]
        mla px,py,width,px      ;@ px = py * width + px
        .unreq width
        .unreq py
        add addr, px,lsl #1     ;@ addr = addr + px*2  I guess...
        .unreq px

        forecolor .req r3
        bit .req r0
        mask .req r1
	bgcolor .req r5
	
	ldr forecolor,=VICforeColour
	ldrh forecolor,[forecolor]
	ldr bgcolor,=VICbgColour
	ldrh bgcolor,[bgcolor]
        mov mask,#0b10000000
octloop$:
        tst octet,mask
        streqh bgcolor,[addr]
	strneh forecolor,[addr]
        add addr, #2
        mov mask, mask, lsr #1
        cmp mask,#0b00000000
        bne octloop$

        .unreq forecolor
	.unreq bgcolor
        .unreq addr
        .unreq octet
        .unreq bit
        pop {r4,r5,pc}

.globl drawForeColourOctet
drawForeColourOctet:
        push {r4,lr}
        px .req r0
        py .req r1

        addr .req r4
        ldr addr,=graphicsAddress
        ldr addr,[addr]

        width .req r3
        ldr width,[addr,#0]
        ldr addr,[addr,#32]
        mla px,py,width,px      ;@ px = py * width + px
        .unreq width
        .unreq py
        add addr, px,lsl #1     ;@ addr = addr + px*2  I guess...
        .unreq px

        forecolor .req r3
        bit .req r0
        mask .req r1
	
	ldr forecolor,=VICforeColour
	ldrh forecolor,[forecolor]
        mov mask,#0b10000000
octloop2$:
	strh forecolor,[addr]
        add addr, #2
        mov mask, mask, lsr #1
        cmp mask,#0b00000000
        bne octloop2$

        .unreq forecolor
        .unreq addr
        .unreq bit
        pop {r4,pc}
