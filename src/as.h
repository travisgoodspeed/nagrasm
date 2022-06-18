/*
 * as.h - header
 *
 * $Id: as.h 13 2004-02-11 06:09:11Z eric $
 *
 * Copyright 1991, 1992, 1994, 1995, 1996, 2004 Eric Smith.
 *
 * Based on Motorola Freeware assemblers.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.  Note that permission is
 * not granted to redistribute this program under the terms of any
 * other version of the General Public License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111  USA
 */

//#define DEBUG 1
/*
 *	machine dependent type definitions
 */

/*#if defined (macintosh) || defined (__TURBOC__) || defined (sparc)*/
#define SInt16 signed short int
/*#endif*/

/*
 *	machine independent definitions and global variables
 */

#define lobyte(x) ((x)&0xff)
#define hibyte(x) (((x)>>8)&0xff)

#ifdef macintosh
#include <Types.h>
#else
typedef int Boolean;
#endif

#ifndef __TURBOC__
#define far
#endif

#define YES	1
#define NO	0
#define ERR	(-1)

#ifndef true
#define true 1
#endif
#ifndef false
#define false 0
#endif

#define MAXFILENAMELEN 200
#define MAXBUF	512
#define MAXOP	10	/* longest mnemonic */
#define MAXLAB	512	/* longest label */
#define P_LIMIT 128

#define F_LIMIT 100     /* max number of input files per assembly */

/*	Character Constants	*/
#define NEWLINE '\n'
#define TAB	'\t'
#define BLANK	' '
#define EOS	'\0'

/*	Opcode Classes		*/
#define PSEUDO  0

/* global variables */
extern int     Line_num;	       /* current line number	       */
extern int     Err_count;	       /* total number of errors       */
extern int     Warn_count;	       /* total number of warnings     */
extern char    Line[MAXBUF];	       /* input line buffer	       */
extern char    Label[MAXLAB];	       /* label on current line        */
extern char    Op[MAXOP];	       /* opcode mnemonic on current line      */
extern char    Operand[MAXBUF];        /* remainder of line after op	       */
extern char    *Optr;		       /* pointer into current Operand field   */
extern SInt16  Result;		       /* result of expression evaluation      */
extern Boolean Force_word;	       /* Result should be a word when set     */
extern Boolean Force_byte;	       /* Result should be a byte when set     */
extern SInt16  Pc;		       /* Program Counter	       */
extern SInt16  Old_pc;		       /* Program Counter at beginning */

extern SInt16  Last_sym;	       /* result of last lookup        */

extern int     Pass;		       /* Current pass #	       */
extern int     N_files; 	       /* Number of files to assemble  */
extern int     Cfn;		       /* Current file number 1...n    */
extern int     Ffn;		       /* forward ref file #	       */
extern int     F_ref;		       /* next line with forward ref   */
extern char    *Argv[F_LIMIT];	       /* pointer to file names, entry 0 unused */

extern Boolean LowByteFirst;	       /* true if words are to be emitted */
				       /* low byte first (e.g. 6502) */
extern Boolean ByteSelOps;	       /* true if '>' and '<' are hibyte and */
				       /* lowbyte operators instead of forcing */
				       /* word and byte operand size */

extern Boolean Lflag;		       /* listing flag 0=nolist, 1=list*/

extern int     P_force; 	       /* force listing line to include Old_pc */
extern int     P_total; 	       /* current number of bytes collected    */
extern int     P_bytes[P_LIMIT];       /* Bytes collected for listing  */
extern int     P_wflag[P_LIMIT];       /* Flag for first byte of word */

extern Boolean Cflag;		       /* cycle count flag */
extern int     Cycles;		       /* # of cycles per instruction  */
extern int     Cycle_adjust;           /* cycle count adjustment code */
extern long    Ctotal;		       /* # of cycles seen so far */
extern int     Sflag;		       /* symbol table flag, 0=no symbol */
extern int     N_page;		       /* new page flag */
extern int     Page_num;	       /* page number */
extern int     tabWidth;	       /* number of characters per tab stop */
extern int     CREflag; 	       /* cross reference table flag */

extern char    Obj_name[];

extern FILE    *ListFile;		 /* object file's file descriptor*/
extern char    List_name[];

