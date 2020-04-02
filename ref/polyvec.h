#ifndef POLYVEC_H
#define POLYVEC_H

#include <stdint.h>
#include "params.h"
#include "poly.h"

/* Vectors of polynomials of length L */
typedef struct {
  poly vec[L];
} polyvecl;

#define expand_mat NAMESPACE(expand_mat)
void expand_mat(polyvecl mat[K], const uint8_t rho[SEEDBYTES]);

#define polyvecl_freeze NAMESPACE(polyvecl_freeze)
void polyvecl_freeze(polyvecl *v);

#define polyvecl_add NAMESPACE(polyvecl_add)
void polyvecl_add(polyvecl *w, const polyvecl *u, const polyvecl *v);

#define polyvecl_ntt NAMESPACE(polyvecl_ntt)
void polyvecl_ntt(polyvecl *v);
#define polyvecl_pointwise_acc_montgomery \
        NAMESPACE(polyvecl_pointwise_acc_montgomery)
void polyvecl_pointwise_acc_montgomery(poly *w,
                                       const polyvecl *u,
                                       const polyvecl *v);


#define polyvecl_chknorm NAMESPACE(polyvecl_chknorm)
int polyvecl_chknorm(const polyvecl *v, uint32_t B);



/* Vectors of polynomials of length K */
typedef struct {
  poly vec[K];
} polyveck;

#define polyveck_reduce NAMESPACE(polyveck_reduce)
void polyveck_reduce(polyveck *v);
#define polyveck_csubq NAMESPACE(polyveck_csubq)
void polyveck_csubq(polyveck *v);
#define polyveck_freeze NAMESPACE(polyveck_freeze)
void polyveck_freeze(polyveck *v);

#define polyveck_add NAMESPACE(polyveck_add)
void polyveck_add(polyveck *w, const polyveck *u, const polyveck *v);
#define polyveck_sub NAMESPACE(polyveck_sub)
void polyveck_sub(polyveck *w, const polyveck *u, const polyveck *v);
#define polyveck_shiftl NAMESPACE(polyveck_shiftl)
void polyveck_shiftl(polyveck *v);

#define polyveck_ntt NAMESPACE(polyveck_ntt)
void polyveck_ntt(polyveck *v);
#define polyveck_invntt_tomont NAMESPACE(polyveck_invntt_tomont)
void polyveck_invntt_tomont(polyveck *v);

#define polyveck_chknorm NAMESPACE(polyveck_chknorm)
int polyveck_chknorm(const polyveck *v, uint32_t B);

#define polyveck_power2round NAMESPACE(polyveck_power2round)
void polyveck_power2round(polyveck *v1, polyveck *v0, const polyveck *v);
#define polyveck_decompose NAMESPACE(polyveck_decompose)
void polyveck_decompose(polyveck *v1, polyveck *v0, const polyveck *v);
#define polyveck_make_hint NAMESPACE(polyveck_make_hint)
unsigned int polyveck_make_hint(polyveck *h,
                                const polyveck *v0,
                                const polyveck *v1);
#define polyveck_use_hint NAMESPACE(polyveck_use_hint)
void polyveck_use_hint(polyveck *w, const polyveck *v, const polyveck *h);

#endif
