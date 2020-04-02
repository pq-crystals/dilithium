#ifndef NTT_H
#define NTT_H

#include <stdint.h>
#include "params.h"

#define ntt NAMESPACE(ntt)
void ntt(uint32_t p[N]);

#define invntt_tomont NAMESPACE(invntt_tomont)
void invntt_tomont(uint32_t p[N]);

#endif
