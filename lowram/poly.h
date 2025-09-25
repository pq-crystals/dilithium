#ifndef POLY_H
#define POLY_H

#include <stdint.h>
#include "params.h"
#include "fips202.h"

typedef struct {
  int32_t coeffs[N];
} poly;

#define poly_reduce DILITHIUM_NAMESPACE(poly_reduce)
void poly_reduce(poly *a);
#define poly_caddq DILITHIUM_NAMESPACE(poly_caddq)
void poly_caddq(poly *a);

#define poly_add DILITHIUM_NAMESPACE(poly_add)
void poly_add(poly *c, const poly *a, const poly *b);

#define poly_ntt DILITHIUM_NAMESPACE(poly_ntt)
void poly_ntt(poly *a);
#define poly_invntt_tomont DILITHIUM_NAMESPACE(poly_invntt_tomont)
void poly_invntt_tomont(poly *a);
#define poly_pointwise_montgomery DILITHIUM_NAMESPACE(poly_pointwise_montgomery)
void poly_pointwise_montgomery(poly *c, const poly *a, const poly *b);

#define poly_power2round DILITHIUM_NAMESPACE(poly_power2round)
void poly_power2round(poly *a1, poly *a0, const poly *a);

#define poly_chknorm DILITHIUM_NAMESPACE(poly_chknorm)
int poly_chknorm(const poly *a, int32_t B);
#define poly_uniform DILITHIUM_NAMESPACE(poly_uniform)
void poly_uniform(poly *a,
                  const uint8_t seed[SEEDBYTES],
                  uint16_t nonce);
#define poly_uniform_eta DILITHIUM_NAMESPACE(poly_uniform_eta)
void poly_uniform_eta(poly *a,
                      const uint8_t seed[CRHBYTES],
                      uint16_t nonce);
#define poly_challenge DILITHIUM_NAMESPACE(poly_challenge)
void poly_challenge(poly *c, const uint8_t seed[CTILDEBYTES]);

#define polyeta_pack DILITHIUM_NAMESPACE(polyeta_pack)
void polyeta_pack(uint8_t *r, const poly *a);

#define polyt1_pack DILITHIUM_NAMESPACE(polyt1_pack)
void polyt1_pack(uint8_t *r, const poly *a);

#define polyt0_pack DILITHIUM_NAMESPACE(polyt0_pack)
void polyt0_pack(uint8_t *r, const poly *a);

#define polyz_pack DILITHIUM_NAMESPACE(polyz_pack)
void polyz_pack(uint8_t *r, const poly *a);
#define polyz_unpack DILITHIUM_NAMESPACE(polyz_unpack)
void polyz_unpack(poly *r, const uint8_t *a);

#define polyw1_pack DILITHIUM_NAMESPACE(polyw1_pack)
void polyw1_pack(uint8_t *r, const poly *a);

/* Functions from lowram.c that were moved to poly.c */
#define poly_challenge_compress DILITHIUM_NAMESPACE(poly_challenge_compress)
void poly_challenge_compress(uint8_t c[68], const poly *cp);
#define poly_challenge_decompress DILITHIUM_NAMESPACE(poly_challenge_decompress)
void poly_challenge_decompress(poly *cp, const uint8_t c[68]);
#define poly_schoolbook DILITHIUM_NAMESPACE(poly_schoolbook)
void poly_schoolbook(poly *c, const uint8_t ccomp[68], const uint8_t *t0);
#define poly_schoolbook_t1 DILITHIUM_NAMESPACE(poly_schoolbook_t1)
void poly_schoolbook_t1(poly *c, const uint8_t ccomp[68], const uint8_t *t1);
#define polyw_pack DILITHIUM_NAMESPACE(polyw_pack)
void polyw_pack(uint8_t buf[K*768], poly *w);
#define polyw_unpack DILITHIUM_NAMESPACE(polyw_unpack)
void polyw_unpack(poly *w, const uint8_t buf[K*768]);
#define polyw_sub DILITHIUM_NAMESPACE(polyw_sub)
void polyw_sub(poly* c, const uint8_t buf[3*256], const poly *a);
#define poly_highbits DILITHIUM_NAMESPACE(poly_highbits)
void poly_highbits(poly *a1, const poly *a);
#define poly_lowbits DILITHIUM_NAMESPACE(poly_lowbits)
void poly_lowbits(poly *a0, const poly *a);
#define poly_uniform_pointwise_montgomery_polywadd_lowram DILITHIUM_NAMESPACE(poly_uniform_pointwise_montgomery_polywadd_lowram)
void poly_uniform_pointwise_montgomery_polywadd_lowram(uint8_t wcomp[3*N], const poly *b, const uint8_t  seed[SEEDBYTES], uint16_t nonce, keccak_state *state);
#define poly_uniform_gamma1_lowram DILITHIUM_NAMESPACE(poly_uniform_gamma1_lowram)
void poly_uniform_gamma1_lowram(poly *a, const uint8_t seed[CRHBYTES], uint16_t nonce, keccak_state *state);
#define poly_uniform_gamma1_add_lowram DILITHIUM_NAMESPACE(poly_uniform_gamma1_add_lowram)
void poly_uniform_gamma1_add_lowram(poly *a, const poly *b, const uint8_t seed[CRHBYTES], uint16_t nonce, keccak_state *state);
#define poly_challenge_lowram DILITHIUM_NAMESPACE(poly_challenge_lowram)
void poly_challenge_lowram(poly *c, const uint8_t seed[CTILDEBYTES]);
#define poly_make_hint_lowram DILITHIUM_NAMESPACE(poly_make_hint_lowram)
unsigned int poly_make_hint_lowram(poly *a, const poly *t, const uint8_t w[768]);
#define poly_use_hint_lowram DILITHIUM_NAMESPACE(poly_use_hint_lowram)
void poly_use_hint_lowram(poly *b, const poly *a, const uint8_t h_i[OMEGA], unsigned int number_of_hints);
#define poly_pointwise_acc_montgomery DILITHIUM_NAMESPACE(poly_pointwise_acc_montgomery)
void poly_pointwise_acc_montgomery(poly *c, const poly *a, const poly *b);

/* smallpoly type and functions - using 16-bit coefficients */
typedef struct
{
    int16_t coeffs[N];
} smallpoly;

typedef smallpoly smallhalfpoly;

#define poly_small_ntt_copy DILITHIUM_NAMESPACE(poly_small_ntt_copy)
void poly_small_ntt_copy(smallpoly *, const poly *);

#define poly_small_basemul_invntt DILITHIUM_NAMESPACE(poly_small_basemul_invntt)
void poly_small_basemul_invntt(poly *r, const smallpoly *a, const smallpoly *b);

#define small_polyeta_unpack DILITHIUM_NAMESPACE(small_polyeta_unpack)
void small_polyeta_unpack(smallpoly *r, const uint8_t *a);

/* Small poly NTT wrapper functions */
#define poly_small_ntt DILITHIUM_NAMESPACE(poly_small_ntt)
void poly_small_ntt(smallpoly *a);


#endif
