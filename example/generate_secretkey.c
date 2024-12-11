#include <stdio.h>
#include <dilithium/api.h>
#include <dilithium/randombytes.h>
#include "size.h"
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/buffer.h>
#include <unistd.h>
#include "debug.h"
#include "base64.h"

#define DEBUG_BASE64 0

int main(int argc, char *argv[]) {
    unsigned char sk[PQCLEAN_DILITHIUM5_CRYPTO_SECRETKEYBYTES];
    unsigned char pk[PQCLEAN_DILITHIUM5_CRYPTO_PUBLICKEYBYTES];
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
    if (pqcrystals_dilithium5_ref_keypair(pk, sk) != 0) {
        fprintf(stderr, "Key generation failed\n");
        return 1;
    }
    
    // Debug output for generated keys
    if (DEBUG_BASE64) {
        debug_hexa("Public Key", pk, PQCLEAN_DILITHIUM5_CRYPTO_PUBLICKEYBYTES);
        debug_hexa("Secret Key", sk, PQCLEAN_DILITHIUM5_CRYPTO_SECRETKEYBYTES);
    }

    // Replace public key writing code with
    if (write_to_base64_file(pk_path,
                            "-----BEGIN DILITHIUM PUBLIC KEY-----",
                            "-----END DILITHIUM PUBLIC KEY-----",
                            pk,
                            PQCLEAN_DILITHIUM5_CRYPTO_PUBLICKEYBYTES) != 0) {
        fprintf(stderr, "Error writing public key\n");
        return 1;
    }

    // Replace secret key writing code with
    if (write_to_base64_file(sk_path,
                            "-----BEGIN DILITHIUM SECRET KEY-----",
                            "-----END DILITHIUM SECRET KEY-----",
                            sk,
                            PQCLEAN_DILITHIUM5_CRYPTO_SECRETKEYBYTES) != 0) {
        fprintf(stderr, "Error writing secret key\n");
        return 1;
    }

    return 0;
} 