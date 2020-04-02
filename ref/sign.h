#ifndef SIGN_H
#define SIGN_H

#include <stdint.h>
#include "params.h"
#include "poly.h"
#include "polyvec.h"

#define challenge NAMESPACE(challenge)
void challenge(poly *c, const uint8_t mu[CRHBYTES], const polyveck *w1);

#define crypto_sign_keypair NAMESPACE(crypto_sign_keypair)
int crypto_sign_keypair(unsigned char *pk, unsigned char *sk);

#define crypto_sign NAMESPACE(crypto_sign)
int crypto_sign(unsigned char *sm, unsigned long long *smlen,
                const unsigned char *msg, unsigned long long len,
                const unsigned char *sk);

#define crypto_sign_open NAMESPACE(crypto_sign_open)
int crypto_sign_open(unsigned char *m, unsigned long long *mlen,
                     const unsigned char *sm, unsigned long long smlen,
                     const unsigned char *pk);

#endif
