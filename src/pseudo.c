/*
 * pseudo.c - pseudo-op handling
 *
 * $Id: pseudo.c 13 2004-02-11 06:09:11Z eric $
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

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "as.h"



oper pseudo[] = {
   {"bsz", 0, PSEUDO, ZMB, 0, 0},
   {"card", 0, PSEUDO, MACHINE, 0, 0},
   {"cpu", 0, PSEUDO, MACHINE, 0, 0},
   {"db", 0, PSEUDO, FCB, 0, 0},
   {"define", 0, PSEUDO, DEF, 0, 0},
   {"device", 0, PSEUDO, MACHINE, 0, 0},
   {"ds", 0, PSEUDO, RMB, 0, 0},
   {"dw", 0, PSEUDO, FDB, 0, 0},
   {"else", 0, PSEUDO, ELSE + DOFCOND, 0, 0}, // else
   {"end", 0, PSEUDO, NULL_OP, 0, 0},
   {"endif", 0, PSEUDO, ENDIF + DOFCOND, 0, 0}, // end if
//  {"endm", 0, PSEUDO, ENDM, 0, 0} ,
   {"epx", 0, PSEUDO, EPX, 0, 0},
   {"equ", 0, PSEUDO, EQU + NOLABEL, 0, 0},  // equate
   {"equb", 0, PSEUDO, EQUB + NOLABEL, 0, 0},  // equate bit?
   {"error", 0, PSEUDO, ERROR, 0, 0},
   {"error1", 0, PSEUDO, ERROR1, 0, 0},
   {"error2", 0, PSEUDO, ERROR2, 0, 0},
   {"fatal", 0, PSEUDO, FATAL, 0, 0},
   {"fcb", 0, PSEUDO, FCB, 0, 0},
   {"fcc", 0, PSEUDO, FCC, 0, 0},
   {"fcs", 0, PSEUDO, FCS, 0, 0},
   {"fdb", 0, PSEUDO, FDB, 0, 0},
   {"fill", 0, PSEUDO, FILL, 0, 0},
   {"if", 0, PSEUDO, IF + DOFCOND, 0, 0},  // if
   {"if1", 0, PSEUDO, IF1 + DOFCOND, 0, 0},  // if pass 1
   {"if2", 0, PSEUDO, IF2 + DOFCOND, 0, 0},  // if pass 2
   {"ifdef", 0, PSEUDO, IFDEF + DOFCOND, 0, 0},  // if defined
   {"ife", 0, PSEUDO, IFE + DOFCOND, 0, 0},  // if equal
   {"ifndef", 0, PSEUDO, IFNDEF + DOFCOND, 0, 0},  // if not defined
   {"ift", 0, PSEUDO, IF + DOFCOND, 0, 0},  // if true
   {"include", 0, PSEUDO, INCLUDE, 0, 0},
   {"mach", 0, PSEUDO, MACHINE, 0, 0},
   {"machine", 0, PSEUDO, MACHINE, 0, 0},
//  {"macro", 0, PSEUDO, MACRO, 0, 0} ,
   {"nam", 0, PSEUDO, NULL_OP, 0, 0},
   {"name", 0, PSEUDO, NULL_OP, 0, 0},
   {"opt", 0, PSEUDO, OPT, 0, 0},
   {"org", 0, PSEUDO, ORG, 0, 0},
   {"pag", 0, PSEUDO, PAGE + DOFCOND, 0, 0},
   {"page", 0, PSEUDO, PAGE + DOFCOND, 0, 0},
   {"print", 0, PSEUDO, PRINT, 0, 0},
   {"print1", 0, PSEUDO, PRINT1, 0, 0},
   {"print2", 0, PSEUDO, PRINT2, 0, 0},
   {"rmb", 0, PSEUDO, RMB, 0, 0},
   {"rom", 0, PSEUDO, ROM, 0, 0},
   {"rom10", 0, PSEUDO, ROM10, 0, 0},
   {"rom11", 0, PSEUDO, ROM11, 0, 0},
   {"rom2", 0, PSEUDO, ROM2, 0, 0},
   {"rom3", 0, PSEUDO, ROM3, 0, 0},
   {"set", 0, PSEUDO, SET + NOLABEL, 0, 0},
   {"spc", 0, PSEUDO, NULL_OP, 0, 0},
   {"ttl", 0, PSEUDO, NULL_OP, 0, 0},
   {"undef", 0, PSEUDO, UNDEF, 0, 0},
   {"undefine", 0, PSEUDO, UNDEF, 0, 0},
   {"unset", 0, PSEUDO, UNDEF, 0, 0},
   {"warn", 0, PSEUDO, WARN, 0, 0},
   {"warn1", 0, PSEUDO, WARN1, 0, 0},
   {"warn2", 0, PSEUDO, WARN2, 0, 0},
   {"zmb", 0, PSEUDO, ZMB, 0, 0}
};

int NPSE = (sizeof (pseudo) / sizeof (oper));


/*
 *	do_include --- do include pseudo op processing
 */
