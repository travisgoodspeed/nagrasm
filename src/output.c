/*
 * output.c - output file handling
 *
 * $Id: output.c 13 2004-02-11 06:09:11Z eric $
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

#ifndef WIN32
#include <unistd.h>
#endif

#ifdef macintosh
#include <strings.h>
#include <files.h>
#endif

#include "as.h"


#define kObjBytesPerLine 6


static int ListingPadding;
static int ListingPaddingC;


/*
 *	fatal --- fatal error handler
 */

void
fatal (char *str)
{
   if (ListFile != NULL)
      fprintf (ListFile, "%s\n", str);
   fprintf (stderr, "%s\n", str);
   exit (-1);
}


static void
print_loc (FILE * outfile)
{
   char filename[MAXFILENAMELEN];
   int line;

   GetCurrentSourceLoc (filename, &line);

   if (filename[0] == '\0')
      fprintf (outfile, "  # not in file ");
   else
      fprintf (outfile, "  File %s; Line %d ", filename, line);
}


/*
 *	error --- error in a line
 *			print line number and error
 *
 *	changed to conform to MPW standards for error messages
 *	by Eric Smith, 12/15/89
 */

void
error (char *str)
{
   if (ListFile != NULL)
   {
      if (Pass == 1)
	 print_loc (ListFile);
      fprintf (ListFile, "# Error - %s\n", str);
   }
   print_line (stderr);
   print_loc (stderr);
   fprintf (stderr, "# Error - %s\n", str);
   Err_count++;
}


/*
 *	warn --- trivial error in a line
 *			print line number and error
 *
 *	changed to conform to MPW standards for error messages
 *	by Eric Smith, 12/15/89
 */

void
warn (char *str)
{
   if (ListFile != NULL)
   {
      if (Pass == 1)
	 print_line (ListFile);
      print_loc (ListFile);
      fprintf (ListFile, "# Warning - %s\n", str);
   }
   print_line (stderr);
   print_loc (stderr);
   fprintf (stderr, "  # Warning - %s\n", str);
   Warn_count++;
}


void
OpenListFile (void)
{
#ifdef macintosh
   char temp[256];
#endif

   ListingPadding = tabWidth - ((11 + 3 * kObjBytesPerLine) % tabWidth);
   ListingPaddingC = tabWidth - ((17 + 3 * kObjBytesPerLine) % tabWidth);

   if (ListFile == NULL)
   {
      unlink (List_name);	       /* don't care if it fails */
#ifdef macintosh
      strcpy (temp, List_name);
      HCreate (0, 0, c2pstr (temp), 'MPS ', 'TEXT');	/* don't care if it fails */
      faccess (List_name, F_STABINFO, (long *) tabWidth);	/* don't care if it fails */
#endif
      if ((ListFile = fopen (List_name, "w")) == NULL)
	 fatal ("Can't open listing file");
   }
}


void
CloseListFile (void)
{
   if (ListFile != NULL)
      fclose (ListFile);
}


/*
 *      page_feed --- go to a new page
 */

void
page_feed (void)
{
   if (ListFile != NULL)
   {
      fprintf (ListFile, "\f");
      fprintf (ListFile, "%-10s", Argv[Cfn]);
      fprintf (ListFile, "                                   ");
      fprintf (ListFile, "page %3d\n\n\n", Page_num++);
   }
}


/* print a buffer to a file, entabbing it and stripping trailing blanks */
static void
entab_print (FILE * outfile, char *buffer)
{
   int col, ncol;
   char c;

   col = 0;
   ncol = 0;

   while (*buffer != '\0')
   {
      if ((c = *buffer++) == ' ')
	 ncol++;
      else
      {
	 while (ncol > col)
	 {
	    if ((col + tabWidth - (col % tabWidth)) <= ncol)
	    {
	       fputc ('\t', outfile);
	       col = col + tabWidth - (col % tabWidth);
	    }
	    else
	    {
	       fputc (' ', outfile);
	       col++;
	    }
	 }
	 fputc (c, outfile);
	 col++;
	 ncol++;
      }
   }
   fputc ('\n', outfile);
}


