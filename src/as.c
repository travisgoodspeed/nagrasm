/*
 * as.c - main program
 *
 * $Id: as.c 13 2004-02-11 06:09:11Z eric $
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

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef macintosh
#include <strings.h>
#include <files.h>
#endif

#include "as.h"


/* global variables */
int Line_num = 0;		/* current line number          */
int Err_count = 0;		/* total number of errors       */
int Warn_count = 0;		/* total number of warnings     */
char Line[MAXBUF] = { 0 };	       /* input line buffer            */
char Label[MAXLAB] = { 0 };	       /* label on current line        */
char Op[MAXOP] = { 0 };		       /* opcode mnemonic on current line      */
char Operand[MAXBUF] = { 0 };	       /* remainder of line after op           */
char *Optr = 0;			/* pointer into current Operand field   */
SInt16 Result = 0;		/* result of expression evaluation      */
Boolean Force_word = 0;		/* Result should be a word when set     */
Boolean Force_byte = 0;		/* Result should be a byte when set     */
SInt16 Pc = 0;			/* Program Counter              */
SInt16 Old_pc = 0;		/* Program Counter at beginning */

SInt16 Last_sym = 0;		/* result of last lookup        */

int Pass = 0;			/* Current pass #               */
int N_files = 0;		/* Number of files to assemble  */
int Cfn = 0;			/* Current file number 1...n    */
int Ffn = 0;			/* forward ref file #           */
int F_ref = 0;			/* next line with forward ref   */
char *Argv[F_LIMIT];		/* pointer to file names, entry 0 unused */

Boolean LowByteFirst = 0;	/* true if words are to be emitted */
				/* low byte first (e.g. 6502) */
Boolean ByteSelOps = 0;		/* true if '>' and '<' are hibyte and */
				/* lowbyte operators instead of forcing */
				/* word and byte operand size */

Boolean Lflag = 0;		/* listing flag 0=nolist, 1=list */

int P_force = 0;		/* force listing line to include Old_pc */
int P_total = 0;		/* current number of bytes collected    */
int P_bytes[P_LIMIT] = { 0 };	       /* Bytes collected for listing  */
int P_wflag[P_LIMIT] = { 0 };	       /* Flag for first byte of word */

Boolean Cflag = 0;		/* cycle count flag */
int Cycles = 0;			/* # of cycles per instruction  */
int Cycle_adjust;		/* cycle count adjustment code */
long Ctotal = 0;		/* # of cycles seen so far */
int Sflag = 0;			/* symbol table flag, 0=no symbol */
int N_page = 0;			/* new page flag */
int Page_num = 2;		/* page number */
int tabWidth = 8;		/* number of characters per tab stop */
int CREflag = 0;		/* cross reference table flag */

FILE *Objfil = 0;		/* object file's file descriptor */
FILE *ListFile = 0;		/* listing file's file descriptor */
char Obj_name[256] = "";
char List_name[256] = "";

struct nlist *root = NULL;	/* root node of the tree */

char *alloc ();

/* conditional assembly stuff added by Eric Smith, 9/14/90 */

int condNestLevel;
Boolean condElseFound[kMaxCondNestLevel];
Boolean condCurrentVal[kMaxCondNestLevel];


/* macro stuff */

int macroDefNestLevel;

macroDefNode *macroRoot;
macroDefNode *macroBeingDefined;


/* function prototypes */

static void initialize (void);
static void re_init (void);
static void make_pass (void);
static Boolean parse_line (void);
static void process (void);


/*
 *  usage --- write usage message to standard output
 */
void
usage (char *name)
{
   fprintf (stderr, "NagrASM - A cross assembler for 6805 and ST7 based CPUs\n");
   fprintf (stderr, "Version 1.0.1 - 12 Nov 2005\n");
   fprintf (stderr, "Usage: NagrASM [options] [files]\n");
   fprintf (stderr, "\nOptions:\n");
   fprintf (stderr, "  -?, -help       print usage and list of supported processors\n");
   fprintf (stderr, "  -mach xxx       target processor, e.g., -mach ST7\n");
   fprintf (stderr, "  -def sym[=val]  define symbol (default value 1)\n");
   fprintf (stderr, "Listing Options:\n");
   fprintf (stderr, "  -l, -nol        listing on/off\n");
   fprintf (stderr, "  -c, -noc        cycle counting on/off\n");
   fprintf (stderr, "  -s              symbol table\n");
   fprintf (stderr, "  -cre            cross reference\n");
   fprintf (stderr, "Output File Formats:\n");
   fprintf (stderr, "  -hex            Intel hex output (.hex) - default\n");
   fprintf (stderr, "  -s19            Motorola S19 output (.s19)\n");
   fprintf (stderr, "  -4k             E000-EFFF, 4k binary output (.bin)\n");
   fprintf (stderr, "  -8k             C000-DFFF, 8k binary output (.bin)\n");
   fprintf (stderr, "  -64k            0000-FFFF, 64k binary output (.bin)\n");
   fprintf (stderr, "  -ne2            NagraEdit ROM2 patch file output (.ne2)\n");
   fprintf (stderr, "  -ep3            NagraEdit ROM3 Extended Patch file output (.ep3)\n");
   fprintf (stderr, "  -ep10           NagraEdit ROM10 Extended Patch file output (.ep10)\n");
   fprintf (stderr, "  -ep11           NagraEdit ROM11 Extended Patch file output (.ep11)\n");
   fprintf (stderr, "Defaults:\n");
   fprintf (stderr, "     CPU: 6805\n");
   fprintf (stderr, "  output: intel hex\n");
}


