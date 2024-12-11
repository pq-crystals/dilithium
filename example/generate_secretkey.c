#include <stdio.h>
#include <dilithium/api.h>
#include <dilithium/randombytes.h>
#include "size.h"

int main(void) {
    unsigned char sk[PQCLEAN_DILITHIUM5_CRYPTO_SECRETKEYBYTES];
    unsigned char pk[PQCLEAN_DILITHIUM5_CRYPTO_PUBLICKEYBYTES];
    
    // Generate keypair
    if (pqcrystals_dilithium5_ref_keypair(pk, sk) != 0) {
        fprintf(stderr, "Key generation failed\n");
        return 1;
    }
    
    // Write secret key to stdout
    fwrite(sk, 1, PQCLEAN_DILITHIUM5_CRYPTO_SECRETKEYBYTES, stdout);
    return 0;
} 