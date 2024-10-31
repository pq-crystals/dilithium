#ifndef SMALLPOLY_H
#define SMALLPOLY_H
#include "params.h"
#include "poly.h"
#include "polyvec.h"

typedef struct
{
    int16_t coeffs[N];
} smallpoly;

typedef smallpoly smallhalfpoly;

void poly_small_ntt_copy(smallpoly *, poly *);
void poly_small_basemul(int16_t r[N], const int16_t a[N], const int16_t b[N]);

void polyvecl_small_ntt(smallpoly v[L]);
void polyveck_small_ntt(smallpoly v[K]);

void polyvecl_small_basemul_invntt(polyvecl *r, const smallpoly *a, const smallpoly b[L]);
void poly_small_basemul_invntt(poly *r, const smallpoly *a, const smallpoly *b);

void small_polyeta_unpack(smallpoly *r, const uint8_t *a);

#endif
