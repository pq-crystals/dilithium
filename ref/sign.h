#ifndef SIGN_H
#define SIGN_H

#include <stdint.h>
#include "params.h"
#include "polyvec.h"
#include "poly.h"

#define challenge DILITHIUM_NAMESPACE(_challenge)
void challenge(poly *c, const uint8_t mu[CRHBYTES], const polyveck *w1);

#define crypto_sign_keypair DILITHIUM_NAMESPACE(_keypair)
int crypto_sign_keypair(unsigned char *pk, unsigned char *sk);

#define crypto_sign_signature DILITHIUM_NAMESPACE(_signature)
int crypto_sign_signature(unsigned char *sig, unsigned long long *siglen,
                          const unsigned char *m, unsigned long long mlen,
                          const unsigned char *sk);

#define crypto_sign DILITHIUM_NAMESPACE()
int crypto_sign(unsigned char *sm, unsigned long long *smlen,
                const unsigned char *m, unsigned long long mlen,
                const unsigned char *sk);

#define crypto_sign_verify DILITHIUM_NAMESPACE(_verify)
int crypto_sign_verify(const unsigned char *sig, unsigned long long siglen,
                       const unsigned char *m, unsigned long long mlen,
                       const unsigned char *pk);

#define crypto_sign_open DILITHIUM_NAMESPACE(_open)
int crypto_sign_open(unsigned char *m, unsigned long long *mlen,
                     const unsigned char *sm, unsigned long long smlen,
                     const unsigned char *pk);

#endif
