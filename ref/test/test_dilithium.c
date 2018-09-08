#include <stdio.h>
#include <string.h>
#include "cpucycles.h"
#include "speed.h"
#include "../randombytes.h"
#include "../api.h"

#define MLEN 59
#define NTESTS 5000

unsigned long long timing_overhead;
#ifdef DBENCH
unsigned long long *tred, *tadd, *tmul, *tround, *tsample, *tpack, *tshake;
#endif

int main(void)
{
  unsigned int i;
  int ret;
  unsigned long long j, mlen, smlen;
  unsigned char m[MLEN];
  unsigned char sm[MLEN + CRYPTO_BYTES];
  unsigned char m2[MLEN + CRYPTO_BYTES];
  unsigned char pk[CRYPTO_PUBLICKEYBYTES];
  unsigned char sk[CRYPTO_SECRETKEYBYTES];
  unsigned long long tkeygen[NTESTS], tsign[NTESTS], tverify[NTESTS];
#ifdef DBENCH
  unsigned long long t[7][NTESTS];

  memset(t, 0, sizeof(t));
#endif

  timing_overhead = cpucycles_overhead();

  for(i = 0; i < NTESTS; ++i) {
    randombytes(m, MLEN);

#ifdef DBENCH
    tred = t[0] + i;
    tadd = t[1] + i;
    tmul = t[2] + i;
    tround = t[3] + i;
    tsample = t[4] + i;
    tpack = t[5] + i;
    tshake = t[6] + i;
#endif

    tkeygen[i] = cpucycles_start();
    crypto_sign_keypair(pk, sk);
    tkeygen[i] = cpucycles_stop() - tkeygen[i] - timing_overhead;

    tsign[i] = cpucycles_start();
    crypto_sign(sm, &smlen, m, MLEN, sk);
    tsign[i] = cpucycles_stop() - tsign[i] - timing_overhead;

    tverify[i] = cpucycles_start();
    ret = crypto_sign_open(m2, &mlen, sm, smlen, pk);
    tverify[i] = cpucycles_stop() - tverify[i] - timing_overhead;

    if(ret) {
      printf("Verification failed\n");
      return -1;
    }

    if(mlen != MLEN) {
      printf("Message lengths don't match\n");
      return -1;
    }

    for(j = 0; j < mlen; ++j) {
      if(m[j] != m2[j]) {
        printf("Messages don't match\n");
        return -1;
      }
    }
  }

  print_results("keygen:", tkeygen, NTESTS);
  print_results("sign: ", tsign, NTESTS);
  print_results("verify: ", tverify, NTESTS);

#ifdef DBENCH
  print_results("modular reduction:", t[0], NTESTS);
  print_results("addition:", t[1], NTESTS);
  print_results("multiplication:", t[2], NTESTS);
  print_results("rounding:", t[3], NTESTS);
  print_results("rejection sampling:", t[4], NTESTS);
  print_results("packing:", t[5], NTESTS);
  print_results("SHAKE:", t[6], NTESTS);
#endif

  return 0;
}
