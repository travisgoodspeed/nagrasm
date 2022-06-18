/*
 * eval.c - expression evaluator
 *
 * $Id: eval.c 13 2004-02-11 06:09:11Z eric $
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

#include "as.h"


static int c_nums = 0;		/* don't allow c style number */

/*
 *	is_opr --- is character an expression operator?
 */
static Boolean
is_opr (char *c)
{
   if (any (c[0], "<>"))
      return (c[1] == c[0]);
   else if (any (c[0], "+*-/&%|^"))
      return (YES);
   return (NO);
}


static SInt16 eval_subexp (void);

static Boolean
get_based_number (char **cptr, int base, SInt16 * val)
{
   int digits;
   int digitval;

   *val = 0;
   digits = 0;
   while (1)
   {
      if (**cptr >= '0' && **cptr <= '9')
	 digitval = **cptr - '0';
      else if (**cptr >= 'A' && **cptr <= 'F')
	 digitval = (**cptr - 'A') + 10;
      else if (**cptr >= 'a' && **cptr <= 'f')
	 digitval = (**cptr - 'a') + 10;
      else
	 break;
      if (digitval < base)
      {
	 *val = ((*val) * base) + digitval;
	 (*cptr)++;
	 digits++;
      }
      else
	 break;
   }
   return (digits != 0);
}


static Boolean
get_suffix_number (char **cptr, int default_base, SInt16 * val)
{
   char *optr2;
   int digitval;
   int base = default_base;
   int basechar = 1;

   *val = 0;

   for (optr2 = *cptr; any (*optr2, "0123456789ABCDEFabcdef"); optr2++);

   if (*optr2 == 'h' || *optr2 == 'H')
   {
      base = 16;
      optr2--;
   }
   else if (*optr2 == 'o' || *optr2 == 'O')
   {
      base = 8;
      optr2--;
   }
   else
   {
      optr2--;
      if (*optr2 == 'b' || *optr2 == 'B')
      {
	 base = 2;
	 optr2--;
      }
      else if (*optr2 == 'd' || *optr2 == 'D')
      {
	 base = 10;
	 optr2--;
      }
      else
	 basechar = 0;
   }

   if (optr2 < *cptr)
      return (0);

   while (*cptr <= optr2)
   {
      if (**cptr >= '0' && **cptr <= '9')
	 digitval = **cptr - '0';
      else if (**cptr >= 'A' && **cptr <= 'F')
	 digitval = (**cptr - 'A') + 10;
      else if (**cptr >= 'a' && **cptr <= 'f')
	 digitval = (**cptr - 'a') + 10;
      else			       /* should never happen */
	 return (0);
      if (digitval < base)
      {
	 *val = ((*val) * base) + digitval;
	 (*cptr)++;
      }
      else
	 return (0);
   }

   if (basechar)		       /* skip the base suffix */
      (*cptr)++;

   return (1);
}


Boolean
get_number (char **cptr, SInt16 * val)
{
   switch (**cptr)
   {
     case '%':
	(*cptr)++;
	return (get_based_number (cptr, 2, val));
     case '@':
	(*cptr)++;
	return (get_based_number (cptr, 8, val));
     case '$':
	(*cptr)++;
	return (get_based_number (cptr, 16, val));
     case '0':
	if (c_nums)
	{
	   if (*((*cptr) + 1) == 'x')
	   {			       /* C-style hex constant */
	      (*cptr) += 2;
	      return (get_based_number (cptr, 16, val));
	   }
	   else
	   {			       /* C-style octal constant */

	      /* (*cptr)++; We can't do this because the number might just be "0" */
	      return (get_suffix_number (cptr, 8, val));
	   }
	}
	else
	   return (get_suffix_number (cptr, 10, val));
     case '1':
     case '2':
     case '3':
     case '4':
     case '5':
     case '6':
     case '7':
     case '8':
     case '9':
	return (get_suffix_number (cptr, 10, val));
     default:
	return (0);		       /* should never get here */
   }
   return (0);			       /* should really never get here */
}


/*
 *	get_term --- evaluate a single item in an expression
 */
