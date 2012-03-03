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

#ifndef __defined_stuff_h
#define __defined_stuff_h

#define _XOPEN_SOURCE 600
#undef  _GNU_SOURCE

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>

typedef struct {
    struct sockaddr_in addr;
    int fd;
} sock_data_t;

static inline
int sock_data_cmp (const sock_data_t *s1, const sock_data_t *s2)
{
    return memcmp(&s1->addr, &s2->addr, sizeof(struct sockaddr_in));
}

static inline
int sock_data_equal (const sock_data_t *s1, const sock_data_t *s2)
{
    return sock_data_cmp(s1, s2) == 0;
}

static inline
int sock_data_init (sock_data_t *s, const char *IPaddr, int port)
{
    s->addr.sin_family = AF_INET;
    s->addr.sin_port = htons(port);
    s->fd = -1;
    if (IPaddr == NULL) {
        s->addr.sin_addr.s_addr = INADDR_ANY;
        return 1;
    } else {
        /* NOTE: returns 0 on error */
        return inet_pton(AF_INET, IPaddr, &s->addr.sin_addr);
    }
}

static inline
const char * sock_data_ipstring (const sock_data_t *s, char * strrep)
{
    return inet_ntop(AF_INET, (const void *) &s->addr.sin_addr, strrep,
                     INET_ADDRSTRLEN);
}

int tcp_connect (sock_data_t *sd, int *e);
int tcp_serve (sock_data_t *sd, int backlog, int *e);

#endif // __defined_stuff_h

