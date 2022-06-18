/*
 * ast7.c - 6805 and ST7 support
 *
 * $Id: ast7.c 13 2004-02-11 06:09:11Z eric $
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
#include "as.h"


/*
 *	ST7 specific processing
 *	PDY 90 = Prefix, Direct Y
 *	PIY 91 = Prefix, (Indirect and Index Y)
 *	PIX 92 = Prefix, (Indirect) or (Indirect and Index X)
 */

/* opcode classes */
#define INH		1	       /* Inherent addressing                          */
#define INHY		2	       /* Inherent addressing on register Y            */
#define GEN		3	       /* General Addressing                           */
#define GEN2		4	       /* Group 2 (Read/Modify/Write)                  */
#define GENOY		5	       /* General Addressing, no index of Y            */
#define GENOX		6	       /* General Addressing, no index of X            */
#define GENOI		7	       /* General Addressing, no Immediate             */
#define GENOIY		8	       /* General except for Immediate, no index of Y  */
#define GENOIX		9	       /* General except for Immediate, no index of X  */
#define REL		10	       /* Short Relative                               */
#define BTB		11	       /* Bit test and branch                          */
#define BTB2		12	       /* Bit test and branch, alternate syntax        */
#define SETCLR		13	       /* Bit set or clear                             */
#define SETCLR2		14	       /* Bit set or clear                             */
#define TFR		15	       /* transfer register to register (6809 opcode)  */
#define INH9		16	       /* Inherent addressing, ST7 only           */
#define INHS		17	       /* Inherent addressing, ST7 only           */
#define POP		18	       /* pop, ST7 only                           */
#define PUSH		19	       /* push, ST7 only                          */

#define PREFIX_DIRECT_Y			0x9000
#define PREFIX_INDEX_Y			0x9000
#define PREFIX_INDIRECT_Y		0x9100
#define PREFIX_INDIRECT			0x9200
#define PREFIX_INDIRECT_X		0x9200


/* addressing modes */
#define INDEXX		0x01	       /* indexed */
#define INDEXY		0x02	       /* indexed */
#define INDIRECT	0x04	       /* indirect */
#define IMMED		0x08	       /* immediate */
#define OTHER		0x10	       /* NOTA */

/* indirect modes */
#define IERROR		-1
#define ISHORT		0
#define ILONG		1

/*
 * prefix 90 = same as normal, except all "x" are changed to "y"
 * prefix 91 = same as 90, except indirect addressing
 * prefix 92 = same as normal, except indirect addressing
 */

/* processor variants */
#define HMOS		0
#define MC6805		4
#define ST7		8


/*============================================================
 *	functions
  ============================================================*/


/*
 *	localinitst7 --- machine specific initialization
 */
static void far
localinitst7 (void)
{
}

/*
 *  localtermst7 --- machine specific termination
 */
static void far
localtermst7 (void)
{
}

/*
 *  localselectst7 --- machine specific selection
 */
static Boolean far
localselectst7 (char *proc, char *arg)
{
   if (arg == NULL)
   {
      LowByteFirst = 0;
      ByteSelOps = 0;
      return (YES);
   }
   return (NO);
}

/*
 *  localdeselect5 --- machine specific deselection
 */
static void far
localdeselectst7 (void)
{
}

/*
 * eval_ind() -- ealuate indirect expression
 *
 * short indirect
 * [$9D]
 * [00:$9D]
 * [$9D.b]
 * [$9D.s]
 *
 * long indirect
 * [$9D:$9D+1]
 * [$9D:$9E]
 * [$9D.w]
 * [$9D.l]
 */
static int
eval_ind ()
{
   int i;

   if (*Optr++ != '[')
   {
      if (Pass == 1)
	 error ("Not an indirect operand");
      return (IERROR);
   }

   eval ();
   i = Result & 0xFFFF;
   if (i > 0xFF)
   {
      if (Pass == 1)
	 error ("Indirect address must be in zero page");
      return (IERROR);
   }

   switch (*Optr++)
   {
     case ']':
	return (ISHORT);
     case '.':
	switch (mapdn (*Optr++))
	{
	  case 'b':		       /* byte */
	  case 's':		       /* short */
	     if (*Optr++ == ']')
		return (ISHORT);
	     else if (Pass == 1)
		error ("Malformed indirect expression");
	     return (IERROR);
	  case 'l':		       /* long */
	  case 'w':		       /* word */
	  case 'd':		       /* double */
	     if (*Optr++ == ']')
		return (ILONG);
	     else if (Pass == 1)
		error ("Malformed indirect expression");
	     return (IERROR);
	  default:
	     if (Pass == 1)
		error ("Malformed indirect length modifier");
	     return (IERROR);
	}
     case ':':
	eval ();		       /* evaluate 2nd half of the indirect operand */

	if (*Optr++ != ']')	       /* if the next character don't close it, then error */
	   if (Pass == 1)
	      error ("Malformed indirect expression");

	if (i == 0)		       /* if the 1st half, is 00, then it is short */
	   return (ISHORT);

	if (Result - i != 1)	       /* if the 2 bytes aren't sequential, then error */
	   if (Pass == 1)
	      error ("Indirect word bytes must be sequential");

	Result = i;		       /* because the 1st half is what matters */
	return (ILONG);
   }
   return (IERROR);
}

