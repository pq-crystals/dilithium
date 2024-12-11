#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stddef.h>
#include <dilithium/api.h>
#include "size.h"
#include <stdint.h>

int main(int argc, char *argv[]) {
    uint8_t sk[PQCLEAN_DILITHIUM5_CRYPTO_SECRETKEYBYTES];
    uint8_t sig[PQCLEAN_DILITHIUM5_CRYPTO_BYTES];
    size_t siglen;
    FILE *sk_file, *msg_file;
    uint8_t *msg;
    size_t msg_len;
    int opt;
    char *sk_path = NULL, *msg_path = NULL;

    while ((opt = getopt(argc, argv, "k:i:")) != -1) {
        switch (opt) {
            case 'k': sk_path = optarg; break;
            case 'i': msg_path = optarg; break;
            default:
                fprintf(stderr, "Usage: %s -k private -i plaintext\n", argv[0]);
                return 1;
        }
    }

    if (!sk_path || !msg_path) {
        fprintf(stderr, "Missing required arguments\n");
        return 1;
    }

    // Read secret key
    sk_file = fopen(sk_path, "rb");
    if (!sk_file || fread(sk, 1, PQCLEAN_DILITHIUM5_CRYPTO_SECRETKEYBYTES, sk_file) != PQCLEAN_DILITHIUM5_CRYPTO_SECRETKEYBYTES) {
        fprintf(stderr, "Error reading secret key\n");
        return 1;
    }
    fclose(sk_file);

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
    if (pqcrystals_dilithium5_ref_signature(
            sig, &siglen,
            msg, msg_len,
            NULL, 0,
            sk
        ) != 0) {
        fprintf(stderr, "Signature creation failed\n");
        return 1;
    }

    // Write signature to stdout
    fwrite(sig, 1, siglen, stdout);
    free(msg);
    return 0;
}