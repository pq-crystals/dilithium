#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dilithium/api.h>
#include "size.h"
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/buffer.h>
#include "base64.h"

#define MAX_LINE_LENGTH 1024

int main(int argc, char *argv[]) {
    unsigned char pk[PQCLEAN_DILITHIUM5_CRYPTO_PUBLICKEYBYTES];
    unsigned char sig[PQCLEAN_DILITHIUM5_CRYPTO_BYTES];
    FILE *pk_file, *sig_file, *msg_file;
    unsigned char *msg;
    size_t msg_len;
    int opt;
    char *pk_path = NULL, *sig_path = NULL, *msg_path = NULL;
    char line[MAX_LINE_LENGTH];
    int in_key = 0;
    BIO *bio, *b64;

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

    // Read and decode base64 public key
    if (read_from_base64_file(pk_path,
                             "-----BEGIN DILITHIUM PUBLIC KEY-----",
                             "-----END DILITHIUM PUBLIC KEY-----",
                             pk,
                             PQCLEAN_DILITHIUM5_CRYPTO_PUBLICKEYBYTES) != 0) {
        fprintf(stderr, "Error reading public key\n");
        return 1;
    }

    // Read and decode base64 signature
    if (read_from_base64_file(sig_path,
                             "-----BEGIN DILITHIUM SIGNATURE-----",
                             "-----END DILITHIUM SIGNATURE-----",
                             sig,
                             PQCLEAN_DILITHIUM5_CRYPTO_BYTES) != 0) {
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
    int result = pqcrystals_dilithium5_ref_verify(sig, PQCLEAN_DILITHIUM5_CRYPTO_BYTES,
                                                 msg, msg_len, NULL, 0, pk);
    
    if (result != 0) {
        fprintf(stderr, "Signature verification failed\n");
    } else {
        fprintf(stderr, "Signature verified successfully!\n");
    }
    
    free(msg);
    return result;
}