/*
 *  opt_def --- handle command line -def option
 */
void opt_def (char *cptr, char *progname)
{
   char ident[MAXBUF];
   char *iptr;
   SInt16 val;

   // check if the name we're about to define starts with a digit, which is illegal
   if (*cptr >= '0' && *cptr <= '9')
   {
      fprintf (stderr, "%s: symbol name in -def option may not begin with a digit\n", progname);
      exit (1);
   }
   
   // copy name to buffer 'ident'
   for (iptr = &ident[0]; alphan (*cptr); *iptr++ = *cptr++);
   *iptr='\0'; // terminate string
   
   if (*cptr == '=')
   {
      cptr++;
      if (get_number (&cptr, &val))
      {
	 if (*cptr == '\0')
	    install (&ident[0], val, YES);
	 else
	 {
	    fprintf (stderr, "%s: extra characters after constant value in -def option for '%s'\n", progname, ident);
	    exit (1);
	 }

      }
      else
      {
	 fprintf (stderr, "%s: bad constant in -def option for '%s'\n", progname, ident);
	 exit (1);
      }
   }
   else if (*cptr == '\0')
      install (&ident[0], 1, YES);
   else
   {
      fprintf (stderr, "%s: bad character in identifier in -def option\n", progname);
      exit (1);
   }
}


/*
 *  as ---  cross assembler main program
 */
