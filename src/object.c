/*
 * object.c - object file handling
 *
 * $Id: object.c 13 2004-02-11 06:09:11Z eric $
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
#include <fcntl.h>
#include <string.h>

#ifndef WIN32
#include <unistd.h>
#endif

#ifdef macintosh
#include <strings.h>
#include <files.h>
#endif

#include "as.h"


#define E_LIMIT 16


static int obj_format = 0;

static int E_total = 0;		/* total # bytes in record */
static int E_bytes[E_LIMIT] = { 0 };   /* Emitted held bytes */
static unsigned int E_pc = 0;	/* address of start of record */
static int E_next = 0;		/* expected next address */

static char Obj_Name[256] = "";	/* object file name */
static FILE *Obj_File = NULL;	/* object file's file descriptor */

static unsigned char binimage[0x10000];	/* 64k binary image */

static void
LocalOpenObjectFile (void)
{
   int i = 0;
   int j = 0;
#ifdef macintosh
   char temp[256];
#endif

   if (obj_format == 0)
      obj_format = F_INTEL_HEX;

   do
   {
      if (Obj_Name[i] == '.')
	 j = i;
   }
   while (Obj_Name[i++] != 0);
   if (j != 0)
      Obj_Name[j] = 0;

/*	
  for (i = 0; Obj_Name [i] != '\0'; i++)
    if (Obj_Name [i] == '.')
      {
	Obj_Name [i] = '\0';
	break;
      }
*/

   switch (obj_format)
   {
     case F_BIN_64K:
     case F_BIN_4K:
     case F_BIN_8K:
	strcat (Obj_Name, ".bin");
	for (i = 0; i <= 0xFFFF; i++)
	   binimage[i] = 0;
	break;
     case F_MOT_S19:
	strcat (Obj_Name, ".s19");
	break;
     case F_INTEL_HEX:
	strcat (Obj_Name, ".hex");
	break;
     case F_NAGRAEDIT_NE2:
	strcat (Obj_Name, ".ne2");
	break;
     case F_NAGRAEDIT_EP3:
	strcat (Obj_Name, ".ep3");
	break;
     case F_NAGRAEDIT_EP10:
	strcat (Obj_Name, ".ep10");
	break;
     case F_NAGRAEDIT_EP11:
	strcat (Obj_Name, ".ep11");
	break;
   }

#ifdef macintosh
   strcpy (temp, Obj_Name);
   HCreate (0, 0, c2pstr (temp), 'MPS ', 'TEXT');	/* don't care if it fails */
#endif
   if ((Obj_File = fopen (Obj_Name, "w")) == NULL)
      fatal ("Can't create object file");
   switch (obj_format)
   {
     case F_NAGRAEDIT_NE2:
	fprintf (Obj_File, "!Version 1\n");	/* Patch File Header */
	break;
     case F_NAGRAEDIT_EP3:
	fprintf (Obj_File, "!Version 2\n");	/* Patch File Header */
	break;
     case F_NAGRAEDIT_EP10:
	fprintf (Obj_File, "!Version 3\n");	/* Patch File Header */
	break;
     case F_NAGRAEDIT_EP11:
	fprintf (Obj_File, "!Version 4\n");	/* Patch File Header */
	break;
   }
}

void
ReinitObject (void)
{
   E_total = 0;
   E_pc = 0;
   E_next = 0;
}


#define hexout(byte) fprintf (Obj_File, "%02X", (byte) & 0xff);

/*
 *	dump_record --- write one object code record
 */

void
dump_record ()
{
   int i;
   int chksum = 0;
   unsigned char epxdelim[] = "=1234567:9ABCDEF";

   if ((Pass == 1) || (E_total == 0))
   {
      E_pc = E_next;
      return;
   }
   switch (obj_format)
   {
     case F_INTEL_HEX:
	chksum = (E_total - 1) + hibyte (E_pc) + lobyte (E_pc);
	fprintf (Obj_File, ":");
	hexout (E_total);	       /* byte count */
	hexout (hibyte (E_pc));	       /* high byte of PC */
	hexout (lobyte (E_pc));	       /* low byte of PC */
	hexout (0x00);
	break;
     case F_MOT_S19:
	chksum = (E_total + 3) + hibyte (E_pc) + lobyte (E_pc);
	fprintf (Obj_File, "S1");      /* record header preamble */
	hexout (E_total + 3);	       /* byte count +3 */
	hexout (hibyte (E_pc));	       /* high byte of PC */
	hexout (lobyte (E_pc));	       /* low byte of PC */
	break;
     case F_NAGRAEDIT_NE2:
     case F_NAGRAEDIT_EP3:
     case F_NAGRAEDIT_EP10:
     case F_NAGRAEDIT_EP11:
	fprintf (Obj_File, "$");       /* record header preamble */
	hexout (hibyte (E_pc));	       /* high byte of PC */
	hexout (lobyte (E_pc));	       /* low byte of PC */
	fprintf (Obj_File, "%.1s", &epxdelim[E_total & 0x0F]);	/* delimiter between address and data */
	break;
   }

   switch (obj_format)
   {
     case F_INTEL_HEX:
     case F_MOT_S19:
     case F_NAGRAEDIT_NE2:
     case F_NAGRAEDIT_EP3:
     case F_NAGRAEDIT_EP10:
     case F_NAGRAEDIT_EP11:
	for (i = 0; i < E_total; i++)
	{
	   chksum += lobyte (E_bytes[i]);
	   hexout (lobyte (E_bytes[i]));	/* data byte */
	}
	if (obj_format == F_MOT_S19 || obj_format == F_INTEL_HEX)
	   hexout (lobyte (~chksum));  /* checksum */
	fprintf (Obj_File, "\n");
	break;
     case F_BIN_64K:
     case F_BIN_4K:
     case F_BIN_8K:
	// how in the fuck they ever got this to work is amazing
	// I have no idea why E_pc goes fuckin' crazy forcing me to
	// AND it with FFFF
	for (i = 0; i < E_total; i++)
	   binimage[(E_pc & 0xFFFF) + i] = lobyte (E_bytes[i]);
   }

   E_pc = E_next;
   E_total = 0;
}

