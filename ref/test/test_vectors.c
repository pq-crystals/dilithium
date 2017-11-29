#include <stdio.h>
#include "../params.h"
#include "../sign.h"
#include "../poly.h"
#include "../polyvec.h"
#include "../rng.h"

#define NVECTORS 10

int main(void) {
  unsigned int i, j, k, l;
  unsigned char seed[SEEDBYTES + CRHBYTES];
  poly c;
  polyvecl s, y, mat[K];
  polyveck w, tmp;

  for (i = 0; i < 48; ++i)
    seed[i] = i;

  randombytes_init(seed, NULL, 256);

  for(i = 0; i < NVECTORS; ++i) {
    printf("count = %u\n", i);

    randombytes(seed, sizeof(seed));
    printf("seed = ");
    for(j = 0; j < sizeof(seed); ++j)
      printf("%.2hhX", seed[j]);
    printf("\n");

    expand_mat(mat, seed);
    printf("mat = ");
    for(j = 0; j < K; ++j)
      for(k = 0; k < L; ++k)
        for(l = 0; l < N; ++l)
          printf("%.8X", mat[j].vec[k].coeffs[l]);
    printf("\n");

    for(j = 0; j < L; ++j)
      poly_uniform_eta(&s.vec[j], seed, j);

    printf("s = ");
    for(j = 0; j < L; ++j)
      for(k = 0; k < N; ++k)
        printf("%.8X", s.vec[j].coeffs[k]);
    printf("\n");

    for(j = 0; j < L; ++j)
      poly_uniform_gamma1m1(&y.vec[j], seed, j);

    printf("y = ");
    for(j = 0; j < L; ++j)
      for(k = 0; k < N; ++k)
        printf("%.8X", y.vec[j].coeffs[k]);
    printf("\n");

    polyvecl_ntt(&y);
    for(j = 0; j < K; ++j) {
      polyvecl_pointwise_acc_invmontgomery(w.vec+j, mat+j, &y);
      poly_invntt_montgomery(w.vec+j);
    }
    polyveck_freeze(&w);
    polyveck_decompose(&w, &tmp, &w);

    printf("w1 = ");
    for(j = 0; j < K; ++j)
      for(k = 0; k < N; ++k)
        printf("%.8X", w.vec[j].coeffs[k]);
    printf("\n");

    challenge(&c, seed + SEEDBYTES, &w);
    printf("c = ");
    for(j = 0; j < N; ++j)
      printf("%.8X", c.coeffs[j]);
    printf("\n");
  }
  
  return 0;
}
