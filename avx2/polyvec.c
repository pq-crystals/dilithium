#include <stdint.h>
#include "params.h"
#include "reduce.h"
#include "rounding.h"
#include "poly.h"
#include "polyvec.h"

/**************************************************************/
/************ Vectors of polynomials of length L **************/
/**************************************************************/

void polyvecl_freeze(polyvecl *v) {
  unsigned int i;

  for(i = 0; i < L; ++i)
    poly_freeze(v->vec+i);
}

void polyvecl_add(polyvecl *w, const polyvecl *u, const polyvecl *v) {
  unsigned int i;

  for(i = 0; i < L; ++i)
    poly_add(w->vec+i, u->vec+i, v->vec+i);
}

void polyvecl_ntt(polyvecl *v) {
  unsigned int i;

  for(i = 0; i < L; ++i)
    poly_ntt(v->vec+i);
}

void polyvecl_pointwise_acc_invmontgomery(poly *w,
                                         const polyvecl *u,
                                         const polyvecl *v) 
{
  unsigned int i;
  poly t;

  poly_pointwise_invmontgomery(w, u->vec+0, v->vec+0);

  for(i = 1; i < L; ++i) {
    poly_pointwise_invmontgomery(&t, u->vec+i, v->vec+i);
    poly_add(w, w, &t);
  }

  for(i = 0; i < N; ++i) 
    w->coeffs[i] = reduce32(w->coeffs[i]);
}

int polyvecl_chknorm(const polyvecl *v, uint32_t bound)  {
  unsigned int i;
  int ret = 0;

  for(i = 0; i < L; ++i)
    ret |= poly_chknorm(v->vec+i, bound);

  return ret;
}

/**************************************************************/
/************ Vectors of polynomials of length K **************/
/**************************************************************/

void polyveck_freeze(polyveck *v)  {
  unsigned int i;

  for(i = 0; i < K; ++i)
    poly_freeze(v->vec+i);
}

void polyveck_add(polyveck *w, const polyveck *u, const polyveck *v) {
  unsigned int i;

  for(i = 0; i < K; ++i)
    poly_add(w->vec+i, u->vec+i, v->vec+i);
}

void polyveck_sub(polyveck *w, const polyveck *u, const polyveck *v) {
  unsigned int i;

  for(i = 0; i < K; ++i)
    poly_sub(w->vec+i, u->vec+i, v->vec+i);
}

void polyveck_neg(polyveck *v) { 
  unsigned int i;

  for(i = 0; i < K; ++i)
    poly_neg(v->vec+i);
}

void polyveck_shiftl(polyveck *v, unsigned int k) { 
  unsigned int i;

  for(i = 0; i < K; ++i)
    poly_shiftl(v->vec+i, k);
}

void polyveck_ntt(polyveck *v)
{
  unsigned int i;

  for(i = 0; i < K; ++i)
    poly_ntt(v->vec+i);
}
 
void polyveck_invntt_montgomery(polyveck *v) {
  unsigned int i;

  for(i = 0; i < K; ++i)
    poly_invntt_montgomery(v->vec+i);
}

int polyveck_chknorm(const polyveck *v, uint32_t bound) {
  unsigned int i;
  int ret = 0;

  for(i = 0; i < K; ++i)
    ret |= poly_chknorm(v->vec+i, bound);

  return ret;
}

void polyveck_pack(unsigned char *r, const polyveck *a) {
  unsigned int i;

  for(i = 0; i < K; ++i)
    poly_pack(r + i*POL_SIZE_PACKED, a->vec+i);
}

void polyveck_unpack(polyveck *r, const unsigned char *a) {
  unsigned int i;

  for(i = 0; i < K; ++i)
    poly_unpack(r->vec+i, a + i*POL_SIZE_PACKED);
}

void polyveck_power2round(polyveck *v1, polyveck *v0, const polyveck *v) {
  unsigned int i, j;

  for(i = 0; i < K; ++i)
    for(j = 0; j < N; ++j)
      v1->vec[i].coeffs[j] = power2round(v->vec[i].coeffs[j], &v0->vec[i].coeffs[j]);
}

void polyveck_decompose(polyveck *v1, polyveck *v0, const polyveck *v) {
  unsigned int i, j;

  for(i = 0; i < K; ++i)
    for(j = 0; j < N; ++j)
      v1->vec[i].coeffs[j] = decompose(v->vec[i].coeffs[j], &v0->vec[i].coeffs[j]);
}

unsigned int polyveck_make_hint(polyveck *h, const polyveck *u, const polyveck *v) {
  unsigned int i, j, s = 0;

  for(i = 0; i < K; ++i)
    for(j = 0; j < N; ++j) {
      h->vec[i].coeffs[j] = make_hint(u->vec[i].coeffs[j], v->vec[i].coeffs[j]);
      s += h->vec[i].coeffs[j];
    }

  return s;
}

void polyveck_use_hint(polyveck *w, const polyveck *u, const polyveck *h) {
  unsigned int i, j;

  for(i = 0; i < K; ++i)
    for(j = 0; j < N; ++j)
      w->vec[i].coeffs[j] = use_hint(u->vec[i].coeffs[j], h->vec[i].coeffs[j]);
}

