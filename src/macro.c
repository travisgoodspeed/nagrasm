/*
 * macro.c - macro processor
 *
 * $Id: macro.c 13 2004-02-11 06:09:11Z eric $
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


macroDefNode *
FindMacro (char *name)
{
   macroDefNode *np;
   int i;

   np = macroRoot;
   while (np != NULL)
   {
      i = strcmpCI (name, np->name);
      if (i == 0)
	 return (np);
      if (i < 0)
	 np = np->Lnext;
      else
	 np = np->Rnext;
   }
   return (NULL);
}


macroDefNode *
CreateMacro (char *name, char *operand)
{
   macroDefNode *np, *p, *backp;
   int i;

   np = (macroDefNode *) alloc (sizeof (macroDefNode));
   if (np == (macroDefNode *) ERR)
   {
      error ("Macro table full");
      return (NULL);
   }
   np->name = alloc (strlen (name) + 1);
   if (np->name == (char *) ERR)
   {
      error ("Macro table full");
      return (NULL);
   }
   strcpy (np->name, name);

   p = macroRoot;
   backp = NULL;

   while (p != NULL)
   {
      backp = p;
      i = strcmpCI (name, p->name);
      if (i < 0)
	 p = p->Lnext;
      else
	 p = p->Rnext;
   }

   if (backp == NULL)
      macroRoot = np;
   else if (strcmpCI (name, backp->name) < 0)
      backp->Lnext = np;
   else
      backp->Rnext = np;

   return (np);
}
