#ifndef API_H
#define API_H

#include "config.h"

#if DILITHIUM_MODE == 1
#define CRYPTO_PUBLICKEYBYTES 896U
#define CRYPTO_SECRETKEYBYTES 2096U
#define CRYPTO_BYTES 1387U

#ifndef DILITHIUM_USE_AES
#define CRYPTO_ALGNAME "Dilithium1"
#define NAMESPACE(s) dilithium1_avx2_##s
#else
#define CRYPTO_ALGNAME "Dilithium1-AES"
#define NAMESPACE(s) dilithium1aes_avx2_##s
#endif

#elif DILITHIUM_MODE == 2
#define CRYPTO_PUBLICKEYBYTES 1184U
#define CRYPTO_SECRETKEYBYTES 2800U
#define CRYPTO_BYTES 2044U

#ifndef DILITHIUM_USE_AES
#define CRYPTO_ALGNAME "Dilithium2"
#define NAMESPACE(s) dilithium2_avx2_##s
#else
#define CRYPTO_ALGNAME "Dilithium2-AES"
#define NAMESPACE(s) dilithium2aes_avx2_##s
#endif

#elif DILITHIUM_MODE == 3
#define CRYPTO_PUBLICKEYBYTES 1472U
#define CRYPTO_SECRETKEYBYTES 3504U
#define CRYPTO_BYTES 2701U

#ifndef DILITHIUM_USE_AES
#define CRYPTO_ALGNAME "Dilithium3"
#define NAMESPACE(s) dilithium3_avx2_##s
#else
#define CRYPTO_ALGNAME "Dilithium3-AES"
#define NAMESPACE(s) dilithium3aes_avx2_##s
#endif

#elif DILITHIUM_MODE == 4
#define CRYPTO_PUBLICKEYBYTES 1760U
#define CRYPTO_SECRETKEYBYTES 3856U
#define CRYPTO_BYTES 3366U

#ifndef DILITHIUM_USE_AES
#define CRYPTO_ALGNAME "Dilithium4"
#define NAMESPACE(s) dilithium4_avx2_##s
#else
#define CRYPTO_ALGNAME "Dilithium4-AES"
#define NAMESPACE(s) dilithium4aes_avx2_##s
#endif

#endif

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
