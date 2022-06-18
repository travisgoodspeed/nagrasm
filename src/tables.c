/*
 * tables.c
 *
 * $Id: tables.c 13 2004-02-11 06:09:11Z eric $
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


#ifdef debug
#include <types.h>
#include <strings.h>
char ds[120];
#endif

extern procinfo MST7;

procinfo *proctab[] = {
   NULL, &MST7
};

int maxproc = (sizeof (proctab) / sizeof (*(proctab)));

int currentproc = 0;

int proc_variant = 0;

oper *table;
int NMNE;
void (far * do_op) (int opcode, int class);
nlist *ptable;
int ptableSize;


#ifdef __TURBOC__
#pragma argsused
#endif

void far
badop (int opcode, int class)
{				       /* this should be defined in AS.C */
}


static void
deselectproc (void)
{
   if ((currentproc != 0) && ((proctab[currentproc]->desel) != 0))
      (*(proctab[currentproc]->desel)) ();
   currentproc = 0;
   NMNE = 0;
   table = NULL;
   ptableSize = 0;
   ptable = NULL;
   proc_variant = 0;
   do_op = badop;
}


void
initall (void)
{
   int i;

   currentproc = 0;
   deselectproc ();

   for (i = 1; i <= (maxproc - 1); i++)
   {
      if ((proctab[i]->init) != 0)
	 (*(proctab[i]->init)) ();
   }
}

void
termall (void)
{
   int i;
   deselectproc ();
   for (i = 1; i <= (maxproc - 1); i++)
      if ((proctab[i]->term) != 0)
	 (*(proctab[i]->term)) ();
}

/*
 * selectproc - Selects CPU
 * arg=pointer to cpu string
 * force=flag to make it not change if called again.
 *
 * If cpu type is ROM2/3/10/11, and an output format has
 * not been selected, then it will be set to ne2, ep3, ep10, or ep11.
 */
int
selectproc (char *arg, char force)
{
   int i, j;
   char procName[MAXBUF];
   char *rest;

   deselectproc ();

   rest = charpos (arg, ", \t\n");
   if (rest == NULL)
   {
      strcpy (procName, arg);
      rest = NULL;
   }
   else
   {
      strncpy (procName, arg, rest - arg);
      procName[rest - arg] = '\0';
      while (*rest == ',')
	 rest++;
      if ((*rest == '\n') || (*rest == EOS))
	 rest = NULL;
   }
   if (strcmpCI (procName, "ROM2") == 0)
      SetObjectFormat (F_NAGRAEDIT_NE2, force);
   if (strcmpCI (procName, "ROM3") == 0)
      SetObjectFormat (F_NAGRAEDIT_EP3, force);
   if (strcmpCI (procName, "ROM10") == 0)
      SetObjectFormat (F_NAGRAEDIT_EP10, force);
   if (strcmpCI (procName, "ROM11") == 0)
      SetObjectFormat (F_NAGRAEDIT_EP11, force);
//    if(!strncmpCI(procName,"rom",3))
//      SetObjectFormat(F_MOT_S19, force);

   for (i = 1; i < maxproc; i++)
      for (j = 0; (proctab[i]->name[j].name != '\0'); j++)
      {
	 if (strcmpCI (procName, proctab[i]->name[j].name) == 0)
	 {
	    currentproc = i;
	    NMNE = (proctab[i]->NMNE);
	    table = (proctab[i]->table);
	    ptableSize = (proctab[i]->ptableSize);
	    ptable = (proctab[i]->ptable);
	    do_op = (proctab[i]->do_op);
	    proc_variant = (proctab[i]->name[j].variant);
	    if ((proctab[i]->sel) != 0)
	       return ((*(proctab[i]->sel)) (procName, rest));
	    return YES;
	 }
      }
   return NO;
}


void
list_procs (void)
{
   int i, j;

   fprintf (stderr, "\nList of supported processor families:\n\n");
   for (i = 1; i < maxproc; i++)
   {
      fprintf (stderr, "%s:", proctab[i]->description);
      for (j = 0; (proctab[i]->name[j].name != '\0'); j++)
	 fprintf (stderr, "  %s", proctab[i]->name[j].name);
      fprintf (stderr, "\n");
   }

}
