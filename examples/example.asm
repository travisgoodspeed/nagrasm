; here is a file, with all the pseudo ops that
; the assembler understands, and examples of
; how to use them, in addition to examples of
; the extended addressing modes the ST7 can handle.
; Some pseudo ops have been disabled and should be
; enabled if you want to see them in action. If you
; perfer to use a period as the first character for
; a pseudo op, then you can do that.

; *** the OPT pseudo op to set options ***
; syntax:
; [<label>] OPT <string> [<comments>] - sets assembler specific options
; [<label>] OPT l        [<comments>] - enables .lst file
; [<label>] OPT nol      [<comments>] - disables .lst file
; [<label>] OPT c        [<comments>] - enables cycle counting
; [<label>] OPT noc      [<comments>] - disables cycle counting
; [<label>] OPT s        [<comments>] - print symbol table
; [<label>] OPT cre      [<comments>] - print cross reference table
; [<label>] OPT rom2     [<comments>] - selects 6805 CPU and .ne2 file format
; [<label>] OPT rom3     [<comments>] - selects 6805 CPU and .ep3 file format
; [<label>] OPT rom10    [<comments>] - selects ST7 CPU and .ep10 file format
; [<label>] OPT rom11    [<comments>] - selects ST7 CPU and .ep11 file format
; [<label>] OPT hex      [<comments>] - selects intel .hex file format
; [<label>] OPT s19      [<comments>] - selects motorola .s19 file format
; [<label>] OPT 4k       [<comments>] - selects 4k binary file
; [<label>] OPT 8k       [<comments>] - selects 8k binary file
; [<label>] OPT 64k      [<comments>] - selects 64k binary file

; Some lines are commented out, remove the comments to see how they work
	opt l		; enables .lst file
; everything in between will be output to the .lst file
; the "l" and "nol" options override the "-l" and "-nol" command line options
;	opt nol		; disables .lst file
	opt c		; enable cycle counting in the .lst file
;	opt noc		; disable cycle counting in the .lst file
	opt s		; print symbol table
	opt cre		; print cross reference table
; options that select the CPU and output file format
	opt rom3	; selects 6805 CPU, and .ep3 output file
	opt rom10	; selects ST7 CPU, and .ep10 output file
	opt rom11	; selects ST7 CPU, and .ep11 output file
; options that select the output file format
; the first output format selected is the one that is used
	opt hex		; selects intel .hex output file
	opt s19		; selects motorola .s19 output file
	opt 4k		; selects 4k binary output file (E000-EFFF)
	opt 8k		; selects 8k binary output file (C000-DFFF)
	opt 64k		; selects 64k binary output file (0000-FFFF)

; *** pseudo ops that set the CPU type (also see OPT above) ***
; syntax:
; [<label>] CARD    <string>     [<comments>] - Select CPU type
; [<label>] DEVICE  <string>     [<comments>] - Select CPU type
; [<label>] MACH    <string>     [<comments>] - Select CPU type
; [<label>] MACHINE <string>     [<comments>] - Select CPU type
; [<label>] ROM     <expression> [<comments>] - same as ROM<expression> below
; [<label>] ROM2                 [<comments>] - Selects 6805 CPU, and .ne2 output file
; [<label>] ROM3                 [<comments>] - Selects 6805 CPU, and .ep3 output file
; [<label>] ROM10                [<comments>] - Selects ST7 CPU, and .ep10 output file
; [<label>] ROM11                [<comments>] - Selects ST7 CPU, and .ep11 output file
	CARD ST16CFxx	; selects 6805 CPU
	DEVICE 6805	; select 6805 CPU
	MACH ST19CFxx	; select ST7 CPU
	MACHINE ROM11	; select ST7 CPU, and .ep11 output file
	ROM 3		; select 6805 CPU, and .ep3 output file
	ROM10		; select ST7 CPU, and .ep11 output file

; *** pseudo ops that change the address ***
; syntax:
; [<label>] ORG <expression> [<comments>] - Orgin, new address for PC
; [<label>] RMB <expression> [<comments>] - Reserve Memory Bytes
; [<label>] DS  <expression> [<comments>] - Define Storage
labelorg:	org $E000 ; new address
labelrmb:	rmb $40	  ; Reserve Memory Bytes - skips $40 bytes
labelds:	ds $40    ; Define Storage - skips $40 bytes

