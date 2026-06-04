#ifndef PACKING_H
#define PACKING_H

#include <stdint.h>
#include "params.h"
#include "polyvec.h"

#endif

/* Functions from lowram.c moved to packing.c */


#define pack_sig_h DILITHIUM_NAMESPACE(pack_sig_h)
void pack_sig_h(uint8_t sig[CRYPTO_BYTES], const poly *h_elem, const unsigned int idx, unsigned int *hints_written);

#define pack_sig_h_zero DILITHIUM_NAMESPACE(pack_sig_h_zero)
void pack_sig_h_zero(uint8_t sig[CRYPTO_BYTES], unsigned int *hints_written);


#define unpack_sig_h_indices DILITHIUM_NAMESPACE(unpack_sig_h_indices)
int unpack_sig_h_indices(uint8_t h_i[OMEGA], unsigned int *number_of_hints, unsigned int idx, const uint8_t sig[CRYPTO_BYTES]);

#define pack_pk_rho DILITHIUM_NAMESPACE(pack_pk_rho)
void pack_pk_rho(uint8_t pk[CRYPTO_PUBLICKEYBYTES], const uint8_t rho[SEEDBYTES]);

#define pack_pk_t1 DILITHIUM_NAMESPACE(pack_pk_t1)
void pack_pk_t1(uint8_t pk[CRYPTO_PUBLICKEYBYTES], const poly *t1, const unsigned int idx);

#define pack_sk_s1 DILITHIUM_NAMESPACE(pack_sk_s1)
void pack_sk_s1(uint8_t sk[CRYPTO_SECRETKEYBYTES], const poly *s1_elem, const unsigned int idx);

#define pack_sk_s2 DILITHIUM_NAMESPACE(pack_sk_s2)
void pack_sk_s2(uint8_t sk[CRYPTO_SECRETKEYBYTES], const poly *s2_elem, const unsigned int idx);

#define pack_sk_t0 DILITHIUM_NAMESPACE(pack_sk_t0)
void pack_sk_t0(uint8_t sk[CRYPTO_SECRETKEYBYTES], const poly *t0_elem, const unsigned int idx);

#define pack_sk_rho DILITHIUM_NAMESPACE(pack_sk_rho)
void pack_sk_rho(uint8_t sk[CRYPTO_SECRETKEYBYTES], const uint8_t rho[SEEDBYTES]);

#define pack_sk_key DILITHIUM_NAMESPACE(pack_sk_key)
void pack_sk_key(uint8_t sk[CRYPTO_SECRETKEYBYTES], const uint8_t key[SEEDBYTES]);

#define pack_sk_tr DILITHIUM_NAMESPACE(pack_sk_tr)
void pack_sk_tr(uint8_t sk[CRYPTO_SECRETKEYBYTES], const uint8_t tr[TRBYTES]);

#define unpack_sk_s1 DILITHIUM_NAMESPACE(unpack_sk_s1)
void unpack_sk_s1(smallpoly *a, const uint8_t *sk, unsigned int idx);

#define unpack_sk_s2 DILITHIUM_NAMESPACE(unpack_sk_s2)
void unpack_sk_s2(smallpoly *a, const uint8_t *sk, unsigned int idx);

