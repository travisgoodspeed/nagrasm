#define	ST7 1

#if ST7
	cpu ST7		; selects ST7 CPU
	org $C000
#else
	cpu 6805	; selects 6805 CPU
	org $E000
#endif


; =-=-=-=-= ADC - ADd with Carry =-=-=-=-=
	adc #$A9
	adc $B9
	adc $C9C9
	adc $D9D9,x
	adc $E9,x
	adc ,x
#if ST7
	adc $D9D9,y
	adc $E9,y
	adc ,y
	adc [$D9.w],y
	adc [$E9],y
	adc [$B9]
	adc [$C9.w]
	adc [$D9.w],x
	adc [$E9],x
#endif
; =-=-=-=-= ADD - ADDition =-=-=-=-=
	add #$AB
	add $BB
	add $CBCB
	add $DBDB,x
	add $EB,x
	add ,x
#if ST7
	add $DBDB,y
	add $EB,y
	add ,y
	add [$DB.w],y
	add [$EB],y
	add [$BB]
	add [$CB.w]
	add [$DB.w],x
	add [$EB],x
#endif
; =-=-=-=-= AND - logical AND =-=-=-=-=
	and #$A4
	and $B4
	and $C4C4
	and $D4D4,x
	and $E4,x
	and ,x
#if ST7
	and $D4D4,y
	and $E4,y
	and ,y
	and [$D4.w],y
	and [$E4],y
	and [$B4]
	and [$C4.w]
	and [$D4.w],x
	and [$E4],x
#endif
; =-=-=-=-= ASL (same as LSL) =-=-=-=-=
	asl $BA
	asla
	aslx
#if ST7
	asly
#endif
	asl $EA,x
	asl ,x
#if ST7
	asl $EA,y
	asl ,y
	asl [$EA],y
	asl [$BA]
	asl [$EA],x
#endif
; =-=-=-=-= ASR =-=-=-=-=
	asr $37
	asra
	asrx
#if ST7
	asry
#endif
	asr $67,x
	asr ,x
#if ST7
	asr $67,y
	asr ,y
	asr [$67],y
	asr [$37]
	asr [$67],x
#endif
; =-=-=-=-= BCC (same as BHS) - Branch if Carry is Clear =-=-=-=-=
	bcc *+$26
#if ST7
	bcc [$24]
#endif
; =-=-=-=-= BCLR - Bit CLeaR =-=-=-=-=
	bclr 0,$11
	bclr0 $11
	bclr 1,$13
	bclr1 $13
	bclr 2,$15
	bclr2 $15
	bclr 3,$17
	bclr3 $17
	bclr 4,$19
	bclr4 $19
	bclr 5,$1B
	bclr5 $1B
	bclr 6,$1D
	bclr6 $1D
	bclr 7,$1F
	bclr7 $1F
#if ST7
	bclr 0,[$11]
	bclr0 [$11]
	bclr 1,[$13]
	bclr1 [$13]
	bclr 2,[$15]
	bclr2 [$15]
	bclr 3,[$17]
	bclr3 [$17]
	bclr 4,[$19]
	bclr4 [$19]
	bclr 5,[$1B]
	bclr5 [$1B]
	bclr 6,[$1D]
	bclr6 [$1D]
	bclr 7,[$1F]
	bclr7 [$1F]
#endif
; =-=-=-=-= BCS (same as BLO) - Branch if Carry is Set =-=-=-=-=
	bcs *+$27
#if ST7
	bcs [$25]
#endif
; =-=-=-=-= BEQ - Branch of EQual =-=-=-=-=
	beq *+$29
#if ST7
	beq [$27]
#endif
; =-=-=-=-= BHCC - Branch if Half Carry is Clear =-=-=-=-=
	bhcc *+$2A
#if ST7
	bhcc [$28]
#endif
; =-=-=-=-= BHCS - Branch if Half Carry is Set =-=-=-=-=
	bhcs *+$2B
