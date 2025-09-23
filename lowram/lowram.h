#ifndef STACK_H
#define STACK_H

#include "poly.h"
#include "smallpoly.h"
#include <stdint.h>
#include <stddef.h>
#include "fips202.h"

#define unpack_pk_t1 DILITHIUM_NAMESPACE(unpack_pk_t1)
void unpack_pk_t1(poly *t1, size_t idx, const uint8_t pk[CRYPTO_PUBLICKEYBYTES]);
#define unpack_sig_z DILITHIUM_NAMESPACE(unpack_sig_z)
int unpack_sig_z(polyvecl *z, const uint8_t sig[CRYPTO_BYTES]);
#define unpack_sig_h DILITHIUM_NAMESPACE(unpack_sig_h)
int unpack_sig_h(poly *h, size_t idx, const uint8_t sig[CRYPTO_BYTES]);
#define unpack_sig_c DILITHIUM_NAMESPACE(unpack_sig_c)
int unpack_sig_c(uint8_t c[CTILDEBYTES], const uint8_t sig[CRYPTO_BYTES]);


#define pack_sig_c DILITHIUM_NAMESPACE(pack_sig_c)
void pack_sig_c(uint8_t sig[CRYPTO_BYTES], const uint8_t c[CTILDEBYTES]);
#define pack_sig_z DILITHIUM_NAMESPACE(pack_sig_z)
void pack_sig_z(uint8_t sig[CRYPTO_BYTES], const polyvecl *z);
#define pack_sig_h DILITHIUM_NAMESPACE(pack_sig_h)
void pack_sig_h(uint8_t sig[CRYPTO_BYTES],
                const poly *h_elem,
                const unsigned int idx,
                unsigned int *hints_written);
#define pack_sig_h_zero DILITHIUM_NAMESPACE(pack_sig_h_zero)
void pack_sig_h_zero(uint8_t sig[CRYPTO_BYTES],
                unsigned int *hints_written);

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

#define polyw_add DILITHIUM_NAMESPACE(polyw_add)
void polyw_add(uint8_t buf[3*256], poly *p);
#define polyw_sub DILITHIUM_NAMESPACE(polyw_sub)
void polyw_sub(poly* c, const uint8_t buf[3*256], const poly *a);

#define poly_highbits DILITHIUM_NAMESPACE(poly_highbits)
void poly_highbits(poly *a1, const poly *a);
#define poly_lowbits DILITHIUM_NAMESPACE(poly_lowbits)
void poly_lowbits(poly *a0, const poly *a);

#define unpack_sk_s1 DILITHIUM_NAMESPACE(unpack_sk_s1)
void unpack_sk_s1(smallpoly *a, const uint8_t *sk, size_t idx);
#define unpack_sk_s2 DILITHIUM_NAMESPACE(unpack_sk_s2)
void unpack_sk_s2(smallpoly *a, const uint8_t *sk, size_t idx);

#define poly_uniform_pointwise_montgomery_polywadd_lowram DILITHIUM_NAMESPACE(poly_uniform_pointwise_montgomery_polywadd_lowram)
void poly_uniform_pointwise_montgomery_polywadd_lowram(uint8_t wcomp[3*N], const poly *b, const uint8_t  seed[SEEDBYTES], uint16_t nonce, keccak_state *state);
#define poly_uniform_gamma1_lowram DILITHIUM_NAMESPACE(poly_uniform_gamma1_lowram)
void poly_uniform_gamma1_lowram(poly *a, const uint8_t seed[CRHBYTES], uint16_t nonce, keccak_state *state);
#define poly_uniform_gamma1_add_lowram DILITHIUM_NAMESPACE(poly_uniform_gamma1_add_lowram)
void poly_uniform_gamma1_add_lowram(poly *a, const poly *b, const uint8_t seed[CRHBYTES], uint16_t nonce, keccak_state *state);
#define poly_challenge_lowram DILITHIUM_NAMESPACE(poly_challenge_lowram)
void poly_challenge_lowram(poly *c, const uint8_t seed[CTILDEBYTES]);

#define poly_make_hint_lowram DILITHIUM_NAMESPACE(poly_make_hint_lowram)
size_t poly_make_hint_lowram(poly *a, const poly *t, const uint8_t w[768]);
#define unpack_sig_h_indices DILITHIUM_NAMESPACE(unpack_sig_h_indices)
int unpack_sig_h_indices(uint8_t h_i[OMEGA], unsigned int *number_of_hints, unsigned int idx, const uint8_t sig[CRYPTO_BYTES]);
#define poly_use_hint_lowram DILITHIUM_NAMESPACE(poly_use_hint_lowram)
void poly_use_hint_lowram(poly *b, const poly *a, const uint8_t h_i[OMEGA], unsigned int number_of_hints);

#define pack_pk_rho DILITHIUM_NAMESPACE(pack_pk_rho)
void pack_pk_rho(uint8_t pk[CRYPTO_PUBLICKEYBYTES],
                 const uint8_t rho[SEEDBYTES]);

#define pack_pk_t1 DILITHIUM_NAMESPACE(pack_pk_t1)
void pack_pk_t1(uint8_t pk[CRYPTO_PUBLICKEYBYTES],
             const poly *t1,
             const unsigned int idx);

#define pack_sk_s1 DILITHIUM_NAMESPACE(pack_sk_s1)
void pack_sk_s1(uint8_t sk[CRYPTO_SECRETKEYBYTES],
                const poly *s1_elem,
                const unsigned int idx);

#define pack_sk_s2 DILITHIUM_NAMESPACE(pack_sk_s2)
void pack_sk_s2(uint8_t sk[CRYPTO_SECRETKEYBYTES],
                const poly *s2_elem,
                const unsigned int idx);

#define pack_sk_t0 DILITHIUM_NAMESPACE(pack_sk_t0)
void pack_sk_t0(uint8_t sk[CRYPTO_SECRETKEYBYTES],
                const poly *t0_elem,
                const unsigned int idx);

#define pack_sk_rho DILITHIUM_NAMESPACE(pack_sk_rho)
void pack_sk_rho(uint8_t sk[CRYPTO_SECRETKEYBYTES],
                 const uint8_t rho[SEEDBYTES]);

#define pack_sk_key DILITHIUM_NAMESPACE(pack_sk_key)
void pack_sk_key(uint8_t sk[CRYPTO_SECRETKEYBYTES],
                 const uint8_t key[SEEDBYTES]);

#define pack_sk_tr DILITHIUM_NAMESPACE(pack_sk_tr)
void pack_sk_tr(uint8_t sk[CRYPTO_SECRETKEYBYTES],
                const uint8_t tr[TRBYTES]);

#define poly_pointwise_acc_montgomery DILITHIUM_NAMESPACE(poly_pointwise_acc_montgomery)
void poly_pointwise_acc_montgomery(poly *c, const poly *a, const poly *b);
#endif
