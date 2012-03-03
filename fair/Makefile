.PHONY : all clean

LDFLAGS := -L/usr/local/lib

APPS := dict-test

all: $(APPS)

clean:
	rm -f $(APPS)
	rm -f *.o

dict-test: dict-test.o dictionary.o stuff.o
	$(CC) -L/usr/local/lib -ldacav $^ -o $@
