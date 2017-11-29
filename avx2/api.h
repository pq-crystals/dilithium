#ifndef API_H
#define API_H

#ifndef MODE
#define MODE 2
#endif

#if MODE == 0
#define CRYPTO_PUBLICKEYBYTES 896U
#define CRYPTO_SECRETKEYBYTES 2096U
#define CRYPTO_BYTES 1487U

#elif MODE == 1
#define CRYPTO_PUBLICKEYBYTES 1184U
#define CRYPTO_SECRETKEYBYTES 2800U
#define CRYPTO_BYTES 2044U

#elif MODE == 2
#define CRYPTO_PUBLICKEYBYTES 1472U
#define CRYPTO_SECRETKEYBYTES 3504U
#define CRYPTO_BYTES 2701U

#elif MODE == 3
#define CRYPTO_PUBLICKEYBYTES 1760U
#define CRYPTO_SECRETKEYBYTES 3856U
#define CRYPTO_BYTES 3366U

#endif

#define CRYPTO_ALGNAME "Dilithium"

int crypto_sign_keypair(unsigned char *pk, unsigned char *sk);

int crypto_sign(unsigned char *sm, unsigned long long *smlen,
                const unsigned char *msg, unsigned long long len, 
                const unsigned char *sk);

int crypto_sign_open(unsigned char *m, unsigned long long *mlen,
                     const unsigned char *sm, unsigned long long smlen,
                     const unsigned char *pk);

#endif
