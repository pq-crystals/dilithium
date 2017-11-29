#ifndef POL_H
#define POL_H

#include <stdint.h>
#include "params.h"
#include "fips202.h"

typedef uint32_t pol[N] __attribute__((aligned(32)));

void pol_copy(pol b, const pol a);
void pol_freeze(pol a);

void pol_add(pol c, const pol a, const pol b);
void pol_sub(pol c, const pol a, const pol b);
void pol_neg(pol a);
void pol_shiftl(pol a, unsigned int k);

void pol_ntt(pol a);
void pol_invntt_montgomery(pol a);
void pol_pointwise_invmontgomery(pol c, const pol a, const pol b);

int pol_chknorm(const pol a, uint32_t B);
void pol_uniform(pol a, unsigned char *buf);
void pol_uniform_eta(pol a,
                     const unsigned char seed[SEEDBYTES],
                     unsigned char nonce);
void pol_uniform_eta_4x(pol a0,
                        pol a1,
                        pol a2,
                        pol a3,
                        const unsigned char seed[SEEDBYTES], 
                        unsigned char nonce0,
                        unsigned char nonce1,
                        unsigned char nonce2,
                        unsigned char nonce3);
void pol_uniform_gamma1m1(pol a,
                          const unsigned char seed[SEEDBYTES + CRHBYTES],
                          uint16_t nonce);
void pol_uniform_gamma1m1_4x(pol a0,
                             pol a1,
                             pol a2,
                             pol a3,
                             const unsigned char seed[SEEDBYTES + CRHBYTES],
                             uint16_t nonce0,
                             uint16_t nonce1,
                             uint16_t nonce2,
                             uint16_t nonce3);

void pol_pack(unsigned char *r, const pol a);
void pol_unpack(pol r, const unsigned char *a);

void poleta_pack(unsigned char *r, const pol a);
void poleta_unpack(pol r, const unsigned char *a);

void polt1_pack(unsigned char *r, const pol a);
void polt1_unpack(pol r, const unsigned char *a);
void polt0_pack(unsigned char *r, const pol a);
void polt0_unpack(pol r, const unsigned char *a);

void polz_pack(unsigned char *r, const pol a);
void polz_unpack(pol r, const unsigned char *a);

void polw1_pack(unsigned char *r, const pol a);

#endif