typedef struct llink { /* linked list to hold line numbers */
       int L_num; /* line number */
       struct llink *next; /* pointer to next node */
} llink;

typedef struct nlist { /* basic symbol table entry */
	char	*name;
	struct nlist *Lnext ; /* left node of the tree leaf */
	struct nlist *Rnext; /* right node of the tree leaf */
	llink *L_list; /* pointer to linked list of line numbers */
	SInt16	 def;
	char     bit_sym;
	char     bit;
	char	delete;
} nlist;

extern nlist *root;	       /* root node of the tree */

typedef struct	{   /* an entry in the mnemonic table */
	char	*mnemonic;	/* its name */
	int     seq;            /* for multiple mnemonics */
	char	class;		/* its class */
	int	opcode; 	/* its base opcode */
	int	cycles; 	/* its base # of cycles */
        int     req_proc;       /* required processor */
  } oper;

/* conditional assembly stuff added by Eric Smith, 9/14/90 */

#define kMaxCondNestLevel 10

extern int condNestLevel;
extern Boolean condElseFound [kMaxCondNestLevel];
extern Boolean condCurrentVal [kMaxCondNestLevel];

/* macro stuff */

#define kMaxMacroDefNestLevel 1

extern int macroDefNestLevel;

typedef struct macroLine
  {
    char *line;
    struct macroLine *nextLine;
  } macroLine;

typedef struct macroDefNode
  {
    char		*name;
    macroLine		*firstLine;
    struct macroDefNode *Lnext;
    struct macroDefNode *Rnext;
  } macroDefNode;

extern macroDefNode *macroRoot;
extern macroDefNode *macroBeingDefined;


/*************************************************************************/
/*                                                                       */
/* Defined in object.c                                                   */
/*                                                                       */
/*************************************************************************/

#define F_MOT_S19	 1 /* Motorola S19 format */
#define F_INTEL_HEX      2 /* INTEL Hex format */
#define F_NAGRAEDIT_NE2  3 /* NagraEdit Patch format for ROM2 */
#define F_NAGRAEDIT_EP3  4 /* NagraEdit Extended Patch format for ROM3 */
#define F_NAGRAEDIT_EP10 5 /* NagraEdit Extended Patch format for ROM10 */
#define F_NAGRAEDIT_EP11 6 /* NagraEdit Extended Patch format for ROM11 */
#define F_BIN_64K	 7 /* binary 64k */
#define F_BIN_4K	 8 /* binary 4k */
#define F_BIN_8K	 9 /* binary 8k */

int SetObjectFormat (int format, char force);
void OpenObjectFile (char *name);
void CloseObjectFile (Boolean keep);
void ReinitObject (void);

void dump_record (void);
void emit (int byte);  /* emit one byte */
void eword (int wd);   /* emit one word */
void eword2 (int wd);  /* emit one word on a word-addressed machine */
void emit_char(unsigned char ch); /* emit one character */


/*************************************************************************/
/*                                                                       */
/* Defined in util.c                                                     */
/*                                                                       */
/*************************************************************************/


/*
 *	charpos --- return a pointer to the first character in str1 that
 *                  matches any character in str2
 */

char *charpos (char *str, char *str2);


/*
 *	delim --- check if character is a delimiter
 */

Boolean delim (char c);


/*
 *	skip_white --- move pointer to next non-whitespace char
 */

char *skip_white (char *ptr);


/*
 *	any --- does str contain c?
 */

Boolean any (char c, char *str);


/*
 *	mapdn --- convert A-Z to a-z
 */

char mapdn (char c);


/*
 *	strcmpCI --- case insensitive string comparison
 */
int strcmpCI (char *str1, char *str2);

/*
 *	strncmpCI --- case insensitive string comparison, with count
 */
int strncmpCI (char *str1, char *str2, int len);


/*
 *	head --- is str2 the head of str1?
 */

Boolean head (char *str1, char *str2);


/*
 *	alpha --- is character a legal letter
 */

Boolean alpha (char c);


/*
 *	alphan --- is character a legal letter or digit
 */

Boolean alphan (char c);


/*
 *  white_space --- is character whitespace?
 */

Boolean white_space (char c);


/*
 *  alloc --- allocate memory
 */

char *alloc (int nbytes);


