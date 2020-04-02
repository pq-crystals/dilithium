#ifndef ROUNDING_H
#define ROUNDING_H

#include <stdint.h>
#include "params.h"

#define power2round NAMESPACE(power2round)
uint32_t power2round(const uint32_t a, uint32_t *a0);

#define decompose NAMESPACE(decompose)
uint32_t decompose(uint32_t a, uint32_t *a0);

#define make_hint NAMESPACE(make_hint)
unsigned int make_hint(const uint32_t a0, const uint32_t a1);

#define use_hint NAMESPACE(use_hint)
uint32_t use_hint(const uint32_t a, const unsigned int hint);

#endif
