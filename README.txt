NagrASM 1.0.1 release notes
Modifications to asm68 by The NagrASM Team
12 Nov 2005

   NagrASM was written, because I don't like Intel style of asm, that all the
     ST7 assemblers, that I've seen, use. The ST7 is the same core used in the
     ST19CFxx series of smart cards. The Nagra ROM10 and ROM11 are based on
     this smart card. While although this cross assembler could be used to
     assemble programs for getting free TV, that is *NOT* why I wrote this or
     released it. I have written and released this cross assembler with plans
     of reprogramming Nagra ROM10 and ROM11 smart cards for other, non-TV
     related things, like possibly "secure" authentication. Since the older
     generations of Nagra smart cards are being phased out all over the world,
     this cross assembler might be able to give those old smart cards a new
     (legit) use. I will not be held responsible for what you use this cross
     assembler for. Since this is ST7 compatable, it could be used to assemble
     programs for ANY system that uses the ST7 core, not just smart cards.
     
Overview
--------
  The general features as compared to the asm68 assembler are:

  The assembler will default to a 6805, and hex format, if nothing else
    is selected.

  Cycle counting may not be accurate for the 6805 and/or ST16CFxx, but
    should be accurate for the ST7 and ST19CFxx CPUs.

  Added support for the ST7's Y register, and indirect addressing.

  First character in a line that has no label can be '#', so that when
    used with the conditionals (if, else, etc) will look more like their
    C counterparts, but do not act 100% the same. You have been warned.

  Added an "undef" opcode to "undefine" a variable. If you try to use it after
    that point in an expression, you will most likely get an error.

  If the 1st character of an opcode is a '.', then it is ignored, so
    pseudo opcodes like ".equ", and ".db", for example, will work. This
    gives you the option of making some really non-standard formatted
    source code, which will probably make it not assemble with other
    assemblers.

  Added a "card" pseudo op, that acts the same as the "device" and "machine"
    pseudo ops.

  Added PRINT1 and PRINT2 pseudo ops, which is the same as PRINT, but specific
    to pass1 or pass2

  Added WARN1 and WARN2 pseudo ops, which is the same as WARN, but specific
    to pass1 or pass2

  Added support for the 6809's "tfr" opcode, which transfers between
    registers for example "tfr a,y" will transfer the contents of register
    A into register Y

  Added support for additional opcodes in the ST7, as follows.
    [New opcodes - only work in zero page]
      swap, swapa, swapx, swapy - swaps nibbles (ST7 only)
    [opcodes based on register X, converted to register Y] (ST7 only)
      asly/lsly roly incy ldy cmpy pushy clry muly
      asry lsry rory decy sty tsty popy comy 
    [several register-to-register transfers] (ST7 only except where noted)
      tfr a,s | tas | transfer A to S
      tfr a,x | tax | transfer A to X (6805)
      tfr a,y | tay | transfer A to Y
      tfr s,a | tsa | transfer S to A (6805)
      tfr s,x | tsx | transfer S to X
      tfr s,y | tsy | transfer S to Y
      tfr x,a | txa | transfer X to A (6805)
      tfr x,s | txs | transfer X to S
      tfr x,y | txy | transfer X to Y
      tfr y,a | tya | transfer Y to A
      tfr y,s | tys | transfer Y to S
      tfr y,x | tyx | transfer Y to X
    [and some register/stack opcodes] (ST7 only)
      push a  | pusha  | pushes A on the stack
      push x  | pushx  | pushes X on the stack
      push y  | pushy  | pushes Y on the stack
      push cc | pushcc | pushes CC on the stack
      pop a   | popa   | pops A off the stack
      pop x   | popx   | pops X off the stack
      pop y   | popy   | pops Y off the stack
      pop cc  | popcc  | pops CC off the stack

  Please note, that "x" and "y" based opcodes, will not work with an opposing
    index, so something like "ldy $C123,x", and "ldx $C123,y" are not valid,
    but "ldy $C123,y", and "ldx $C123,x" are valid.

  Added support for bit oriented opcodes where the bit is part of the
    opcode itself, like "bset0 $9D", in addition to "bset 0,$9D" style.

  Supports several formats for indirect addressing.
    Short indirect addressing formats:
      (of course $9D could be replaced with a label)
      lda [$9D] ; perfered format
      lda [00:$9D]
      lda [$9D.b] ; byte
      lda [$9D.s] ; short
    Here are some Long indirect addressing formats:
      lda [$9D:$9E]
      lda [$9D:$9D+1]
      lda [$9D.l] ; long
      lda [$9D.d] ; double (byte)
      lda [$9D.w] ; word

  If the ROM2, ROM3, ROM10, or ROM11 is selected as the CPU, then it will
    default to output .ne2, .ep3, .ep10, or .ep11 files, respectively. If
    you do not want that to happen, then either 1) select the file format
    before the CPU type, or 2) select the ST16CFxx or ST19CFxx as the CPU.

  Added a "rom" pseudo op, that accepts an expression for the type of ROM
    you're programming for. I also added a "ROM2", "ROM3", "ROM10", and
    "ROM11" pseudo ops, which selects the respective CPU for that ROM type.