#if ST7
	bhcs [$29]
#endif
; =-=-=-=-= BHI - Branch if HIgher =-=-=-=-=
	bhi *+$24
#if ST7
	bhi [$22]
#endif
; =-=-=-=-= BHS (same as BCC) - Branch if Higher or Same =-=-=-=-=
	bhs *+$26
#if ST7
	bhs [$24]
#endif
; =-=-=-=-= BIH =-=-=-=-=
	bih *+$31
#if ST7
	bih [$2F]
#endif
; =-=-=-=-= BIL =-=-=-=-=
	bil *+$30
#if ST7
	bil [$2E]
#endif
; =-=-=-=-= BIT - BIt Test and compare =-=-=-=-=
	bit #$A5
	bit $B5
	bit $C5C5
	bit $D5D5,x
	bit $E5,x
	bit ,x
#if ST7
	bit $D5D5,y
	bit $E5,y
	bit ,y
	bit [$D5.w],y
	bit [$E5],y
	bit [$B5]
	bit [$C5.w]
	bit [$D5.w],x
	bit [$E5],x
#endif
; =-=-=-=-= BLO (same as BCS) - Branch if LOwer =-=-=-=-=
	blo *+$27
#if ST7
	blo [$25]
#endif
; =-=-=-=-= BLS - Branch if Less or Same =-=-=-=-=
	bls *+$25
#if ST7
	bls [$23]
#endif
; =-=-=-=-= BMC =-=-=-=-=
	bmc *+$2E
#if ST7
	bmc [$2C]
#endif
; =-=-=-=-= BMI =-=-=-=-=
	bmi *+$2D
#if ST7
	bmi [$2B]
#endif
; =-=-=-=-= BMS =-=-=-=-=
	bms *+$2F
#if ST7
	bms [$2D]
#endif
; =-=-=-=-= BNE - Branch if Not Equal =-=-=-=-=
	bne *+$28
#if ST7
	bne [$26]
#endif
; =-=-=-=-= BPL - Branch if PLus =-=-=-=-=
	bpl *+$2C
#if ST7
	bpl [$2A]
#endif
; =-=-=-=-= BRA - BRAnch =-=-=-=-=
	bra *+$22
#if ST7
	bra [$20]
#endif
; =-=-=-=-= BRCLR - BRanch if bit is CLeaR =-=-=-=-=
	brclr 0,$01,*+$4
	brclr0 $01,*+$4
	brclr 1,$03,*+$6
	brclr1 $03,*+$6
	brclr 2,$05,*+$8
	brclr2 $05,*+$8
	brclr 3,$07,*+$A
	brclr3 $07,*+$A
	brclr 4,$09,*+$C
	brclr4 $09,*+$C
	brclr 5,$0B,*+$E
	brclr5 $0B,*+$E
	brclr 6,$0D,*+$10
	brclr6 $0D,*+$10
	brclr 7,$0F,*+$12
	brclr7 $0F,*+$12
#if ST7
	brclr 0,[$01],*+$5
	brclr0 [$01],*+$5
	brclr 1,[$03],*+$7
	brclr1 [$03],*+$7
	brclr 2,[$05],*+$9
	brclr2 [$05],*+$9
	brclr 3,[$07],*+$B
	brclr3 [$07],*+$B
	brclr 4,[$09],*+$D
	brclr4 [$09],*+$D
	brclr 5,[$0B],*+$F
	brclr5 [$0B],*+$F
	brclr 6,[$0D],*+$11
	brclr6 [$0D],*+$11
	brclr 7,[$0F],*+$13
	brclr7 [$0F],*+$13
#endif
; =-=-=-=-= BRN - Branch Never (basicly a 2 or 3 byte NOP) =-=-=-=-=
	brn *+$23
#if ST7
	brn [$21]
