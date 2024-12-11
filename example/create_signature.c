#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stddef.h>
#include <dilithium/api.h>
#include "size.h"
#include <stdint.h>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/buffer.h>
#include "debug.h"
#include "base64.h"

#define MAX_LINE_LENGTH 1024

int main(int argc, char *argv[]) {
    uint8_t sk[PQCLEAN_DILITHIUM5_CRYPTO_SECRETKEYBYTES];
    uint8_t pk[PQCLEAN_DILITHIUM5_CRYPTO_PUBLICKEYBYTES];
    uint8_t sig[PQCLEAN_DILITHIUM5_CRYPTO_BYTES];
    size_t siglen;
    FILE *sk_file = NULL, *msg_file = NULL;
    uint8_t *msg;
    size_t msg_len;
    int opt;
    char *sk_path = NULL, *msg_path = NULL, *pk_path = NULL;
    char line[MAX_LINE_LENGTH];
    int in_key = 0;
    size_t line_length;
    uint8_t buffer[1024];
    size_t total_bytes = 0;
    int bytes_read;

    while ((opt = getopt(argc, argv, "s:i:p:")) != -1) {
        switch (opt) {
            case 's': sk_path = optarg; break;
            case 'i': msg_path = optarg; break;
            case 'p': pk_path = optarg; break;
            default:
                fprintf(stderr, "Usage: %s -s secret_key -i plaintext -p public_key\n", argv[0]);
                return 1;
        }
    }

    if (!sk_path || !msg_path || !pk_path) {
        fprintf(stderr, "Missing required arguments\n");
        return 1;
    }

    // Read and decode base64 secret key
    FILE *debug_sk_file = fopen(sk_path, "r");
    if (!debug_sk_file) {
        fprintf(stderr, "Error opening secret key file: %s\n", sk_path);
        return 1;
    }

    // Debug: Print raw file contents
    printf("Reading secret key from: %s\n", sk_path);
    char debug_buffer[256];
    while (fgets(debug_buffer, sizeof(debug_buffer), debug_sk_file)) {
        printf("Line: %s", debug_buffer);
    }
    fclose(debug_sk_file);
    
    printf("Attempting to decode base64 with:\n");
    printf("Header: %s\n", "-----BEGIN DILITHIUM SECRET KEY-----");
    printf("Footer: %s\n", "-----END DILITHIUM SECRET KEY-----");
    printf("Expected key size: %u bytes\n", PQCLEAN_DILITHIUM5_CRYPTO_SECRETKEYBYTES);
    
    int read_result = read_from_base64_file(sk_path, 
                             "-----BEGIN DILITHIUM SECRET KEY-----",
                             "-----END DILITHIUM SECRET KEY-----",
                             sk, 
                             PQCLEAN_DILITHIUM5_CRYPTO_SECRETKEYBYTES);
    if (read_result != 0) {
        fprintf(stderr, "Error reading secret key (code: %d)\n", read_result);
        return 1;
    }
    debug_hexa("Secret key", sk, PQCLEAN_DILITHIUM5_CRYPTO_SECRETKEYBYTES);

    // Read and decode base64 public key
    if (read_from_base64_file(pk_path,
                             "-----BEGIN DILITHIUM PUBLIC KEY-----",
                             "-----END DILITHIUM PUBLIC KEY-----",
                             pk,
                             PQCLEAN_DILITHIUM5_CRYPTO_PUBLICKEYBYTES) != 0) {
        fprintf(stderr, "Error reading public key\n");
        return 1;
    }
    debug_hexa("Public key", pk, PQCLEAN_DILITHIUM5_CRYPTO_PUBLICKEYBYTES);

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
    // Debug output: message length and hex dump
    debug_hexa("Message", msg, msg_len);

    // Create signature
    if (pqcrystals_dilithium5_ref_signature(sig, &siglen, msg, msg_len, NULL, 0, sk) != 0) {
        fprintf(stderr, "Signature creation failed\n");
        return 1;
    }

    // Immediately verify the signature before outputting
    if (pqcrystals_dilithium5_ref_verify(sig, siglen, msg, msg_len, NULL, 0, pk) != 0) {
        fprintf(stderr, "Signature verification failed - internal error\n");
        return 1;
    }

    // Write signature to stdout using base64 functions
    if (write_to_base64_file("/dev/stdout", 
                            "-----BEGIN DILITHIUM SIGNATURE-----",
                            "-----END DILITHIUM SIGNATURE-----",
                            sig, siglen) != 0) {
        fprintf(stderr, "Error writing signature\n");
        return 1;
    }

    free(msg);
    return 0;
}