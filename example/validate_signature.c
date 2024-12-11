#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dilithium/api.h>
#include "size.h"

#define CONTEXT_LEN 13

int main(int argc, char *argv[]) {
    unsigned char pk[PQCLEAN_DILITHIUM5_CRYPTO_PUBLICKEYBYTES];
    unsigned char sig[PQCLEAN_DILITHIUM5_CRYPTO_BYTES];
    unsigned char ctx[CONTEXT_LEN] = { 0 };
    FILE *pk_file, *sig_file, *msg_file;
    unsigned char *msg;
    size_t msg_len;
    int opt;
    char *pk_path = NULL, *sig_path = NULL, *msg_path = NULL;

    while ((opt = getopt(argc, argv, "p:s:i:")) != -1) {
        switch (opt) {
            case 'p': pk_path = optarg; break;
            case 's': sig_path = optarg; break;
            case 'i': msg_path = optarg; break;
            default:
                fprintf(stderr, "Usage: %s -p publickey -i plaintext -s signature\n", argv[0]);
                return 1;
        }
    }

    if (!pk_path || !sig_path || !msg_path) {
        fprintf(stderr, "Missing required arguments\n");
        return 1;
    }

    // Read public key
    pk_file = fopen(pk_path, "rb");
    if (!pk_file || fread(pk, 1, PQCLEAN_DILITHIUM5_CRYPTO_PUBLICKEYBYTES, pk_file) != PQCLEAN_DILITHIUM5_CRYPTO_PUBLICKEYBYTES) {
        fprintf(stderr, "Error reading public key\n");
        return 1;
    }
    fclose(pk_file);

    // Read signature
    sig_file = fopen(sig_path, "rb");
    if (!sig_file || fread(sig, 1, PQCLEAN_DILITHIUM5_CRYPTO_BYTES, sig_file) != PQCLEAN_DILITHIUM5_CRYPTO_BYTES) {
        fprintf(stderr, "Error reading signature\n");
        return 1;
    }
    fclose(sig_file);

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
    int result = pqcrystals_dilithium5_ref_verify(
        sig, PQCLEAN_DILITHIUM5_CRYPTO_BYTES,
        msg, msg_len,
        ctx, CONTEXT_LEN,
        pk
    );
    free(msg);
    return result;
}