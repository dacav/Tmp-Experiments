#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>

static const size_t MaxBuf = 256;

int main (int argc, char *argv[])
{
    FILE *in;
    uint8_t bytes[MaxBuf];
    int i;

    if (argc < 2) {
        fprintf(stderr, "input file?\n");
        exit(EXIT_FAILURE);
    }

    in = fopen(argv[1], "rb");
    i = 0;
    while (!feof(in)) {
        fread((void *)bytes, sizeof(uint8_t), MaxBuf, in);
        fprintf(stderr, "\rread: %05i", i++);
        usleep(5000);
    }
    fclose(in);
    fprintf(stderr, "\nBye\n");

    exit(EXIT_SUCCESS);
}
