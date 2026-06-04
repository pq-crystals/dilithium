#ifndef POLYVEC_H
#define POLYVEC_H

#include <stdint.h>
#include "params.h"
#include "poly.h"

/* Vectors of polynomials of length L */
typedef struct {
  poly vec[L];
} polyvecl;

/* Vectors of polynomials of length K */
typedef struct {
  poly vec[K];
} polyveck;

/* Small poly NTT functions moved from poly.h */



#endif
