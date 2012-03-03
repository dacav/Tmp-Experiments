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

#include <dacav/dacav.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>

#include "dictionary.h"

// Arbitrary and reasonable(?) prime number.
static const unsigned DEFAULT_N_BUCKETS = 23;

struct dict {
    dhash_t *ht;
};

static
uintptr_t sockaddr_hash (const void *key)
{
    return ((const struct sockaddr_in *)key)->sin_addr.s_addr;
}

static
int sockaddr_cmp (const void *v0, const void *v1)
{
    // TODO replace with proper comparison procedure & friends
    return memcmp(v0, v1, sizeof(struct sockaddr_in));
}

static
void * sockaddr_copy (const void *s)
{
    void * ret = malloc(sizeof(struct sockaddr_in));
    assert(ret != NULL);
    memcpy(ret, s, sizeof(struct sockaddr_in));
    return ret;
}

dict_t dict_new (int af, char *conf)
{
    if (af != AF_INET) {
        fprintf(stderr, "dictionary: Only AF_INET is supported for the"
                        " moment\n");
        abort();
    }

    /* TODO: here add the configuration reading of GRAPES */

    dict_t ret = malloc(sizeof(struct dict));
    assert(ret != NULL);

    dhash_cprm_t cprm = {
        .cp = sockaddr_copy,
        .rm = free
    };
    ret->ht = dhash_new(DEFAULT_N_BUCKETS, sockaddr_hash,
                        sockaddr_cmp, &cprm, NULL);
    return ret;
}

void dict_delete (dict_t D)
{
    dhash_free(D->ht);
    free(D);
}

int dict_lookup (dict_t D, const struct sockaddr * addr, int *fd)
{
    intptr_t out;

    if (dhash_search(D->ht, (const void *)addr, (void **)&fd)
            == DHASH_FOUND) {
        *fd = out;
        return 0;
    }
    return -1;
}

int dict_insert (dict_t D, const struct sockaddr * addr, int fd)
{
    intptr_t in = fd;
    return dhash_insert(D->ht, (const void *)addr, (void *)in)
           == DHASH_FOUND ? 1 : 0;
}

int dict_remove (dict_t D, const struct sockaddr * addr)
{
    return dhash_delete(D->ht, (const void *)addr, NULL)
           == DHASH_FOUND ? 0 : -1;
}

#include <stdio.h>

void dict_scan (dict_t D, dict_scancb_t cback, void *ctx)
{
    diter_t *it = dhash_iter_new(D->ht);
    int go = 1;
    while (go && diter_hasnext(it)) {
        dhash_pair_t *P = diter_next(it);

        intptr_t fd = (intptr_t) dhash_val(P);
        switch (cback(ctx, dhash_key(P), (int)fd)) {
            case 3: // delete and continue
                diter_remove(it, NULL);
            case 1: // keep and continue
                break;
            case 2: // delete and stop
                diter_remove(it, NULL);
            case 0: // keep and stop
                go = 0;
                break;
        }
    }
    dhash_iter_free(it);
}

