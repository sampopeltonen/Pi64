.pc = $0801 "Basic Upstart"
:BasicUpstart(start)

.pc =$0810 "Main"
start:
        lda #GREY 
        sta $d020   // border

	lda #BLACK
        sta $d021   // BG1

	lda #YELLOW
	sta $d022   // BG2
	
	lda #GREEN
	sta $d023   // BG3
	
	lda #$d8
	sta $d016   // enter multi color text mode


// title line clear and green
	ldx #0
	lda #%00000101
!loop:
	sta $d828,x
	inx
	cpx #$28
	bne !loop-


// word "commodore" to white and clear
	ldx #0
	lda #%00000001
!loop:
	sta $d831,x
	inx
	cpx #$9
	bne !loop-

// second line char selectable color change to red
	ldx #0
	lda #%00001010
!loop:
	sta $d878,x
	inx
	cpx #$28
	bne !loop-

// one char to clear
	lda #%00000010
	sta $d89d

        rts