/*
 * check for the following syntax:
 * op X = indexed X
 * op ,X = indexed X
 * op Y = indexed Y
 * op ,Y = indexed Y
 * 
 * op [????],X = indirect, indexed X
 * op [????],Y = indirect, indexed Y
 * 
 * op [xx] = indirect short
 * op [00:xx] = indirect short
 * op [xx.b] = indirect short (byte)
 * op [xx.s] = indirect short
 * 
 * op [xx:??] = indirect long
 * op [xx.w] = indirect long (word or wide)
 * op [xx.l] = indirect long
 *
 * level of priority, 1st one to match is returned
 * 1. '#' as 1st char, return IMMED (immediate)
 * 2. '[' anywhere in the operand, return INDIRECT
 * 3. 'x' or 'y' as operand, return INDEXX or INDEXY (indexed)
 * 4. ',' anywhere in the operand, return INDEXX (indexed)
 * 5. everything else returns OTHER
 *
 * returns IMMED
 *         INDIRECT
 *         INDEXX
 *         INDEXY
 *         OTHER
 * 
 *
 *	prefix 90 = same as normal, except all "x" are changed to "y"
 *	prefix 91 = same as 90, except indirect addressing
 *	prefix 92 = same as normal, except indirect addressing
 * index of Y = prefix 90
 * indirect, AND with index of X = prefix 92
 * indirect, AND index of Y = prefix 91
 * 
 */


/*
 * set_address_mode - returns the current guess as to the current addressing mode
 */

static int
set_address_mode (void)
{
   char *peek;

   /* guess at addressing mode */
   peek = Optr;

/********** see if we have an immediate operand **********/
   if (*Optr == '#')
      return (IMMED);		       // if 1st char of operand is "#", then we're in immediate mode

/********** see if an indirect mode was indicated and check for unbalanced brackets **********/
   while (!delim (*peek) && *peek != EOS)	// check for '[' in operand field
      if (*peek++ == '[')
      {
	 // we found the 1st '[', so lets see if we have the matching one
	 while (!delim (*peek) && *peek != EOS)	// check for matching ']'
	    if (*peek++ == ']')	       // we found it, so return
	       return (INDIRECT);
	 // if we didn't find the ']' by the time we get here, then spit out an error
	 if (Pass == 1)
	    error ("Missing ']'");
	 return (INDIRECT);
      }

   // check for ']' in operand field, which would indicate unbalanced braces
   peek = Optr;
   while (!delim (*peek) && *peek != EOS)
      if (*peek++ == ']')
      {
	 if (Pass == 1)
	    error ("Missing '['");
	 return (INDIRECT);
      }

/********** take a guess, and see if we're in an indexed mode **********/
   peek = Optr;

   if (*peek == ',' || *peek == '$')   // see if we have something like "lda ,x" or "lda $x"
      peek++;

   switch (mapdn (*peek++))
   {
     case 'x':
	if (delim (*peek))	       // see if the "x" or "y" is the only thing for that operand
	   return (INDEXX);
	break;
     case 'y':
	if (delim (*peek))	       // see if the "x" or "y" is the only thing for that operand
	   return (INDEXY);
	break;
   }

   /* see if we're in indexed mode, without a comma */
   if (head (Operand, "x"))	       // see if we're in index mode, with just "x" or "y" as the only operand
      if (delim (*(Operand + 1)))      // see if the "x" or "y" is the only thing for that operand
	 return (INDEXX);

   if (head (Operand, "y"))	       // see if we're in index mode, with just "x" or "y" as the only operand
      if (delim (*(Operand + 1)))
      {				       // see if the "x" or "y" is the only thing for that operand
	 if (!(proc_variant & ST7))
	    if (Pass == 1)
	       error ("The 6805 does not have a Y register");
	 return (INDEXY);
      }

/********** see if an indexed mode was indicated **********/
   peek = Optr;
   while (!delim (*peek) && *peek != EOS)	// check for comma in operand field
      if (*peek++ == ',')
      {
	 if (delim (*peek))
	    peek++;
	 switch (mapdn (*peek))
	 {
	   case 'x':
	      return (INDEXX);
	   case 'y':
	      if (!(proc_variant & ST7))
		 if (Pass == 1)
		    error ("The 6805 does not have a Y register");
	      return (INDEXY);
	   default:
	      return (INDEXX);
	 }
      }

/********** no match, so return the default **********/
   return (OTHER);
}

/*
 *	do_opst7 --- process mnemonic
 *
 *  Called with the base opcode and it's class. Optr points to
 *  the beginning of the operand field.
 *  
 *	prefix 90 = same as normal, except all "x" are changed to "y"
 *	prefix 91 = same as 90, except indirect addressing
 *	prefix 92 = same as normal, except indirect addressing
 */
