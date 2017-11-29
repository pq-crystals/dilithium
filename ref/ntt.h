#ifndef NTT_H
#define NTT_H

#include <stdint.h>

void ntt(uint32_t *p);
void invntt_frominvmont(uint32_t *p);

#endif