/*
 *	emit --- emit a byte to code file
 */

void
emit_at (int addr, int byte)
{
   if (Pass == 1)
      return;
   if (Obj_File == NULL)
      LocalOpenObjectFile ();
   if (addr != E_next)
   {
      E_next = addr;
      dump_record ();
   }
   if (P_total < P_LIMIT)
   {
      P_wflag[P_total] = false;
      P_bytes[P_total++] = byte;
   }
   E_bytes[E_total++] = byte;
   E_next++;
   if (E_total >= E_LIMIT)
      dump_record ();
}


void
emit (int byte)
{
   if (Pass == 1)
   {
      Pc++;
      return;
   }
   emit_at (Pc++, byte);
}


void
eword (int wd)
{
   if (Pass == 1)
   {
      Pc += 2;
      return;
   }
   if (LowByteFirst)
   {
      emit_at (Pc++, lobyte (wd));
      emit_at (Pc++, hibyte (wd));
   }
   else
   {
      emit_at (Pc++, hibyte (wd));
      emit_at (Pc++, lobyte (wd));
   }
   if ((P_total - 1) < P_LIMIT)
   {
      P_wflag[P_total - 2] = true;
   }
}


void
eword2 (int wd)
{
   if (Pass == 1)
   {
      Pc++;
      return;
   }
   if (LowByteFirst)
   {
      emit_at (Pc << 1, lobyte (wd));
      emit_at ((Pc++ << 1) + 1, hibyte (wd));
   }
   else
   {
      emit_at (Pc << 1, hibyte (wd));
      emit_at ((Pc++ << 1) + 1, lobyte (wd));
   }
   if ((P_total - 1) < P_LIMIT)
   {
      P_wflag[P_total - 2] = true;
   }
}

/*
 * emit_char - prints a character to the object file
 */
void
emit_char (unsigned char ch)
{
   if (Pass == 1)
      return;
   switch (obj_format)
   {
     case F_NAGRAEDIT_NE2:
     case F_NAGRAEDIT_EP3:
     case F_NAGRAEDIT_EP10:
     case F_NAGRAEDIT_EP11:
	if (Obj_File == NULL)
	   LocalOpenObjectFile ();
	fprintf (Obj_File, "%c", ch);
	break;
   }
}


void
OpenObjectFile (char *name)
{
   strcpy (Obj_Name, name);
}


void
CloseObjectFile (Boolean keep)
{
   int i;
   unsigned char warnflag = 0;
   if (Obj_File != NULL)
   {
      dump_record ();
      switch (obj_format)
      {
	case F_MOT_S19:
	   fprintf (Obj_File, "S9030000FC\n");
	   break;
	case F_INTEL_HEX:
	   fprintf (Obj_File, ":00000001FF\n");
	   break;
	case F_BIN_64K:
	   fwrite (&binimage[0], 1, 0x10000, Obj_File);
	   break;
	case F_BIN_4K:
	   fwrite (&binimage[0xE000], 1, 0x1000, Obj_File);
	   for (i = 0; i <= 0xDFFF; i++)
	      if (binimage[i] != 0)
		 warnflag = 1;
	   for (i = 0xF000; i <= 0xFFFF; i++)
	      if (binimage[i] != 0)
		 warnflag = 1;
	   break;
	case F_BIN_8K:
	   fwrite (&binimage[0xC000], 1, 0x2000, Obj_File);
	   for (i = 0; i <= 0xDFFF; i++)
	      if (binimage[i] != 0)
		 warnflag = 1;
	   for (i = 0xF000; i <= 0xFFFF; i++)
	      if (binimage[i] != 0)
		 warnflag = 1;
	   break;
      }
      if (warnflag)
	 fprintf (stderr, "WARNING: Not all data written to output file\n");
      fclose (Obj_File);
      if (!keep)
	 unlink (Obj_Name);	       /* don't care if it fails */
   }
}

/*
 * SetObjectFormat - sets output file type
 * format = output format
 * force = flag to force no changes if called again
 *
 * The "force" option will force the format to stay that type, and not change
 */

int
SetObjectFormat (int format, char force)
{
   static int i = 0;

   if (i == 1 && obj_format != 0)
      return (0);
   if (force == 1)
   {
      i = 1;			       /* set flag so we won't get set again */
      obj_format = format;
      return (1);
   }
   if (obj_format == 0)
   {
      obj_format = format;
      return (1);
   }
   if (i == 0 && Pass == 2)
   {
      i = 1;
      obj_format = format;
      return (1);
   }
   return (0);
}
