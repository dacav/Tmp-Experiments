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

#include "dictionary.h"

#if 0
/* WORK IN PROGRESS!!! */

// Arbitrary and reasonable(?) prime number.
static const unsigned DEFAULT_N_BUCKETS = 23;

struct dict {
    dhash_t *ht;
};

uintptr_t hash_sockaddr_in (const void *key)
{
    return ((const struct sockaddr_in *)key)->sin_addr.s_addr;
}

int cmp_sockaddr_in (const void *v0, const void *v1)
{
    // TODO replace with net_helper-all comparison & friends
    return memcmp(v0, v1, sizeof(struct sockaddr_in));
}

dict_t dict_new (int af)
{
    if (af != AF_INET) {
        fprintf(stderr, "dictionary: Only AF_INET is supported for the"
                        " moment\n");
        abort();
    }

    dict_t ret = malloc(sizeof(struct dict));
    if (ret == NULL) return NULL;
    ret->ht = dhash_new(DEFAULT_N_BUCKETS, hash_sockaddr_in,
                        cmp_sockaddr_in);
    return ret;
}

void dict_delete (dict_t D)
{
}

int dict_lookup (dict_t D, const struct sockaddr * addr, int *fd)
{
}

int dict_insert (dict_t D, const struct sockaddr * addr, int fd)
{
}

void dict_scan (dict_t D, void *ctx, dict_scancb_t cback)
{
}

#endif

