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

/*

   Test for connection dictionary. How it works:

   -    It opens a server on the TCP/9001 port and loops periodically
        (1sec) seeking for new connections. At each round it dumps the
        collected table;

   -    You can do connections using `nc localhost 9000 &` (remember to
        make it background if you want to run multiple nc!);

   -    The server collects up to 10 connections, then it frees the
        connections: first the ones having even file descriptor, then the
        odd ones;

    -   The server terminates.

 */

#ifndef _POSIX_C_SOURCE
#   define _POSIX_C_SOURCE 200112L
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "dictionary.h"
#include "stuff.h"

static const int BUFLEN = 50;
static const int DEFAULT_BACKLOG = 10;

static void err_alert (int err, int fatal)
{
    char buffer[BUFLEN];

    strerror_r(err, buffer, BUFLEN);
    fprintf(stderr, "%srror in building server %s\n",
            fatal ? "Fatal e" : "E", buffer);
    if (fatal) exit(EXIT_FAILURE);
}

static int scan_cb_print (void *ctx, const struct sockaddr *addr, int fd)
{
    char buffer[INET_ADDRSTRLEN];

    const struct sockaddr_in *addr_in = (const struct sockaddr_in *) addr;
    inet_ntop(AF_INET, (const void *) &addr_in->sin_addr, buffer,
              INET_ADDRSTRLEN),
    fprintf(stderr, "Address %s(%i) -> fd=%i\n", buffer,
            ntohs(addr_in->sin_port), fd);

    return 1;
}

static int scan_cb_delete (void *ctx, const struct sockaddr *addr, int fd)
{
    char buffer[INET_ADDRSTRLEN];
    uintptr_t parity = (uintptr_t) ctx;

    const struct sockaddr_in *addr_in = (const struct sockaddr_in *) addr;
    inet_ntop(AF_INET, (const void *) &addr_in->sin_addr, buffer,
              INET_ADDRSTRLEN);

    if (fd % 2 == parity) {
        fprintf(stderr, "Dropping address %s(%i) -> fd=%i\n",
                buffer, ntohs(addr_in->sin_port), fd);
        close(fd);
        return 3;
    } else {
        fprintf(stderr, "Keeping address %s(%i) -> fd=%i\n",
                buffer, ntohs(addr_in->sin_port), fd);
        return 1;
    }
}

int main (int argc, char *argv[])
{
    sock_data_t serv;
    int err;
    dict_t D;

    sock_data_init(&serv, NULL, 9001);
    if (tcp_serve(&serv, DEFAULT_BACKLOG, &err)) {
        err_alert(err, 1);
    }

    int loop = 10;
    D = dict_new_default();
    while (loop) {
        socklen_t len = sizeof(struct sockaddr_in);
        struct sockaddr_in addr;
        int fd;

        /* Accepting phase */
        while ((fd = accept(serv.fd, (struct sockaddr *)&addr,
                &len)) != -1) {
            dict_insert(D, (struct sockaddr *)&addr, fd);
            write(fd, "Ok, thanks! :)\n", strlen("Ok, thanks! :)\n"));
            loop --;
            fprintf(stderr, "-%i\n", loop);
        }
        if (errno != EAGAIN && errno != EWOULDBLOCK) {
            err_alert(errno, 1);
        }

        sleep(1);
        fprintf(stderr, "starting to scan...\n");
        dict_scan(D, scan_cb_print, NULL);
    }
    fprintf(stderr, "closing even...\n");
    dict_scan(D, scan_cb_delete, (void *)0);
    fprintf(stderr, "closing odd...\n");
    dict_scan(D, scan_cb_delete, (void *)1);
    fprintf(stderr, "closing local fd...\n");
    close(serv.fd);
    fprintf(stderr, "goodbye.\n");

    dict_delete(D);

    exit(EXIT_SUCCESS);
}
