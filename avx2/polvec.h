#ifndef POLVEC_H
#define POLVEC_H

#include <stdint.h>
#include "params.h"
#include "pol.h"

/* Vectors of polynomials of length L */
typedef pol polvecl[L];

void polvecl_copy(polvecl w, const polvecl v);
void polvecl_freeze(polvecl v);

void polvecl_add(polvecl w, const polvecl u, const polvecl v);

void polvecl_ntt(polvecl v);
void polvecl_pointwise_acc_invmontgomery(pol w, const polvecl u,
                                         const polvecl v);

int polvecl_chknorm(const polvecl v, uint32_t B);

/* Vectors of polynomials of length K */
typedef pol polveck[K];

void polveck_freeze(polveck v);

void polveck_add(polveck w, const polveck u, const polveck v);
void polveck_sub(polveck w, const polveck u, const polveck v);
void polveck_neg(polveck v);
void polveck_shiftl(polveck v, unsigned int k);

void polveck_ntt(polveck v);
void polveck_invntt_montgomery(polveck v);

int polveck_chknorm(const polveck v, uint32_t B);

void polveck_pack(unsigned char *r, const polveck a);
void polveck_unpack(polveck r, const unsigned char *a);

void polveck_power2round(polveck v1, polveck v0, const polveck v);
void polveck_decompose(polveck v1, polveck v0, const polveck v);
unsigned int polveck_make_hint(polveck h, const polveck u, const polveck v);
void polveck_use_hint(polveck w, const polveck v, const polveck h);

#endif