static void far
do_opst7 (int opcode, int class)
{				/* base opcode, mnemonic class */
   int dist;			/* relative branch distance */
   int amode;			/* indicated addressing mode */
   int indir = 0;		/* indirect length */
   int index = 0;		/* index type */
   int temp;

//      printf("do_opst7: opcode = %.4X Operand = %s class = %.2X Optr = %s\n",opcode, Operand, class, Optr);
   amode = set_address_mode ();

/********** go emit opcodes **********/
   switch (class)
   {
     case INHY:		       // extended inherent addressing
	if (!(proc_variant & ST7))
	{
	   if (Pass == 1)
	      error ("The 6805 does not have a Y register");
	}
	else
	   eword (PREFIX_DIRECT_Y | opcode);
	return;
     case INHS:		       // inherent addressing using register S
	if (!(proc_variant & ST7))
	{
	   if (Pass == 1)
	      error ("The 6805 does not support register S in this manner");
	   return;
	}
     case INH9:		       // inherent addressing with Y register
	if (!(proc_variant & ST7))
	{
	   if (Pass == 1)
	      error ("The 6805 does not have a Y register");
	   return;
	}
     case INH:			       // inherent addressing
	emit (opcode);
	return;
     case POP:
     case PUSH:
	switch (mapdn (*Optr++))
	{
	  case 'a':
	     if (delim (*Optr))
		emit (opcode);
	     else if (Pass == 1)
		error ("Malformed push/pop opcode");
	     return;
	  case 'x':
	     if (delim (*Optr))
		emit (opcode + 1);
	     else if (Pass == 1)
		error ("Malformed push/pop opcode");
	     return;
	  case 'y':
	     Cycles++;
	     if (delim (*Optr))
		eword (PREFIX_DIRECT_Y + opcode + 1);
	     else if (Pass == 1)
		error ("Malformed push/pop opcode");
	     return;
	  case 'c':
	     if (delim (*(Optr + 1)) && *Optr == 'c')
		emit (opcode + 2);
	     else if (Pass == 1)
		error ("Malformed push/pop opcode");
	     return;
	  case 's':
	     if (Pass == 1)
		error ("push/pop not supported for register S");
	     return;
	  default:
	     if (Pass == 1)
		error ("Malformed push/pop opcode");
	     return;
	}
     case TFR:
	temp = mapdn (*Optr++);
	if (*Optr++ != ',')
	   if (Pass == 1)
	      error ("malformed TFR opcode");
	if (!delim (*(Optr + 1)))
	   if (Pass == 1)
	      error ("malformed TFR opcode");

	switch (temp)
	{
	  case 'a':
	     switch (mapdn (*Optr))
	     {
	       case 's':
		  if (proc_variant & ST7)
		     emit (0x95);
		  else if (Pass == 1)
		     error ("The 6805 does not support A to S transfers");
		  return;
	       case 'x':
		  emit (0x97);
		  return;
	       case 'y':
		  if (proc_variant & ST7)
		  {
		     eword (0x9097);
		     Cycles++;
		  }
		  else if (Pass == 1)
		     error ("The 6805 does not have a Y register");
		  return;
	       case 'a':
		  if (Pass == 1)
		     error ("Can not transfer A to A");
		  return;
	       default:
		  if (Pass == 1)
		     error ("Unknown register");
		  return;
	     }
	  case 's':
	     switch (mapdn (*Optr))
	     {
	       case 'a':
		  emit (0x9E);
		  return;
	       case 'x':
		  if (proc_variant & ST7)
		     emit (0x96);
		  else if (Pass == 1)
		     error ("The 6805 does not support S to X transfers");
		  return;
	       case 'y':
		  if (proc_variant & ST7)
		  {
		     eword (0x9096);
		     Cycles++;
		  }
		  else if (Pass == 1)
		     error ("The 6805 does not have a Y register");
		  return;
	       case 's':
		  if (Pass == 1)
		     error ("Can not transfer S to S");
		  return;
	       default:
		  if (Pass == 1)
		     error ("Unknown register");
		  return;
	     }
	  case 'x':
	     switch (mapdn (*Optr))
	     {
	       case 'a':
		  emit (0x9F);
		  return;
	       case 's':
		  if (proc_variant & ST7)
		     emit (0x94);
		  else if (Pass == 1)
		     error ("The 6805 does not support X to S transfers");
		  return;
	       case 'y':
		  if (proc_variant & ST7)
		  {
		     eword (0x9093);
		     Cycles++;
		  }
		  else if (Pass == 1)
		     error ("The 6805 does not have a Y register");
		  return;
	       case 'x':
		  if (Pass == 1)
		     error ("Can not transfer X to X");
		  return;
	       default:
		  if (Pass == 1)
		     error ("Unknown register");
		  return;
	     }
	     return;
	  case 'y':
	     if (!(proc_variant & ST7))
	     {
		if (Pass == 1)
		   error ("The 6805 does not have a Y register");
	     }
	     else
		switch (mapdn (*Optr))
		{
		  case 'a':
		     eword (0x909F);
		     Cycles++;
		     return;
		  case 's':
		     eword (0x9094);
		     Cycles++;
		     return;
		  case 'x':
		     emit (0x93);
		     return;
		  case 'y':
		     if (Pass == 1)
			error ("Can not transfer Y to Y");
		     return;
		  default:
		     if (Pass == 1)
			error ("Unknown register");
		     return;
		}
	     return;
	  default:
	     if (Pass == 1)
		error ("Unknown register");
	     return;
	}
     case REL:			       /* short relative branches */
	if (amode == INDIRECT)
	{
	   if (!(proc_variant & ST7))
	      if (Pass == 1)
		 error ("The 6805 does not support indirect addressing");
	   if (eval_ind () != ISHORT)
	      if (Pass == 1)
		 error ("Long Indirect addressing mode not allowed");
	   eword (PREFIX_INDIRECT | opcode);
	   Cycles += 2;
	   emit (Result);
	}
	else
	{
	   if (eval ())
	      dist = Result - (Pc + 2);
	   else
	      dist = -2;
	   emit (opcode);
	   if ((dist > 127 || dist < -128) && Pass == 2)
	   {
	      error ("Branch out of range");
	      dist = -2;
	   }
	   emit (lobyte (dist));
	}
	return;
     case GENOIX:		       /* general, no X index, no immediate */
	if (!(proc_variant & ST7))
	   if (Pass == 1)
	      error ("The 6805 does not have a Y register");
     case GENOIY:		       /* general, no Y index, no immediate */
     case GENOI:		       /* general, no immediate */
     case GEN2:		       /* general, no immediate, and no long */
	if (amode == IMMED)
	{
	   if (Pass == 1)
	      error ("Immediate Addressing Illegal");
	   return;
	}

     case GENOX:		       /* general addressing, no X index */
	if (!(proc_variant & ST7) && amode == GENOX)
	   if (Pass == 1)
	      error ("The 6805 does not have a Y register");
     case GENOY:		       /* general addressing, no Y index */
	if ((class == GENOX || class == GENOIX) && amode == INDEXX)
	{
	   if (Pass == 1)
	      error ("Illegal Index of X");
	   return;
	}
	if ((class == GENOY || class == GENOIY) && amode == INDEXY)
	{
	   if (Pass == 1)
	      error ("Illegal Index of Y");
	   return;
	}

     case GEN:			       /* general addressing */
	switch (amode)
	{
				/********** process general IMMEDIATE address mode **********/
	  case IMMED:
	     opcode = opcode - 0x10;   /* adjust opcode for an immediate addressing mode */

	     if (class == GENOX)
	     {
		eword (PREFIX_DIRECT_Y | opcode);
		Cycles++;
	     }
	     else
		emit (opcode);

	     Optr++;
	     eval ();
	     emit (lobyte (Result));
	     return;

				/********** process INDEXed address modes **********/
				/********* INDIRECT address modes **********/
				/********** OTHER address modes **********/
	  case INDIRECT:
	  case INDEXX:
	  case INDEXY:
	  case OTHER:

				/********** see if we're in an a index mode with no offset **********/
	     temp = mapdn (*Optr);
	     if (((temp == ',' || temp == '$') &&
		  (mapdn (*(Optr + 1)) == 'x' || mapdn (*(Optr + 1)) == 'y') && delim (*(Optr + 2))) || (delim (*(Optr + 1)) && (temp == 'x' || temp == 'y')))
	     {
		if (temp == ',' || temp == '$')
		   Optr++;
		else if (Pass == 1)
		   warn ("Indexed addressing assumed");

		if (!delim (*(Optr + 1)))
		{
		   if (Pass == 1)
		      error ("llegal Index Mode");
		   return;
		}

		temp = mapdn (*Optr);
		Cycles++;	       /* add one cycle to the base, for being an indexed mode */
		switch (temp)
		{
		  case 'x':
		     emit (opcode + 0x40);
		     return;
		  case 'y':
		     if (!(proc_variant & ST7))
			if (Pass == 1)
			   error ("The 6805 does not have a Y register");
		     eword (opcode + 0x40 + PREFIX_INDEX_Y);
		     Cycles++;	       /* add one cycle to the base, for being a 2 byte opcode */
		     return;
		  default:
		     if (Pass == 1)
			error ("Unknown Index Mode");
		     return;
		}
	     }

				/********** evaluate 1st expression of operand **********/
	     if (temp == '[')
	     {
		if (!(proc_variant & ST7))
		   if (Pass == 1)
		      error ("The 6805 does not support indirect addressing");
		indir = eval_ind ();
		switch (indir)
		{
		  case ILONG:
		     Cycles++;	       /* add one additinal byte for long indirect addressing */
		  case ISHORT:
		  default:
		     Cycles++;	       /* add one byte for short indirect addressing */
		}


	     }
	     else
		eval ();

				/********** throw out an error if opcode does not support long addressing **********/
	     if (opcode < 0x80 && (Force_word || (Result & 0xFFFF) > 0xFF))
		if (Pass == 1)
		   error ("Long Addressing is Illegal");

	     if (opcode < 0x80 && (indir == ILONG))
		if (Pass == 1)
		   error ("Long Indirect Addressing is Illegal");

				/********** see if we're in an index mode **********/
	     if ((Result & 0xFFFF) > 0xFF || indir == ILONG)
		opcode += 0x10;

	     if (*Optr == ',')
	     {
		Cycles++;
		if ((Result & 0xFFFF) > 0xFF || indir == ILONG)
		{
		   opcode += 0x10;
		}
		else if ((Result & 0xFFFF) > 0 || indir == ISHORT)
		{
		   opcode += 0x30;
		}
		else if ((Result & 0xFFFF) == 0)
		{
		   opcode += 0x40;
		}

				/********** set index **********/
		Optr++;		       /* point to next char after the comma */
		if (delim (*Optr))
		   Optr++;	       /* if that char is a delimeter, then point to the next char */
		switch (mapdn (*Optr))
		{
		  default:
		     warn ("No proper index specified, defaulting to X");
		  case 'x':
		     if (class == GENOX || class == GENOIX)
			if (Pass == 1)
			   error ("X Indexed Addressing is not allowed");
		     index = INDEXX;
		     break;
		  case 'y':
		     if (!(proc_variant & ST7))
			if (Pass == 1)
			   error ("The 6805 does not have a Y register");
		     if (class == GENOY || class == GENOIY)
			if (Pass == 1)
			   error ("Y Indexed Addressing is not allowed");
		     index = INDEXY;
		     break;
		}
	     }


				/********** by the time we get here, 'index' and 'indir' are set **********/

	     switch (index | amode)
	     {
	       case INDIRECT:
		  Cycles++;
		  if (class == GENOX || class == GENOIX)
		     eword (opcode + PREFIX_INDIRECT_Y);
		  else
		     eword (opcode + PREFIX_INDIRECT);
		  break;
	       case INDEXX | INDIRECT:
		  Cycles++;
		  eword (opcode + PREFIX_INDIRECT);
		  break;
	       case INDEXY | INDIRECT:
		  Cycles++;
		  eword (opcode + PREFIX_INDIRECT_Y);
		  break;
	       case INDEXY:
		  Cycles++;
		  eword (opcode + PREFIX_INDEX_Y);
		  break;
	       case OTHER:
	       default:
		  if (class == GENOX || class == GENOIX)
		  {
		     Cycles++;
		     eword (opcode + PREFIX_DIRECT_Y);
		  }
		  else
		     emit (opcode);
	     }

				/********** if Force_word is true **********/
	     if (Force_word)
	     {
		eword (Result);

		Cycles += 2;
		return;
	     }

				/********** if Force_byte is true **********/
	     if (Force_byte)
	     {

		emit (lobyte (Result));

		Cycles += 1;
		return;
	     }
				/********** **********/
	     if ((Result >= 0 && Result <= 0xFF) || amode == INDIRECT)
	     {
		emit (lobyte (Result));
		Cycles += 1;
		return;
	     }
				/********** if not Force_byte or Force_word **********/
	     else
	     {
		eword (Result);
		Cycles += 2;
		return;
	     }
				/********** **********/
	  default:
	     if (Pass == 1)
		error ("Unknown Addressing Mode");
	     return;
	}
	return;

     case SETCLR:
     case BTB:
	/*
	 * bset  0,$9D
	 * brset 0,$9D,label
	 * brclr 7,$[$9D],label
	 */

	/* process the 1st digit of the operand, and make sure it is 0-7 */
	eval ();
	if (Result < 0 || Result > 7)
	{
	   if (Pass == 1)
	      error ("Bit Number must be 0-7");
	   return;
	}

	/* check for 1st comma */
	if (*Optr++ != ',')
	   if (Pass == 1)
	      error ("Missing comma");
	/* spit out the opcode, as a word or byte */
	if (amode == INDIRECT)
	{
	   eword (opcode | (Result << 1) | PREFIX_INDIRECT);
	   Cycles += 2;
	}
	else
	   emit (opcode | (Result << 1));

//      amode == INDIRECT ? eword(opcode | (Result << 1) | PREFIX_INDIRECT) : emit(opcode | (Result << 1));
     case SETCLR2:
     case BTB2:
	if (class != BTB && class != SETCLR)
	{
	   if (amode == INDIRECT)
	   {
	      eword (opcode | PREFIX_INDIRECT);
	      Cycles += 2;
	   }
	   else
	      emit (opcode);
	}

//          amode == INDIRECT ? eword(opcode | PREFIX_INDIRECT) : emit(opcode);

	/* skip any spaces, and evaluate next value */
	Optr = skip_white (Optr);

	if (amode == INDIRECT)
	{
	   if (eval_ind () != ISHORT)
	      if (Pass == 1)
		 error ("Long Indirect not allowed");
	}
	else
	   eval ();

	emit (lobyte (Result));

	/* bit set and clear opcodes only have 2 opcodes, so exit here */
	if (class == SETCLR || class == SETCLR2)
	   return;

	/* else it's bit test and branch */
	Optr = skip_white (Optr);

	if (*Optr++ != ',')
	   if (Pass == 1)
	      error ("Missing comma");

	Optr = skip_white (Optr);
	if (eval ())
	   if (amode == INDIRECT)
	      dist = Result - (Old_pc + 4);
	   else
	      dist = Result - (Old_pc + 3);
	else if (amode == INDIRECT)
	   dist = -4;
	else
	   dist = -3;

	if ((dist > 127 || dist < -128) && Pass == 2)
	{
	   if (Pass == 1)
	      error ("Branch out of Range");
	   dist = -3;
	}
	emit (lobyte (dist));
	return;

     default:
	fatal ("Error in Mnemonic table");
   }
}

