#ifndef NTT_H
#define NTT_H

#include "params.h"
#include <stdint.h>

#define ntt_avx NAMESPACE(ntt_avx)
void ntt_avx(uint32_t a[N], const uint32_t *qdata);
#define invntt_avx NAMESPACE(invntt_avx)
void invntt_avx(uint32_t a[N], const uint32_t *qdata);

#define pointwise_avx NAMESPACE(pointwise_avx)
void pointwise_avx(uint32_t c[N],
                   const uint32_t a[N],
                   const uint32_t b[N],
                   const uint32_t *qdata);
#define pointwise_acc_avx NAMESPACE(pointwise_acc_avx)
void pointwise_acc_avx(uint32_t c[N],
                       const uint32_t *a,
                       const uint32_t *b,
                       const uint32_t *qdata);

#endif