#endif
; =-=-=-=-= BRSET - BRanch if bit is SET =-=-=-=-=
	brset 0,$00,*+$3
	brset0 $00,*+$3
	brset 1,$02,*+$5
	brset1 $02,*+$5
	brset 2,$04,*+$7
	brset2 $04,*+$7
	brset 3,$06,*+$9
	brset3 $06,*+$9
	brset 4,$08,*+$B
	brset4 $08,*+$B
	brset 5,$0A,*+$D
	brset5 $0A,*+$D
	brset 6,$0C,*+$F
	brset6 $0C,*+$F
	brset 7,$0E,*+$11
	brset7 $0E,*+$11
#if ST7
	brset 0,[$00],*+$4
	brset0 [$00],*+$4
	brset 1,[$02],*+$6
	brset1 [$02],*+$6
	brset 2,[$04],*+$8
	brset2 [$04],*+$8
	brset 3,[$06],*+$A
	brset3 [$06],*+$A
	brset 4,[$08],*+$C
	brset4 [$08],*+$C
	brset 5,[$0A],*+$E
	brset5 [$0A],*+$E
	brset 6,[$0C],*+$10
	brset6 [$0C],*+$10
	brset 7,[$0E],*+$12
	brset7 [$0E],*+$12
#endif
; =-=-=-=-= BSET - Bit SET =-=-=-=-=
	bset 0,$10
	bset0 $10
	bset 1,$12
	bset1 $12
	bset 2,$14
	bset2 $14
	bset 3,$16
	bset3 $16
	bset 4,$18
	bset4 $18
	bset 5,$1A
	bset5 $1A
	bset 6,$1C
	bset6 $1C
	bset 7,$1E
	bset7 $1E
#if ST7
	bset 0,[$10]
	bset0 [$10]
	bset 1,[$12]
	bset1 [$12]
	bset 2,[$14]
	bset2 [$14]
	bset 3,[$16]
	bset3 [$16]
	bset 4,[$18]
	bset4 [$18]
	bset 5,[$1A]
	bset5 [$1A]
	bset 6,[$1C]
	bset6 [$1C]
	bset 7,[$1E]
	bset7 [$1E]
#endif
; =-=-=-=-= BSR - Branch to SubRoutine =-=-=-=-=
	bsr *-$51
#if ST7
	bsr [$AD]
#endif
; =-=-=-=-= CLC =-=-=-=-=
	clc
; =-=-=-=-= CLI =-=-=-=-=
	cli
; =-=-=-=-= CLR - CLeaR =-=-=-=-=
	clr $3F
	clra
	clrx
#if ST7
	clry
#endif
	clr $6F,x
	clr ,x
#if ST7
	clr $6F,y
	clr ,y
	clr [$6F],y
	clr [$3F]
	clr [$6F],x
#endif
; =-=-=-=-= CMP - CoMPare =-=-=-=-=
	cmp #$A1
	cmp $B1
	cmp $C1C1
	cmp $D1D1,x
	cmp $E1,x
	cmp ,x
#if ST7
	cmp $D1D1,y
	cmp $E1,y
	cmp ,y
	cmp [$D1.w],y
	cmp [$E1],y
	cmp [$B1]
	cmp [$C1.w]
	cmp [$D1.w],x
	cmp [$E1],x
#endif
; =-=-=-=-= CMPX - CoMPare with X =-=-=-=-=
	cmpx #$A3
	cmpx $B3
	cmpx $C3C3
	cmpx $D3D3,x
	cmpx $E3,x
	cmpx ,x
#if ST7
	cmpx [$B3]
	cmpx [$C3.w]
	cmpx [$D3.w],x
	cmpx [$E3],x
; =-=-=-=-= CMPY - CoMPare with Y =-=-=-=-=
	cmpy #$A3
	cmpy $B3
	cmpy $C3C3
	cmpy $D3D3,y
	cmpy $E3,y
	cmpy ,y
	cmpy [$B3]
	cmpy [$C3.w]
	cmpy [$D3.w],y
	cmpy [$E3],y
