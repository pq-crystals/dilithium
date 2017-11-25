#ifndef NTT_H
#define NTT_H

#include "poly.h"

void ntt(uint32_t *p);
void invntt_frommontgomery(uint32_t *p);

#endif
