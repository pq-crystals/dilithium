#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include "../randombytes.h"
#include "../sign.h"

#define MLEN 59
#define CTXLEN 14
#define NTESTS 1


int main(void)
{
  FILE *fin = fopen("test/input.txt", "rb");
  FILE *fout = fopen("test/output.txt", "w");
  if (!fin || !fout) {
    printf("File error\n");
    return 1;
  }

  // Read message from input.txt
  uint8_t m[MLEN + CRYPTO_BYTES] = {0};
  size_t mlen = fread(m, 1, MLEN, fin);
  fclose(fin);

  // KeyGen
  uint8_t pk[CRYPTO_PUBLICKEYBYTES];
  uint8_t sk[CRYPTO_SECRETKEYBYTES];
  crypto_sign_keypair(pk, sk);

  fprintf(fout, "KeyGen Stage:\n- Input: None\n- Output:\n");
  fprintf(fout, "* Public Key: ");
  for (int i = 0; i < CRYPTO_PUBLICKEYBYTES; i++) fprintf(fout, "%02x", pk[i]);
  fprintf(fout, "\n* Secret Key: ");
  for (int i = 0; i < CRYPTO_SECRETKEYBYTES; i++) fprintf(fout, "%02x", sk[i]);
  fprintf(fout, "\n\n");

  // Signing
  uint8_t sig[CRYPTO_BYTES];
  size_t siglen = 0;
  crypto_sign_signature(sig, &siglen, m, mlen, NULL, 0, sk);

  fprintf(fout, "Signing Stage:\n- Input: input.txt, sk\n- Output:\n");
  fprintf(fout, "* Signature: ");
  for (size_t i = 0; i < siglen; i++) fprintf(fout, "%02x", sig[i]);
  fprintf(fout, "\n\n");


  // Make sig invalid to test, delete it to make valid
  sig[0] ^= 0xFF;

  // Verify
  int valid = crypto_sign_verify(sig, siglen, m, mlen, NULL, 0, pk);
  fprintf(fout, "Verifying Stage:\n- Input: input.txt, sig, pk\n- Output: ");
  fprintf(fout, "%s\n", valid == 0 ? "Valid" : "Invalid");

  fclose(fout);
  return 0;
}
