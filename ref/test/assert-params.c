/*
This code ensures that the defines in api.h are aligned with the ones in param.h
It doesnt generate any executable code.
*/

#include "../params.h"
#include "../api.h"

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

#define pqcrystals_dilithium(mode, val) pqcrystals_dilithium_(mode, val)
#define pqcrystals_dilithium_(mode, val) pqcrystals_dilithium##mode##_##val

#define pqcrystals_dilithium_PUBLICKEYBYTES pqcrystals_dilithium(DILITHIUM_MODE, PUBLICKEYBYTES)
#define pqcrystals_dilithium_SECRETKEYBYTES pqcrystals_dilithium(DILITHIUM_MODE, SECRETKEYBYTES)
#define pqcrystals_dilithium_BYTES pqcrystals_dilithium(DILITHIUM_MODE, BYTES)

#if pqcrystals_dilithium_PUBLICKEYBYTES != CRYPTO_PUBLICKEYBYTES
#pragma message "building dilithium" STR(DILITHIUM_MODE)
#error pqcrystals_dilithium_PUBLICKEYBYTES != CRYPTO_PUBLICKEYBYTES
#endif

#if pqcrystals_dilithium_SECRETKEYBYTES != CRYPTO_SECRETKEYBYTES
#pragma message "building dilithium" STR(DILITHIUM_MODE)
#error pqcrystals_dilithium_SECRETKEYBYTES != CRYPTO_SECRETKEYBYTES
#endif

#if pqcrystals_dilithium_BYTES != CRYPTO_BYTES
#pragma message "building dilithium" STR(DILITHIUM_MODE)
#error pqcrystals_dilithium_BYTES != CRYPTO_BYTES
#endif