void
do_include (void)
{
   char delim;
   char filename[MAXFILENAMELEN];
   char *tempstr = &filename[0];
   Optr = skip_white (Optr);
   delim = *Optr++;
   while (*Optr != EOS && *Optr != delim)
      *tempstr++ = *Optr++;
   *tempstr = 0;
   if (*Optr != delim)
      error ("Missing Delimiter");
   else
   {
      Optr++;
      if (!OpenSourceFile (filename))
	 error ("Unable to open include file");
   }
}

void
print_str (void)
{
   char strdelim;		// string delimiter
   if (*Operand == EOS)
      error ("Blank String");
   strdelim = *Optr++;
   while (*Optr != EOS && *Optr != strdelim)
      Optr++;
   if (*Optr == strdelim)
      *Optr = EOS;
   else
      error ("Missing Delimiter");
   fprintf (stdout, "%s\n", (Operand + 1));
}

/*
 *	do_pseudo --- do pseudo op processing
 */
void
do_pseudo (int op /* which op */ )
{
   char fccdelim;
   int fill;
   char ident[MAXBUF];
   unsigned int i;
   char *skip_white ();

   P_force++;
   switch (op)
   {
     case RMB:			       /* reserve memory bytes */
	if (eval ())
	   Pc += Result;
	else
	   error ("Undefined Operand during Pass One");
	break;
     case ZMB:			       /* zero memory bytes */
	if (eval ())
	{
	   while (Result--)
	      emit (0);
	}
	else
	   error ("Undefined Operand during Pass One");
	break;
     case FILL:		       /* fill memory with constant */
	eval ();
	fill = Result;

	if (*Optr == ',' || *Optr == ' ')
	{
	   Optr++;
	   Optr = skip_white (Optr);
	   eval ();
	   while (Result--)
	      emit (fill);
	}
	else
	   error ("Bad fill");
	break;
     case FCB:			       /* form constant byte(s) */
	do
	{
	   Optr = skip_white (Optr);
	   if ((*Optr == '"') || (*Optr == '\''))
	   {
	      fccdelim = *Optr++;
	      while (*Optr != EOS && *Optr != fccdelim)
		 emit (*Optr++);
	      if (*Optr == fccdelim)
		 Optr++;
	      else
		 error ("Missing Delimiter");
	   }
	   else
	   {
	      eval ();
	      if (Result > 0xFF)
	      {
		 if (!Force_byte)
		    warn ("Value truncated");
		 Result = lobyte (Result);
	      }
	      emit (Result);
	   }
	}
	while (*Optr++ == ',');
	break;
     case FDB:			       /* form double byte(s) */
	do
	{
	   Optr = skip_white (Optr);
	   eval ();
	   eword (Result);
	}
	while (*Optr++ == ',');
	break;
     case FCC:			       /* form constant characters */
	if (*Operand == EOS)
	   break;
	fccdelim = *Optr++;
	while (*Optr != EOS && *Optr != fccdelim)
	   emit (*Optr++);
	if (*Optr == fccdelim)
	   Optr++;
	else
	   error ("Missing Delimiter");
	break;
     case FCS:			       /* form constant characters, high bit set on last */
	if (*Operand == EOS)
	   break;
	fccdelim = *Optr++;
	while (*Optr != EOS && *Optr != fccdelim)
	{
	   if ((*(Optr + 1) == EOS) || (*(Optr + 1) == fccdelim))
	      emit ((*Optr++) | 0x80);
	   else
	      emit (*Optr++);
	}
	if (*Optr == fccdelim)
	   Optr++;
	else
	   error ("Missing Delimiter");
	break;
     case ORG:			       /* origin */
	if (eval ())
	   Old_pc = Pc = Result;
	else
	   error ("Undefined Operand during Pass One");
	break;
     case EQU:			       /* equate */
     case SET:
	if (*Label == EOS)
	{
	   error ("EQU requires label");
	   break;
	}
	if (eval ())
	{
	   install (Label, Result, (op == SET));
	   Old_pc = Result;	       /* override normal */
	}
	else
	   error ("Undefined Operand during Pass One");
	break;
     case EQUB:
	if (*Label == EOS)
	{
	   error ("EQUB requires label");
	   break;
	}
	if (!eval ())
	{
	   error ("Undefined Operand during Pass One");
	   break;
	}
	i = Result;
	if (*Optr++ != ',')
	{
	   error ("missing bit number");
	   break;
	}
	if (!eval ())
	{
	   error ("Undefined Operand during Pass One");
	   break;
	}
	install_bit (Label, i, Result);
	break;
     case DEF:			       /* define */
	i = 0;
	while (!delim (ident[i++] = *Optr++))
	{
	}
	ident[--i] = 0;
	if (eval ())
	{
	   install (ident, Result, YES);
	   Old_pc = Result;	       /* override normal */
	}
	else
	   error ("Undefined Operand during Pass One");
	break;
     case OPT:			       /* assembler option */
	P_force = 0;
	if (head (Operand, "l"))
	   Lflag = 1;
	else if (head (Operand, "nol"))
	   Lflag = 0;
	else if (head (Operand, "rom2"))
	   selectproc ("rom2", 1);
	else if (head (Operand, "rom3"))
	   selectproc ("rom3", 1);
	else if (head (Operand, "rom10"))
	   selectproc ("rom10", 1);
	else if (head (Operand, "rom11"))
	   selectproc ("rom11", 1);
	else if (head (Operand, "64k"))	// select 64k binary output file
	   SetObjectFormat (F_BIN_64K, 1);
	else if (head (Operand, "4k")) // output 4k binary file
	   SetObjectFormat (F_BIN_4K, 1);
	else if (head (Operand, "8k")) // output 8k binary file
	   SetObjectFormat (F_BIN_8K, 1);
	else if (head (Operand, "s19"))	// output motorola .s19 file
	   SetObjectFormat (F_MOT_S19, 1);
	else if (head (Operand, "hex"))	// output intel .hex file
	   SetObjectFormat (F_INTEL_HEX, 1);
	else if (head (Operand, "c"))  // enable cycle counting
	{
	   Cflag = 1;
	   Ctotal = 0;
	}
	else if (head (Operand, "noc"))	// disable cycle counting
	{
	   Cflag = 2;
	}
	else if (head (Operand, "s"))  // enable symbol table
	   Sflag = 1;
	else if (head (Operand, "cre"))	// enable cross reference table
	   CREflag = 1;
	else
	   error ("Unrecognized OPT");
	if (Lflag || Sflag || CREflag)
	   OpenListFile ();
	break;
     case LIST:
	P_force = 0;
	Lflag = 1;
	break;
     case NOLIST:
	P_force = 0;
	Lflag = 0;
	break;
     case PAGE:		       /* go to a new page */
	P_force = 0;
	N_page = 1;
	if (Pass == 2)
	   if (Lflag)
	      page_feed ();
	break;
     case NULL_OP:		       /* ignored psuedo ops */
	P_force = 0;
	break;
     case ROM:
	P_force = 1;
	eval ();
	switch (Result)
	{
	  case 2:
	     selectproc ("ROM2", 1);
	     break;
	  case 3:
	     selectproc ("ROM3", 1);
	     break;
	  case 10:
	     selectproc ("ROM10", 1);
	     break;
	  case 11:
	     selectproc ("ROM11", 1);
	     break;
	  default:
	     error ("unknown ROM type");
	}
	break;
     case ROM2:
	P_force = 1;
	selectproc ("ROM2", 1);
	break;
     case ROM3:
	P_force = 1;
	selectproc ("ROM3", 1);
	break;
     case ROM10:
	P_force = 1;
	selectproc ("ROM10", 1);
	break;
     case ROM11:
	P_force = 1;
	selectproc ("ROM11", 1);
	break;

     case MACHINE:		       /* select CPU type */
	P_force = 1;
	i = 0;
	for (i = 0; i < strlen (Optr); i++)
	{
	   ident[i] = *(Optr + i);
	   if (delim (ident[i]))
	      break;
	}
	ident[i] = EOS;

	if (!selectproc (ident, 1))
	   error ("Unrecognized machine type");
	break;

     case IF:			       /* conditional */
     case IFE:
	P_force = 0;
	if (condNestLevel >= kMaxCondNestLevel)
	{
	   error ("Conditionals nested too deeply");
	   break;
	}
	condElseFound[++condNestLevel] = NO;
	i = 0;

	if (isalpha (*Optr) || *Optr == '_' || *Optr == '.')	/* see if 1st char is a legal label start */
	{
	   for (i = 0; i < strlen (Optr); i++)	/* if it looks like a label, then copy it */
	   {
	      ident[i] = *(Optr + i);
	      if (delim (ident[i]))    /* stop at end of operand */
		 break;
	   }
	   ident[i] = EOS;	       /* make it a legit string */
	   fill = strlen (ident);
	   for (i = 1; i < strlen (ident); i++)
	   {
	      if (!(isalnum (ident[i]) || ident[i] == '.' || ident[i] == '_' || ident[i] == '$'))
		 ident[i] = 0;
	   }

	   if (i == fill)	       /* if original and new length are the same, then it must be just a label */
	   {
	      if (lookup_ne (ident) == NULL)
		 Result = 0;
	      else
	      {
		 if (!eval ())
		    Result = 0;
	      }
	   }
	   else if (!eval ())
	      Result = 0;
	}
	else /* if not letter, then it must be an expression, so evaluate it */ if (!eval ())
	   Result = 0;
	i = (Result == 0) ^ (op == IF);
	condCurrentVal[condNestLevel] = (condCurrentVal[condNestLevel - 1] & i);
#ifdef CONDTEST
	printf ("%d IF %d %d %d\n", condNestLevel, i, condCurrentVal[condNestLevel - 1], condCurrentVal[condNestLevel]);
#endif
	Old_pc = Result;	       /* override normal */
	break;

     case IFDEF:
     case IFNDEF:
	P_force = 0;
	if (condNestLevel >= kMaxCondNestLevel)
	{
	   error ("Conditionals nested too deeply");
	   break;
	}
	condElseFound[++condNestLevel] = 0;
	i = (lookup_ne (Optr) != NULL) ^ (op == IFNDEF);
	condCurrentVal[condNestLevel] = (condCurrentVal[condNestLevel - 1] & i);
#ifdef CONDTEST
	printf ("%d IFDEF %d %d %d\n", condNestLevel, i, condCurrentVal[condNestLevel - 1], condCurrentVal[condNestLevel]);
#endif
	Old_pc = i;		       /* override normal */
	break;
     case IF1:
     case IF2:
	P_force = 0;
	if (condNestLevel >= kMaxCondNestLevel)
	{
	   error ("Conditionals nested too deeply");
	   break;
	}
	condElseFound[++condNestLevel] = 0;
	i = (Pass == 1) ^ (op == IF2);
	condCurrentVal[condNestLevel] = (condCurrentVal[condNestLevel - 1] & i);
#ifdef CONDTEST
	printf ("%d IF1 %d %d %d\n", condNestLevel, i, condCurrentVal[condNestLevel - 1], condCurrentVal[condNestLevel]);
#endif
	Old_pc = i;		       /* override normal */
	break;
     case ELSE:		       /* conditional */
	P_force = 0;
	if (condNestLevel == 0)
	{
	   error ("ELSE without IF");
	   break;
	}
	if (condElseFound[condNestLevel])
	{
	   error ("More than one ELSE found for one IF");
	   break;
	}
	condElseFound[condNestLevel] = 1;
	condCurrentVal[condNestLevel] = condCurrentVal[condNestLevel - 1] & !condCurrentVal[condNestLevel];
#ifdef CONDTEST
	printf ("%d ELSE %d\n", condNestLevel, condCurrentVal[condNestLevel]);
#endif
	break;
     case ENDIF:		       /* conditional */
	P_force = 0;
	if (condNestLevel == 0)
	{
	   error ("ENDIF without IF");
	   break;
	}
#ifdef CONDTEST
	printf ("%d ENDIF\n", condNestLevel);
#endif
	condNestLevel--;
	break;

     case INCLUDE:
	P_force = 0;
	do_include ();
	break;

     case PRINT:
	P_force = 0;
	print_str ();
	break;

     case PRINT1:
	P_force = 0;
	if (Pass == 1)
	   print_str ();
	break;

     case PRINT2:
	P_force = 0;
	if (Pass == 2)
	   print_str ();
	break;

     case WARN:
	P_force = 0;
	warn (Optr);
	break;

     case WARN1:
	P_force = 0;
	if (Pass == 1)
	   warn (Optr);
	break;

     case WARN2:
	P_force = 0;
	if (Pass == 2)
	   warn (Optr);
	break;

     case ERROR:
	P_force = 0;
	error (Optr);
	break;

     case ERROR1:
	P_force = 0;
	if (Pass == 1)
	   error (Optr);
	break;

     case ERROR2:
	P_force = 0;
	if (Pass == 2)
	   error (Optr);
	break;

     case FATAL:
	P_force = 0;
	if (*Operand == EOS)
	   error ("Blank String");
	fccdelim = *Optr++;
	while (*Optr != EOS && *Optr != fccdelim)
	   Optr++;
	if (*Optr == fccdelim)
	   *Optr = EOS;
	else
	   error ("Missing Delimiter");
	fatal (Operand + 1);
	break;

     case UNDEF:
	P_force = 0;
	i = 0;
	for (i = 0; i < strlen (Optr); i++)
	{
	   ident[i] = *(Optr + i);
	   if (delim (ident[i]))
	      break;
	}
	ident[i] = EOS;

	if (lookup_ne (ident) != NULL)
	   undefine (ident);
	break;
/*
    case MACRO:
      P_force = 0;
      if (macroDefNestLevel >= kMaxMacroDefNestLevel)
	{
	  error ("Macro definitions nested too deeply");
	  break;
	}
      if (*Label == EOS)
	{
	  error ("EQU requires label");
	  break;
	}
			if (FindMacro (Label) != NULL)
			{
				error ("Duplicate macro definition");
				break;
			}
      macroBeingDefined = CreateMacro (Label, Operand);
      macroDefNestLevel++;
      break;
    case ENDM:
      P_force = 0;
      if (macroDefNestLevel == 0)
	{
	  error ("ENDM without MACRO");
	  break;
	}
      macroBeingDefined = NULL;
      macroDefNestLevel--;
      break;
*/
     case EPX:			       /* print text directly to the object file */
	P_force = 1;
	if (Pass == 2)
	{
	   if (*Operand == EOS)	       /* if no string, then exit */
	      break;
	   dump_record ();	       /* flush the output */
	   fccdelim = *Optr++;	       /* get delimiter */
	   while (*Optr != EOS && *Optr != fccdelim)	/* while not end of string, or 2nd delimiter */
	      emit_char (*Optr++);     /* then print this character to the output file */
	   emit_char ('\n');

	   if (*Optr == fccdelim)
	      Optr++;
	   else
	      error ("Missing Delimiter");
	}
	break;
     default:
	fatal ("Unrecognized pseudo-op");
   }
}
