PROGNAME=dataset2sql
OBJS=$(patsubst %.c, %.o, $(wildcard *.c))

$(PROGNAME): $(OBJS)
	$(CC) -o $@ $^

clean:
	rm -f $(PROGNAME)
	rm -f *.o