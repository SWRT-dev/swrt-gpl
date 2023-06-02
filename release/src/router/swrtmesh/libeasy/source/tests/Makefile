PROG = test_event
OBJS = test_event.o

PROG_CFLAGS = $(CFLAGS) -fstrict-aliasing -I..
PROG_LDFLAGS = $(LDFLAGS) -L..
PROG_LIBS= -ljson-c \
	   -lnl-genl-3 -lnl-3 \
	   -leasy

%.o: %.c
	$(CC) $(PROG_CFLAGS) -c $< -o $@

.PHONY: all

all: test_event

test_event: $(OBJS)
	$(CC) $(PROG_LDFLAGS) -o $@ $^ $(PROG_LIBS)

clean:
	rm -f *.o $(PROG)
