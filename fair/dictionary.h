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

   This module provides an independent interface for a generic dictionary.

   The dictionary maps (pointers to) objects of type `struct sockaddr`
   into integers (file descriptors).

   Behind the interface it may be implemented with either a local
   algorithm or an external library (the point here is avoiding
   dependencies).

 */

#ifndef DICTIONARY_H
#define DICTIONARY_H

#include <stdint.h>
#include <stddef.h>
#include <arpa/inet.h>

typedef struct dict * dict_t;

/** Constructor for the dictionary.
 *
 * @param[in] af Either AF_INET or AF_INET6 (the latter is not supported,
 *               it's there just for future enhancements);
 * @param[in] conf A string configuration which may provides
 *                 implementation-dependent parameter
 *
 * @warning The only supported value for the af parameter is AF_INET, for
 *          the moment.
 *
 * @warning The conf parameter depends on the implementation.
 *
 * @return The newly allocated dictionary.
 */
dict_t dict_new (int af, char *conf);

/** Easy for the dictionary.
 *
 * @return The newly allocated dictionary.
 */
static inline
dict_t dict_new_default ()
{
    return dict_new(AF_INET, NULL);
}

/** Destructor for the dictionary.
 *
 * @param[in] D The dictionary to be destroyed.
 */
void dict_delete (dict_t D);

/** Lookup function.
 *
 * @param[in] D The dictionary;
 * @param[in] addr The address to search;
 * @param[out] fd The file descriptor (valid only if 0 is returned);
 *
 * @retval 0 on success;
 * @retval 1 on failure.
 */
int dict_lookup (dict_t D, const struct sockaddr * addr, int *fd);

/** Insertion function.
 *
 * @param[in] D The dictionary;
 * @param[in] addr The address to map;
 * @param[in] fd The file descriptor to be mapped on the address.
 */
int dict_insert (dict_t D, const struct sockaddr * addr, int fd);

/** Callback for dictionary looping.
 *
 * Provide a function complying with this type in order to scan a
 * dictionary. Note that this function will be parametrized with pointers
 * to the internally stored data, so you may use it to modify them.
 *
 * @warning If that's not intuitively clear, try not to modify the
 *          dictionary while you are looping on it, please...
 *
 * @param[in] ctx The user context argument;
 * @param[in,out] addr The address;
 * @param[in,out] fd The file descriptor.
 *
 * @see dict_scan
 */
typedef int (* dict_scancb_t) (void *ctx, struct sockaddr **addr,
                               int *fd);

/** Implementation-agnostic scanning procedure for the dictionary.
 *
 * This function can be used to scan on a dictionary. The dictionary can
 * so far be either scanned or modified pair by pair.
 *
 * @note The ctx parameter is provided in order to bring some user context
 *       inside the callback function. Please use it and avoid global
 *       variables, which are EVIL.
 *
 * @warning If that's not intuitively clear, try not to modify the
 *          dictionary while you are looping on it, please...
 *
 * @param[in,out] D The dictionary to loop on;
 * @param[in] cback The callback;
 * @param[in] ctx The user context;
 */
void dict_scan (dict_t D, dict_scancb_t cback, void *ctx);

#endif // DICTIONARY_H

