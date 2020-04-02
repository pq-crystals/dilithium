#include <stddef.h>
#include <stdio.h>
#include "../randombytes.h"
#include "../sign.h"

#define MLEN 59
#define NTESTS 1000

int main(void)
{
  unsigned int i, j;
  int ret;
  unsigned long long mlen, smlen;
  unsigned char m[MLEN];
  unsigned char sm[MLEN + CRYPTO_BYTES];
  unsigned char m2[MLEN + CRYPTO_BYTES];
  unsigned char pk[CRYPTO_PUBLICKEYBYTES];
  unsigned char sk[CRYPTO_SECRETKEYBYTES];

  for(i = 0; i < NTESTS; ++i) {
    randombytes(m, MLEN);

    crypto_sign_keypair(pk, sk);
    crypto_sign(sm, &smlen, m, MLEN, sk);
    ret = crypto_sign_open(m2, &mlen, sm, smlen, pk);

    if(ret) {
      fprintf(stderr, "Verification failed\n");
      return -1;
    }

    if(mlen != MLEN) {
      fprintf(stderr, "Message lengths don't match\n");
      return -1;
    }

    for(j = 0; j < mlen; ++j) {
      if(m[j] != m2[j]) {
        fprintf(stderr, "Messages don't match\n");
        return -1;
      }
    }

    randombytes((unsigned char *)&j, sizeof(j));
    do {
      randombytes(m2, 1);
    } while(!m2[0]);
    sm[j % CRYPTO_BYTES] += m2[0];
    ret = crypto_sign_open(m2, &mlen, sm, smlen, pk);
    if(!ret) {
      fprintf(stderr, "Trivial forgeries possible\n");
      return -1;
    }
  }

  return 0;
}
