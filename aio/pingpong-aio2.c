/*
 * Copyright (c) 2012 Giovanni Simoni, Arber Fama
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#include <aio.h>

#include "stuff.h"

int tcp_connect (sock_data_t *sd, int *e);
int tcp_serve (sock_data_t *sd, int backlog, int *e);

static const unsigned DEFAULT_BACKLOG = 50;

/* -- Interface exported symbols -------------------------------------- */

static const unsigned BUFLEN = INET_ADDRSTRLEN > 256 ?
                               INET_ADDRSTRLEN : 256;


static const size_t Max = (1<<30);

static void aio_send(int fd) {
    struct aiocb aio_info;
    uint8_t *buffer;
    unsigned i;

    buffer = (uint8_t *)calloc(Max, sizeof(uint8_t));
    for (i = 0; i < Max; i ++) {
        if ((buffer[i] = i % 256) == 0) {
            fprintf(stderr, "\r%03.02f%% ", (float)i / Max * 100);
            fflush(stderr);
        }
    }
    fprintf(stderr, "\r%03.02f%% \nOk\n", 100.0f);

    memset(&aio_info, 0, sizeof(struct aiocb));

    aio_info.aio_fildes = fd;
    aio_info.aio_buf = (void *) buffer;
    aio_info.aio_nbytes = Max;

    aio_write(&aio_info);

    int run = 1;
    i = 0;
    while (run) {
        if (aio_error(&aio_info) == EINPROGRESS) {
            fprintf(stderr, "Work in progress... (%i)\n", i ++);
            usleep(500000);
        } else {
            fprintf(stderr, "Sent %li\n", (long int)aio_return(&aio_info));
            run = 0;
        }
    }

    free(buffer);
}

static void client (const char *addr, short port)
{
    sock_data_t self;
    char buffer[BUFLEN];
    int e;

    sock_data_init(&self, addr, port);

    if (tcp_connect(&self, &e)) {
        strerror_r(e, buffer, BUFLEN);
        fprintf(stderr, "Connecting to server: %s\n", buffer);
        return;
    }

    aio_send(self.fd);
    close(self.fd);
}

int main (int argc, char **argv)
{
    const char *ip;
    short port;

    ip = "127.0.0.1";
    port = 9000;

    if (argc > 1) {
        ip = argv[1];
        if (argc > 2) {
            short p;
            p = atoi(argv[2]);
            if (p != 0) port = p;
        }
    }

    client(ip, port);
    printf("Bye 0\n");

    return 0;
}
