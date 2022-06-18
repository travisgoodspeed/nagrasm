/*
 * input.c - input file handling
 *
 * $Id: input.c 13 2004-02-11 06:09:11Z eric $
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
#include <stdlib.h>
#include <string.h>

#include "as.h"


#define MaxIncludeNest 10

static int CurrentSourceNest = 0;
static FILE *SourceFile[MaxIncludeNest];
static char *SourceFileName[MaxIncludeNest];
static int SourceLine[MaxIncludeNest];


char *
GetString (char *s, int n, FILE * iop)
      /* get at most n chars from iop, expanding tabs */
{
   int c = 0;			/* don't need to init here, but it stops a compiler warning */
#ifdef check_for_cr
   int c2;
#endif
   char *cs;
   int col;

   cs = s;
   col = 0;

   while ((col < (n - 1)) && (c = getc (iop)) != EOF)
   {				       /* read chars if not file end */
      if (c == '\t')
      {
	 do
	 {
	    *(cs++) = ' ';
	    col++;
	 }
	 while ((col < (n - 1)) && ((col % tabWidth) != 0));
	 continue;
      }
#ifdef check_for_cr
      if (c == '\r')
      {
	 if ((c2 = getc (iop)) == EOF)
	    break;
	 if (c2 != '\n')
	    ungetc (c2, iop);
	 c = '\n';
      }
#endif
      *(cs++) = c;
      col++;
      if (c == '\n')
	 break;
   }
   *cs = '\0';			       /* replace NEWLINE with NULL as in standard fgets() */
   return ((c == EOF && cs == s) ? NULL : s);	/* return NULL if this is EOF */
}


Boolean
OpenSourceFile (char *name)
{
   if ((SourceFile[CurrentSourceNest] = fopen (name, "r")) != NULL)
   {
      SourceLine[CurrentSourceNest] = 0;
      if ((SourceFileName[CurrentSourceNest] = (char *) malloc (strlen (name) + 1)) != 0)
	 strcpy (SourceFileName[CurrentSourceNest++], name);
#ifdef macintosh
      if (faccess (*np, F_GTABINFO, &tabWidth) == -1)
	 tabWidth = 8;
#endif
      return (YES);
   }
   else
      return (NO);
}

Boolean
GetSourceLine (char *buf, int *line)
{
   if ((CurrentSourceNest > 0) && (GetString (buf, MAXBUF - 1, SourceFile[CurrentSourceNest - 1]) != NULL))
   {
      /* check for EOF */
      *line = ++SourceLine[CurrentSourceNest - 1];
      return (YES);
   }
   return (NO);
}


Boolean
CloseSourceFile (void)
{
   if (CurrentSourceNest > 0)
   {
      if (SourceFileName[CurrentSourceNest - 1] != NULL)
	 free (SourceFileName[CurrentSourceNest - 1]);
      fclose (SourceFile[--CurrentSourceNest]);
   }
   return (CurrentSourceNest > 0);
}


void
GetCurrentSourceLoc (char *name, int *line)
{
   if (CurrentSourceNest > 0)
   {
      if (SourceFileName[CurrentSourceNest - 1] != NULL)
	 strcpy (name, SourceFileName[CurrentSourceNest - 1]);
      else
	 name[0] = '\0';
      *line = SourceLine[CurrentSourceNest - 1];
   }
   else
   {
      name[0] = '\0';
      *line = 0;
   }
}
