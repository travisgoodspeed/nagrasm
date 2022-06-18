/*
 * util.c - utility functions
 *
 * $Id: util.c 13 2004-02-11 06:09:11Z eric $
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

#include "as.h"


/*
 *	any --- does str contain c?
 */

Boolean
any (char c, char *str)
{
   while (*str != EOS)
      if (*str++ == c)
	 return (YES);
   return (NO);
}


/*
 *	charpos --- return a pointer to the first character in str1 that
 *                  matches any character in str2
 */

char *
charpos (char *str, char *str2)
{
   char *p;

   for (; *str != '\0'; str++)
      for (p = str2; *p != '\0'; p++)
	 if (*str == *p)
	    return (str);
   return (NULL);
}


/*
 *	delim --- check if character is a delimiter
 */

Boolean
delim (char c)
{
   if (any (c, " \t\n\r"))
      return (YES);
   if (c == EOS)
      return (YES);
   return (NO);
}


/*
 *	skip_white --- move pointer to next non-whitespace char
 */

char *
skip_white (char *ptr)
{
   while (*ptr == BLANK || *ptr == TAB)
      ptr++;
   return (ptr);
}


/*
 *	mapdn --- convert A-Z to a-z
 */

char
mapdn (char c)
{
   if (c >= 'A' && c <= 'Z')
      return (c + 040);
   return (c);
}


/*
 *	strcmpCI --- case insensitive string comparison
 */
int
strcmpCI (char *str1, char *str2)
{
   char c1, c2;

   do
   {
      c1 = mapdn (*(str1++));
      c2 = mapdn (*(str2++));
   }
   while ((c1 == c2) && (c1 != EOS));
   return (c1 - c2);
}

/*
 *	strncmpCI --- case insensitive string comparison
 */
int
strncmpCI (char *str1, char *str2, int len)
{
   int i;

   for (i = 0; i < len; i++)
      if (mapdn (*(str1 + i)) != mapdn (*(str2 + i)))
	 return 0;
   return 1;
}

/*
 *	head --- is str2 the head of str1?
 */

Boolean
head (char *str1, char *str2)
{
   while (*str1 != EOS && *str2 != EOS)
   {
      if (mapdn (*str1) != mapdn (*str2))
	 break;
      str1++;
      str2++;
   }
   if ((*str1 == *str2) || ((*str2 == EOS) && any (*str1, " \t\n\r,+-];*")))
      return (YES);
   return (NO);
}


/*
 *	alpha --- is character a legal letter
 */

Boolean
alpha (char c)
{
   if (c <= 'z' && c >= 'a')
      return (YES);
   if (c <= 'Z' && c >= 'A')
      return (YES);
   if (c == '_')
      return (YES);
   if (c == '.')
      return (YES);
   return (NO);
}


/*
 *	alphan --- is character a legal letter or digit
 */

Boolean
alphan (char c)
{
   if (alpha (c))
      return (YES);
   if (c <= '9' && c >= '0')
      return (YES);
   if (c == '$')
      return (YES);		       /* allow imbedded $ */
   return (NO);
}


/*
 *  white_space --- is character whitespace?
 */

Boolean
white_space (char c)
{
   if (c == TAB || c == BLANK || c == '\n' || c == '\r')
      return (YES);
   return (NO);
}


/*
 *  alloc --- allocate memory
 */

char *
alloc (int nbytes)
{
   /* char *malloc(); */

   return (malloc (nbytes));
}
