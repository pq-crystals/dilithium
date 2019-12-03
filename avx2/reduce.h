#ifndef REDUCE_H
#define REDUCE_H

#include <stdint.h>

void reduce_avx(uint32_t a[N]) asm("reduce_avx");
void csubq_avx(uint32_t a[N]) asm("csubq_avx");

#endif
