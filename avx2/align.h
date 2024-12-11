/* SPDX-License-Identifier: Apache-2.0 OR CC0-1.0
 * Copyright: Léo Ducas, Eike Kiltz, Tancrède Lepoint, Vadim Lyubashevsky, Gregor Seiler, Peter Schwabe, Damien Stehlé
 * */

#ifndef ALIGN_H
#define ALIGN_H

#include <stdint.h>
#include <immintrin.h>

#define ALIGNED_UINT8(N)        \
    union {                     \
        uint8_t coeffs[N];      \
        __m256i vec[(N+31)/32]; \
    }

#define ALIGNED_INT32(N)        \
    union {                     \
        int32_t coeffs[N];      \
        __m256i vec[(N+7)/8];   \
    }

#endif
