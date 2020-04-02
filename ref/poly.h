#ifndef POLY_H
#define POLY_H

#include <stdint.h>
#include "params.h"

typedef struct {
  uint32_t coeffs[N];
} poly __attribute__((aligned(32)));

#define poly_reduce NAMESPACE(poly_reduce)
void poly_reduce(poly *a);
#define poly_csubq NAMESPACE(poly_csubq)
void poly_csubq(poly *a);
#define poly_freeze NAMESPACE(poly_freeze)
void poly_freeze(poly *a);

#define poly_add NAMESPACE(poly_add)
void poly_add(poly *c, const poly *a, const poly *b);
#define poly_sub NAMESPACE(poly_sub)
void poly_sub(poly *c, const poly *a, const poly *b);
#define poly_shiftl NAMESPACE(poly_shiftl)
void poly_shiftl(poly *a);

#define poly_ntt NAMESPACE(poly_ntt)
void poly_ntt(poly *a);
#define poly_invntt_tomont NAMESPACE(poly_invntt_tomont)
void poly_invntt_tomont(poly *a);
#define poly_pointwise_montgomery NAMESPACE(poly_pointwise_montgomery)
void poly_pointwise_montgomery(poly *c, const poly *a, const poly *b);

#define poly_power2round NAMESPACE(poly_power2round)
void poly_power2round(poly *a1, poly *a0, const poly *a);
#define poly_decompose NAMESPACE(poly_decompose)
void poly_decompose(poly *a1, poly *a0, const poly *a);
#define poly_make_hint NAMESPACE(poly_make_hint)
unsigned int poly_make_hint(poly *h, const poly *a0, const poly *a1);
#define poly_use_hint NAMESPACE(poly_use_hint)
void poly_use_hint(poly *b, const poly *a, const poly *h);

#define poly_chknorm NAMESPACE(poly_chknorm)
int  poly_chknorm(const poly *a, uint32_t B);
#define poly_uniform NAMESPACE(poly_uniform)
void poly_uniform(poly *a,
                  const uint8_t seed[SEEDBYTES],
                  uint16_t nonce);
#define poly_uniform_eta NAMESPACE(poly_uniform_eta)
void poly_uniform_eta(poly *a,
                      const uint8_t seed[SEEDBYTES],
                      uint16_t nonce);
#define poly_uniform_gamma1m1 NAMESPACE(poly_uniform_gamma1m1)
void poly_uniform_gamma1m1(poly *a,
                           const uint8_t seed[CRHBYTES],
                           uint16_t nonce);

#define polyeta_pack NAMESPACE(polyeta_pack)
void polyeta_pack(uint8_t *r, const poly *a);
#define polyeta_unpack NAMESPACE(polyeta_unpack)
void polyeta_unpack(poly *r, const uint8_t *a);

#define polyt1_pack NAMESPACE(polyt1_pack)
void polyt1_pack(uint8_t *r, const poly *a);
#define polyt1_unpack NAMESPACE(polyt1_unpack)
void polyt1_unpack(poly *r, const uint8_t *a);

#define polyt0_pack NAMESPACE(polyt0_pack)
void polyt0_pack(uint8_t *r, const poly *a);
#define polyt0_unpack NAMESPACE(polyt0_unpack)
void polyt0_unpack(poly *r, const uint8_t *a);

#define polyz_pack NAMESPACE(polyz_pack)
void polyz_pack(uint8_t *r, const poly *a);
#define polyz_unpack NAMESPACE(polyz_unpack)
void polyz_unpack(poly *r, const uint8_t *a);

#define polyw1_pack NAMESPACE(polyw1_pack)
void polyw1_pack(uint8_t *r, const poly *a);

#endif
