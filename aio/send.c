#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <aio.h>
#include <errno.h>
#include <unistd.h>

static const size_t Max = (1<<30);

int main (int argc, char *argv[])
{
    struct aiocb aio_info;
    uint8_t *buffer;
    FILE *out;
    unsigned i;

    if (argc < 2) {
        fprintf(stderr, "output file?\n");
        exit(EXIT_FAILURE);
    }

    buffer = (uint8_t *)calloc(Max, sizeof(uint8_t));
    for (i = 0; i < Max; i ++) {
        if ((buffer[i] = i % 256) == 0) {
            fprintf(stderr, "\r%03.02f%% ", (float)i / Max * 100);
            fflush(stderr);
        }
    }
    fprintf(stderr, "\r%03.02f%% \nOk\n", 100.0f);

    memset(&aio_info, 0, sizeof(struct aiocb));

    out = fopen(argv[1], "wb");
    aio_info.aio_fildes = fileno(out);
    aio_info.aio_buf = (void *) buffer;
    aio_info.aio_nbytes = Max;

    aio_write(&aio_info);

#ifdef DIE_BADLY
    /* Heh, if it has been copied then this should not be a problem
     * right? */
    free(buffer);
#endif

    int run = 1;
    i = 0;
    while (run) {
        if (aio_error(&aio_info) == EINPROGRESS) {
            fprintf(stderr, "Work in progress... (%i)\n", i ++);
            usleep(500000);
        } else {
            fprintf(stderr, "Sent %li\n", aio_return(&aio_info));
            run = 0;
        }
    }

    fclose(out);
    free(buffer);

    exit(EXIT_SUCCESS);
}

