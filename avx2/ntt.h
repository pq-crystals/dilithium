#ifndef NTT_H
#define NTT_H

#include <stdint.h>
#include "params.h"

extern const uint32_t zetas[N];
extern const uint32_t zetas_inv[N];

void ntt_levels0t2_avx(uint64_t tmp[N],
                       const uint32_t a[N],
                       const uint32_t zetas[7]) asm("ntt_levels0t2_avx");
void ntt_levels3t8_avx(uint32_t a[N],
                       const uint64_t tmp[N],
                       const uint32_t zetas[31]) asm("ntt_levels3t8_avx");

void invntt_levels0t4_avx(uint64_t tmp[N],
                          const uint32_t a[N],
                          const uint32_t zetas_inv[31]) asm("invntt_levels0t4_avx");
void invntt_levels5t7_avx(uint32_t a[N],
                          const uint64_t tmp[N],
                          const uint32_t zetas_inv[7]) asm("invntt_levels5t7_avx");

void pointwise_avx(uint32_t c[N], const uint32_t a[N], const uint32_t b[N])
        asm("pointwise_avx");
void pointwise_acc_avx(uint32_t c[N], const uint32_t *a, const uint32_t *b)
        asm("pointwise_acc_avx");

#endif
