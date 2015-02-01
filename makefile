SHELL = /bin/bash

PREFIX=.
BINDIR=$(PREFIX)/.
INCDIR = $(PREFIX)/.
SRCDIR = $(PREFIX)/.
OBJDIR = $(PREFIX)/.

CC = /usr/bin/gcc
LD = /usr/bin/gcc
AR = /usr/bin/ar
RM = /bin/rm
CP = /bin/cp

EXECS = $(BINDIR)/daemon

CFLAGS += -c -g -O2 -W -Wall -std=gnu99 -pedantic -I $(INCDIR)

DATE = `date +%d%m%Y`

ARCHIVE = code$(DATE).tar

HDRS  = $(INCDIR)/daemon.h $(INCDIR)/debug.h $(PREFIX)/makefile

# future library code objects
#AOBJS = $(OBJDIR)/sdr_mem.o		$(OBJDIR)/sdr_mem_utils.o

SPL_FLAGS =
# (Un)comment any/or all of the SPL_FLAGS lines below to
# en/dis-able any of the features
SPL_FLAGS += -DDEBUG_ON

#LDFLAGS += -l pthread

all: $(EXECS)

copy:
	echo 'Source Code backed up into '$(ARCHIVE)' file'
	tar cvf $(ARCHIVE) makefile input test_app.sdr_mem.ini \
			~/sdr_mem.ini $(SRCDIR)/*c $(INCDIR)/*h

#clean:
#	$(RM) -f $(BINDIR)/* $(OBJDIR)/*

$(BINDIR)/daemon: $(OBJDIR)/daemon.o
	$(LD) -o $@ $(LDFLAGS) $^

#$(BINDIR)/lib_sdr_mem.a: $(AOBJS)
#	$(AR) rvs $@ $(AOBJS)

$(OBJDIR)/daemon.o: $(SRCDIR)/daemon.c $(HDRS)
	$(CC) -o $@ $(CFLAGS) $(SPL_FLAGS) $(SRCDIR)/daemon.c