/*************************************************************************/
/*                                                                       */
/* Defined in output.c                                                   */
/*                                                                       */
/*************************************************************************/


void OpenListFile (void);

void CloseListFile (void);

void page_feed (void);

void print_line(FILE *outfile);

/*
 *	fatal --- fatal error handler
 */

void fatal (char *str);


/*
 *	error --- error in a line
 *			print line number and error
 */

void error (char *str);


/*
 *	warn --- trivial error in a line
 *			print line number and error
 */

void warn (char *str);


/*
 *  stable --- prints the symbol table in alphabetical order
 */

void stable (nlist *ptr);


/*
 *  cross  --  prints the cross reference table 
 */

void cross (nlist *point);


/*************************************************************************/
/*                                                                       */
/* Defined in input.c                                                    */
/*                                                                       */
/*************************************************************************/


Boolean OpenSourceFile (char *name);
/* returns false if unable to open file */

Boolean GetSourceLine (char *buf, int *line);
/* returns false if error or end of file */

void GetCurrentSourceLoc (char *name, int *line);

Boolean CloseSourceFile (void);
/* returns false if no files left open */


/*************************************************************************/
/*                                                                       */
/* Defined in symtab.c                                                   */
/*                                                                       */
/*************************************************************************/


/*
 *	install --- add a symbol to the table
 */

Boolean install (char *str, SInt16 val, Boolean redefOK);


/*
 *	install_bit --- add a bit symbol to the table
 */

Boolean install_bit (char *str, SInt16 addr, int bit);

/*
 *  * undefine --- undefine a variable, or at least mark it as not being used
 *   */
void undefine(char *name);

/*
 *	lookup_ne --- find string in symbol table, no error if not found
 */

nlist *lookup_ne (char *name);


/*
 *	lookup --- find string in symbol table
 */

nlist *lookup (char *name);


/*
 *	lookup_bit
 */

int lookup_bit (char *name, SInt16 *addr, int *bit);


/*
 *	add_ref --- add reference to an identifier
 */
void add_ref (nlist *pointer);


/*
 *	mne_look --- mnemonic lookup
 *
 *	Return pointer to an oper structure if found.
 *	Searches both the machine mnemonic table and the pseudo table.
 */

oper *mne_look (char *str);


/*************************************************************************/
/*                                                                       */
/* Defined in ffwd.c                                                     */
/*                                                                       */
/*************************************************************************/


/*
 *	fwdinit --- initialize forward ref file
 */

void fwdinit (void);

/*
 *	fwdreinit --- reinitialize forward ref file
 */

void fwdreinit (void);


/*
 *	fwdmark --- mark current file/line as containing a forward ref
 */

void fwdmark (void);


/*
 *	fwdnext --- get next forward ref
 */

void fwdnext (void);


/*************************************************************************/
/*                                                                       */
/* Defined in pseudo.c                                                   */
/*                                                                       */
/*************************************************************************/


#define DOFCOND 256 /* execute pseudo-op even in false conditional */
#define NOLABEL 512 /* don't do standard label processing for pseudo-op */
#define PROCPSEUDO 1024 /* processor specific pseudo-op */

/* built-in pseudo-ops, listed here because processor specific
   pseudo-ops may be aliases to built-ins */

