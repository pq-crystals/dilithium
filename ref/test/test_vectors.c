#include <stdio.h>
#include "../params.h"
#include "../sign.h"
#include "../poly.h"
#include "../polyvec.h"
#include "../rng.h"

#define NVECTORS 1

int main(void) {
  unsigned int i, j, k, l;
  unsigned char seed[SEEDBYTES + CRHBYTES];
  polyvecl mat[K];
  poly p0, p1, p2, p3;

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

    poly_uniform_gamma1m1(&p0, seed, 0);
    poly_uniform_gamma1m1(&p1, seed, 1);
    poly_uniform_gamma1m1(&p2, seed, 2);
    poly_uniform_gamma1m1(&p3, seed, 3);
    printf("p0_uniform_gamma1m1 = ");
    for(j = 0; j < N; ++j)
      printf("%.8X", p0.coeffs[j]);
    printf("\n");

    printf("p1_uniform_gamma1m1 = ");
    for(j = 0; j < N; ++j)
      printf("%.8X", p1.coeffs[j]);
    printf("\n");

    printf("p2_uniform_gamma1m1 = ");
    for(j = 0; j < N; ++j)
      printf("%.8X", p2.coeffs[j]);
    printf("\n");

    printf("p3_uniform_gamma1m1 = ");
    for(j = 0; j < N; ++j)
      printf("%.8X", p3.coeffs[j]);
    printf("\n");

    poly_uniform_eta(&p0, seed, 0);
    poly_uniform_eta(&p1, seed, 1);
    poly_uniform_eta(&p2, seed, 2);
    poly_uniform_eta(&p3, seed, 3);
    printf("p0_uniform_eta = ");
    for(j = 0; j < N; ++j)
      printf("%.8X", p0.coeffs[j]);
    printf("\n");

    printf("p1_uniform_eta = ");
    for(j = 0; j < N; ++j)
      printf("%.8X", p1.coeffs[j]);
    printf("\n");

    printf("p2_uniform_eta = ");
    for(j = 0; j < N; ++j)
      printf("%.8X", p2.coeffs[j]);
    printf("\n");

    printf("p3_uniform_eta = ");
    for(j = 0; j < N; ++j)
      printf("%.8X", p3.coeffs[j]);
    printf("\n");
  }
  
  return 0;
}
