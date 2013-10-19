.pc = $0801 "Basic Upstart"
:BasicUpstart(start)

.pc =$0810 "Main"
start:
        lda #BLACK  // black
        sta $d020   // border and
        sta $d021   // screen
        lda #23     // small let-
        sta $d018   // ter mode

        sei
        lda #%01111111
        sta $dc0d  // CIA irqs off
        sta $dd0d

        lda $dc0d // negate pending irqs
        lda $dd0d

        lda #$01   // VIC raster
        sta $d01a
        
        lda #49
        sta $d012  // set raster line #

        lda #$1b
        sta $d011

        lda #$35
        sta $01   // mem config

        lda #<irqstart
        sta $fffe
        lda #>irqstart
        sta $ffff

        cli

        lda #1
        sta quit
!loop:
        lda quit
        bne !loop-
        rts
quit:
        .byte 0


irqstart:
	pha        
	txa
	pha        
	tya
	pha        
	
	lda #$ff   
	sta $d019  // clear interrupt condition
	           
	lda #YELLOW
	sta $d020
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	lda #BROWN
	sta $d020           
	           
	
	           
	           
	
	pla
	tay        
	pla
	tax        
	pla        
	
	rti        
	           
