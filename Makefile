CC=gcc
PROG=flowly
OBJDIR=build
SRCDIR=src
CFLAGS="-Wall"

FILES=config flowstat list sflow utils
OBJS=$(FILES:%=$(OBJDIR)/%.o)

all: $(PROG)

$(PROG): $(OBJS) $(SRCDIR)/flowly.c
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(SRCDIR)/flowly.c

# compile each *.o
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) -c $(CFLAGS) -o "$@" "$<"

clean:
	rm -vfr *~ flowly
