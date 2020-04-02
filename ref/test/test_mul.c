#include <stdint.h>
#include <stdio.h>
#include "../params.h"
#include "../randombytes.h"
#include "../poly.h"

#define NTESTS 100000

static void poly_naivemul(poly *c, const poly *a, const poly *b) {
  unsigned int i,j;
  uint32_t r[2*N] = {0};

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
  uint8_t seed[SEEDBYTES];
  uint16_t nonce = 0;
  poly a, b, c, d;

  randombytes(seed, sizeof(seed));
  for(i = 0; i < NTESTS; ++i) {
    poly_uniform(&a, seed, nonce++);
    poly_uniform(&b, seed, nonce++);

    c = a;
    poly_ntt(&c);
    for(j = 0; j < N; ++j)
      c.coeffs[j] = (uint64_t)c.coeffs[j]*8265825 % Q;
    poly_invntt_tomont(&c);
    for(j = 0; j < N; ++j) {
      if(c.coeffs[j]%Q != a.coeffs[j])
        fprintf(stderr, "ERROR in ntt/invntt: c[%u] = %u != %u\n",
                j, c.coeffs[j]%Q, a.coeffs[j]);
    }

    poly_naivemul(&c, &a, &b);
    poly_ntt(&a);
    poly_ntt(&b);
    poly_pointwise_montgomery(&d, &a, &b);
    poly_invntt_tomont(&d);
    poly_csubq(&d);

    for(j = 0; j < N; ++j) {
      if(d.coeffs[j] != c.coeffs[j])
        fprintf(stderr, "ERROR in multiplication: d[%u] = %u != %u\n",
                j, d.coeffs[j], c.coeffs[j]);
    }
  }

  return 0;
}
