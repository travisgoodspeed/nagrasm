# define ST7 1

# if ST7
	cpu ST7
	.org $c000
# else
	cpu 6805
	.org $e000
# endif

;********** Single byte opcodes **********
;=-= Opcodes 0x =-=-=-=-=-=-=-=-=
	brset0 $00,*+3
	brclr0 $01,*+4
	brset1 $02,*+5
	brclr1 $03,*+6
	brset2 $04,*+7
	brclr2 $05,*+8
	brset3 $06,*+9
	brclr3 $07,*+10
	brset4 $08,*+11
	brclr4 $09,*+12
	brset5 $0A,*+13
	brclr5 $0B,*+14
	brset6 $0C,*+15
	brclr6 $0D,*+16
	brset7 $0E,*+17
	brclr7 $0F,*+18
;=-= Opcodes 0x =-=-=-=-=-=-=-=-=
	brset 0,$00,*+3
	brclr 0,$01,*+4
	brset 1,$02,*+5
	brclr 1,$03,*+6
	brset 2,$04,*+7
	brclr 2,$05,*+8
	brset 3,$06,*+9
	brclr 3,$07,*+10
	brset 4,$08,*+11
	brclr 4,$09,*+12
	brset 5,$0A,*+13
	brclr 5,$0B,*+14
	brset 6,$0C,*+15
	brclr 6,$0D,*+16
	brset 7,$0E,*+17
	brclr 7,$0F,*+18
;=-= Opcodes 1x =-=-=-=-=-=-=-=-=
	bset0 $10
	bclr0 $11
	bset1 $12
	bclr1 $13
	bset2 $14
	bclr2 $15
	bset3 $16
	bclr3 $17
	bset4 $18
	bclr4 $19
	bset5 $1A
	bclr5 $1B
	bset6 $1C
	bclr6 $1D
	bset7 $1E
	bclr7 $1F
;=-= Opcodes 1x =-=-=-=-=-=-=-=-=
	bset 0,$10
	bclr 0,$11
	bset 1,$12
	bclr 1,$13
	bset 2,$14
	bclr 2,$15
	bset 3,$16
	bclr 3,$17
	bset 4,$18
	bclr 4,$19
	bset 5,$1A
	bclr 5,$1B
	bset 6,$1C
	bclr 6,$1D
	bset 7,$1E
	bclr 7,$1F
;=-= Opcodes 2x =-=-=-=-=-=-=-=-=
label3:	bra *+$22
	brn *+$23
	bhi *+$24
	bls *+$25
	bcc *+$26
	bcs *+$27 ; same as blo
	blo *+$27 ; same as bcs
	bne *+$28
	beq *+$29
	bhcc *+$2A
	bhcs *+$2B
	bpl *+$2C
	bmi *+$2D
	bmc *+$2E
	bms *+$2F
	bil *+$30
	bih *+$31
;=-= Opcodes 3x =-=-=-=-=-=-=-=-=
	neg $30
	com $33
	lsr $34
	ror $36
	asr $37
	asl $38 ; same as lsl
	lsl $38 ; same as asl
	rol $39
	dec $3A
	inc $3C
	tst $3D
#if ST7
	swap $3E
#endif
	clr $3F
;=-= Opcodes 4x =-=-=-=-=-=-=-=-=
	nega
	coma
	mul
	lsra
	rora
	asra
	asla ; same as lsla
	lsla ; same as asla
	rola
	deca
	inca
	tsta
#if ST7
	swapa
#endif
	clra
;=-= Opcodes 5x =-=-=-=-=-=-=-=-=
	negx
	comx
	lsrx
	rorx
	asrx
	aslx ; same as lslx
	lslx ; same as aslx
	rolx
	decx
	incx
	tstx
#if ST7
	swapx
#endif
	clrx
;=-= Opcodes 6x =-=-=-=-=-=-=-=-=
	neg $60,x
	com $63,x
	lsr $64,x
	ror $66,x
	asr $67,x
	asl $68,x ; same as lsl
	lsl $68,x ; same as asl
	rol $69,x
	dec $6A,x
	inc $6C,x
	tst $6D,x
#if ST7
	swap $6E,x
#endif
	clr $6F,x
;=-= Opcodes 7x =-=-=-=-=-=-=-=-=
	neg ,x
	com ,x
	lsr ,x
	ror ,x
	asr ,x
	asl ,x ; same as lsl
	lsl ,x ; same as asl
	rol ,x
	dec ,x
	inc ,x
	tst ,x
#if ST7
	swap ,x
#endif
	clr ,x
;=-= Opcodes 8x =-=-=-=-=-=-=-=-=
	rti
	rts
	swi
