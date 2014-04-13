CC=gcc
PROG=flowly
OBJDIR=build
SRCDIR=src
CFLAGS="-Wall"

all: $(PROG)

$(PROG): $(OBJDIR)/flowly.o $(OBJDIR)/config.o

$(OBJDIR)/config.o: $(SRCDIR)/flowly.c

$(OBJDIR)/flowly.o: $(SRCDIR)/flowly.c

clean:
	rm -vfr *~ flowly
