.PHONY : all clean

APPS := pingpong select-on-socket

CFLAGS := #-DBLOCKING

all: $(APPS)

clean:
	rm -f $(APPS)
	rm -f *.o

pingpong: pingpong.o stuff.o

accept-on-socket: accept-on-socket.o stuff.o

