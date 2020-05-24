#ifndef CONFIG_H
#define CONFIG_H

//#define DILITHIUM_MODE 3
//#define DILITHIUM_USE_AES
//#define DILITHIUM_RANDOMIZED_SIGNING
//#define USE_RDPMC
//#define DBENCH

#ifndef DILITHIUM_MODE
#define DILITHIUM_MODE 3
#endif

#ifdef DILITHIUM_USE_AES
#if DILITHIUM_MODE == 1
#define DILITHIUM_NAMESPACE(s) pqcrystals_dilithium1aes_ref##s
#elif DILITHIUM_MODE == 2
#define DILITHIUM_NAMESPACE(s) pqcrystals_dilithium2aes_ref##s
#elif DILITHIUM_MODE == 3
#define DILITHIUM_NAMESPACE(s) pqcrystals_dilithium3aes_ref##s
#elif DILITHIUM_MODE == 4
#define DILITHIUM_NAMESPACE(s) pqcrystals_dilithium4aes_ref##s
#endif
#else
#if DILITHIUM_MODE == 1
#define DILITHIUM_NAMESPACE(s) pqcrystals_dilithium1_ref##s
#elif DILITHIUM_MODE == 2
#define DILITHIUM_NAMESPACE(s) pqcrystals_dilithium2_ref##s
#elif DILITHIUM_MODE == 3
#define DILITHIUM_NAMESPACE(s) pqcrystals_dilithium3_ref##s
#elif DILITHIUM_MODE == 4
#define DILITHIUM_NAMESPACE(s) pqcrystals_dilithium4_ref##s
#endif
#endif

#endif
