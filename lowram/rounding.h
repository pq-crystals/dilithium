#ifndef ROUNDING_H
#define ROUNDING_H

#include <stdint.h>
#include "params.h"

#define power2round DILITHIUM_NAMESPACE(power2round)
int32_t power2round(int32_t *a0, int32_t a);

/* Functions moved from poly.h */
#define highbits DILITHIUM_NAMESPACE(highbits)
int32_t highbits(int32_t a);

#define lowbits DILITHIUM_NAMESPACE(lowbits)
int32_t lowbits(int32_t a);

#define make_hint_lowram DILITHIUM_NAMESPACE(make_hint_lowram)
int32_t make_hint_lowram(int32_t z, int32_t r);
#define decompose DILITHIUM_NAMESPACE(decompose)
int32_t decompose(int32_t *a0, int32_t a);
#define use_hint DILITHIUM_NAMESPACE(use_hint)
int32_t use_hint(int32_t a, unsigned int hint);

#endif