/*
 *	print_line --- pretty print input line
 */
void
print_line (FILE * outfile)
{
   char filename[MAXFILENAMELEN];
   int line;
   int i;
   register char *ptr;
   char outbuf[255];
   char *outptr;
   int j;

   if (outfile != NULL)
   {
      GetCurrentSourceLoc (filename, &line);
      outptr = outbuf;

      sprintf (outptr, "%04d ", line);
      outptr += strlen (outptr);

      if (P_total || P_force)
	 sprintf (outptr, "%04X", Old_pc & 0xffff);
      else
	 sprintf (outptr, "    ");
      outptr += strlen (outptr);

      for (i = 0; (i < P_total) && (i < kObjBytesPerLine); i++)
      {
	 if (P_wflag[i])
	 {
	    if ((i + 1) == kObjBytesPerLine)
	       break;
	    if (LowByteFirst)
	       j = P_bytes[i] | (P_bytes[i + 1] << 8);
	    else
	       j = (P_bytes[i] << 8) | P_bytes[i + 1];
	    sprintf (outptr, " %04X ", j);
	    i++;
	 }
	 else
	    sprintf (outptr, " %02X", lobyte (P_bytes[i]));
	 outptr += strlen (outptr);
      }
      if(Cflag!=2)
      {
	      for (; i < kObjBytesPerLine; i++)
	      {
		      sprintf (outptr, "   ");
		      outptr += strlen (outptr);
	      }
      sprintf (outptr, "  ");
      outptr += strlen (outptr);
      }

      if (Cflag)
      {
	 if (Cycles && Cflag != 2)
	    sprintf (outptr, "[%2d ] ", Cycles);
	 else if (Cflag==2)
	    sprintf (outptr, "              Total [%3d]   ",(int) Ctotal);
	 else
	    sprintf (outptr, "      ");
	 if(Cflag==2)
	 {
		 Cflag=0;
	 }
	 outptr += strlen (outptr);
      }

      /* move the start of the source line over to the next tab stop */
      for (j = 0; j < (Cflag ? ListingPaddingC : ListingPadding); j++)
	 *outptr++ = ' ';

      ptr = Line;
      while (*ptr != '\n')	       /* just echo the source line back out */
	 *outptr++ = *ptr++;
      *outptr = '\0';

      /* now output any excess object bytes */
      for (; i < P_total; i++)
      {
	 if (i % kObjBytesPerLine == 0)
	 {
	    entab_print (outfile, outbuf);
	    outptr = outbuf;
	    sprintf (outptr, "         ");
	    outptr += strlen (outptr);
	 }
	 sprintf (outptr, " %02X", lobyte (P_bytes[i]));
	 outptr += strlen (outptr);
      }
      entab_print (outfile, outbuf);
   }
}


/*
 *  stable --- prints the symbol table in alphabetical order
 */
void
stable (nlist * ptr)
{
   if ((ptr != NULL) && (ListFile != NULL))
   {
      stable (ptr->Lnext);
      fprintf (ListFile, "%-16s %04X\n", ptr->name, ptr->def & 0xffff);
      stable (ptr->Rnext);
   }
}


/*
 *  cross  --  prints the cross reference table 
 */
void
cross (nlist * point)
{
   struct llink *tp;
   int i = 1;
   if ((point != NULL) && (ListFile != NULL))
   {
      cross (point->Lnext);
      fprintf (ListFile, "%-32s %04X *", point->name, point->def & 0xffff);
      tp = point->L_list;
      while (tp != NULL)
      {
	 if (i++ > 10)
	 {
	    i = 1;
	    fprintf (ListFile, "\n                            ");
	 }
	 fprintf (ListFile, "%04d ", tp->L_num);
	 tp = tp->next;
      }
      fprintf (ListFile, "\n");
      cross (point->Rnext);
   }
}