#define RMB	0	/* Reserve Memory Bytes 	*/
#define FCB	1	/* Form Constant Bytes		*/
#define FDB	2	/* Form Double Bytes (words)	*/
#define FCC	3	/* Form Constant Characters	*/
#define FCS     4       /* Form Constant Characters, high bit set on last */
#define ORG	5	/* Origin			*/
#define EQU	6	/* Equate			*/
#define ZMB	7	/* Zero memory bytes		*/
#define FILL	8	/* block fill constant bytes	*/
#define OPT	9	/* assembler option		*/
#define NULL_OP 10	/* null pseudo op		*/
#define PAGE	11	/* new page			*/
#define MACHINE 12	/* machine			*/
#define IF	13	/* if				*/
#define IFE     14      /* if not                       */
#define IFDEF	15	/* if defined			*/
#define IFNDEF	16	/* if not defined		*/
#define IF1     17      /* if pass 1                    */
#define IF2     18      /* if pass 2                    */
#define ELSE	19	/* else 			*/
#define ENDIF	20	/* endif			*/
#define DEF	21	/* define (6502)		*/
#define LIST	22	/* list (6502)			*/
#define NOLIST	23	/* nolist (6502)		*/
#define SET	24	/* set (EQU w/ no error for redefinition */
#define PRINT   25      /* print to std out             */
#define WARN    26      /* user defined warning         */
#define ERROR   27      /* user defined error           */
#define FATAL   28      /* user defined fatal error     */
#define INCLUDE 29      /* include                      */
#define MACRO	30	/* macro definition		*/
#define ENDM	31	/* macro definition end 	*/
#define EQUB    32      /* equate bit                   */
#define	EPX	33	/* nagraedit patch file comment */
#define PRINT1  34      /* print to std out if pass1    */
#define PRINT2  35      /* print to std out if pass2    */
#define WARN1	36	/* print warning if pass1	*/
#define WARN2	37	/* print warning if pass2	*/
#define ERROR1	38	/* print error if pass1	*/
#define ERROR2	39	/* print error if pass2	*/
#define ROM	40	/* for selecting ROM type */
#define ROM2	41	/* for selecting ROM type */
#define ROM3	42	/* for selecting ROM type */
#define ROM10	43	/* for selecting ROM type */
#define ROM11	44	/* for selecting ROM type */
#define UNDEF	45	/* for undefining a label */

extern oper pseudo [];

extern int NPSE;

/*
 *	do_pseudo --- do pseudo op processing
 */

void do_pseudo (int op /* which op */);


/*************************************************************************/
/*                                                                       */
/* Defined in eval.c                                                     */
/*                                                                       */
/*************************************************************************/


/*
 *	eval --- evaluate expression
 *
 *	an expression is constructed like this:
 *
 *	expr ::=  expr + term |
 *		  expr - term |
 *		  expr * term |
 *		  expr / term |
 *		  expr | term |
 *		  expr & term |
 *		  expr % term |
 *		  expr ^ term |
 *		  expr << term |     Added by Eric Smith, 12/15/89
 *		  expr >> term ;     Added by Eric Smith, 12/15/89
 *
 *	term ::=  symbol |
 *		  * |
 *		  constant ;
 *
 *	symbol ::=  string of alphanumerics with non-initial digit
 *
 *	constant ::= hex constant |
 *		     binary constant |
 *		     octal constant |
 *		     decimal constant |
 *		     ascii constant;
 *
 *	hex constant ::= '$' {hex digits};
 *
 *	octal constant ::= '@' {octal digits};
 *
 *	binary constant ::= '%' { 1 | 0 };
 *
 *	decimal constant ::= {decimal digits};
 *
 *	ascii constant ::= ''' any printing char;
 *
 */

Boolean get_number (char **cptr, SInt16 *val);

int eval (void);


/*************************************************************************/
/*                                                                       */
/* Defined in tables.c                                                   */
/*                                                                       */
/*************************************************************************/


extern oper *table;
extern int NMNE;
extern void (far *do_op)(int opcode, int class);
extern nlist *ptable;
extern int ptableSize;
extern int proc_variant;

/* structure used to attach processor modules to assembler */

typedef struct
{
  char *name;
  int variant;
} proc_name;

typedef struct {
    char *description;  /* human-readable description */
    proc_name *name;    /* array of processor name/variant code pairs */
    void (far *init)(void);  /* initialization routine */
    void (far *term)(void);  /* termination routine */
    Boolean (far *sel)(char *proc, char *arg);   /* selection routine */
    void (far *desel)(void); /* deselection routine */
    void (far *do_op)(int opcode, int class); /* instruction processing routine */
    oper *table;     /* mnemonic table */
    int NMNE;	     /* number of entries in mnemonic table */
    nlist *ptable;    /* processor-specific predefined symbols */
    int ptableSize;  /* number of entries in ptable */
  } procinfo;

void initall (void);

void termall (void);

extern int selectproc (char *arg, char force); /* select a processor type by name */
				   /* returns boolean value for success */

void list_procs (void); /* write a list of supported processors */
                        /* to standard error output */


/*************************************************************************/
/*                                                                       */
/* Defined in macro.c                                                    */
/*                                                                       */
/*************************************************************************/


macroDefNode *FindMacro (char *name);

macroDefNode *CreateMacro (char *name, char *operand);
