#include <stdio.h>
#include <dilithium/randombytes.h>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/buffer.h>
#include <unistd.h>
#include "base64.h"

#ifndef DILITHIUM_MODE
#error "DILITHIUM_MODE must be 2, 3, or 5"
#endif
#ifdef DILITHIUM_USE_AVX2
    #include <dilithium/avx2/api.h>
#else
    #include <dilithium/api.h>
#endif

#if DILITHIUM_MODE == 2
    #ifdef DILITHIUM_USE_AVX2
        #define CRYPTO_KEYPAIR pqcrystals_dilithium2_avx2_keypair
    #else
        #define CRYPTO_KEYPAIR pqcrystals_dilithium2_ref_keypair
    #endif
    #define CRYPTO_SECRETKEYBYTES pqcrystals_dilithium2_SECRETKEYBYTES
    #define CRYPTO_PUBLICKEYBYTES pqcrystals_dilithium2_PUBLICKEYBYTES
    #define SK_HEADER "-----BEGIN DILITHIUM2 SECRET KEY-----"
    #define SK_FOOTER "-----END DILITHIUM2 SECRET KEY-----"
    #define PK_HEADER "-----BEGIN DILITHIUM2 PUBLIC KEY-----"
    #define PK_FOOTER "-----END DILITHIUM2 PUBLIC KEY-----"
#elif DILITHIUM_MODE == 3
    #ifdef DILITHIUM_USE_AVX2
        #define CRYPTO_KEYPAIR pqcrystals_dilithium3_avx2_keypair
    #else
        #define CRYPTO_KEYPAIR pqcrystals_dilithium3_ref_keypair
    #endif
    #define CRYPTO_SECRETKEYBYTES pqcrystals_dilithium3_SECRETKEYBYTES
    #define CRYPTO_PUBLICKEYBYTES pqcrystals_dilithium3_PUBLICKEYBYTES
    #define SK_HEADER "-----BEGIN DILITHIUM3 SECRET KEY-----"
    #define SK_FOOTER "-----END DILITHIUM3 SECRET KEY-----"
    #define PK_HEADER "-----BEGIN DILITHIUM3 PUBLIC KEY-----"
    #define PK_FOOTER "-----END DILITHIUM3 PUBLIC KEY-----"
#elif DILITHIUM_MODE == 5
    #ifdef DILITHIUM_USE_AVX2
        #define CRYPTO_KEYPAIR pqcrystals_dilithium5_avx2_keypair
    #else
        #define CRYPTO_KEYPAIR pqcrystals_dilithium5_ref_keypair
    #endif
    #define CRYPTO_SECRETKEYBYTES pqcrystals_dilithium5_SECRETKEYBYTES
    #define CRYPTO_PUBLICKEYBYTES pqcrystals_dilithium5_PUBLICKEYBYTES
    #define SK_HEADER "-----BEGIN DILITHIUM5 SECRET KEY-----"
    #define SK_FOOTER "-----END DILITHIUM5 SECRET KEY-----"
    #define PK_HEADER "-----BEGIN DILITHIUM5 PUBLIC KEY-----"
    #define PK_FOOTER "-----END DILITHIUM5 PUBLIC KEY-----"
#endif

int main(int argc, char *argv[]) {
    unsigned char sk[CRYPTO_SECRETKEYBYTES];
    unsigned char pk[CRYPTO_PUBLICKEYBYTES];
    int opt;
    char *pk_path = NULL;
    char *sk_path = NULL;
    
    // Parse command line arguments
    while ((opt = getopt(argc, argv, "p:s:")) != -1) {
        switch (opt) {
            case 'p': pk_path = optarg; break;
            case 's': sk_path = optarg; break;
            default:
                fprintf(stderr, "Usage: %s -p public_key_file -s secret_key_file\n", argv[0]);
                return 1;
        }
    }
    
    // Add check for secret key path
    if (!sk_path) {
        fprintf(stderr, "Error: Secret key output path (-s) is required\n");
        return 1;
    }
    
    // Base64 encode and save public key
    // Now required - error if no path provided
    if (!pk_path) {
        fprintf(stderr, "Error: Public key output path (-p) is required\n");
        return 1;
    }

    // Generate keypair
    if (CRYPTO_KEYPAIR(pk, sk) != 0) {
        fprintf(stderr, "Key generation failed\n");
        return 1;
    }
    
    // Write public key
    if (write_to_base64_file(pk_path,
                            PK_HEADER,
                            PK_FOOTER,
                            pk,
                            CRYPTO_PUBLICKEYBYTES) != 0) {
        fprintf(stderr, "Error writing public key\n");
        return 1;
    }

    // Write secret key
    if (write_to_base64_file(sk_path,
                            SK_HEADER,
                            SK_FOOTER,
                            sk,
                            CRYPTO_SECRETKEYBYTES) != 0) {
        fprintf(stderr, "Error writing secret key\n");
        return 1;
    }

    return 0;
} 