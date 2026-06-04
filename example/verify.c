#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "base64.h"
#include <openssl/bio.h>

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
        #define CRYPTO_VERIFY pqcrystals_dilithium2_avx2_verify
    #else
        #define CRYPTO_VERIFY pqcrystals_dilithium2_ref_verify
    #endif
    #define CRYPTO_SECRETKEYBYTES pqcrystals_dilithium2_SECRETKEYBYTES
    #define CRYPTO_PUBLICKEYBYTES pqcrystals_dilithium2_PUBLICKEYBYTES
    #define CRYPTO_BYTES pqcrystals_dilithium2_BYTES
    #define PK_HEADER "-----BEGIN DILITHIUM2 PUBLIC KEY-----"
    #define PK_FOOTER "-----END DILITHIUM2 PUBLIC KEY-----"
    #define SIG_HEADER "-----BEGIN DILITHIUM2 SIGNATURE-----"
    #define SIG_FOOTER "-----END DILITHIUM2 SIGNATURE-----"
#elif DILITHIUM_MODE == 3
    #ifdef DILITHIUM_USE_AVX2
        #define CRYPTO_VERIFY pqcrystals_dilithium3_avx2_verify
    #else
        #define CRYPTO_VERIFY pqcrystals_dilithium3_ref_verify
    #endif
    #define CRYPTO_SECRETKEYBYTES pqcrystals_dilithium3_SECRETKEYBYTES
    #define CRYPTO_PUBLICKEYBYTES pqcrystals_dilithium3_PUBLICKEYBYTES
    #define CRYPTO_BYTES pqcrystals_dilithium3_BYTES
    #define PK_HEADER "-----BEGIN DILITHIUM3 PUBLIC KEY-----"
    #define PK_FOOTER "-----END DILITHIUM3 PUBLIC KEY-----"
    #define SIG_HEADER "-----BEGIN DILITHIUM3 SIGNATURE-----"
    #define SIG_FOOTER "-----END DILITHIUM3 SIGNATURE-----"
#elif DILITHIUM_MODE == 5
    #ifdef DILITHIUM_USE_AVX2
        #define CRYPTO_VERIFY pqcrystals_dilithium5_avx2_verify
    #else
        #define CRYPTO_VERIFY pqcrystals_dilithium5_ref_verify
    #endif
    #define CRYPTO_SECRETKEYBYTES pqcrystals_dilithium5_SECRETKEYBYTES
    #define CRYPTO_PUBLICKEYBYTES pqcrystals_dilithium5_PUBLICKEYBYTES
    #define CRYPTO_BYTES pqcrystals_dilithium5_BYTES
    #define PK_HEADER "-----BEGIN DILITHIUM5 PUBLIC KEY-----"
    #define PK_FOOTER "-----END DILITHIUM5 PUBLIC KEY-----"
    #define SIG_HEADER "-----BEGIN DILITHIUM5 SIGNATURE-----"
    #define SIG_FOOTER "-----END DILITHIUM5 SIGNATURE-----"
#endif

int main(int argc, char *argv[]) {
    unsigned char pk[CRYPTO_PUBLICKEYBYTES];
    unsigned char sig[CRYPTO_BYTES];
    FILE *msg_file;
    unsigned char *msg;
    size_t msg_len;
    int opt;
    char *pk_path = NULL, *sig_path = NULL, *msg_path = NULL;

    while ((opt = getopt(argc, argv, "p:i:S:")) != -1) {
        switch (opt) {
            case 'p': pk_path = optarg; break;
            case 'i': msg_path = optarg; break;
            case 'S': sig_path = optarg; break;
            default:
                fprintf(stderr, "Usage: %s -p publickey -i message -S signature\n", argv[0]);
                return 1;
        }
    }

    if (!pk_path || !sig_path || !msg_path) {
        fprintf(stderr, "Missing required arguments\n");
        return 1;
    }

    // Read and decode base64 public key
    if (read_from_base64_file(pk_path,
                             PK_HEADER,
                             PK_FOOTER,
                             pk,
                             CRYPTO_PUBLICKEYBYTES) != 0) {
        fprintf(stderr, "Error reading public key\n");
        return 1;
    }

    // Read and decode base64 signature
    if (read_from_base64_file(sig_path,
                             SIG_HEADER,
                             SIG_FOOTER,
                             sig,
                             CRYPTO_BYTES) != 0) {
        fprintf(stderr, "Error reading signature\n");
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

    // Verify signature
    int result = CRYPTO_VERIFY(sig, CRYPTO_BYTES,
                              msg, msg_len, NULL, 0, pk);
    
    if (result != 0) {
        fprintf(stderr, "Signature verification failed\n");
    } else {
        fprintf(stderr, "Signature verified successfully!\n");
    }
    
    free(msg);
    return result;
}