; *** pseudo ops that output bytes, words, and chars  ***
; syntax:
; [<label>] BSZ  <expression> [<comments>] - Block Storage of Zeroes
; [<label>] ZMB  <expression> [<comments>] - Zero Memory Bytes
; [<label>] FILL <expression>,<count> [<comments>] - Fill memory with count bytes of expression
; [<label>] FILL <expression> <count> [<comments>] - Fill memory with count bytes of expression
; [<label>] FCB  <expression> [<comments>] - Form Constant Bytes
; [<label>] DB   <expression> [<comments>] - Define Byte
; [<label>] FDB  <expression> [<comments>] - Form Double Byte
; [<label>] DW   <expression> [<comments>] - Define Word
; [<label>] FCC  <string> [<comments>] - Form Constant Characters
; [<label>] FCS  <string> [<comments>] - Form Constant String (High bit set on last char)
labelbsz:	bsz 6 ; Block Storage of Zeroes
labelzmb:	zmb 6 ; Zero Memory Bytes
labelfill:	fill $9d,6 ; FILL memory with byte
labelfill1:	fill $9d, 6 ; FILL memory with byte
labelfill2:	fill $9d 6 ; FILL memory with byte
labelfcb:	fcb $01,$23,$45 ; Form Constant Bytes
labeldb:	db $67,$89,$AB ; Define Bytes
labelfdb:	fdb $0123,$4567 ; Form Double Bytes
labeldw:	dw $89AB,$CDEF ; Define Words
labelfcc:	fcc "abcd" ; Form Constant Characters
labelfcs:	fcs "efgh" ; Form Constant String, high bit set on last character

; *** pseudo ops that define variables ***
; syntax:
; <label>   EQU     <expression> [<comments>] - Equals, or equate
; <label>   SET      <expression> [<comments>] - Define or redefine variable
; [<label>] DEFINE  <variable> <expression> [<comments>] - Define or redefine variable
; #DEFINE  <variable> <expression> [<comments>] - Define or redefine variable
; [<label>] UNDEF   <variable>     [<comments>] - Undefine a variable
; #UNDEF   <variable>              [<comments>] - Undefine a variable
labelequ:	equ $0123 ; equals, or equate
labelset	set $4567 ; set value
labeldefine:	define labeldef $4567 ; makes "labeldef" equal to $4567
		lda labelequ
		lda labeldef
		lda labeldefine
#define alternate $89AB ; C PreProcessor style
#define alternate $CDEF ; redefine the same value
alternate	set $0123 ; example of redefining the same value
		lda alternate
#undef test1 ; undefine test1, if you use it in an expression it will give you an error

; *** pseudo ops that print data ***
; syntax:
; [<label>] EPX     <string> [<comments>] - Prints line to .epx file
; [<label>] PRINT   <string> [<comments>] - print line to screen on pass 1 and pass 2
; [<label>] PRINT1  <string> [<comments>] - print line to screen on pass 1
; [<label>] PRINT2  <string> [<comments>] - print line to screen on pass 2
labelepx:	epx "; output this string to an .epx file" ; only works if you select ROMx cpu type
labelprint:	print "this message will print on both pass 1 and pass 2" ; this is a comment
labelprint1:	print1 "this will print on pass 1" ; this is another comment
labelprint2:	print1 "this will print on pass 2" ; and this is yet another comment

; *** pseudo ops that do nothing ***
; syntax:
; [<label>] NAM  [<comments>] - nothing
; [<label>] NAME [<comments>] - nothing
; [<label>] TTL  [<comments>] - nothing
; [<label>] SPC  [<comments>] - nothing
labelend:	end	; end processing
		nam	; name
		name	; same as above
		ttl	; title?
		spc	; no idea

; **** warnings and errors ***
; syntax:
; [<label>] WARN   <string> - Generate warning on pass 1 and pass 2
; [<label>] WARN1  <string> - Generate warning on pass 1
; [<label>] WARN2  <string> - Generate warning on pass 2
; [<label>] ERROR  <string> - Generate error on pass 1 and pass 2
; [<label>] ERROR1 <string> - Generate error on pass 2
; [<label>] ERROR2 <string> - Generate error on pass 2
; [<label>] FATAL  <string> [<comments>] - generate fatal error
; the following are disabled, delete the ";" in
; first column if you want to see them in action
; no comments allowed on the following pseudo ops, except "fatal"
;labelwarn:	warn this will produce 2 warnings
;labelwarn1:	warn1 This is a warning on pass 1
;labelwarn2:	warn2 This is a warning on pass 2
;labelerror:	error this will produce 2 errors
;labelerror1:	error1 this is an error on pass 1
;labelerror2:	error2 this is an error on pass 2, but only if another error didn't stop it on pass1
;labelfatal:	fatal "This is fatal" ; fatal comments

; **** include ****
; syntax:
; [<label>] INCLUDE <string> [<comments>] - include file
; #INCLUDE <string> [<comments>] - include file
#include "example.inc" ; your file is included below this line
	include "example.inc" ; your file is included below this line


; **** IF/ELSE/ENDIF statements ****
; do not put comments on your IF lines, otherwise you may get the
; wrong results

; syntax:
; [<label>] IF     <expression> - if expression is true
; [<label>] IFT    <expression> - if expression is true
; [<label>] IFE    <expression> - if expression is equal to zero
; [<label>] IFDEF  <string>     - if string is defined
; [<label>] IFNDEF <string>     - if string is not defined
;  [<conditional lines>]        - lines that are acted upon if the IF is true
; [<label>] ELSE                - if IF is not true
;  [<conditional lines>]        - lines that are acted upon if the IF is false
; [<label>] ENDIF               - ends an IF control structure