static SInt16
get_term (void)
{
   char hold[MAXBUF];
   char *tmp;
   SInt16 val = 0;		/* local value being built */
   Boolean minus;		/* unary minus flag */
   Boolean ones_comp;		/* ones complement flag */
   Boolean hi_byte;
   Boolean lo_byte;
   struct nlist *pointer;

   minus = NO;
   ones_comp = NO;
   hi_byte = NO;
   lo_byte = NO;

   if (*Optr == '-')
   {
      Optr++;
      minus = YES;
   }
   else if (*Optr == '~')
   {
      Optr++;
      ones_comp = YES;
   }
   else if ((ByteSelOps == 1) && (*Optr == '>'))
   {
      Optr++;
      hi_byte = YES;
   }
   else if ((ByteSelOps == 1) && (*Optr == '<'))
   {
      Optr++;
      lo_byte = YES;
   }

   while (*Optr == '#')
      Optr++;

   /* look at rest of expression */

   if ((*Optr == '*') || (*Optr == '.') || ((*Optr == '$') && !any (mapdn (*(Optr + 1)), "0123456789abcdef")))
   {
      /* current location counter */
      Optr++;
      val = Old_pc;
   }
   else if (any (*Optr, "$@%0123456789"))
   {				       /* constant */
      if (!get_number (&Optr, &val))
      {
	 val = 0;
	 error ("badly formed constant");
      }
   }
   else if (*Optr == '\'')
   {				       /* character literal */
      Optr++;
      val = 0;
      while ((*Optr != EOS) && (*Optr != '\''))
      {
	 val = (val << 8) | *Optr;
	 Optr++;
      }
      if (*Optr == '\'')
	 Optr++;
   }
   else if (*Optr == '(')
   {
      Optr++;
      val = eval_subexp ();
      if (*Optr++ != ')')
	 error ("mismatched parenthesis");
   }
   else if (alpha (*Optr))
   {				       /* a symbol */
      tmp = hold;		       /* collect symbol name */
      while (alphan (*Optr))
	 *tmp++ = *Optr++;
      *tmp = (char) EOS;	       /* the cast shouldn't be necessary, but prevents a warning from the MPW 3.1 C Compiler */
      pointer = lookup (hold);
      if (pointer != NULL)
      {
	 if (Pass == 2)
	    add_ref (pointer);
	 val = Last_sym;
      }
      else
      {
	 if (Pass == 1)
	 {			       /* forward ref here */
	    if (!Force_byte)
	       Force_word++;
	    val = 0;
	 }
      }
      if (Pass == 2 && Line_num == F_ref && Cfn == Ffn)
      {
	 if (!Force_byte)
	    Force_word++;
      }
   }
   else
      /* none of the above */
      val = 0;

   if (minus)
      val = -val;
   else if (ones_comp)
      val = ~val;
   else if (hi_byte)
      val = (val >> 8) & 0xff;
   else if (lo_byte)
      val &= 0xff;

   return (val & 0xffff);
}


static SInt16
eval_subexp (void)
{
   SInt16 left, right;		/* left and right terms for expression */
   char o;			/* operator character */

   if (*Optr == EOS)
      error ("expression expected");

   left = get_term ();		       /* pickup first part of expression */
   while (is_opr (Optr))
   {
      o = *Optr++;		       /* pickup connector and skip */
      if (o == '<' || o == '>')
	 Optr++;
      right = get_term ();	       /* pickup current rightmost side */
      switch (o)
      {
	case '+':
	   left += right;
	   break;
	case '-':
	   left -= right;
	   break;
	case '*':
	   left *= right;
	   break;
	case '/':
	   left /= right;
	   break;
	case '|':
	   left |= right;
	   break;
	case '&':
	   left &= right;
	   break;
	case '%':
	   left %= right;
	   break;
	case '>':
	   left >>= right;
	   break;
	case '<':
	   left <<= right;
	   break;
	case '^':
	   left ^= right;
	   break;
      }
   }

   return (left & 0xffff);
}


#if defined (sparc)
/*
 *     my_memmove
 */
char *
my_memmove (char *s, char *t, int n)
{
   if (s <= t)
      for (; *s++ = *t++; --n);
   else
      for (s += n, t += n; *--s = *--t; --n);
   return (s);
}

#define memmove my_memmove
#endif


/*
 *	eval --- evaluate expression
 */

int
eval (void)
{
   int hi_byte = 0;
   int lo_byte = 0;

#ifdef DEBUG
   fprintf (stderr, "Evaluating \"%s\"\n", Optr);
#endif
   if (*Optr == EOS)
      error ("expression expected");

   Force_byte = NO;
   Force_word = NO;

   if (ByteSelOps == 2)
   {
      int l = strcspn (Optr, ",");
      switch (Optr[l - 1])
      {
	case '<':
	   hi_byte = 1;
	   memmove (Optr + l - 1, Optr + l, strlen (Optr + l) + 1);
	   break;
	case '>':
	   lo_byte = 1;
	   memmove (Optr + l - 1, Optr + l, strlen (Optr + l) + 1);
	   break;
      }
   }
   else if ((!ByteSelOps) && (*Optr == '<'))
   {
      Force_byte++;
      Optr++;
   }
   else if ((!ByteSelOps) && (*Optr == '>'))
   {
      Force_word++;
      Optr++;
   }

   Result = eval_subexp ();

   if (hi_byte)
      Result = (Result >> 8) & 0xff;
   else if (lo_byte)
      Result &= 0xff;

#ifdef DEBUG
   fprintf (stderr, "Result=%x\n", Result);
   fprintf (stderr, "Force_byte=%d  Force_word=%d\n", Force_byte, Force_word);
#endif
   return (YES);
}
