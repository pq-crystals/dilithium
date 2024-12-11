#include <stdio.h>
#include <dilithium/api.h>
#include "size.h"

int main(void) {
    unsigned char sk[PQCLEAN_DILITHIUM5_CRYPTO_SECRETKEYBYTES];
    unsigned char pk[PQCLEAN_DILITHIUM5_CRYPTO_PUBLICKEYBYTES];
    
    // Read secret key from stdin
    if (fread(sk, 1, PQCLEAN_DILITHIUM5_CRYPTO_SECRETKEYBYTES, stdin) != PQCLEAN_DILITHIUM5_CRYPTO_SECRETKEYBYTES) {
        fprintf(stderr, "Error reading secret key\n");
        return 1;
    }
    
    // Extract public key from secret key
    if (pqcrystals_dilithium5_ref_keypair(pk, sk) != 0) {
        fprintf(stderr, "Error extracting public key\n");
        return 1;
    }
    
    // Write public key to stdout
    fwrite(pk, 1, PQCLEAN_DILITHIUM5_CRYPTO_PUBLICKEYBYTES, stdout);
    return 0;
} 