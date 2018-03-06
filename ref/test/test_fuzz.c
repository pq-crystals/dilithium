#include <stdio.h>
#include <stdlib.h>
#include "../api.h"
#include "../randombytes.h"

#define MLEN 59
#define NTESTS 100000

int main(int argc, const char **argv)
{
  int i, j;
  int ret, under_radar = 0;
  unsigned long long mlen, smlen;
  unsigned char m[MLEN];
  unsigned char sm[MLEN + CRYPTO_BYTES];
  unsigned char m2[MLEN + CRYPTO_BYTES];
  unsigned char pk[CRYPTO_PUBLICKEYBYTES];
  unsigned char sk[CRYPTO_SECRETKEYBYTES];

  int fbits = 1;
  if (argc > 1)
    fbits = atoi(argv[1]);

  under_radar = 0;
    randombytes(m, MLEN);
    crypto_sign_keypair(pk, sk);

  for(i = 0; i < NTESTS; ++i) {
    crypto_sign(sm, &smlen, m, MLEN, sk);
    for (j = 0; j < fbits; j++)
      sm[rand()%CRYPTO_BYTES] ^= 1<<(rand()&7);
    ret = crypto_sign_open(m2, &mlen, sm, smlen, pk);
    under_radar += !ret;
  }

  printf("%d out of %d accepted (aka 1-in-%d) for %d-bit faults injected\n",
		  under_radar, NTESTS, under_radar?(NTESTS/under_radar):-1, fbits);
 
  return 0;
}
