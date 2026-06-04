#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stddef.h>
#include <stdint.h>
#include "base64.h"
#include <dilithium/params.h>
#include <dilithium/sign.h>

#define MAX_LINE_LENGTH 1024

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
        #define CRYPTO_SIGN pqcrystals_dilithium2_avx2_signature
    #else
        #define CRYPTO_SIGN pqcrystals_dilithium2_ref_signature
    #endif
    #define SK_HEADER "-----BEGIN DILITHIUM2 SECRET KEY-----"
    #define SK_FOOTER "-----END DILITHIUM2 SECRET KEY-----"
    #define SIG_HEADER "-----BEGIN DILITHIUM2 SIGNATURE-----"
    #define SIG_FOOTER "-----END DILITHIUM2 SIGNATURE-----"
    #define PK_HEADER "-----BEGIN DILITHIUM2 PUBLIC KEY-----"
    #define PK_FOOTER "-----END DILITHIUM2 PUBLIC KEY-----"
#elif DILITHIUM_MODE == 3
    #ifdef DILITHIUM_USE_AVX2
        #define CRYPTO_SIGN pqcrystals_dilithium3_avx2_signature
    #else
        #define CRYPTO_SIGN pqcrystals_dilithium3_ref_signature
    #endif
    #define SK_HEADER "-----BEGIN DILITHIUM3 SECRET KEY-----"
    #define SK_FOOTER "-----END DILITHIUM3 SECRET KEY-----"
    #define SIG_HEADER "-----BEGIN DILITHIUM3 SIGNATURE-----"
    #define SIG_FOOTER "-----END DILITHIUM3 SIGNATURE-----"
    #define PK_HEADER "-----BEGIN DILITHIUM3 PUBLIC KEY-----"
    #define PK_FOOTER "-----END DILITHIUM3 PUBLIC KEY-----"
#elif DILITHIUM_MODE == 5
    #ifdef DILITHIUM_USE_AVX2
        #define CRYPTO_SIGN pqcrystals_dilithium5_avx2_signature
    #else
        #define CRYPTO_SIGN pqcrystals_dilithium5_ref_signature
    #endif
    #define SK_HEADER "-----BEGIN DILITHIUM5 SECRET KEY-----"
    #define SK_FOOTER "-----END DILITHIUM5 SECRET KEY-----"
    #define SIG_HEADER "-----BEGIN DILITHIUM5 SIGNATURE-----"
    #define SIG_FOOTER "-----END DILITHIUM5 SIGNATURE-----"
    #define PK_HEADER "-----BEGIN DILITHIUM5 PUBLIC KEY-----"
    #define PK_FOOTER "-----END DILITHIUM5 PUBLIC KEY-----"
#endif

int main(int argc, char *argv[]) {
    uint8_t sk[CRYPTO_SECRETKEYBYTES];
    uint8_t sig[CRYPTO_BYTES];
    size_t siglen;
    FILE *msg_file = NULL;
    uint8_t *msg;
    size_t msg_len;
    int opt;
    char *sk_path = NULL, *msg_path = NULL, *sig_path = NULL;

    while ((opt = getopt(argc, argv, "s:i:o:")) != -1) {
        switch (opt) {
            case 's': sk_path = optarg; break;
            case 'i': msg_path = optarg; break;
            case 'o': sig_path = optarg; break;
            default:
                fprintf(stderr, "Usage: %s -s secret_key -i message -o signature\n", argv[0]);
                return 1;
        }
    }

    if (!sk_path || !msg_path || !sig_path) {
        fprintf(stderr, "Missing required arguments\n");
        return 1;
    }

    // Read and decode base64 secret key
    int read_result = read_from_base64_file(sk_path, 
                             SK_HEADER,
                             SK_FOOTER,
                             sk, 
                             CRYPTO_SECRETKEYBYTES);
    if (read_result != 0) {
        fprintf(stderr, "Error reading secret key (code: %d)\n", read_result);
        return 1;
    }

    // Read message
    msg_file = fopen(msg_path, "rb");
    if (!msg_file) {
        fprintf(stderr, "Error opening message file\n");
        return 1;
    }
    fseek(msg_file, 0, SEEK_END);
    msg_len = ftell(msg_file);
    fseek(msg_file, 0, SEEK_SET);
    msg = malloc(msg_len);
    if (!msg || fread(msg, 1, msg_len, msg_file) != msg_len) {
        fprintf(stderr, "Error reading message\n");
        return 1;
    }
    fclose(msg_file);

    // Create signature
    if (CRYPTO_SIGN(sig, &siglen, msg, msg_len, NULL, 0, sk) != 0) {
        fprintf(stderr, "Signature creation failed\n");
        return 1;
    }

    // Write signature to stdout using base64 functions
    if (write_to_base64_file(sig_path, 
                            SIG_HEADER,
                            SIG_FOOTER,
                            sig, siglen) != 0) {
        fprintf(stderr, "Error writing signature\n");
        return 1;
    }

    free(msg);
    return 0;
}