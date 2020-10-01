#include <stdint.h>
#include "../sign.h"
#include "../poly.h"
#include "../polyvec.h"
#include "../params.h"
#include "cpucycles.h"
#include "speed_print.h"

#define NTESTS 1000

uint64_t t[NTESTS];

int main(void)
{
  unsigned int i;
  size_t smlen;
  uint8_t pk[CRYPTO_PUBLICKEYBYTES];
  uint8_t sk[CRYPTO_SECRETKEYBYTES];
  uint8_t sm[CRYPTO_BYTES + CRHBYTES];
  __attribute__((aligned(32)))
  uint8_t seed[CRHBYTES] = {0};
  polyvecl mat[K];
  poly *a = &mat[0].vec[0];
  poly *b = &mat[0].vec[1];
  poly *c = &mat[0].vec[2];

  for(i = 0; i < NTESTS; ++i) {
    t[i] = cpucycles();
    polyvec_matrix_expand(mat, seed);
  }
  print_results("polyvec_matrix_expand:", t, NTESTS);

  for(i = 0; i < NTESTS; ++i) {
    t[i] = cpucycles();
    poly_uniform_eta(a, seed, 0);
  }
  print_results("poly_uniform_eta:", t, NTESTS);

  for(i = 0; i < NTESTS; ++i) {
    t[i] = cpucycles();
    poly_uniform_gamma1(a, seed, 0);
  }
  print_results("poly_uniform_gamma1:", t, NTESTS);

  for(i = 0; i < NTESTS; ++i) {
    t[i] = cpucycles();
    poly_ntt(a);
  }
  print_results("poly_ntt:", t, NTESTS);

  for(i = 0; i < NTESTS; ++i) {
    t[i] = cpucycles();
    poly_invntt_tomont(a);
  }
  print_results("poly_invntt_tomont:", t, NTESTS);

  for(i = 0; i < NTESTS; ++i) {
    t[i] = cpucycles();
    poly_pointwise_montgomery(c, a, b);
  }
  print_results("poly_pointwise_montgomery:", t, NTESTS);

  for(i = 0; i < NTESTS; ++i) {
    t[i] = cpucycles();
    poly_challenge(c, seed);
  }
  print_results("poly_challenge:", t, NTESTS);

  for(i = 0; i < NTESTS; ++i) {
    t[i] = cpucycles();
    crypto_sign_keypair(pk, sk);
  }
  print_results("Keypair:", t, NTESTS);

  for(i = 0; i < NTESTS; ++i) {
    t[i] = cpucycles();
    crypto_sign(sm, &smlen, sm, CRHBYTES, sk);
  }
  print_results("Sign:", t, NTESTS);

  for(i = 0; i < NTESTS; ++i) {
    t[i] = cpucycles();
    crypto_sign_verify(sm, CRYPTO_BYTES, sm, CRHBYTES, pk);
  }
  print_results("Verify:", t, NTESTS);

  return 0;
}
