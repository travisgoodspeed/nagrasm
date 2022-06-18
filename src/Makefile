# nagrasm Makefile
#
# $Id: Makefile 13 2004-02-11 06:09:11Z eric $
#
# Copyright 1991, 1992, 1994, 1995, 1996, 2004 Eric Smith.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License version 2 as
# published by the Free Software Foundation.  Note that permission is
# not granted to redistribute this program under the terms of any
# other version of the General Public License.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111  USA

# Conditionals:  uncomment the following defines as nessary.  Note that a
# "0" value is considered true by make, so to disable conditionals comment
# them out or set them to a null string.

#DEBUG=1
#EFENCE=1
#STATIC=1
CFLAGS = -Wall
LDFLAGS =
LDLIBS =

ifdef DEBUG
CFLAGS := $(CFLAGS) -g
LDFLAGS := $(LDFLAGS) -g
else
CFLAGS := $(CFLAGS) -O3
endif

ifdef EFENCE
LDLIBS := $(LDLIBS) -lefence -lpthread
endif

ifdef STATIC
LDLIBS := -Wl,--static $(LDLIBS)
# I have to add the following line, so it will work
# there appears to be a problem with debian unstable
# and I can not get it to compile a static version
# with gcc-3.3 no matter what I do
CC := gcc-3.2
endif


# -----------------------------------------------------------------------------
# You shouldn't have to change anything below this point, but if you do please
# let me know why so I can improve this Makefile.
# -----------------------------------------------------------------------------

VERSION = 1.0.1

PACKAGE = nagrasm

TARGETS = nagrasm

CSRCS = as.c eval.c macro.c pseudo.c symtab.c tables.c util.c \
	input.c output.c object.c \
	st7.c
HDRS = as.h
MISC = COPYING MANIFEST README Makefile asrefman.txt

DISTFILES = $(MISC) $(HDRS) $(CSRCS)
DISTNAME = $(PACKAGE)-$(VERSION)

BIN_DISTFILES = COPYING README asrefman.txt $(TARGETS)


CDEFINES = -D$(PACKAGE)_VERSION=$(VERSION)

CFLAGS := $(CFLAGS) $(CDEFINES)


all: $(TARGETS)


OBJS = $(CSRCS:.c=.o)


nagrasm: $(OBJS)
	$(LINK.o) $^ $(LOADLIBES) $(LDLIBS) -o $@
ifndef DEBUG
	strip $@
	upx --best $@
endif


dist: $(DISTFILES)
	-rm -rf $(DISTNAME)
	mkdir $(DISTNAME)
	for f in $(DISTFILES); do ln $$f $(DISTNAME)/$$f; done
	tar --gzip -chf $(DISTNAME).tar.gz $(DISTNAME)
	-rm -rf $(DISTNAME)


bin-dist-rh: $(BIN_DISTFILES) /etc/redhat-release
	tar --gzip -chf $(DISTNAME)-rh$(shell sed 's/^Red Hat Linux release \([0-9][0-9.]*\) (.*)/\1/' </etc/redhat-release).tar.gz $(BIN_DISTFILES)

bin-dist-fc: $(BIN_DISTFILES) /etc/fedora-release
	tar --gzip -chf $(DISTNAME)-fc$(shell sed 's/^Fedora Core release \([0-9][0-9.]*\) (.*)/\1/' </etc/fedora-release).tar.gz $(BIN_DISTFILES)

clean:
	rm -f *.o *.d $(TARGETS) $(AUTO_CSRCS) $(AUTO_HDRS) $(AUTO_MISC)

wc:
	wc -l $(HDRS) $(CSRCS) $(MISC)

ls-lt:
	ls -lt $(HDRS) $(CSRCS) $(MISC)



DEPENDS = $(CSRCS:.c=.d)

%.d: %.c
	$(CC) -M -MG $(CFLAGS) $< | sed -e 's@ /[^ ]*@@g' -e 's@^\(.*\)\.o:@\1.d \1.o:@' > $@

include $(DEPENDS)
