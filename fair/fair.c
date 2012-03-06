#include "fair.h"

#include <sys/select.h>
#include <errno.h>

struct fill_fd_set_data {
    fd_set *readfds;
    int maxfd;
    size_t count_unused;
};

static
dict_scanact_t scan_fill_fd_set (void *ctx, const struct sockaddr *addr,
                                 peer_info_t *info)
{
    /* Used initially to scan all the file descriptors and put them
     * into the file descriptors set. Also collecting the maximum of them
     * (used by select(2)) and count how much file descriptors have never
     * been used */
    struct fill_fd_set_data *sh = ctx;

    FD_SET(info->fd, sh->readfds);
    if (info->fd > sh->maxfd) {
        sh->maxfd = info->fd;
    }
    if (!info->flags.used) {
        sh->count_unused ++;
    }
    return DICT_SCAN_CONTINUE;
}

struct pick_fair_data {
    fd_set *readfds;
    struct {
        int fd;                 // Result fd
        const struct sockaddr *addr;  // Result address
    } result;
    struct {
        unsigned flip : 1;      // Search logic flipped (i.e. all used once)
        unsigned backup : 1;    // Some already used node can be read
    } flags;
};

static
dict_scanact_t scan_pick_fair (void *ctx, const struct sockaddr *addr,
                               peer_info_t *info)
{
    struct pick_fair_data *pf = ctx;

    if (pf->flags.flip) {

        /* In flipped logic we select the very first file descriptor (any
         * would do fine), then we continue the iteration setting
         * file descriptors to unused.
         *
         * Note: since here we flipped (i.e. "all used" become "all
         * unused", by construction there must be a node willing to
         * transmit (otherwise we would be still locked on select(2)).
         */
        if (pf->result.fd == -1 && FD_ISSET(info->fd, pf->readfds)) {
            /* First one. We'll use this, so we won't set it unused. */
            pf->result.fd = info->fd;
            pf->result.addr = addr;
        } else {
            info->flags.used = 0;
        }
        return DICT_SCAN_CONTINUE;

    } else {

        /* In non-flipped logick we are searching for a non-used node. If
         * we find it we stop and we are ok. However we may not find it,
         * so we also search in the used ones as backup plan */

        if (info->flags.used) {

            if (!pf->flags.backup && FD_ISSET(info->fd, pf->readfds)) {
                /* We have our backup plan */
                pf->flags.backup = 1;
                pf->result.fd = info->fd;
                pf->result.addr = addr;
            }
            return DICT_SCAN_CONTINUE;

        } else {

            if (FD_ISSET(info->fd, pf->readfds)) {
                /* We found our never-used peer */
                pf->result.fd = info->fd;
                pf->result.addr = addr;
            }
            return DICT_SCAN_STOP;

        }

    }
}

int fair_select(dict_t neighbours, int *peer_fd,
                const struct sockaddr **peer_addr,
                struct timeval *wait, int *e)
{
    fd_set readfds;
    int ret;

    struct fill_fd_set_data sh = {
        .readfds = &readfds,
        .maxfd = -1,
        .count_unused = 0
    };

    *peer_addr = NULL;  // TODO: remove after testing

    dict_scan(neighbours, scan_fill_fd_set, (void *) &sh);
    ret = select(sh.maxfd + 1, &readfds, NULL, NULL, wait);
    if (ret <= 0) {
        /* On timeout and error */
        if (e) *e = errno;
        return ret;
    }

    struct pick_fair_data pf = {
        .readfds = &readfds,
        .result = {
            .fd = -1
        },
        .flags = {
            .flip = !sh.count_unused,
            .backup = 0
        }
    };

    dict_scan(neighbours, scan_pick_fair, (void *) &pf);

    assert(pf.result.fd != -1);     // TODO: remove after testing

    *peer_fd = pf.result.fd;
    *peer_addr = pf.result.addr;

    return 1;
}
