#include "stuff.h"

int tcp_connect (sock_data_t *sd, int *e)
{
    int fd;

    if (sd->fd != -1) {
        /* Already connected */
        return 0;
    }

    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) {
        if (e) *e = errno;
        return -1;
    }

    if (connect(fd, (struct sockaddr *) &sd->addr,
            sizeof(struct sockaddr_in)) == -1) {
        if (e) *e = errno;
        close(fd);
        return -2;
    }
    sd->fd = fd;

    return 0;
}

int tcp_serve (sock_data_t *sd, int backlog, int *e)
{
    int fd;

    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) {
        if (e) *e = errno;
        return -1;
    }

    if (bind(fd, (struct sockaddr *) &sd->addr,
            sizeof(struct sockaddr_in))) {
        if (e) *e = errno;
        close(fd);
        return -2;
    }

    if (listen(fd, backlog) == -1) {
        if (e) *e = errno;
        close(fd);
        return -3;
    }
    sd->fd = fd;

    return 0;
}
