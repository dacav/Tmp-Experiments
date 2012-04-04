.PHONY : all clean

CFLAGS := -Wall -I.
CXXFLAGS := -Wall -I.
CC := g++
APPS := recv send pingpong-aio pingpong-aio2
LDLIBS := -lrt

all: $(APPS)

recv : recv.o
send : send.o
pingpong-aio : pingpong-aio.o stuff.o
pingpong-aio2 : pingpong-aio2.o stuff.o

clean:
	@rm -vf *o $(APPS)
