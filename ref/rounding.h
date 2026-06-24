/* SPDX-License-Identifier: Apache-2.0 OR CC0-1.0
 * Copyright: Léo Ducas, Eike Kiltz, Tancrède Lepoint, Vadim Lyubashevsky, Gregor Seiler, Peter Schwabe, Damien Stehlé
 * */

#ifndef ROUNDING_H
#define ROUNDING_H

#include <stdint.h>
#include "params.h"

#define power2round DILITHIUM_NAMESPACE(power2round)
int32_t power2round(int32_t *a0, int32_t a);

#define decompose DILITHIUM_NAMESPACE(decompose)
int32_t decompose(int32_t *a0, int32_t a);

#define make_hint DILITHIUM_NAMESPACE(make_hint)
unsigned int make_hint(int32_t a0, int32_t a1);

#define use_hint DILITHIUM_NAMESPACE(use_hint)
int32_t use_hint(int32_t a, unsigned int hint);

#endif