-----------------------------------------------------------------------------
ASM68 release notes
Copyright 1991, 1992, 2004 Eric L. Smith
10-Feb-2004
Eric L. Smith

  asm68 is an enhanced derivative of the Motorola Freeware assemblers.

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License version 2 as
  published by the Free Software Foundation.  Note that permission is
  not granted to redistribute this program under the terms of any other
  version of the General Public License.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111 USA

Overview
--------
  The general features as compared to the Motorola Freeware assembler are:

  listing goes to file instead of standard out ** describe naming **
  in listing, two byte quantities in object code are shown as a single
    16-bit value
  one assembler supports all Motorola 8-bit processors, using
    a "MACHINE" pseudo-op (e.g., MACHINE 6809), or a -mach option on
    the command line.  Invoke asm68 with the -help option for a list
    of supported processors.
  include files with "INCLUDE" pseudo-op (e.g., "INCLUDE /FOO.ASM/").  Any
    character may be used as the delimiter for the filename.
  source file names and options may occur in any order on the command
    line.  Options must ALL be preceded with a "-".
  new "-def sym[=val]" command line option to define a symbol, with the
    value defaulting to one.  The value may be in binary, octal, decimal,
    or hexadecimal, using either Motorola or Intel notation.  Note that
    a leading dollar sign ("$") for either the symbol name or value may
    need to be escaped in order to prevent a command shell from trying
    to do a variable expansion.
  parenthesis are allowed in expressions (but no operator precedence)
  correct timings for CMOS 6805s (1468[7]05, 68HC[78]05)
  incorrect timings for 6800, 68[7]01, 6802, 68[7]03, 6808, 68120
  FCS pseudo-op (like FCC, but sets the MSB of the last byte)
  SETDP pseudo-op for 6809 supported.
  SET pseudo-op (like EQU, but may redefine existing symbol)
  conditionals (IFxxx, ELSE, ENDIF)
    IF     <expr>  true if expression not equal to 0
    IFE    <expr>  true if expression equal to 0
    IFDEF  <name>  true if name is defined
    IFNDEF <name>  true if name is undefined
    IF1            true if pass 1
    IF2            true if pass 2
  PRINT pseudo-op to output string to standard output during assembly.
  WARN, ERROR, FATAL pseudo-op for user defined warnings, errors, and
    fatal errors.
  MS-DOS lost cluster problem fixed
  symbols made case insensitive
  comments may begin with a semicolon
  comments alone on a line don't have to begin in column 1
  a period (".") may be used to refer to the location counter.
  new binary operators:
    <<, >> shift left, right
    ^ exclusive or
    % modulo
  new unary operators:
    ~ ones complement
  numeric constants may now be Motorola style or Intel style.
  character literals (introduced by "\" delimiter) must have closing
    delimiter ("\") if used other than at the end of an expression
