#ifndef API_H
#define API_H

#include "config.h"

#if DILITHIUM_MODE == 1
#define CRYPTO_PUBLICKEYBYTES 896
#define CRYPTO_SECRETKEYBYTES 2096
#define CRYPTO_BYTES 1387

#ifndef DILITHIUM_USE_AES
#define CRYPTO_ALGNAME "Dilithium1"
#else
#define CRYPTO_ALGNAME "Dilithium1-AES"
#endif

#elif DILITHIUM_MODE == 2
#define CRYPTO_PUBLICKEYBYTES 1184
#define CRYPTO_SECRETKEYBYTES 2800
#define CRYPTO_BYTES 2044

#ifndef DILITHIUM_USE_AES
#define CRYPTO_ALGNAME "Dilithium2"
#else
#define CRYPTO_ALGNAME "Dilithium2-AES"
#endif

#elif DILITHIUM_MODE == 3
#define CRYPTO_PUBLICKEYBYTES 1472
#define CRYPTO_SECRETKEYBYTES 3504
#define CRYPTO_BYTES 2701

#ifndef DILITHIUM_USE_AES
#define CRYPTO_ALGNAME "Dilithium3"
#else
#define CRYPTO_ALGNAME "Dilithium3-AES"
#endif

#elif DILITHIUM_MODE == 4
#define CRYPTO_PUBLICKEYBYTES 1760
#define CRYPTO_SECRETKEYBYTES 3856
#define CRYPTO_BYTES 3366

#ifndef DILITHIUM_USE_AES
#define CRYPTO_ALGNAME "Dilithium4"
#else
#define CRYPTO_ALGNAME "Dilithium4-AES"
#endif

#endif

#define crypto_sign_keypair DILITHIUM_NAMESPACE(crypto_sign_keypair)
int crypto_sign_keypair(unsigned char *pk, unsigned char *sk);

#define crypto_sign_signature DILITHIUM_NAMESPACE(crypto_sign_signature)
int crypto_sign_signature(unsigned char *sig, unsigned long long *siglen,
                          const unsigned char *m, unsigned long long mlen,
                          const unsigned char *sk);

#define crypto_sign DILITHIUM_NAMESPACE(crypto_sign)
int crypto_sign(unsigned char *sm, unsigned long long *smlen,
                const unsigned char *m, unsigned long long mlen,
                const unsigned char *sk);

#define crypto_sign_verify DILITHIUM_NAMESPACE(crypto_sign_verify)
int crypto_sign_verify(const unsigned char *sig, unsigned long long *siglen,
                       const unsigned char *m, unsigned long long mlen,
                       const unsigned char *pk);

#define crypto_sign_open DILITHIUM_NAMESPACE(crypto_sign_open)
int crypto_sign_open(unsigned char *m, unsigned long long *mlen,
                     const unsigned char *sm, unsigned long long smlen,
                     const unsigned char *pk);

#endif
