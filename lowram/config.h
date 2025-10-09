#ifndef CONFIG_H
#define CONFIG_H

//#define DILITHIUM_MODE 2
// #define DILITHIUM_RANDOMIZED_SIGNING
//#define USE_RDPMC
//#define DBENCH

#ifndef DILITHIUM_MODE
#define DILITHIUM_MODE 2
#endif

#if DILITHIUM_MODE == 2
#define CRYPTO_ALGNAME "Dilithium2"
#define DILITHIUM_NAMESPACETOP pqcrystals_dilithium2_lowram
#define DILITHIUM_NAMESPACE(s) pqcrystals_dilithium2_lowram_##s
#elif DILITHIUM_MODE == 3
#define CRYPTO_ALGNAME "Dilithium3"
#define DILITHIUM_NAMESPACETOP pqcrystals_dilithium3_lowram
#define DILITHIUM_NAMESPACE(s) pqcrystals_dilithium3_lowram_##s
#elif DILITHIUM_MODE == 5
#define CRYPTO_ALGNAME "Dilithium5"
#define DILITHIUM_NAMESPACETOP pqcrystals_dilithium5_lowram
#define DILITHIUM_NAMESPACE(s) pqcrystals_dilithium5_lowram_##s
#endif

#endif