#endif
; =-=-=-=-= COM - COMpliment =-=-=-=-=
	com $3F
	coma
	comx
#if ST7
	comy
#endif
	com $6F,x
	com ,x
#if ST7
	com $6F,y
	com ,y
	com [$6F],y
	com [$3F]
	com [$6F],x
#endif
; =-=-=-=-= CPX - ComPare with X =-=-=-=-=
	cpx #$A3
	cpx $B3
	cpx $C3C3
	cpx $D3D3,x
	cpx $E3,x
	cpx ,x
#if ST7
	cpx [$B3]
	cpx [$C3.w]
	cpx [$D3.w],x
	cpx [$E3],x
; =-=-=-=-= CPY - ComPare with Y =-=-=-=-=
	cpy #$A3
	cpy $B3
	cpy $C3C3
	cpy $D3D3,y
	cpy $E3,y
	cpy ,y
	cpy [$B3]
	cpy [$C3.w]
	cpy [$D3.w],y
	cpy [$E3],y
#endif
; =-=-=-=-= DEC - DECriment =-=-=-=-=
	dec $3A
	deca
	decx
	dex
#if ST7
	decy
	dey
#endif
	dec $6A,x
	dec ,x
#if ST7
	dec $6A,y
	dec ,y
	dec [$6A],y
	dec [$3A]
	dec [$6A],x
#endif
; =-=-=-=-= EOR - Exclusive OR =-=-=-=-=
	eor #$A8
	eor $B8
	eor $C8C8
	eor $D8D8,x
	eor $E8,x
	eor ,x
#if ST7
	eor $D8D8,y
	eor $E8,y
	eor ,y
	eor [$D8.w],y
	eor [$E8],y
	eor [$B8]
	eor [$C8.w]
	eor [$D8.w],x
	eor [$E8],x
#endif
; =-=-=-=-= INC =-=-=-=-=
	inc $3C
	inca
	incx
	inx
#if ST7
	incy
	iny
#endif
	inc $6C,x
	inc ,x
#if ST7
	inc $6C,y
	inc ,y
	inc [$6C],y
	inc [$3C]
	inc [$6C],x
#endif
; =-=-=-=-= JMP - JuMP to address =-=-=-=-=
	jmp $BC
	jmp $CCCC
	jmp $DCDC,x
	jmp $EC,x
	jmp ,x
#if ST7
	jmp $DCDC,y
	jmp $EC,y
	jmp ,y
	jmp [$DC.w],y
	jmp [$EC],y
	jmp [$BC]
	jmp [$CC.w]
	jmp [$DC.w],x
	jmp [$EC],x
#endif
; =-=-=-=-= JSR - Jump to SubRoutine =-=-=-=-=
	jsr $BD
	jsr $CDCD
	jsr $DDDD,x
	jsr $ED,x
	jsr ,x
#if ST7
	jsr $DDDD,y
	jsr $ED,y
	jsr ,y
	jsr [$DD.w],y
	jsr [$ED],y
	jsr [$BD]
	jsr [$CD.w]
	jsr [$DD.w],x
	jsr [$ED],x
#endif
; =-=-=-=-= LDA - LoaD register A =-=-=-=-=
	lda #$A6
	lda $B6
	lda $C6C6
	lda $D6D6,x
	lda $E6,x
	lda ,x
#if ST7
	lda $D6D6,y
	lda $E6,y
	lda ,y
	lda [$D6.w],y
	lda [$E6],y
	lda [$B6]
	lda [$C6.w]
	lda [$D6.w],x
	lda [$E6],x
#endif
; =-=-=-=-= LDX - LoaD register X =-=-=-=-=
	ldx #$AE
	ldx $BE
	ldx $CECE
	ldx $DEDE,x
	ldx $EE,x
	ldx ,x
