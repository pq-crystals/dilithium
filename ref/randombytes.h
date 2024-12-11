/* SPDX-License-Identifier: Apache-2.0 OR CC0-1.0
 * Copyright: Léo Ducas, Eike Kiltz, Tancrède Lepoint, Vadim Lyubashevsky, Gregor Seiler, Peter Schwabe, Damien Stehlé
 * */

#ifndef RANDOMBYTES_H
#define RANDOMBYTES_H

#include <stddef.h>
#include <stdint.h>

void randombytes(uint8_t *out, size_t outlen);

#endif