/* please note this table *must* be in aphabetical order
 *
 * the opcodes that say they are ST7 only, and have
 * MC6805 listed in the proc col, is so the proper error
 * message can be generated, instead of unknown opcode
 */
static oper tablest7[] = {
   /* mnem serial    mode    opcode  cycle   proc */
   {"adc", 0, GEN, 0xB9, 2, 0},	       /* adc */
   {"add", 0, GEN, 0xBB, 2, 0},	       /* add */
   {"and", 0, GEN, 0xB4, 2, 0},	       /* and */
   {"asl", 0, GEN2, 0x38, 4, 0},       /* sla */
   {"asla", 0, INH, 0x48, 3, 0},       /* sla */
   {"aslx", 0, INH, 0x58, 3, 0},       /* sla */
   {"asly", 0, INHY, 0x58, 4, MC6805}, /* sla - ST7 only */
   {"asr", 0, GEN2, 0x37, 4, 0},       /* sra */
   {"asra", 0, INH, 0x47, 3, 0},       /* sra */
   {"asrx", 0, INH, 0x57, 3, 0},       /* sra */
   {"asry", 0, INHY, 0x57, 4, MC6805}, /* sra - ST7 only */
   {"bcc", 0, REL, 0x24, 3, 0},	       /* jrnc - Jump Relative on Not Carry */
   {"bclr", 0, SETCLR, 0x11, 5, 0},    /* bres - Bit RESet */
   {"bclr0", 0, SETCLR2, 0x11, 5, 0},  /* bres - Bit RESet */
   {"bclr1", 0, SETCLR2, 0x13, 5, 0},  /* bres - Bit RESet */
   {"bclr2", 0, SETCLR2, 0x15, 5, 0},  /* bres - Bit RESet */
   {"bclr3", 0, SETCLR2, 0x17, 5, 0},  /* bres - Bit RESet */
   {"bclr4", 0, SETCLR2, 0x19, 5, 0},  /* bres - Bit RESet */
   {"bclr5", 0, SETCLR2, 0x1B, 5, 0},  /* bres - Bit RESet */
   {"bclr6", 0, SETCLR2, 0x1D, 5, 0},  /* bres - Bit RESet */
   {"bclr7", 0, SETCLR2, 0x1F, 5, 0},  /* bres - Bit RESet */
   {"bcs", 0, REL, 0x25, 3, 0},	       /* jrc - Jump Relative on Carry */
   {"beq", 0, REL, 0x27, 3, 0},	       /* jreq - Jump Relative on EQual */
   {"bhcc", 0, REL, 0x28, 3, 0},       /* jrnh - Jump Relative on Not Half-carry */
   {"bhcs", 0, REL, 0x29, 3, 0},       /* jrh - Jump Relative on Half-carry */
   {"bhi", 0, REL, 0x22, 3, 0},	       /* jrugt - Jump Relative if Unsigned Greather than */
   {"bhs", 0, REL, 0x24, 3, 0},	       /* jruge - Jump Relative if Unsigned Greater than or Equal */
   {"bih", 0, REL, 0x2F, 3, 0},	       /* jrih - Jump Relative if Interrupt is High */
   {"bil", 0, REL, 0x2E, 3, 0},	       /* jril - Jump Relative if Interrupt is Low */
   {"bit", 0, GEN, 0xB5, 2, 0},	       /* bcp - Bit ComPare */
   {"blo", 0, REL, 0x25, 3, 0},	       /* jrult - Jump Relative if Unsigned Lower Than */
   {"bls", 0, REL, 0x23, 3, 0},	       /* jrule - Jump Relative if Unsigned Lower or Equal */
   {"bmc", 0, REL, 0x2C, 3, 0},	       /* jrnm - Jump Relative if Not interrupt Mask */
   {"bmi", 0, REL, 0x2B, 3, 0},	       /* jrmi - Jump Relative if MInus */
   {"bms", 0, REL, 0x2D, 3, 0},	       /* jrm - Jump Relative if interrupt Mask */
   {"bne", 0, REL, 0x26, 3, 0},	       /* jrne - Jump Relative if Not Equal */
   {"bpl", 0, REL, 0x2A, 3, 0},	       /* jrpl - Jump Relative if PLus */
   {"bra", 0, REL, 0x20, 3, 0},	       /* jrt - Jump Relative if True (always) */
   {"brclr", 0, BTB, 0x01, 5, 0},      /* btjf - Bit Test, and Jump if False */
   {"brclr0", 0, BTB2, 0x01, 5, 0},    /* btjf - Bit Test, and Jump if False */
   {"brclr1", 0, BTB2, 0x03, 5, 0},    /* btjf - Bit Test, and Jump if False */
   {"brclr2", 0, BTB2, 0x05, 5, 0},    /* btjf - Bit Test, and Jump if False */
   {"brclr3", 0, BTB2, 0x07, 5, 0},    /* btjf - Bit Test, and Jump if False */
   {"brclr4", 0, BTB2, 0x09, 5, 0},    /* btjf - Bit Test, and Jump if False */
   {"brclr5", 0, BTB2, 0x0B, 5, 0},    /* btjf - Bit Test, and Jump if False */
   {"brclr6", 0, BTB2, 0x0D, 5, 0},    /* btjf - Bit Test, and Jump if False */
   {"brclr7", 0, BTB2, 0x0F, 5, 0},    /* btjf - Bit Test, and Jump if False */
   {"brn", 0, REL, 0x21, 3, 0},	       /* jrf - Jump Relative if False (never) */
   {"brset", 0, BTB, 0x00, 5, 0},      /* btjt - Bit Test, and Jump if True */
   {"brset0", 0, BTB2, 0x00, 5, 0},    /* btjt - Bit Test, and Jump if True */
   {"brset1", 0, BTB2, 0x02, 5, 0},    /* btjt - Bit Test, and Jump if True */
   {"brset2", 0, BTB2, 0x04, 5, 0},    /* btjt - Bit Test, and Jump if True */
   {"brset3", 0, BTB2, 0x06, 5, 0},    /* btjt - Bit Test, and Jump if True */
   {"brset4", 0, BTB2, 0x08, 5, 0},    /* btjt - Bit Test, and Jump if True */
   {"brset5", 0, BTB2, 0x0A, 5, 0},    /* btjt - Bit Test, and Jump if True */
   {"brset6", 0, BTB2, 0x0C, 5, 0},    /* btjt - Bit Test, and Jump if True */
   {"brset7", 0, BTB2, 0x0E, 5, 0},    /* btjt - Bit Test, and Jump if True */
   {"bset", 0, SETCLR, 0x10, 5, 0},    /* bset - Bit SET */
   {"bset0", 0, SETCLR2, 0x10, 5, 0},  /* bset - Bit SET */
   {"bset1", 0, SETCLR2, 0x12, 5, 0},  /* bset - Bit SET */
   {"bset2", 0, SETCLR2, 0x14, 5, 0},  /* bset - Bit SET */
   {"bset3", 0, SETCLR2, 0x16, 5, 0},  /* bset - Bit SET */
   {"bset4", 0, SETCLR2, 0x18, 5, 0},  /* bset - Bit SET */
   {"bset5", 0, SETCLR2, 0x1A, 5, 0},  /* bset - Bit SET */
   {"bset6", 0, SETCLR2, 0x1C, 5, 0},  /* bset - Bit SET */
   {"bset7", 0, SETCLR2, 0x1E, 5, 0},  /* bset - Bit SET */
   {"bsr", 0, REL, 0xAD, 6, 0},	       /* callr - CALL subroutine Relative */
   {"clc", 0, INH, 0x98, 2, 0},	       /* rcf - Reset Carry Flag */
   {"cli", 0, INH, 0x9A, 2, 0},	       /* rim - Reset Interrupt Mask */
   {"clr", 0, GEN2, 0x3F, 4, 0},       /* clr - CLeaR */
   {"clra", 0, INH, 0x4F, 3, 0},       /* clr - CLeaR */
   {"clrx", 0, INH, 0x5F, 3, 0},       /* clr - CLeaR */
   {"clry", 0, INHY, 0x5F, 4, MC6805}, /* clr - CLeaR  - ST7 only */
   {"cmp", 0, GEN, 0xB1, 2, 0},	       /* cp - ComPare */
   {"cmpx", 0, GENOY, 0xB3, 2, 0},     /* cp - ComPare */
   {"cmpy", 0, GENOX, 0xB3, 2, MC6805},	/* cp - ComPare ST7 only */
   {"com", 0, GEN2, 0x33, 4, 0},       /* cpl - ComPlement, Logical */
   {"coma", 0, INH, 0x43, 3, 0},       /* cpl - ComPlement, Logical */
   {"comx", 0, INH, 0x53, 3, 0},       /* cpl - ComPlement, Logical */
   {"comy", 0, INHY, 0x53, 4, MC6805}, /* cpl - ComPlement, Logical  ST7 only */
   {"cpx", 0, GENOY, 0xB3, 2, 0},      /* cpl - ComPlement, Logical */
   {"cpy", 0, GENOX, 0xB3, 2, MC6805}, /* cpl - ComPlement, Logical   ST7 only */
   {"dec", 0, GEN2, 0x3A, 4, 0},       /* dec - DECrement */
   {"deca", 0, INH, 0x4A, 3, 0},       /* dec - DECrement */
   {"decx", 0, INH, 0x5A, 3, 0},       /* dec - DECrement */
   {"decy", 0, INHY, 0x5A, 4, MC6805}, /* dec - DECrement  ST7 only */
   {"dex", 0, INH, 0x5A, 3, 0},	       /* dec - DECrement */// where did this come from?
   {"dey", 0, INHY, 0x5A, 4, MC6805},  /* dec - DECrement */// where did this come from?
   {"eor", 0, GEN, 0xB8, 2, 0},	       /* xor - eXclusive OR */
   {"inc", 0, GEN2, 0x3C, 4, 0},       /* inc - INCrement */
   {"inca", 0, INH, 0x4C, 3, 0},       /* inc - INCrement */
   {"incx", 0, INH, 0x5C, 3, 0},       /* inc - INCrement */
   {"incy", 0, INHY, 0x5C, 4, MC6805}, /* inc - INCrement    ST7 only */
   {"inx", 0, INH, 0x5C, 3, 0},	       /* inc - INCrement */// where did this come from?
   {"iny", 0, INHY, 0x5C, 4, MC6805},  /* inc - INCrement */// where did this come from?
   {"jmp", 0, GENOI, 0xBC, 1, 0},      /* jp - JumP */
   {"jsr", 0, GENOI, 0xBD, 4, 0},      /* call - CALL subroutine */
   {"lda", 0, GEN, 0xB6, 2, 0},	       /* ld - LoaD */
   {"ldx", 0, GENOY, 0xBE, 2, 0},      /* ld - LoaD */
   {"ldy", 0, GENOX, 0xBE, 2, MC6805}, /* ld - LoaD    ST7 only */
   {"lsl", 0, GEN2, 0x38, 4, 0},       /* sll - Shift Left, Logical */
   {"lsla", 0, INH, 0x48, 3, 0},       /* sll - Shift Left, Logical */
   {"lslx", 0, INH, 0x58, 3, 0},       /* sll - Shift Left, Logical */
   {"lsly", 0, INHY, 0x58, 4, MC6805}, /* sll - Shift Left, Logical */
   {"lsr", 0, GEN2, 0x34, 4, 0},       /* srl - Shift Right, Logical */
   {"lsra", 0, INH, 0x44, 3, 0},       /* srl - Shift Right, Logical */
   {"lsrx", 0, INH, 0x54, 3, 0},       /* srl - Shift Right, Logical */
   {"lsry", 0, INHY, 0x54, 4, MC6805}, /* srl - Shift Right, Logical   ST7 only */
   {"mul", 0, INH, 0x42, 11, MC6805},  /* mul - MULtiply */
   {"muly", 0, INHY, 0x42, 12, MC6805},	/* mul - MULtiply */
   {"neg", 0, GEN2, 0x30, 4, 0},       /* neg - NEGate */
   {"nega", 0, INH, 0x40, 3, 0},       /* neg - NEGate */
   {"negx", 0, INH, 0x50, 3, 0},       /* neg - NEGate */
   {"negy", 0, INHY, 0x50, 4, MC6805}, /* neg - NEGate    ST7 only */
   {"nop", 0, INH, 0x9D, 2, 0},	       /* nop - No OPeration */
   {"ora", 0, GEN, 0xBA, 2, 0},	       /* or - logical OR */
   {"pop", 0, POP, 0x84, 4, ST7},      /* pop - ST7 only */
   {"popa", 0, INH, 0x84, 4, ST7},     /* pop - ST7 only */
   {"popcc", 0, INH, 0x86, 4, ST7},    /* pop - ST7 only */
   {"popx", 0, INH, 0x85, 4, ST7},     /* pop - ST7 only */
   {"popy", 0, INHY, 0x85, 5, ST7},    /* pop - ST7 only */
   {"push", 0, PUSH, 0x88, 3, ST7},    /* push - ST7 only */
   {"pusha", 0, INH, 0x88, 3, ST7},    /* push - ST7 only */
   {"pushcc", 0, INH, 0x8A, 3, ST7},   /* push - ST7 only */
   {"pushx", 0, INH, 0x89, 3, ST7},    /* push - ST7 only */
   {"pushy", 0, INHY, 0x89, 4, ST7},   /* push - ST7 only */
   {"rol", 0, GEN2, 0x39, 4, 0},       /* rlc - Rotate Left, through Carry */
   {"rola", 0, INH, 0x49, 3, 0},       /* rlc - Rotate Left, through Carry */
   {"rolx", 0, INH, 0x59, 3, 0},       /* rlc - Rotate Left, through Carry */
   {"roly", 0, INHY, 0x59, 4, MC6805}, /* rlc - Rotate Left, through Carry   ST7 only */
   {"ror", 0, GEN2, 0x36, 4, 0},       /* rrc - Rotate Right, through Carry */
   {"rora", 0, INH, 0x46, 3, 0},       /* rrc - Rotate Right, through Carry */
   {"rorx", 0, INH, 0x56, 3, 0},       /* rrc - Rotate Right, through Carry */
   {"rory", 0, INHY, 0x56, 4, MC6805}, /* rrc - Rotate Right, through Carry  ST7 only */
   {"rsp", 0, INH, 0x9C, 2, 0},	       /* rsp - Reset Stack Pointer */
   {"rti", 0, INH, 0x80, 9, 0},	       /* iret - Interrupt RETurn */
   {"rts", 0, INH, 0x81, 6, 0},	       /* ret - RETurn from subroutine */
   {"sbc", 0, GEN, 0xB2, 2, 0},	       /* sbc - SuBtract with Carry */
   {"sec", 0, INH, 0x99, 2, 0},	       /* scf - Set Carry Flag */
   {"sei", 0, INH, 0x9B, 2, 0},	       /* sim - Set Interrupt Mask */
   {"sta", 0, GENOI, 0xB7, 3, 0},      /* ld - LoaD */
   {"stop", 0, INH, 0x8E, 2, 0},       /* halt */
   {"stx", 0, GENOIY, 0xBF, 3, 0},     /* ld - LoaD */
   {"sty", 0, GENOIX, 0xBF, 3, MC6805},	/* ld - LoaD   ST7 only */
   {"sub", 0, GEN, 0xB0, 2, 0},	       /* sub - SUBtract */
   {"swap", 0, GEN2, 0x3E, 4, ST7},    /* swap - swap nibbles   ST7 only */
   {"swapa", 0, INH, 0x4E, 3, ST7},    /* swap - swap nibbles   ST7 only */
   {"swapx", 0, INH, 0x5E, 3, ST7},    /* swap - swap nibbles   ST7 only */
   {"swapy", 0, INHY, 0x5E, 4, ST7},   /* swap - swap nibbles   ST7 only */
   {"swi", 0, INH, 0x83, 10, 0},       /* trap */
   {"tas", 0, INHS, 0x95, 2, MC6805},  /* ld - LoaD   ST7 only */
   {"tax", 0, INH, 0x97, 2, 0},	       /* ld - LoaD */
   {"tay", 0, INHY, 0x97, 3, MC6805},  /* ld - LoaD   ST7 only */
   {"tfr", 0, TFR, 0x02, 2, MC6805},   /* ld - LoaD   MC6809 opcode */
   {"tsa", 0, INH, 0x9E, 2, MC6805},   /* ld - LoaD   ST16CFxx only */
   {"tst", 0, GEN2, 0x3D, 3, 0},       /* tnz - Test for Negative or Zero */
   {"tsta", 0, INH, 0x4D, 3, 0},       /* tnz - Test for Negative or Zero */
   {"tstx", 0, INH, 0x5D, 3, 0},       /* tnz - Test for Negative or Zero */
   {"tsty", 0, INHY, 0x5D, 4, MC6805}, /* tnz - Test for Negative or Zero    ST7 only */
   {"tsx", 0, INHS, 0x96, 2, MC6805},  /* ld - LoaD   ST7 only */
   {"tsy", 0, INHY, 0x96, 3, MC6805},  /* ld - LoaD    ST7 only */
   {"txa", 0, INH, 0x9F, 2, 0},	       /*  ld - LoaD  */
   {"txs", 0, INHS, 0x94, 2, MC6805},  /* ld - LoaD    ST7 only */
   {"txy", 0, INHY, 0x93, 3, MC6805},  /* ld - LoaD    ST7 only */
   {"tya", 0, INHY, 0x9F, 3, MC6805},  /* ld - LoaD    ST7 only */
   {"tys", 0, INHY, 0x94, 3, MC6805},  /* ld - LoaD    ST7 only */
   {"tyx", 0, INH9, 0x93, 2, MC6805},  /* ld - LoaD    ST7 only */
   {"wait", 0, INH, 0x8F, 2, 0}	       /* wfi - Wait For Interrupt */
};

/* define the structure that interfaces to the assembler */

static proc_name namet7[] = {
   {"6805", MC6805},
   {"ST7", MC6805 + ST7},
   {"ST16CF", MC6805},
   {"ST16CFxx", MC6805},
   {"ROM2", MC6805},
   {"ROM3", MC6805},
   {"ST19CF", MC6805 + ST7},
   {"ST19CFxx", MC6805 + ST7},
   {"ROM10", MC6805 + ST7},
   {"ROM11", MC6805 + ST7},
   {NULL, 0}
};

procinfo MST7 = {
   "6805 and ST7 style CPUs",
   namet7,
   localinitst7,
   localtermst7,
   localselectst7,
   localdeselectst7,
   do_opst7,
   tablest7,
   (sizeof (tablest7) / sizeof (oper)),
   NULL,			       /* no processor-specific symbols */
   0
};
