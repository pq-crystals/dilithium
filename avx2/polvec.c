#include <stdint.h>
#include "params.h"
#include "reduce.h"
#include "rounding.h"
#include "pol.h"
#include "polvec.h"

/**************************************************************/
/************ Vectors of polynomials of length L **************/
/**************************************************************/

void polvecl_copy(polvecl w, const polvecl v) {
  unsigned int i;

  for(i = 0; i < L; ++i)
    pol_copy(w[i], v[i]);
}

void polvecl_freeze(polvecl v) {
  unsigned int i;

  for(i = 0; i < L; ++i)
    pol_freeze(v[i]);
}

void polvecl_add(polvecl w, const polvecl u, const polvecl v) {
  unsigned int i;

  for(i = 0; i < L; ++i)
    pol_add(w[i], u[i], v[i]);
}

void polvecl_ntt(polvecl v) {
  unsigned int i;

  for(i = 0; i < L; ++i)
    pol_ntt(v[i]);
}

void polvecl_pointwise_acc_invmontgomery(pol w,
                                         const polvecl u,
                                         const polvecl v) 
{
  unsigned int i;
  pol t;

  pol_pointwise_invmontgomery(w, u[0], v[0]);

  for(i = 1; i < L; ++i) {
    pol_pointwise_invmontgomery(t, u[i], v[i]);
    pol_add(w, w, t);
  }

  for(i = 0; i < N; ++i) 
    w[i] = reduce32(w[i]);
}

int polvecl_chknorm(const polvecl v, uint32_t bound)  {
  unsigned int i;
  int ret = 0;

  for(i = 0; i < L; ++i)
    ret |= pol_chknorm(v[i], bound);

  return ret;
}

/**************************************************************/
/************ Vectors of polynomials of length K **************/
/**************************************************************/

void polveck_freeze(polveck v)  {
  unsigned int i;

  for(i = 0; i < K; ++i)
    pol_freeze(v[i]);
}

void polveck_add(polveck w, const polveck u, const polveck v) {
  unsigned int i;

  for(i = 0; i < K; ++i)
    pol_add(w[i], u[i], v[i]);
}

void polveck_sub(polveck w, const polveck u, const polveck v) {
  unsigned int i;

  for(i = 0; i < K; ++i)
    pol_sub(w[i], u[i], v[i]);
}

void polveck_neg(polveck v) { 
  unsigned int i;

  for(i = 0; i < K; ++i)
    pol_neg(v[i]);
}

void polveck_shiftl(polveck v, unsigned int k) { 
  unsigned int i;

  for(i = 0; i < K; ++i)
    pol_shiftl(v[i], k);
}

void polveck_ntt(polveck v)
{
  unsigned int i;

  for(i = 0; i < K; ++i)
    pol_ntt(v[i]);
}
 
void polveck_invntt_montgomery(polveck v) {
  unsigned int i;

  for(i = 0; i < K; ++i)
    pol_invntt_montgomery(v[i]);
}

int polveck_chknorm(const polveck v, uint32_t bound) {
  unsigned int i;
  int ret = 0;

  for(i = 0; i < K; ++i)
    ret |= pol_chknorm(v[i], bound);

  return ret;
}

void polveck_pack(unsigned char *r, const polveck a) {
  unsigned int i;

  for(i = 0; i < K; ++i)
    pol_pack(r + i*POL_SIZE_PACKED, a[i]);
}

void polveck_unpack(polveck r, const unsigned char *a) {
  unsigned int i;

  for(i = 0; i < K; ++i)
    pol_unpack(r[i], a + i*POL_SIZE_PACKED);
}

void polveck_power2round(polveck v1, polveck v0, const polveck v) {
  unsigned int i, j;

  for(i = 0; i < K; ++i)
    for(j = 0; j < N; ++j)
      v1[i][j] = power2round(v[i][j], &v0[i][j]);
}

void polveck_decompose(polveck v1, polveck v0, const polveck v) {
  unsigned int i, j;

  for(i = 0; i < K; ++i)
    for(j = 0; j < N; ++j)
      v1[i][j] = decompose(v[i][j], &v0[i][j]);
}

unsigned int polveck_make_hint(polveck h, const polveck u, const polveck v) {
  unsigned int i, j, s = 0;

  for(i = 0; i < K; ++i)
    for(j = 0; j < N; ++j) {
      h[i][j] = make_hint(u[i][j], v[i][j]);
      s += h[i][j];
    }

  return s;
}

void polveck_use_hint(polveck w, const polveck u, const polveck h) {
  unsigned int i, j;

  for(i = 0; i < K; ++i)
    for(j = 0; j < N; ++j)
      w[i][j] = use_hint(u[i][j], h[i][j]);
}

