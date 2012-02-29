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

#include "stuff.h"

int tcp_connect (sock_data_t *sd, int *e);
int tcp_serve (sock_data_t *sd, int backlog, int *e);

static const unsigned DEFAULT_BACKLOG = 50;

/* -- Interface exported symbols -------------------------------------- */

static const unsigned BUFLEN = INET_ADDRSTRLEN > 256 ?
                               INET_ADDRSTRLEN : 256;

static void client ()
{
    sock_data_t self;
    char buffer[BUFLEN];
    int e;

    sock_data_init(&self, "127.0.0.1", 9000);

    if (tcp_connect(&self, &e)) {
        strerror_r(e, buffer, BUFLEN);
        fprintf(stderr, "Connecting to server: %s\n", buffer);
        return;
    }

    write(self.fd, "hello world", strlen("hello world"));
    close(self.fd);
}

static void server ()
{
    sock_data_t serv;
    sock_data_t clnt;
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

    socklen_t addrlen = sizeof(struct sockaddr_in);
    clnt.fd = accept(serv.fd, (struct sockaddr *)&clnt.addr,
                     &addrlen);
    if (clnt.fd == -1) {
        strerror_r(errno, buffer, BUFLEN);
        fprintf(stderr, "No accept: %s\n", buffer);
        close(serv.fd);
        return;
    }

    printf("Got connection from %s\n",
           sock_data_ipstring(&clnt, buffer));

    memset((void *)buffer, 0, BUFLEN);
    while ((R = read(clnt.fd, buffer, BUFLEN - 1)) > 0) {
        buffer[R] = 0;
        printf("> %s\n", buffer);
    }
    if (R == 0) {
        fprintf(stderr, "End of transmission\n");
    } else {
        strerror_r(errno, buffer, BUFLEN);
        fprintf(stderr, "Error in transmission: %s\n", buffer);
    }
    close(clnt.fd);
    close(serv.fd);
}

int main (int argc, char **argv)
{
    pid_t P = fork();

    if (P == 0) {
        // child
        sleep(2); // This is a dirty way of giving the server time to be
                  // running
        client();
        printf("Bye 0\n");
    } else {
        // father
        server();
        waitpid(P, NULL, 0);
        printf("Bye 1\n");
    }

    return 0;
}
