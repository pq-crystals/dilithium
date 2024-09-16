#ifndef STACK_H
#define STACK_H

#include "poly.h"
#include "smallpoly.h"
#include <stdint.h>
#include <stddef.h>
#include "fips202.h"

#define unpack_pk_t1 DILITHIUM_NAMESPACE(unpack_pk_t1)
void unpack_pk_t1(poly *t1, size_t idx, const unsigned char pk[CRYPTO_PUBLICKEYBYTES]);
#define unpack_sig_z DILITHIUM_NAMESPACE(unpack_sig_z)
int unpack_sig_z(polyvecl *z, const unsigned char sig[CRYPTO_BYTES]);
#define unpack_sig_h DILITHIUM_NAMESPACE(unpack_sig_h)
int unpack_sig_h(poly *h, size_t idx, const unsigned char sig[CRYPTO_BYTES]);
#define unpack_sig_c DILITHIUM_NAMESPACE(unpack_sig_c)
int unpack_sig_c(uint8_t c[CTILDEBYTES], const unsigned char sig[CRYPTO_BYTES]);


#define pack_sig_c DILITHIUM_NAMESPACE(pack_sig_c)
void pack_sig_c(uint8_t sig[CRYPTO_BYTES], const uint8_t c[CTILDEBYTES]);
#define pack_sig_z DILITHIUM_NAMESPACE(pack_sig_z)
void pack_sig_z(uint8_t sig[CRYPTO_BYTES], const polyvecl *z);
#define pack_sig_h DILITHIUM_NAMESPACE(pack_sig_h)
void pack_sig_h(unsigned char sig[CRYPTO_BYTES],
                const poly *h_elem,
                const unsigned int idx,
                unsigned int *hints_written);
#define pack_sig_h_zero DILITHIUM_NAMESPACE(pack_sig_h_zero)
void pack_sig_h_zero(unsigned char sig[CRYPTO_BYTES],
                unsigned int *hints_written);

void poly_challenge_compress(uint8_t c[68], const poly *cp);
void poly_challenge_decompress(poly *cp, const uint8_t c[68]);


void poly_schoolbook(poly *c, const uint8_t ccomp[68], const uint8_t *t0);
void poly_schoolbook_t1(poly *c, const uint8_t ccomp[68], const uint8_t *t1);
void polyw_pack(uint8_t buf[K*768], poly *w);
void polyw_unpack(poly *w, const uint8_t buf[K*768]);

void polyw_add(uint8_t buf[3*256], poly *p);
void polyw_sub(poly* c, uint8_t buf[3*256], poly *a);

void poly_highbits(poly *a1, const poly *a);
void poly_lowbits(poly *a0, const poly *a);

void unpack_sk_s1(smallpoly *a, const uint8_t *sk, size_t idx);
void unpack_sk_s2(smallpoly *a, const uint8_t *sk, size_t idx);

void poly_uniform_pointwise_montgomery_polywadd_lowram(uint8_t wcomp[3*N], poly *b, const uint8_t  seed[SEEDBYTES], uint16_t nonce, keccak_state *state);
void poly_uniform_gamma1_lowram(poly *a, const uint8_t seed[CRHBYTES], uint16_t nonce, keccak_state *state);
void poly_uniform_gamma1_add_lowram(poly *a, poly *b, const uint8_t seed[CRHBYTES], uint16_t nonce, keccak_state *state);
void poly_challenge_lowram(poly *c, const uint8_t seed[CTILDEBYTES]);

size_t poly_make_hint_lowram(poly *a, poly *t, uint8_t w[768]);
int unpack_sig_h_indices(uint8_t h_i[OMEGA], unsigned int * number_of_hints, unsigned int idx, const unsigned char sig[CRYPTO_BYTES]);
void poly_use_hint_lowram(poly *b, const poly *a, uint8_t h_i[OMEGA], unsigned int number_of_hints);

void pack_pk_rho(unsigned char pk[CRYPTO_PUBLICKEYBYTES],
                 const unsigned char rho[SEEDBYTES]);

void pack_pk_t1(unsigned char pk[CRYPTO_PUBLICKEYBYTES],
             const poly *t1,
             const unsigned int idx);

void pack_sk_s1(unsigned char sk[CRYPTO_SECRETKEYBYTES],
                const poly *s1_elem,
                const unsigned int idx);

void pack_sk_s2(unsigned char sk[CRYPTO_SECRETKEYBYTES],
                const poly *s2_elem,
                const unsigned int idx);

void pack_sk_t0(unsigned char sk[CRYPTO_SECRETKEYBYTES],
                const poly *t0_elem,
                const unsigned int idx);

void pack_sk_rho(unsigned char sk[CRYPTO_SECRETKEYBYTES],
                 const unsigned char rho[SEEDBYTES]);

void pack_sk_key(unsigned char sk[CRYPTO_SECRETKEYBYTES],
                 const unsigned char key[SEEDBYTES]);

void pack_sk_tr(unsigned char sk[CRYPTO_SECRETKEYBYTES],
                const unsigned char tr[TRBYTES]);

#define poly_pointwise_acc_montgomery DILITHIUM_NAMESPACE(poly_pointwise_acc_montgomery)
void poly_pointwise_acc_montgomery(poly *c, const poly *a, const poly *b);
#endif