int
main (int argc, char **argv)
{
   char **np;
   FILE *fopen ();
   int j = 0;
   Boolean Usage;

   initall ();			       /* target machine specific init. */

   Lflag = 0;			       /* set default values for command line options */
   Cflag = 0;
   Sflag = 0;
   CREflag = 0;
   Usage = 0;

   if (argc < 2)
   {
      usage (argv[0]);
      exit (1);
   }

   selectproc ("6805", 0);	       /* default to the 6805, unless the user selects something else */
   j = 0;
   while (++j < argc)
   {
      if (*argv[j] == '-')
      {
	 if ((strcmpCI (argv[j], "-?") == 0) || (strcmpCI (argv[j], "-help") == 0))
	    Usage = 1;
	 else if (strcmpCI (argv[j], "-l") == 0)
	    Lflag = 1;
	 else if (strcmpCI (argv[j], "-nol") == 0)
	    Lflag = 0;
	 else if (strcmpCI (argv[j], "-c") == 0)
	    Cflag = 1;
	 else if (strcmpCI (argv[j], "-noc") == 0)
	    Cflag = 0;
	 else if (strcmpCI (argv[j], "-s") == 0)
	    Sflag = 1;
	 else if (strcmpCI (argv[j], "-cre") == 0)
	    CREflag = 1;
	 else if (strcmpCI (argv[j], "-s19") == 0)
	    SetObjectFormat (F_MOT_S19, 1);
	 else if (strcmpCI (argv[j], "-hex") == 0)
	    SetObjectFormat (F_INTEL_HEX, 1);
	 else if (strcmpCI (argv[j], "-64k") == 0)
	    SetObjectFormat (F_BIN_64K, 1);
	 else if (strcmpCI (argv[j], "-4k") == 0)
	    SetObjectFormat (F_BIN_4K, 1);
	 else if (strcmpCI (argv[j], "-8k") == 0)
	    SetObjectFormat (F_BIN_8K, 1);
	 else if (strcmpCI (argv[j], "-mach") == 0)
	 {
	    if ((++j < argc) && (*argv[j] != '-'))
	    {
	       if (!selectproc (argv[j], 1))
	       {
		  fprintf (stderr, "%s: unrecognized argument '%s' to -machine option\n", argv[0], argv[j]);
		  list_procs ();
		  exit (1);
	       }
	    }
	    else
	    {
	       fprintf (stderr, "%s: -mach option requires argument\n", argv[0]);
	       usage (argv[0]);
	       exit (1);
	    }
	 }
	 else if (strcmpCI (argv[j], "-def") == 0)
	 {
	    if ((++j < argc) && (*argv[j] != '-'))
	    {
	       opt_def (argv[j], argv[0]);
	    }
	    else
	    {
	       fprintf (stderr, "%s: -def option requires argument\n", argv[0]);
	       usage (argv[0]);
	       exit (1);
	    }
	 }
	 else
	 {
	    fprintf (stderr, "%s: unrecognized option '%s'\n", argv[0], argv[j]);
	    usage (argv[0]);
	    exit (1);
	 }
      }
      else if (*argv[j] != 0)
	 Argv[++N_files] = argv[j];
   }

   if (Usage)
   {
      usage (argv[0]);
      list_procs ();
      if (N_files < 1)
	 return (0);
   }

   if (N_files < 1)
   {
      fprintf (stderr, "%s - no files specified\n", argv[0]);
      usage (argv[0]);
      exit (1);
   }

   initialize ();

   if (Lflag || Cflag || Sflag || CREflag)
      OpenListFile ();

   Cfn = 0;
   np = Argv;
   Pass = 1;
   Line_num = 0;		       /* reset line number */
   while (++Cfn <= N_files)
      if (OpenSourceFile (*++np))
	 make_pass ();
      else
	 fprintf (stderr, "%s: can't open %s\n", argv[0], *np);
   if (Err_count == 0)
   {
      Pass = 2;
      re_init ();
      Cfn = 0;
      np = Argv;
      Line_num = 0;
      while (++Cfn <= N_files)
	 if (OpenSourceFile (*++np))
	    make_pass ();
      if (Sflag == 1)
      {
//          page_feed();
	 if ((ListFile != NULL))
	    fprintf (ListFile, "\n\
*******************************************************************************\n\
*                                SYMBOL TABLE                                 *\n\
*******************************************************************************\n");
	 stable (root);		       /* print symbol table */
      }
      if (CREflag == 1)
      {
//          page_feed();
	 if ((ListFile != NULL))
	    fprintf (ListFile, "\n\
*******************************************************************************\n\
*                            CROSS REFERERCE TABLE                            *\n\
*******************************************************************************\n");
	 cross (root);		       /* print cross reference table */
      }
   }
   termall ();
   if ((Err_count != 0) || (Warn_count != 0))
   {
      fprintf (stderr, "%d errors, %d warnings\n", Err_count, Warn_count);
      if (ListFile != NULL)
	 fprintf (ListFile, "%d errors, %d warnings\n", Err_count, Warn_count);
   }
   CloseListFile ();
   CloseObjectFile (Err_count == 0);
   return (Err_count ? 2 : 0);
}


static void
initialize (void)
{
   int i = 0;
   int j;

#ifdef DEBUG
   fprintf (stderr, "Initializing\n");
#endif
   Err_count = 0;
   Warn_count = 0;
   Pc = 0;
   Pass = 1;
   Ctotal = 0;
   N_page = 0;
   Line[MAXBUF - 1] = NEWLINE;

   condCurrentVal[condNestLevel = 0] = YES;

   macroBeingDefined = NULL;
   macroDefNestLevel = 0;

   OpenObjectFile (Argv[1]);

   strcpy (List_name, Argv[1]);	       /* copy first file name into array */
   j = 0;
   do
   {
      if (List_name[i] == '.')
	 j = i;
   }
   while (List_name[i++] != 0);
   if (j != 0)
      List_name[j] = 0;

   strcat (List_name, ".lst");	       /* append .lst to file name. */
}

static void
re_init (void)
{
#ifdef DEBUG
   fprintf (stderr, "Reinitializing\n");
#endif
   Pc = 0;
   ReinitObject ();
   P_total = 0;
   Ctotal = 0;
   N_page = 0;
   condCurrentVal[condNestLevel = 0] = YES;
}

static void
make_pass (void)
{
   char *fgets ();
   int temp;

#ifdef DEBUG
   fprintf (stderr, "Pass %d\n", Pass);
#endif
   do
   {
      while (GetSourceLine (Line, &temp))
      {
	 Line_num++;
	 P_force = 0;		       /* No force unless bytes emitted */
	 N_page = 0;
	 if (parse_line ())
	    process ();
	 if (Pass == 2 && (Lflag || Cflag) && !N_page)
	    print_line (ListFile);
	 P_total = 0;		       /* reset byte count */
	 Cycles = 0;		       /* and per instruction cycle count */
      }
   }
   while (CloseSourceFile ());
   if (macroDefNestLevel != 0)
      fatal ("MACRO without matching ENDM");
   if (condNestLevel != 0)
      fatal ("IF without matching ENDIF");
}


