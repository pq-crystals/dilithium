#ifndef REDUCE_H
#define REDUCE_H

#include <stdint.h>
#include "params.h"

#define reduce_avx NAMESPACE(reduce_avx)
void reduce_avx(uint32_t a[N], const uint32_t *qdata);
#define csubq_avx NAMESPACE(csubq_avx)
void csubq_avx(uint32_t a[N], const uint32_t *qdata);

#endif
