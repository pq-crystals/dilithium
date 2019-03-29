#ifndef REDUCE_H
#define REDUCE_H

#include <stdint.h>

void reduce_avx(uint32_t a[N]);
void csubq_avx(uint32_t a[N]);

#endif
