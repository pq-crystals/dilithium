#ifndef API_H
#define API_H

#include <stddef.h>
#include <stdint.h>
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

#define crypto_sign_keypair DILITHIUM_NAMESPACE(_keypair)
int crypto_sign_keypair(uint8_t *pk, uint8_t *sk);

#define crypto_sign DILITHIUM_NAMESPACE()
int crypto_sign(uint8_t *sm, size_t *smlen,
                const uint8_t *m, size_t mlen,
                const uint8_t *sk);

#define crypto_sign_open DILITHIUM_NAMESPACE(_open)
int crypto_sign_open(uint8_t *m, size_t *mlen,
                     const uint8_t *sm, size_t smlen,
                     const uint8_t *pk);

#endif
