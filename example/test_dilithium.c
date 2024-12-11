#include <stdio.h>
#include <string.h>
#include <dilithium/api.h>
#include <dilithium/randombytes.h>
#include "size.h"

#define NUM_ITERATIONS 10
#define MAX_MSG_LEN 100

int test_iteration(size_t msg_len) {
    // Generate random message
    uint8_t msg[MAX_MSG_LEN];
    randombytes(msg, msg_len);

    printf("\nTesting with %zu byte message: ", msg_len);
    for(size_t i = 0; i < msg_len && i < 16; i++) {
        printf("%02x", msg[i]);
    }
    if (msg_len > 16) printf("...");
    printf("\n");

    // Generate keypair
    uint8_t pk[PQCLEAN_DILITHIUM5_CRYPTO_PUBLICKEYBYTES];
    uint8_t sk[PQCLEAN_DILITHIUM5_CRYPTO_SECRETKEYBYTES];
    
    if (pqcrystals_dilithium5_ref_keypair(pk, sk) != 0) {
        fprintf(stderr, "❌ Key generation failed!\n");
        return 1;
    }

    // Create signature
    uint8_t sig[PQCLEAN_DILITHIUM5_CRYPTO_BYTES];
    size_t siglen;

    if (pqcrystals_dilithium5_ref_signature(sig, &siglen, msg, msg_len, NULL, 0, sk) != 0) {
        fprintf(stderr, "❌ Signature creation failed!\n");
        return 1;
    }

    // Verify signature
    int verify_result = pqcrystals_dilithium5_ref_verify(sig, siglen, msg, msg_len, NULL, 0, pk);
    
    if (verify_result != 0) {
        fprintf(stderr, "❌ Signature verification failed! (error code: %d)\n", verify_result);
        return 1;
    }
    
    printf("✅ Success (message: %zu bytes, signature: %zu bytes)\n", msg_len, siglen);
    return 0;
}

int main(void) {
    printf("Running %d iterations with varying message sizes...\n", NUM_ITERATIONS);
    
    size_t message_sizes[] = {0, 1, 13, 32, 64, 100};
    int num_sizes = sizeof(message_sizes) / sizeof(message_sizes[0]);
    
    int failed = 0;
    for (int i = 0; i < NUM_ITERATIONS; i++) {
        printf("\nIteration %d/%d:\n", i + 1, NUM_ITERATIONS);
        
        for (int s = 0; s < num_sizes; s++) {
            if (test_iteration(message_sizes[s]) != 0) {
                failed++;
            }
        }
    }

    printf("\nTest summary:\n");
    printf("Total iterations: %d\n", NUM_ITERATIONS * num_sizes);
    printf("Failed: %d\n", failed);
    printf("Succeeded: %d\n", NUM_ITERATIONS * num_sizes - failed);
    
    return failed ? 1 : 0;
} 