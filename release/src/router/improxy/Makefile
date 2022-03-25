CFLAGS +=  -Wall -I./include

SOURCES = $(wildcard ./src/*.c)
OBJS = $(patsubst %.c,%.o,$(SOURCES))

%.o: %.c
	$(CC) -MMD $(CFLAGS) -g -c -o $@ $< 

all : improxy

improxy : $(OBJS)
	$(CC) $(LDFLAGS) -o $@  $(OBJS) 

-include $(OBJS:.o=.d)

clean :
	rm -rf ${OBJS} $(DEPS) improxy