/*
 *  parse_line --- split input line into label, op and operand
 */
static Boolean
parse_line (void)
{
   register char *ptrfrm = Line;	/* pointer from */
   register char *ptrto = Label;	/* pointer to */


/* check for ';' leading for comments added to following statement
   by Eric Smith, 12/15/89 */

   /* if 1st char is one listed, then just return, since it is a comment line */
   if (
	 /* *ptrfrm == '#' || */
	 *ptrfrm == '*' || *ptrfrm == ';' || *ptrfrm == '\n' || *ptrfrm == '\r')
      return (0);		       /* a comment line */

   if (*ptrfrm == '#')
      ptrfrm++;
   else
   {
/* copy label to ptrto string */
      while (delim (*ptrfrm) == NO)
	 *ptrto++ = *ptrfrm++;

      /* if there is a ":" after the label name then delete it */
      if (*--ptrto != ':')
	 ptrto++;		       /* allow trailing : */
   }
   /* terminate the string */
   *ptrto = EOS;



   /* skip any spaces between the label and the opcode */
   ptrfrm = skip_white (ptrfrm);

/* the following statements have been put into a conditional to handle the case
   of comment lines in which the comment character ('*' or ';') begins in a column
   other than 1
   by Eric Smith, 12/15/89 */

   ptrto = Op;
   if (*ptrfrm == '.')
      ptrfrm++;
   if (*ptrfrm != '*' && *ptrfrm != ';' && *ptrfrm != '\n' &&
       /* *ptrfrm != '#' && 
        *ptrfrm != '.' && */
       *ptrfrm != '\r')
   {
      while (delim (*ptrfrm) == NO)
	 *ptrto++ = mapdn (*ptrfrm++);

      ptrfrm = skip_white (ptrfrm);
   }
   *ptrto = EOS;

/* the following statements have been put into a conditional to handle the case
   of comment lines in which the comment character ('*' or ';') begins in a column
   other than 1
   by Eric Smith, 12/15/89

   "*" taken back out of the conditional because it screws up operands like
   "*+2"
   Eric Smith 9/16/90
*/

   /* if not an comment, then copy to the rest of the line */
   ptrto = Operand;
   if ( /* *ptrfrm != '*' && */ *ptrfrm != ';' && *ptrfrm != '\n'
       && *ptrfrm != '\r')
   {
      while ((*ptrfrm != '\n') && (*ptrfrm != '\r'))
	 *ptrto++ = *ptrfrm++;
   }
   *ptrto = EOS;

/* the following statements have been added to handle the case of a label standing
   alone on a line
   by Eric Smith, 12/15/89 */

   if (*Label != EOS && *Op == EOS)
   {
      strcpy (Op, "equ");
      strcpy (Operand, "*");
   }
#ifdef DEBUG
   fprintf (stderr, "Label-%s-\n", Label);
   fprintf (stderr, "Op----%s-\n", Op);
   fprintf (stderr, "Operand-%s-\n", Operand);
#endif
   return (1);
}

/*
 *  process --- determine mnemonic class and act on it
 */
static void
process (void)
{
   register oper *i;

   Old_pc = Pc;			       /* setup `old' program counter */
   Optr = Operand;		       /* point to beginning of operand field */

   if (*Op == EOS)
   {				       /* no mnemonic */
      if (*Label != EOS)
	 install (Label, Pc, NO);
   }
   else if ((i = mne_look (Op)) == NULL)
   {
      if (condCurrentVal[condNestLevel])
      {
	 if (table == 0)
	    error ("No MACHINE or MACH directive given");
	 else
	    error ("Unrecognized Mnemonic");
      }
   }
   else if (condCurrentVal[condNestLevel] || ((i->class == PSEUDO) && ((i->opcode & DOFCOND) == DOFCOND)))
   {
      /* if there is a label and we're not in a false conditional and
         this isn't a special pseudo-op, install the label */

      if ((*Label) && (condCurrentVal[condNestLevel]) && ((i->class != PSEUDO) || (!(i->opcode & NOLABEL))))
	 install (Label, Pc, NO);

      if (i->class != PSEUDO)
      {
	 Cycles = i->cycles & 0xff;
	 Cycle_adjust = i->cycles & 0xff00;
      }

      if (i->class != PSEUDO)
	 (*do_op) (i->opcode, i->class);
      else if ((i->opcode & PROCPSEUDO) == PROCPSEUDO)
	 (*do_op) (i->opcode & 0xff, i->class);
      else
	 do_pseudo (i->opcode & 0xff);

      if ((i->class != PSEUDO) && Cflag)
	 Ctotal += Cycles;
   }
   else
   {
      /* conditional false, do nothing */
   }
}
