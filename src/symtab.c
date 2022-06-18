/*
 * symtab.c - symbol table handling
 *
 * $Id: symtab.c 13 2004-02-11 06:09:11Z eric $
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


nlist *
install_core (char *str, SInt16 val, Boolean redefOK)
{
   llink *lp;
   nlist *np, *p, *backp;
   int i;

   if (!alpha (*str))
   {
      error ("Illegal Symbol Name");
      return (NULL);
   }
   if ((np = lookup_ne (str)) != NULL)
   {
      if (redefOK)
      {
	 /* we need to add a cross reference entry here!!! */
	 np->def = val;
	 return (np);
      }
      if (Pass == 2)
      {
	 if (np->def == val)
	    return (np);
	 else
	 {
	    error ("Phasing Error");
	    return (NULL);
	 }
      }
      error ("Symbol Redefined");
      return (NULL);
   }
   /* enter new symbol */
#ifdef DEBUG
   fprintf (stderr, "Installing %s as %d\n", str, val);
#endif
   np = (struct nlist *) calloc (1, sizeof (struct nlist));
   if (!np)
   {
      error ("Symbol table full");
      return (NULL);
   }
   np->name = alloc (strlen (str) + 1);
   if (!np->name)
   {
      error ("Symbol table full");
      return (NULL);
   }
   strcpy (np->name, str);
   np->def = val;
   np->delete = 0;
   np->Lnext = NULL;
   np->Rnext = NULL;
   lp = (struct llink *) alloc (sizeof (struct llink));
   np->L_list = lp;
   lp->L_num = Line_num;
   lp->next = NULL;
   p = root;
   backp = NULL;
   while (p != NULL)
   {
      backp = p;
      i = strcmpCI (str, p->name);
      if (i < 0)
	 p = p->Lnext;
      else
	 p = p->Rnext;
   }
   if (backp == NULL)
      root = np;
   else if (strcmpCI (str, backp->name) < 0)
      backp->Lnext = np;
   else
      backp->Rnext = np;
   return (np);
}


/*
 *	install --- add a symbol to the table
 */
Boolean
install (char *str, SInt16 val, Boolean redefOK)
{
   nlist *np;
   np = install_core (str, val, redefOK);
   if (np)
      np->bit_sym = 0;
   return (np != NULL);
}


Boolean
install_bit (char *str, SInt16 addr, int bit)
{
   nlist *np;

   np = install_core (str, addr, 0);
   if (np)
   {
      np->bit_sym = 1;
      np->bit = bit;
   }
   return (np != NULL);
}

/*
 * undefine --- undefine a variable, or at least mark it as not being used
 */
void
undefine (char *name)
{
   nlist *np;

   np = lookup (name);
   if (np == NULL)
      return;
   if (Pass == 2)
      np->delete = 1;
   return;
}

/*
 *	lookup_ne --- find string in symbol table, no error if not found
 */
nlist *
lookup_ne (char *name)
{
   nlist *np;
   int i;
   int low, high, mid;
   int cond;

   /* Search machine-specific symbol table first */

   if (ptable != NULL)
   {
      low = 0;
      high = ptableSize - 1;
      while (low <= high)
      {
	 mid = (high + low) / 2;
	 if ((cond = strcmpCI (name, ptable[mid].name)) < 0)
	    high = mid - 1;
	 else if (cond > 0)
	    low = mid + 1;
	 else
	 {
	    Last_sym = ptable[mid].def;
	    if (ptable[mid].delete)
	       return (NULL);
	    else
	       return (&ptable[mid]);
	 }
      }
   }

   /* now search user symbol table */

   np = root;
   while (np != NULL)
   {
      i = strcmpCI (name, np->name);
      if (i == 0)
      {
	 Last_sym = np->def;
	 if (np->delete)
	    return (NULL);
	 else
	    return (np);
      }
      else if (i < 0)
	 np = np->Lnext;
      else
	 np = np->Rnext;
   }
   Last_sym = 0;
   return (NULL);
}

/*
 *	lookup --- find string in symbol table
 */
nlist *
lookup (char *name)
{
   nlist *result;

   result = lookup_ne (name);
   if ((result == NULL) && (Pass == 2))
      error ("symbol undefined on pass 2");
   if (result && result->bit_sym)
   {
      error ("bit symbol");
      result = NULL;
   }
   return (result);
}


int
lookup_bit (char *name, SInt16 * addr, int *bit)
{
   nlist *result;

   result = lookup_ne (name);
   if (result && result->bit_sym)
   {
      *addr = result->def;
      *bit = result->bit;
      return (1);
   }

   return (0);
}


/*
 *	add_ref --- add reference to an identifier
 */
void
add_ref (nlist * pointer)
{
   struct llink *pnt, *bpnt;

   pnt = pointer->L_list;
   bpnt = NULL;
   while (pnt != NULL)
   {
      bpnt = pnt;
      pnt = pnt->next;
   }
   pnt = (struct llink *) alloc (sizeof (struct llink));
   if (bpnt == NULL)
      pointer->L_list = pnt;
   else
      bpnt->next = pnt;
   pnt->L_num = Line_num;
   pnt->next = NULL;
}


/*
 *	mne_look --- mnemonic lookup
 *
 *	Return pointer to an oper structure if found.
 *	Searches both the machine mnemonic table and the pseudo table.
 */
oper *
mne_look (char *str)
{
   int low, high, mid;
   int cond;

   /* Search machine mnemonics first */
   if (table != 0)
   {
      low = 0;
      high = NMNE - 1;
      while (low <= high)
      {
	 mid = (high + low) / 2;
	 if ((cond = strcmpCI (str, table[mid].mnemonic)) < 0)
	    high = mid - 1;
	 else if (cond > 0)
	    low = mid + 1;
	 else if ((table[mid].req_proc & proc_variant) == table[mid].req_proc)
	    return (&table[mid]);
	 else
	    break;
      }
   }

   /* Check for pseudo ops */
   low = 0;
   high = NPSE - 1;
   while (low <= high)
   {
      mid = (high + low) / 2;
      if ((cond = strcmpCI (str, pseudo[mid].mnemonic)) < 0)
	 high = mid - 1;
      else if (cond > 0)
	 low = mid + 1;
      else
	 return (&pseudo[mid]);
   }

   return (NULL);
}