#if ST7
	popa
	pop a
	popx
	pop x
	popcc
	pop cc
	pusha
	push a
	pushx
	push x
	pushcc
	push cc
#endif
	stop
	wait
;=-= Opcodes 9x =-=-=-=-=-=-=-=-=
#if ST7
	tyx
	tfr y,x
	txs
	tfr x,s
	tas
	tfr a,s
	tsx
	tfr s,x
#endif
	tax
	tfr a,x
	clc
	sec
	cli
	sei
	rsp
	nop
	tsa
	tfr s,a
	txa
	tfr x,a
;=-= Opcodes Ax =-=-=-=-=-=-=-=-=
label4:	sub #$A0
	cmp #$A1
	sbc #$A2
	cpx #$A3
	and #$A4
	bit #$A5
	lda #$A6
	eor #$A8
	adc #$A9
	ora #$AA
	add #$AB
	bsr label4
	ldx #$AE
;=-= Opcodes Bx =-=-=-=-=-=-=-=-=
	sub $B0
	cmp $B1
	sbc $B2
	cpx $B3
	and $B4
	bit $B5
	lda $B6
	sta $B7
	eor $B8
	adc $B9
	ora $BA
	add $BB
	jmp $BC
	jsr $BD
	ldx $BE
	stx $BF
;=-= Opcodes Cx =-=-=-=-=-=-=-=-=
	sub $C0C0
	cmp $C1C1
	sbc $C2C2
	cpx $C3C3
	and $C4C4
	bit $C5C5
	lda $C6C6
	sta $C7C7
	eor $C8C8
	adc $C9C9
	ora $CACA
	add $CBCB
	jmp $CCCC
	jsr $CDCD
	ldx $CECE
	stx $CFCF
;=-= Opcodes Dx =-=-=-=-=-=-=-=-=
	sub $D0D0,x
	cmp $D1D1,x
	sbc $D2D2,x
	cpx $D3D3,x
	and $D4D4,x
	bit $D5D5,x
	lda $D6D6,x
	sta $D7D7,x
	eor $D8D8,x
	adc $D9D9,x
	ora $DADA,x
	add $DBDB,x
	jmp $DCDC,x
	jsr $DDDD,x
	ldx $DEDE,x
	stx $DFDF,x
;=-= Opcodes Ex =-=-=-=-=-=-=-=-=
	sub $E0,x
	cmp $E1,x
	sbc $E2,x
	cpx $E3,x
	and $E4,x
	bit $E5,x
	lda $E6,x
	sta $E7,x
	eor $E8,x
	adc $E9,x
	ora $EA,x
	add $EB,x
	jmp $EC,x
	jsr $ED,x
	ldx $EE,x
	stx $EF,x
;=-= Opcodes Fx =-=-=-=-=-=-=-=-=
	sub ,x
	cmp ,x
	sbc ,x
	cpx ,x
	and ,x
	bit ,x
	lda ,x
	sta ,x
	eor ,x
	adc ,x
	ora ,x
	add ,x
	jmp ,x
	jsr ,x
	ldx ,x
	stx ,x
;********** Double byte opcodes 0x90 **********
#if ST7
;=-= Opcodes 904x =-=-=-=-=-=-=-=
	muly
;=-= Opcodes 905x =-=-=-=-=-=-=-=
	negy
	comy
	lsry
	rory
	asry
	asly ; same as lsly
	lsly ; same as asly
	roly
	decy
	incy
	tsty
	swapy
	clry
;=-= Opcodes 906x =-=-=-=-=-=-=-=
	neg $60,y
	com $63,y
	lsr $64,y
	ror $66,y
	asr $67,y
	asl $68,y ; same as lsl
	lsl $68,y ; same as asl
	rol $69,y
	dec $6A,y
	inc $6C,y
	tst $6D,y
	swap $6E,y
	clr $6F,y
;=-= Opcodes 907x =-=-=-=-=-=-=-=
	neg ,y
	com ,y
	lsr ,y
	ror ,y
	asr ,y
	asl ,y ; same as lsl
	lsl ,y ; same as asl
	rol ,y
	dec ,y
	inc ,y
	tst ,y
	swap ,y
	clr ,y
;=-= Opcodes 908x =-=-=-=-=-=-=-=
	popy
	pop y
	pushy
	push y
;=-= Opcodes 909x =-=-=-=-=-=-=-=
	txy
	tfr x,y
	tys
	tfr y,s
	tsy
	tfr s,y
	tay
	tfr a,y
	tya
	tfr y,a
;=-= Opcodes 90Ax =-=-=-=-=-=-=-=
	cpy #$A3
	ldy #$AE
