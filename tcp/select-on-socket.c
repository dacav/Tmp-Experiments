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
 */

#include "stuff.h"

#include <sys/select.h>

/* -- Internal functions ---------------------------------------------- */

int tcp_connect (sock_data_t *sd, int *e);
int tcp_serve (sock_data_t *sd, int backlog, int *e);

static const unsigned DEFAULT_BACKLOG = 50;

/* -- Interface exported symbols -------------------------------------- */

static const unsigned BUFLEN = 120;

static sock_data_t server ()
{
    sock_data_t serv;
    char buffer[BUFLEN];
    ssize_t R;
    int e;

    sock_data_init(&serv, NULL, 9000);
    if (tcp_serve(&serv, DEFAULT_BACKLOG, &e) < 0) {
        strerror_r(e, buffer, BUFLEN);
        fprintf(stderr, "Creating server: %s\n",
                buffer);
        return;
    }
    return serv;
}

<<<<<<< HEAD
static void wait_for_it (int fd)
{
    fd_set S;

    FD_SET(fd, &S);
    fprintf(stderr, "Waiting (possibly forver)...");
    select(fd + 1, &S, NULL, NULL, NULL);
    fprintf(stderr, "Ok, we got it!");
}

=======
>>>>>>> ea3268c... start
static void is_incoming (sock_data_t *srv)
{
    char buffer[BUFLEN];

    int fd = srv->fd;
    while (1) {
        sock_data_t clnt;
        socklen_t addrlen = sizeof(struct sockaddr_in);

        sleep(1);
<<<<<<< HEAD
        wait_for_it(fd);
=======
>>>>>>> ea3268c... start
        clnt.fd = accept(fd, (struct sockaddr *)&clnt.addr,
                         &addrlen);
        if (clnt.fd == -1) {
            strerror_r(errno, buffer, BUFLEN);
            fprintf(stderr, "No accept: %s\n", buffer);
            if (errno != EAGAIN && errno != EWOULDBLOCK) {
                fprintf(stderr, "Well... shit\n");
                return;
            }
        } else {
            close(clnt.fd);
            fprintf(stderr, "Got connection from %s\n",
                    sock_data_ipstring(&clnt, buffer));
            fprintf(stderr, "Accept ok. Now closing\n");
        }
    }
}

int main (int argc, char **argv)
{
    sock_data_t srv = server();
    is_incoming(&srv);
}
