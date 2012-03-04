.PHONY : all clean

APPS := dict-test
CFLAGS := -std=c99 -L/usr/local/lib -L/usr/local/lib32 -L/usr/local/lib64

all: $(APPS)

clean:
	rm -f $(APPS)
	rm -f *.o

dict-test: dict-test.o dictionary.o stuff.o
	$(CC) $(CFLAGS) $^ -ldacav -o $@
