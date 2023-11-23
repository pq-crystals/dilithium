#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h> // Added for string functions
#include <inttypes.h>  
#include "../randombytes.h"
#include "../sign.h"

#define MLEN 40881
#define NTESTS 1  // Set to 1 for a single test

int main(int argc, char *argv[])
 {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return -1;
    }

  size_t j;
  int ret;
  size_t mlen, smlen;
  uint8_t b;
  uint8_t m[MLEN + CRYPTO_BYTES];
  uint8_t m2[MLEN + CRYPTO_BYTES];
  uint8_t sm[MLEN + CRYPTO_BYTES];
  uint8_t pk[CRYPTO_PUBLICKEYBYTES];
  uint8_t sk[CRYPTO_SECRETKEYBYTES];

  

   
   system("matlab -nodisplay -r \"run('generate.m'); exit;\"");  // Run the MATLAB script to generate the text file

  FILE *file = fopen(argv[1], "r");

  if (file == NULL) {
    fprintf(stderr, "Error opening the file\n");
    return -1;
  }

  // Read the message from the file
  if (fgets((char *)m, MLEN, file) == NULL) {
    fprintf(stderr, "Error reading message from file\n");
    fclose(file);
    return -1;
  }

  fclose(file); // Close the file

  
  
  for (int i = 0; i < MLEN; i++) {
        printf("%c", m[i]);  // Use PRIu8 for uint8_t
    }
printf("\n");
  

  // Remove newline character from the input
  size_t message_length = strlen((char *)m);
  

  // Create a key pair
  crypto_sign_keypair(pk, sk);

// Sign the modified message
crypto_sign(sm, &smlen, m, message_length, sk);




  // Verify the signature-decryption
  ret = crypto_sign_open(m2, &mlen, sm, smlen, pk);



printf("\n");
printf("\n");
printf("\n");
printf("\n");
printf("\n");
printf("\n");


for (int i = 0; i < mlen; i++) {
        printf("%c", m2[i]);  // Use PRIu8 for uint8_t
    }
printf("\n");


  if (ret) {
    fprintf(stderr, "Verification failed\n");
  } else {
    printf("Verification successful\n");
  }

  printf("CRYPTO_PUBLICKEYBYTES = %d\n", CRYPTO_PUBLICKEYBYTES);
  printf("CRYPTO_SECRETKEYBYTES = %d\n", CRYPTO_SECRETKEYBYTES);
  printf("CRYPTO_BYTES = %d\n", CRYPTO_BYTES);

  return 0;
}

