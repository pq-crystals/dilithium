#include <stdio.h>
#include "../params.h"
#include "../sign.h"
#include "../poly.h"
#include "../polyvec.h"
#include "../packing.h"
#include "../rng.h"

#define NVECTORS 1000

int main(void) {
  unsigned int i, j, k, l;
  unsigned char seed[CRHBYTES];
  unsigned char buf[CRYPTO_BYTES];
  poly c, tmp;
  polyvecl s, y, mat[K];
  polyveck w, w1, w0, t1, t0, h;
  int32_t u;

  for (i = 0; i < CRHBYTES; ++i)
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
    printf("A = ((");
    for(j = 0; j < K; ++j) {
      for(k = 0; k < L; ++k) {
        for(l = 0; l < N; ++l) {
          printf("%7u", mat[j].vec[k].coeffs[l]);
          if(l < N-1) printf(", ");
          else if(k < L-1) printf("), (");
          else if(j < K-1) printf(");\n     (");
          else printf("))\n");
        }
      }
    }

    for(j = 0; j < L; ++j)
      poly_uniform_eta(&s.vec[j], seed, j);

    polyeta_pack(buf, &s.vec[0]);
    polyeta_unpack(&tmp, buf);
    for(j = 0; j < N; ++j)
      if(tmp.coeffs[j] != s.vec[0].coeffs[j])
        printf("ERROR in polyeta_(un)pack!\n");

    printf("s = ((");
    for(j = 0; j < L; ++j) {
      for(k = 0; k < N; ++k) {
        u = s.vec[j].coeffs[k];
        if(u > (Q-1)/2) u -= Q;
        printf("%2d", u);
        if(k < N-1) printf(", ");
        else if(j < L-1) printf("),\n     (");
        else printf(")\n");
      }
    }

    for(j = 0; j < L; ++j)
      poly_uniform_gamma1m1(&y.vec[j], seed, j);

    polyvecl_freeze(&y);
    polyz_pack(buf, &y.vec[0]);
    polyz_unpack(&tmp, buf);
    for(j = 0; j < N; ++j)
      if(tmp.coeffs[j] != y.vec[0].coeffs[j])
        printf("ERROR in polyz_(un)pack!\n");

    printf("y = ((");
    for(j = 0; j < L; ++j) {
      for(k = 0; k < N; ++k) {
        u = y.vec[j].coeffs[k];
        if(u > (Q-1)/2) u -= Q;
        printf("%7d", u);
        if(k < N-1) printf(", ");
        else if(j < L-1) printf("),\n     (");
        else printf(")\n");
      }
    }

    polyvecl_ntt(&y);
    for(j = 0; j < K; ++j) {
      polyvecl_pointwise_acc_invmontgomery(w.vec+j, mat+j, &y);
      poly_reduce(w.vec+j);
      poly_invntt_montgomery(w.vec+j);
    }

    polyveck_csubq(&w);
    polyveck_decompose(&w1, &w0, &w);

    for(j = 0; j < N; ++j) {
      tmp.coeffs[j] = w1.vec[0].coeffs[j]*ALPHA + w0.vec[0].coeffs[j];
      if(tmp.coeffs[j] >= Q) tmp.coeffs[j] -= Q;
      if(tmp.coeffs[j] != w.vec[0].coeffs[j])
        printf("ERROR in poly_decompose\n");
    }

    polyw1_pack(buf, &w1.vec[0]);
    for(j = 0; j < N/2; ++j) {
      tmp.coeffs[2*j+0] = buf[j] & 0xF;
      tmp.coeffs[2*j+1] = buf[j] >> 4;
      if(tmp.coeffs[2*j+0] != w1.vec[0].coeffs[2*j+0]
         || tmp.coeffs[2*j+1] != w1.vec[0].coeffs[2*j+1])
        printf("ERROR in polyw1_pack!\n");
    }

    if(poly_chknorm(&w1.vec[0], 16))
      printf("ERROR in poly_chknorm(.,16)!\n");

    printf("w1 = ((");
    for(j = 0; j < K; ++j) {
      for(k = 0; k < N; ++k) {
        printf("%2u", w1.vec[j].coeffs[k]);
        if(k < N-1) printf(", ");
        else if(j < K-1) printf("),\n      (");
        else printf(")\n");
      }
    }
    printf("w0 = ((");
    for(j = 0; j < K; ++j) {
      for(k = 0; k < N; ++k) {
        u = w0.vec[j].coeffs[k] - Q;
        printf("%7d", u);
        if(k < N-1) printf(", ");
        else if(j < K-1) printf("),\n      (");
        else printf(")\n");
      }
    }

    polyveck_power2round(&t1, &t0, &w);

    for(j = 0; j < N; ++j) {
      tmp.coeffs[j] = (t1.vec[0].coeffs[j] << D) + t0.vec[0].coeffs[j] - Q;
      if(tmp.coeffs[j] != w.vec[0].coeffs[j])
        printf("ERROR in poly_power2round!\n");
    }

    polyt1_pack(buf, &t1.vec[0]);
    polyt1_unpack(&tmp, buf);
    for(j = 0; j < N; ++j) {
      if(tmp.coeffs[j] != t1.vec[0].coeffs[j])
        printf("ERROR in polyt1_(un)pack!\n");
    }
    polyt0_pack(buf, &t0.vec[0]);
    polyt0_unpack(&tmp, buf);
    for(j = 0; j < N; ++j) {
      if(tmp.coeffs[j] != t0.vec[0].coeffs[j])
        printf("ERROR in polyt0_(un)pack!\n");
    }

    printf("t1 = ((");
    for(j = 0; j < K; ++j) {
      for(k = 0; k < N; ++k) {
        printf("%3u", t1.vec[j].coeffs[k]);
        if(k < N-1) printf(", ");
        else if(j < K-1) printf("),\n      (");
        else printf(")\n");
      }
    }
    printf("t0 = ((");
    for(j = 0; j < K; ++j) {
      for(k = 0; k < N; ++k) {
        u = t0.vec[j].coeffs[k] - Q;
        printf("%5d", u);
        if(k < N-1) printf(", ");
        else if(j < K-1) printf("),\n      (");
        else printf(")\n");
      }
    }

    polyveck_freeze(&t0);
    if(poly_chknorm(&t0.vec[0], (1U << (D-1)) + 1))
      printf("ERROR in poly_chknorm(., 1 << (D-1) + 1)!\n");

    challenge(&c, seed, &w);
    printf("c = (");
    for(j = 0; j < N; ++j) {
      u = c.coeffs[j];
      if(u > (Q-1)/2) u -= Q;
      printf("%2d", u);
      if(j < N-1) printf(", ");
      else printf(")\n");
    }

    polyveck_make_hint(&h, &w0, &w1);
    pack_sig(buf, &y, &h, &c);
    unpack_sig(&y, &h, &tmp, buf);
    for(j = 0; j < N; j++)
      if(c.coeffs[j] != tmp.coeffs[j])
        printf("ERROR in (un)pack_sig!\n");

    printf("\n");
  }

  return 0;
}