#if ST7
	ldx [$BE]
	ldx [$CE.w]
	ldx [$DE.w],x
	ldx [$EE],x
; =-=-=-=-= LDY - LoaD register Y =-=-=-=-=
	ldy #$AE
	ldy $BE
	ldy $CECE
	ldy $DEDE,y
	ldy $EE,y
	ldy ,y
	ldy [$BE]
	ldy [$CE.w]
	ldy [$DE.w],y
	ldy [$EE],y
#endif
; =-=-=-=-= LSL (same as ASL) =-=-=-=-=
	lsl $38
	lsra
	lsrx
#if ST7
	lsry
#endif
	lsl $68,x
	lsl ,x
#if ST7
	lsl $68,y
	lsl ,y
	lsl [$68],y
	lsl [$38]
	lsl [$68],x
#endif
; =-=-=-=-= LSR =-=-=-=-=
	lsr $34
	lsra
	lsrx
#if ST7
	lsry
#endif
	lsr $64,x
	lsr ,x
#if ST7
	lsr $64,y
	lsr ,y
	lsr [$64],y
	lsr [$34]
	lsr [$64],x
#endif
; =-=-=-=-= MUL =-=-=-=-=
	mul
#if ST7
	muly
#endif
; =-=-=-=-= NEG =-=-=-=-=
	neg $30
	nega
	negx
#if ST7
	negy
#endif
	neg $60,x
	neg ,x
#if ST7
	neg $60,y
	neg ,y
	neg [$60],y
	neg [$30]
	neg [$60],x
#endif
; =-=-=-=-= NOP =-=-=-=-=
	nop
; =-=-=-=-= ORA - OR with register A =-=-=-=-=
	ora #$AA
	ora $BA
	ora $CACA
	ora $DADA,x
	ora $EA,x
	ora ,x
#if ST7
	ora $DADA,y
	ora $EA,y
	ora ,y
	ora [$DA.w],y
	ora [$EA],y
	ora [$BA]
	ora [$CA.w]
	ora [$DA.w],x
	ora [$EA],x
; =-=-=-=-= POP =-=-=-=-=
	pop a
	popa
	pop x
	popx
	pop y
	popy
	pop cc
	popcc
; =-=-=-=-= PUSH =-=-=-=-=
	push a
	pusha
	push x
	pushx
	push y
	pushy
	push cc
	pushcc
#endif
; =-=-=-=-= ROL =-=-=-=-=
	rol $39
	rola
	rolx
#if ST7
	roly
#endif
	rol $69,x
	rol ,x
#if ST7
	rol $69,y
	rol ,y
	rol [$69],y
	rol [$39]
	rol [$69],x
#endif
; =-=-=-=-= ROR =-=-=-=-=
	ror $36
	rora
	rorx
#if ST7
	rory
#endif
	ror $66,x
	ror ,x
#if ST7
	ror $66,y
	ror ,y
	ror [$66],y
	ror [$36]
	ror [$66],x
#endif
; =-=-=-=-= RSP - Reset Stack Pointer =-=-=-=-=
	rsp
; =-=-=-=-= RTI - ReTurn from Interrupt =-=-=-=-=
	rti
; =-=-=-=-= RTS - ReTurn from Subroutine =-=-=-=-=
	rts
; =-=-=-=-= SBC - SuBtract with Carry =-=-=-=-=
	sbc #$A2
	sbc $B2
	sbc $C2C2
	sbc $D2D2,x
	sbc $E2,x
	sbc ,x
#if ST7
	sbc $D2D2,y
	sbc $E2,y
	sbc ,y
	sbc [$D2.w],y
	sbc [$E2],y
	sbc [$B2]
	sbc [$C2.w]
	sbc [$D2.w],x
	sbc [$E2],x
#endif
; =-=-=-=-= SEC =-=-=-=-=
	sec
; =-=-=-=-= SEI =-=-=-=-=
	sei
