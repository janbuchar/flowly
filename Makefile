CC=gcc
PROG=flowly
OBJDIR=build
SRCDIR=src
CFLAGS=-Wall
LDFLAGS=-lrt

FILES=config flowstat list output sflow utils
OBJS=$(FILES:%=$(OBJDIR)/%.o)

all: $(OBJDIR) $(PROG)

$(PROG): $(OBJS) $(SRCDIR)/$(PROG).c
	$(CC) $(CFLAGS) -o $@ $(LDFLAGS) $(OBJS) $(SRCDIR)/$(PROG).c

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) -c $(CFLAGS) -o "$@" $(LDFLAGS) "$<"

$(OBJDIR):
	mkdir -p $@

clean:
	rm $(PROG)
	rm -rf $(OBJDIR)
