#include <stdint.h>
#include <stdio.h>
#include "cpucycles.h"
#include "speed.h"
#include "../params.h"
#include "../randombytes.h"
#include "../poly.h"

#define NTESTS 1000

static void poly_naivemul(poly *c, const poly *a, const poly *b) {
  unsigned int i,j;
  uint32_t r[2*N];

  for(i = 0; i < 2*N; i++)
    r[i] = 0;

  for(i = 0; i < N; i++)
    for(j = 0; j < N; j++) {
      r[i+j] += ((uint64_t)a->coeffs[i] * b->coeffs[j]) % Q;
      r[i+j] %= Q;
    }

  for(i = N; i < 2*N-1; i++) {
    r[i-N] = r[i-N] + Q - r[i];
    r[i-N] %= Q;
  }

  for(i = 0; i < N; i++)
    c->coeffs[i] = r[i];
}

int main(void) {
  unsigned int i, j;
  unsigned long long t1[NTESTS], t2[NTESTS], overhead;
  unsigned char seed[SEEDBYTES];
  uint16_t nonce = 0;
  poly a, b, c1, c2;

  overhead = cpucycles_overhead();
  randombytes(seed, sizeof(seed));

  for(i = 0; i < NTESTS; ++i) {
    poly_uniform(&a, seed, nonce++);
    poly_uniform(&b, seed, nonce++);

    t1[i] = cpucycles_start();
    poly_naivemul(&c1, &a, &b);
    t1[i] = cpucycles_stop() - t1[i] - overhead;

    t2[i] = cpucycles_start();
    poly_ntt(&a);
    poly_ntt(&b);
    poly_pointwise_invmontgomery(&c2, &a, &b);
    poly_invntt_montgomery(&c2);
    t2[i] = cpucycles_stop() - t2[i] - overhead;

    poly_csubq(&c2);
    for(j = 0; j < N; ++j)
      if(c2.coeffs[j] != c1.coeffs[j])
        printf("FAILURE: c2[%u] = %u != %u\n", j, c2.coeffs[j], c1.coeffs[j]);
  }

  print_results("naive: ", t1, NTESTS);
  print_results("ntt: ", t2, NTESTS);
  return 0;
}