; C style syntax:
; #IF     <expression>   - if expression is not equal to zero
; #IFT    <expression>   - if expression is not equal to zero
; #IFE    <expression>   - if expression is equal to zero
; #IFDEF  <string>       - if string is defined
; #IFNDEF <string>       - if string is not defined
;  [<conditional lines>] - lines that are acted upon if the IF is true
; #ELSE                  - if IF is not true
;  [<conditional lines>] - lines that are acted upon if the IF is false
; #ENDIF                 [<comments>]- ends an IF control structure

; #IF <expression> - if expression is true
; if true, opcode style

; here is an example of what not to do. "labelrmb" is equal to $E000,
; but the following is treated as if it was true, even though it looks
; false. The equal signs, and everything past it is ignored, and since
; "labelrmb" is not zero, then it is treated as true.
	if labelrmb==$c000
	lda labelorg
	else
	ldx labelds
	endif

; if true, C PreProcessor style
; The following is treated exactly the same as the above example.
#if labelrmb
	lda labelorg
#else
	ldx labelds
#endif

; The following is true, if "labelrmb" is equal to $E000.
; if labelrmb minus $E000 is equal to zero (and it is), then
; the expression is "true"
#ife labelrmb-$e000
	lda labelorg
#else
	ldx labelds
#endif

; no label is treated as a "false" condition
#if nolabel
	lda labelorg
#else
	ldx labelds
#endif

; if true (not zero)
#ift nolable
	lda labelorg
#else
	ldx labelds
#endif

; if equal to zero
#ife $1234
	lda labelorg
#else
	ldx labelds
#endif

; if label is defined
#ifdef labelrmb
	lda labelorg
#else
	ldx labelds
#endif

; if label is not defined
#ifndef nolabel
	lda labelorg
#else
	ldx labelds
#endif

; ***** examples of the different addressing modes *****

label1:
; the original 6805 addressing modes
	nega	          ; inherent
	brset0 $80,label1 ; bit branch
	bset 0,label1	  ; bit set/clear
	bra label1	  ; relative 
	sub #$80          ; immediate
	sub $80        	  ; short
	sub $8080         ; long
	sub $8080,x       ; long, and X index
	sub $80,x         ; short and X index
	sub ,x            ; X index
	sub $x            ; X index, EMM Studio syntax
	sub x             ; X index, with warning

; the additional addressing modes added by the ST7
; Y index
	sub $8080,y ; long, and Y index
	sub $80,y   ; short and Y index
	sub ,y      ; Y index
	sub $y      ; Y index, EMM Studio syntax
	sub y       ; Y index, with warning

; indirect addressing
	brset0 [$80],label1 ; indirect bit test and branch
	bset 0,[$81]        ; indirect bit set/clear
	bra [00:$82]        ; relative, and indirect
	sub [$83.b]         ; indirect short
	sub [$84:$85]       ; indirect long

; indirect, and X
	sub [$87:$88],x ; indirect long, and X index
	sub [$88],x     ; indirect short and X index

; indirect, and Y index
	sub [$89:$8A],y ; indirect long, and Y index
	sub [$8A],y     ; indirect short and Y index

; here are some examples of register to register transfers
	; with A as the source
	tas     ; transfer A to S
	tax     ; transfer A to X (6805)
	tay     ; transfer A to Y
	; with S as the source
	tsa     ; transfer S to A (6805)
	tsx     ; transfer S to X
	tsy     ; transfer S to Y
	; with X as the source
	txa     ; transfer X to A (6805)
	txs     ; transfer X to S
	txy     ; transfer X to Y
	; with Y as the source
	tya     ; transfer Y to A
	tys     ; transfer Y to S
	tyx     ; transfer Y to X

; we also support the 6809's "tfr" opcode
; but obvously, we don't support all of the
; 6809's registers, just A, S, X and Y
; NOTICE: the 6805 only supports 3 different
; register-to-register combinations
	; with A as the source
	tfr     a,s     ; transfer A to S
	tfr     a,x     ; transfer A to X (6805)
	tfr     a,y     ; transfer A to Y
	; with S as the source
	tfr     s,a     ; transfer S to A (6805)
	tfr     s,x     ; transfer S to X
	tfr     s,y     ; transfer S to Y
	; with X as the source
	tfr     x,a     ; transfer X to A (6805)
	tfr     x,s     ; transfer X to S
	tfr     x,y     ; transfer X to Y
	; with Y as the source
	tfr     y,a     ; transfer Y to A
	tfr     y,s     ; transfer Y to S
	tfr     y,x     ; transfer Y to X

; Please be aware, the 6805 only has 3 register to register transfer opcodes
; as follows
	tsa     ; transfer S to A
	tax     ; transfer A to X
	txa     ; transfer X to A
