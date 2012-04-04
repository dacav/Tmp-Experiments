.PHONY : all clean

CFLAGS := -Wall -I.
CXXFLAGS := -Wall -I.
CC := g++
APPS := recv send
LDLIBS := -lrt

all: $(APPS)

recv : recv.o
send : send.o

clean:
	@rm -vf *o $(APPS)