; =-=-=-=-= STA - STore register A =-=-=-=-=
	sta $B7
	sta $C7C7
	sta $D7D7,x
	sta $E7,x
	sta ,x
#if ST7
	sta $D7D7,y
	sta $E7,y
	sta ,y
	sta [$D7.w],y
	sta [$E7],y
	sta [$B7]
	sta [$C7.w]
	sta [$D7.w],x
	sta [$E7],x
#endif
; =-=-=-=-= STOP =-=-=-=-=
	stop
; =-=-=-=-= STX - STore register X =-=-=-=-=
	stx $BF
	stx $CFCF
	stx $DFDF,x
	stx $EF,x
	stx ,x
#if ST7
	stx [$BF]
	stx [$CF.w]
	stx [$DF.w],x
	stx [$EF],x
; =-=-=-=-= LDY - LoaD register Y =-=-=-=-=
	sty $BF
	sty $CFCF
	sty $DFDF,y
	sty $EF,y
	sty ,y
	sty [$BF]
	sty [$CF.w]
	sty [$DF.w],y
	sty [$EF],y
#endif
; =-=-=-=-= SUB - SUBtract =-=-=-=-=
	sub #$A0
	sub $B0
	sub $C0C0
	sub $D0D0,x
	sub $E0,x
	sub ,x
#if ST7
	sub $D0D0,y
	sub $E0,y
	sub ,y
	sub [$D0.w],y
	sub [$E0],y
	sub [$B0]
	sub [$C0.w]
	sub [$D0.w],x
	sub [$E0],x
; =-=-=-=-= SWAP - SWAP nibbles in a byte =-=-=-=-=
	swap $3E
	swapa
	swapx
	swapy
	swap $6E,x
	swap ,x
	swap $6E,y
	swap ,y
	swap [$6E],y
	swap [$3E]
	swap [$6E],x
#endif
; =-=-=-=-= SWI - SoftWare Interrupt =-=-=-=-=
	swi
; =-=-=-=-= TAS - Transfer A to S =-=-=-=-=
#if ST7
	tas
#endif
; =-=-=-=-= TAX - Transfer A to X =-=-=-=-=
	tax
; =-=-=-=-= TAY - Transfer A to Y =-=-=-=-=
#if ST7
	tay
#endif
; =-=-=-=-= TFR - TransFeR register to register =-=-=-=-=
#if ST7
	tfr a,s
#endif
	tfr a,x
#if ST7
	tfr a,y
#endif
	tfr s,a
#if ST7
	tfr s,x
	tfr s,y
#endif
	tfr x,a
#if ST7
	tfr x,s
	tfr x,y
	tfr y,a
	tfr y,s
	tfr y,x
#endif
; =-=-=-=-= TSA - Transfer S to A =-=-=-=-=
	tsa
; =-=-=-=-= TST - TeST =-=-=-=-=
	tst $3E
	tsta
	tstx
#if ST7
	tsty
#endif
	tst $6E,x
	tst ,x
#if ST7
	tst $6E,y
	tst ,y
	tst [$6E],y
	tst [$3E]
	tst [$6E],x
; =-=-=-=-= TSX - Transfer S to X =-=-=-=-=
	tsx
; =-=-=-=-= TSY - Transfer S to Y =-=-=-=-=
	tsy
#endif
; =-=-=-=-= TXA - Transfer X to A =-=-=-=-=
	txa
; =-=-=-=-= TXS - Transfer X to S =-=-=-=-=
#if ST7
	txs
; =-=-=-=-= TXY - Transfer X to Y =-=-=-=-=
	txy
; =-=-=-=-= TYA - Transfer Y to A =-=-=-=-=
	tya
; =-=-=-=-= TYS - Transfer Y to S =-=-=-=-=
	tys
; =-=-=-=-= TYX - Transfer Y to X =-=-=-=-=
	tyx
#endif
; =-=-=-=-= WAIT =-=-=-=-=
	wait