;=-= Opcodes 90Bx =-=-=-=-=-=-=-=
	cpy $B3
	ldy $BE
	sty $BF
;=-= Opcodes 90Cx =-=-=-=-=-=-=-=
	cpy $C3C3
	ldy $CECE
	sty $CFCF
;=-= Opcodes 90Dx =-=-=-=-=-=-=-=
	sub $D0D0,y
	cmp $D1D1,y
	sbc $D2D2,y
	cpy $D3D3,y
	and $D4D4,y
	bit $D5D5,y
	lda $D6D6,y
	sta $D7D7,y
	eor $D8D8,y
	adc $D9D9,y
	ora $DADA,y
	add $DBDB,y
	jmp $DCDC,y
	jsr $DDDD,y
	ldy $DEDE,y
	sty $DFDF,y
;=-= Opcodes 90Ex =-=-=-=-=-=-=-=
	sub $E0,y
	cmp $E1,y
	sbc $E2,y
	cpy $E3,y
	and $E4,y
	bit $E5,y
	lda $E6,y
	sta $E7,y
	eor $E8,y
	adc $E9,y
	ora $EA,y
	add $EB,y
	jmp $EC,y
	jsr $ED,y
	ldy $EE,y
	sty $EF,y
;=-= Opcodes 90Fx =-=-=-=-=-=-=-=
	sub ,y
	cmp ,y
	sbc ,y
	cpy ,y
	and ,y
	bit ,y
	lda ,y
	sta ,y
	eor ,y
	adc ,y
	ora ,y
	add ,y
	jmp ,y
	jsr ,y
	ldy ,y
	sty ,y
;********** Double byte opcodes 0x91 **********
;=-= Opcodes 916x =-=-=-=-=-=-=-=
	neg [$60],y
	com [$63],y
	lsr [$64],y
	ror [$66],y
	asr [$67],y
	asl [$68],y ; same as lsl
	lsl [$68],y ; same as asl
	rol [$69],y
	dec [$6A],y
	inc [$6C],y
	tst [$6D],y
	swap [$6E],y
	clr [$6F],y
;=-= Opcodes 91Bx =-=-=-=-=-=-=-=
	cpy [$B3]
	ldy [$BE]
	sty [$BF]
;=-= Opcodes 91Cx =-=-=-=-=-=-=-=
	cpy [$C3.w]
	ldy [$CE.w]
	sty [$CF.w]
;=-= Opcodes 91Dx =-=-=-=-=-=-=-=
	sub [$D0.w],y
	cmp [$D1.w],y
	sbc [$D2.w],y
	cpy [$D3.w],y
	and [$D4.w],y
	bit [$D5.w],y
	lda [$D6.w],y
	sta [$D7.w],y
	eor [$D8.w],y
	adc [$D9.w],y
	ora [$DA.w],y
	add [$DB.w],y
	jmp [$DC.w],y
	jsr [$DD.w],y
	ldy [$DE.w],y
	sty [$DF.w],y
;=-= Opcodes 91Ex =-=-=-=-=-=-=-=
	sub [$E0],y
	cmp [$E1],y
	sbc [$E2],y
	cpy [$E3],y
	and [$E4],y
	bit [$E5],y
	lda [$E6],y
	sta [$E7],y
	eor [$E8],y
	adc [$E9],y
	ora [$EA],y
	add [$EB],y
	jmp [$EC],y
	jsr [$ED],y
	ldy [$EE],y
	sty [$EF],y
;********** Double byte opcodes 0x92 **********
;=-= Opcodes 920x =-=-=-=-=-=-=-=
	brset0 [$00],*+4
	brclr0 [$01],*+5
	brset1 [$02],*+6
	brclr1 [$03],*+7
	brset2 [$04],*+8
	brclr2 [$05],*+9
	brset3 [$06],*+10
	brclr3 [$07],*+11
	brset4 [$08],*+12
	brclr4 [$09],*+13
	brset5 [$0A],*+14
	brclr5 [$0B],*+15
	brset6 [$0C],*+16
	brclr6 [$0D],*+17
	brset7 [$0E],*+18
	brclr7 [$0F],*+19
;=-= Opcodes 920x =-=-=-=-=-=-=-=
	brset 0,[$00],*+4
	brclr 0,[$01],*+5
	brset 1,[$02],*+6
	brclr 1,[$03],*+7
	brset 2,[$04],*+8
	brclr 2,[$05],*+9
	brset 3,[$06],*+10
	brclr 3,[$07],*+11
	brset 4,[$08],*+12
	brclr 4,[$09],*+13
	brset 5,[$0A],*+14
	brclr 5,[$0B],*+15
	brset 6,[$0C],*+16
	brclr 6,[$0D],*+17
	brset 7,[$0E],*+18
	brclr 7,[$0F],*+19
