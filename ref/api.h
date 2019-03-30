#ifndef API_H
#define API_H

#include "config.h"

#if MODE == 1
#define CRYPTO_PUBLICKEYBYTES 896U
#define CRYPTO_SECRETKEYBYTES 2096U
#define CRYPTO_BYTES 1387U

#ifndef USE_AES
#define CRYPTO_ALGNAME "Dilithium1"
#else
#define CRYPTO_ALGNAME "Dilithium1-AES"
#endif

#elif MODE == 2
#define CRYPTO_PUBLICKEYBYTES 1184U
#define CRYPTO_SECRETKEYBYTES 2800U
#define CRYPTO_BYTES 2044U

#ifndef USE_AES
#define CRYPTO_ALGNAME "Dilithium2"
#else
#define CRYPTO_ALGNAME "Dilithium2-AES"
#endif

#elif MODE == 3
#define CRYPTO_PUBLICKEYBYTES 1472U
#define CRYPTO_SECRETKEYBYTES 3504U
#define CRYPTO_BYTES 2701U

#ifndef USE_AES
#define CRYPTO_ALGNAME "Dilithium3"
#else
#define CRYPTO_ALGNAME "Dilithium3-AES"
#endif

#elif MODE == 4
#define CRYPTO_PUBLICKEYBYTES 1760U
#define CRYPTO_SECRETKEYBYTES 3856U
#define CRYPTO_BYTES 3366U

#ifndef USE_AES
#define CRYPTO_ALGNAME "Dilithium4"
#else
#define CRYPTO_ALGNAME "Dilithium4-AES"
#endif

#endif

int crypto_sign_keypair(unsigned char *pk, unsigned char *sk);

int crypto_sign(unsigned char *sm, unsigned long long *smlen,
                const unsigned char *msg, unsigned long long len,
                const unsigned char *sk);

int crypto_sign_open(unsigned char *m, unsigned long long *mlen,
                     const unsigned char *sm, unsigned long long smlen,
                     const unsigned char *pk);

#endif
