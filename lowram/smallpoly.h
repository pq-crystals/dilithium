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

#define poly_small_ntt_copy DILITHIUM_NAMESPACE(poly_small_ntt_copy)
void poly_small_ntt_copy(smallpoly *, const poly *);
#define poly_small_basemul DILITHIUM_NAMESPACE(poly_small_basemul)
void poly_small_basemul(int16_t r[N], const int16_t a[N], const int16_t b[N]);

#define polyvecl_small_ntt DILITHIUM_NAMESPACE(polyvecl_small_ntt)
void polyvecl_small_ntt(smallpoly v[L]);
#define polyveck_small_ntt DILITHIUM_NAMESPACE(polyveck_small_ntt)
void polyveck_small_ntt(smallpoly v[K]);

#define polyvecl_small_basemul_invntt DILITHIUM_NAMESPACE(polyvecl_small_basemul_invntt)
void polyvecl_small_basemul_invntt(polyvecl *r, const smallpoly *a, const smallpoly b[L]);
#define poly_small_basemul_invntt DILITHIUM_NAMESPACE(poly_small_basemul_invntt)
void poly_small_basemul_invntt(poly *r, const smallpoly *a, const smallpoly *b);

#define small_polyeta_unpack DILITHIUM_NAMESPACE(small_polyeta_unpack)
void small_polyeta_unpack(smallpoly *r, const uint8_t *a);

#endif