;=-= Opcodes 921x =-=-=-=-=-=-=-=
	bset0 [$10]
	bclr0 [$11]
	bset1 [$12]
	bclr1 [$13]
	bset2 [$14]
	bclr2 [$15]
	bset3 [$16]
	bclr3 [$17]
	bset4 [$18]
	bclr4 [$19]
	bset5 [$1A]
	bclr5 [$1B]
	bset6 [$1C]
	bclr6 [$1D]
	bset7 [$1E]
	bclr7 [$1F]
;=-= Opcodes 921x =-=-=-=-=-=-=-=
	bset 0,[$10]
	bclr 0,[$11]
	bset 1,[$12]
	bclr 1,[$13]
	bset 2,[$14]
	bclr 2,[$15]
	bset 3,[$16]
	bclr 3,[$17]
	bset 4,[$18]
	bclr 4,[$19]
	bset 5,[$1A]
	bclr 5,[$1B]
	bset 6,[$1C]
	bclr 6,[$1D]
	bset 7,[$1E]
	bclr 7,[$1F]
;=-= Opcodes 922x =-=-=-=-=-=-=-=
	bra [$20]
	brn [$21]
	bhi [$22]
	bls [$23]
	bcc [$24]
	bcs [$25] ; same as blo
	blo [$25] ; same as bcs
	bne [$26]
	beq [$27]
	bhcc [$28]
	bhcs [$29]
	bpl [$2A]
	bmi [$2B]
	bmc [$2C]
	bms [$2D]
	bil [$2E]
	bih [$2F]
;=-= Opcodes 923x =-=-=-=-=-=-=-=
	neg [$30]
	com [$33]
	lsr [$34]
	ror [$36]
	asr [$37]
	asl [$38] ; same as lsl
	lsl [$38] ; same as asl
	rol [$39]
	dec [$3A]
	inc [$3C]
	tst [$3D]
	swap [$3F]
	clr [$3F]
;=-= Opcodes 926x =-=-=-=-=-=-=-=
	neg [$60],x
	com [$63],x
	lsr [$64],x
	ror [$66],x
	asr [$67],x
	asl [$68],x ; same as lsl
	lsl [$68],x ; same as asl
	rol [$69],x
	dec [$6A],x
	inc [$6C],x
	tst [$6D],x
	swap [$6E],x
	clr [$6F],x
;=-= Opcodes 92Ax =-=-=-=-=-=-=-=
	bsr [$AD]
;=-= Opcodes 92Bx =-=-=-=-=-=-=-=
	sub [$B0]
	cmp [$B1]
	sbc [$B2]
	cpx [$B3]
	and [$B4]
	bit [$B5]
	lda [$B6]
	sta [$B7]
	eor [$B8]
	adc [$B9]
	ora [$BA]
	add [$BB]
	jmp [$BC]
	jsr [$BD]
	ldx [$BE]
	stx [$BF]
;=-= Opcodes 92Cx =-=-=-=-=-=-=-=
	sub [$C0.w]
	cmp [$C1.w]
	sbc [$C2.w]
	cpx [$C3.w]
	and [$C4.w]
	bit [$C5.w]
	lda [$C6.w]
	sta [$C7.w]
	eor [$C8.w]
	adc [$C9.w]
	ora [$CA.w]
	add [$CB.w]
	jmp [$CC.w]
	jsr [$CD.w]
	ldx [$CE.w]
	stx [$CF.w]
;=-= Opcodes 92Dx =-=-=-=-=-=-=-=
	sub [$D0.w],x
	cmp [$D1.w],x
	sbc [$D2.w],x
	cpx [$D3.w],x
	and [$D4.w],x
	bit [$D5.w],x
	lda [$D6.w],x
	sta [$D7.w],x
	eor [$D8.w],x
	adc [$D9.w],x
	ora [$DA.w],x
	add [$DB.w],x
	jmp [$DC.w],x
	jsr [$DD.w],x
	ldx [$DE.w],x
	stx [$DF.w],x
;=-= Opcodes 92Ex =-=-=-=-=-=-=-=
	sub [$E0],x
	cmp [$E1],x
	sbc [$E2],x
	cpx [$E3],x
	and [$E4],x
	bit [$E5],x
	lda [$E6],x
	sta [$E7],x
	eor [$E8],x
	adc [$E9],x
	ora [$EA],x
	add [$EB],x
	jmp [$EC],x
	jsr [$ED],x
	ldx [$EE],x
	stx [$EF],x
#endif
