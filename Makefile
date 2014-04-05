CC=gcc
CFLAGS="-Wall"

debug:clean
	$(CC) $(CFLAGS) -g -o flowly main.c
stable:clean
	$(CC) $(CFLAGS) -o flowly main.c
clean:
	rm -vfr *~ flowly
