#ifndef FAIR_H
#define FAIR_H

#include "dictionary.h"

#include <sys/time.h>

/** Fair select of the receiving peer.
 *
 * The function does a select(2) on the neighbors.
 *
 * @param[in] hash The hash table;
 * @param[out] fd  The file descriptor of the peer which sent the
 *                 data;
 * @param[out] addr The corresponding address;
 * @param[out] e Where to copy errno just after the internal select. You
 *               may provide it as null.
 *
 * @retval 1 if there's a node ready for reading (the output parameters
 *           will point to neighbor details);
 * @retval 0 if there's not such a node (the output parameters will be
 *           invalid);
 * @retval -1 On select error.
 */
int fair_select(dict_t neighbours, int *peer_fd,
                const struct sockaddr **peer_addr,
                struct timeval *timeout, int *e);

#endif // FAIR_H

