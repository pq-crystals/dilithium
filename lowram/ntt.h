#ifndef NTT_H
#define NTT_H

#include <stdint.h>
#include "params.h"

#define ntt DILITHIUM_NAMESPACE(ntt)
void ntt(int32_t a[N]);

#define invntt_tomont DILITHIUM_NAMESPACE(invntt_tomont)
void invntt_tomont(int32_t a[N]);

/* Core small NTT functions for 16-bit coefficients */
#define small_ntt DILITHIUM_NAMESPACE(small_ntt)
void small_ntt(int16_t r[256]);

#define small_invntt_tomont DILITHIUM_NAMESPACE(small_invntt_tomont)
void small_invntt_tomont(int16_t r[256]);

#define small_basemul DILITHIUM_NAMESPACE(small_basemul)
void small_basemul(int16_t r[2], const int16_t a[2], const int16_t b[2], int16_t zeta);

extern const int16_t small_zetas[128];

#endif
