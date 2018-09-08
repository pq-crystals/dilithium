#ifndef REDUCE_H
#define REDUCE_H

#include <stdint.h>

void reduce32(uint32_t *a, const uint32_t *b) asm("reduce32");;
void csubq(uint32_t *a, const uint32_t *b) asm("csubq");

#endif
