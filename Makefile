CC=gcc
PROG=flowly
OBJDIR=build
SRCDIR=src
CFLAGS="-Wall"

FILES=config flowstat list sflow utils
OBJS=$(FILES:%=$(OBJDIR)/%.o)

all: $(OBJDIR) $(PROG)

$(PROG): $(OBJS) $(SRCDIR)/flowly.c
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(SRCDIR)/flowly.c

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) -c $(CFLAGS) -o "$@" "$<"

$(OBJDIR):
	mkdir -p $@

clean:
	rm $(PROG)
	rm -rf $(OBJDIR)
