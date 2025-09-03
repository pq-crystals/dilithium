#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include "../randombytes.h"
#include "../sign.h"

#define MLEN 1200 // limit input for testing
#define NTESTS 1 // test count

void run_test(FILE *fout, const uint8_t *m, size_t mlen, int test_idx) 
{
  // KeyGen
  uint8_t pk[CRYPTO_PUBLICKEYBYTES];
  uint8_t sk[CRYPTO_SECRETKEYBYTES];
  crypto_sign_keypair(pk, sk);

  fprintf(fout, "Test #%d\n", test_idx+1);
  fprintf(fout, "KeyGen Stage:\n- Input: None\n- Output:\n");
  fprintf(fout, "* Public Key: ");
  for (int i = 0; i < CRYPTO_PUBLICKEYBYTES; i++) fprintf(fout, "%02x", pk[i]);
  fprintf(fout, "\n* Secret Key: ");
  for (int i = 0; i < CRYPTO_SECRETKEYBYTES; i++) fprintf(fout, "%02x", sk[i]);
  fprintf(fout, "\n\n");

  // Signing (NIST API)
  uint8_t sm[MLEN + CRYPTO_BYTES];
  size_t smlen = 0;
  crypto_sign(sm, &smlen, m, mlen, NULL, 0, sk);

  fprintf(fout, "Signing Stage (NIST API):\n- Input: input.txt, sk\n- Output:\n");
  fprintf(fout, "* Signed Message: ");
  for (size_t i = 0; i < smlen; i++) fprintf(fout, "%02x", sm[i]);
  fprintf(fout, "\n\n");

  // Open/Verify (NIST API)
  uint8_t m2[MLEN + CRYPTO_BYTES] = {0};
  size_t m2len = 0;
  int valid = crypto_sign_open(m2, &m2len, sm, smlen, NULL, 0, pk);
  fprintf(fout, "Verifying Stage (NIST API):\n- Input: signed message, pk\n- Output: %s\n", valid == 0 ? "Valid" : "Invalid");
  if (!valid) {
    fprintf(fout, "* Opened Message: ");
    for (size_t i = 0; i < m2len; i++) fprintf(fout, "%02x", m2[i]);
    fprintf(fout, "\n");
  }
  fprintf(fout, "\n");
}

int main(void)
{
  FILE *fin = fopen("test/input.txt", "rb");
  FILE *fout = fopen("test/output.txt", "w");
  if (!fin || !fout) {
    printf("File error\n");
    return 1;
  }

  // Read message from input.txt only once
  uint8_t m[MLEN + CRYPTO_BYTES] = {0};
  size_t mlen = fread(m, 1, MLEN, fin);
  fclose(fin);

  for (int test = 0; test < NTESTS; ++test) {
    run_test(fout, m, mlen, test);
  }
  fclose(fout);

  // Print testing information
  printf("\n[Testing Information - %d runs]\n\n", NTESTS);
  timing_info_t t = print_timing_info();
  printf("Average KeyGen time: %.6fs (%.2f ms)\n", t.keygen / NTESTS, (t.keygen / NTESTS) * 1000);
  printf("Average Signing time: %.6fs (%.2f ms)\n", t.sign / NTESTS, (t.sign / NTESTS) * 1000);
  printf("Average Verification time: %.6fs (%.2f ms)\n", t.verify / NTESTS, (t.verify / NTESTS) * 1000);
  //printf("Average sum time (3 stages): %.6fs (%.2f ms)\n", t.temp / NTESTS, (t.temp / NTESTS) * 1000);
  printf("Average all time (NIST compliance): %.6fs (%.2f ms)\n", t.all / NTESTS, (t.all / NTESTS) * 1000);
  printf("Public key bytes = %d\n", CRYPTO_PUBLICKEYBYTES);
  printf("Secret key bytes = %d\n", CRYPTO_SECRETKEYBYTES);
  printf("Signature bytes = %d\n", CRYPTO_BYTES);

  return 0;
}