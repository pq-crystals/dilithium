#include "lowram.h"
#include "fips202.h"
#include "symmetric.h"
#include "reduce.h"
#include "rounding.h"

/*
This file implements functions aiding with the reduction of the memory
footprint of ML-DSA. 

The ideas are taken from the paper:

Joppe W. Bos, Joost Renes, and Amber Sprenkels. 2022. Dilithium for Memory
Constrained Devices. In Progress in Cryptology - AFRICACRYPT 2022: 13th
International Conference on Cryptology in Africa, AFRICACRYPT 2022, Fes,
Morocco, July 18–20, 2022, Proceedings. Springer-Verlag, Berlin, Heidelberg,
217–235. https://doi.org/10.1007/978-3-031-17433-9_10
*/

/*************************************************
 * Name:        unpack_pk_t1
 *
 * Description: Unpack only t1 from pk.
 *
 * Arguments:   - poly *t1: pointer to output t1
 *              - const unsigned int idx: unpack n'th element from t1
 *              - uint8_t pk[]: byte array containing bit-packed pk
 **************************************************/
void unpack_pk_t1(poly *t1, unsigned int idx, const uint8_t pk[CRYPTO_PUBLICKEYBYTES]) {
  pk += SEEDBYTES;
  polyt1_unpack(t1, pk + idx * POLYT1_PACKEDBYTES);
}

/*************************************************
* Name:        pack_sig_c
*
* Description: Pack only c into signature.
*
* Arguments:   - uint8_t sig[]: byte array containing bit-packed signature
*              - const uint8_t c: challenge
**************************************************/
void pack_sig_c(uint8_t sig[CRYPTO_BYTES],
                const uint8_t c[CTILDEBYTES]) {
  unsigned int i;

  for(i = 0; i < CTILDEBYTES; ++i)
    sig[i] = c[i];
  sig += CTILDEBYTES;
}

/*************************************************
* Name:        pack_sig_z
*
* Description: Pack only z into signature.
*
* Arguments:   - uint8_t sig[]: byte array containing bit-packed signature
*              - const polyvecl *z: z vector
**************************************************/
void pack_sig_z(uint8_t sig[CRYPTO_BYTES],
                const polyvecl *z) {
  unsigned int i;
  sig += CTILDEBYTES;
  for(i = 0; i < L; ++i)
    polyz_pack(sig + i * POLYZ_PACKEDBYTES, &z->vec[i]);
}

/*************************************************
* Name:        pack_sig_h
*
* Description: Pack only h into signature.
*
* Arguments:   - uint8_t sig[]: byte array containing bit-packed signature
*              - const poly *h_elem: element of h
*              - const unsigned int idx: index of h in vector
*              - unsigned int *hints_written: number of hints already written
**************************************************/
void pack_sig_h(uint8_t sig[CRYPTO_BYTES],
                const poly *h_elem,
                const unsigned int idx,
                unsigned int *hints_written) {
  unsigned int j;
  
  sig += CTILDEBYTES;
  sig += L * POLYZ_PACKEDBYTES;

  // Encode h
  for(j = 0; j < N; j++) {
    if(h_elem->coeffs[j] != 0) {
      sig[*hints_written] = (uint8_t)j;
      (*hints_written)++;
    }
  }
  sig[OMEGA + idx] = (uint8_t)*hints_written;
}

/*************************************************
* Name:        pack_sig_h_zero
*
* Description: Pack only remaining zeros into signature.
*
* Arguments:   - uint8_t sig[]: byte array containing bit-packed signature
*              - unsigned int *hints_written: number of hints written
**************************************************/
void pack_sig_h_zero(uint8_t sig[CRYPTO_BYTES],
                     unsigned int *hints_written) {
  sig += CTILDEBYTES;
  sig += L * POLYZ_PACKEDBYTES;
  while(*hints_written < OMEGA) {
    sig[*hints_written] = 0;
    (*hints_written)++;
  }
}

/*************************************************
 * Name:        unpack_sig_c
 *
 * Description: Unpack only c from signature sig = (z, h, c).
 *
 * Arguments:   - poly *c: pointer to output challenge polynomial
 *              - const uint8_t sig[]: byte array containing
 *                bit-packed signature
 *
 * Returns 1 in case of malformed signature; otherwise 0.
 **************************************************/
int unpack_sig_c(uint8_t c[CTILDEBYTES], const uint8_t sig[CRYPTO_BYTES]) {
  unsigned int i;
  
  for(i = 0; i < CTILDEBYTES; ++i)
    c[i] = sig[i];
  sig += CTILDEBYTES;
  return 0;
}

/*************************************************
 * Name:        unpack_sig_z
 *
 * Description: Unpack only z from signature sig = (z, h, c).
 *
 * Arguments:   - polyvecl *z: pointer to output vector z
 *              - const uint8_t sig[]: byte array containing
 *                bit-packed signature
 *
 * Returns 1 in case of malformed signature; otherwise 0.
 **************************************************/
int unpack_sig_z(polyvecl *z, const uint8_t sig[CRYPTO_BYTES]) {
  unsigned int i;
  
  sig += CTILDEBYTES;
  for(i = 0; i < L; ++i) {
    polyz_unpack(&z->vec[i], sig + i * POLYZ_PACKEDBYTES);
  }
  return 0;
}

/*************************************************
 * Name:        unpack_sig_h
 *
 * Description: Unpack only h from signature sig = (z, h, c).
 *
 * Arguments:   - polyveck *h: pointer to output hint vector h
 *              - const uint8_t sig[]: byte array containing
 *                bit-packed signature
 *
 * Returns 1 in case of malformed signature; otherwise 0.
 **************************************************/
int unpack_sig_h(poly *h, unsigned int idx, const uint8_t sig[CRYPTO_BYTES]) {
  unsigned int i, j, k;
  
  sig += CTILDEBYTES;
  sig += L * POLYZ_PACKEDBYTES;

  /* Decode h */
  k = 0;
  for(i = 0; i < K; ++i) {
    for(j = 0; j < N; ++j) {
      if(i == idx) {
        h->coeffs[j] = 0;
      }
    }

    if(sig[OMEGA + i] < k || sig[OMEGA + i] > OMEGA) {
      return 1;
    }

    for(j = k; j < sig[OMEGA + i]; ++j) {
      /* Coefficients are ordered for strong unforgeability */
      if(j > k && sig[j] <= sig[j - 1]) {
        return 1;
      }
      if(i == idx) {
        h->coeffs[sig[j]] = 1;
      }
    }

    k = sig[OMEGA + i];
  }

  /* Extra indices are zero for strong unforgeability */
  for(j = k; j < OMEGA; ++j) {
    if(sig[j]) {
      return 1;
    }
  }
  return 0;
}

/*************************************************
 * Name:        poly_challenge_compress
 *
 * Description: Compress the challenge polynomial.
 *
 * Arguments:   - uint8_t c[]: byte array for holding the compressed challenge
 *              - const poly *cp: challenge polynomnial
 *
 **************************************************/
void poly_challenge_compress(uint8_t c[68], const poly *cp) {
  unsigned int i, pos;
  uint64_t signs;
  uint64_t mask;
  /* Encode c */
  for(i = 0; i < 68; i++)
    c[i] = 0;
  signs = 0;
  mask = 1;
  pos = 0;
  for(i = 0; i < N; ++i) {
    if(cp->coeffs[i] != 0) {
      c[pos++] = i;
      if(cp->coeffs[i] == -1) {
        signs |= mask;
      }
      mask <<= 1;
    }
  }

  for(i = 0; i < 8; ++i) {
    c[60 + i] = (uint8_t)(signs >> 8 * i);
  }
}

/*************************************************
 * Name:        poly_challenge_decompress
 *
 * Description: Decompress the challenge polynomial.
 *
 * Arguments:   - poly *cp: challenge polynomnial output
 *              - uint8_t c[]: byte array holding the compressed challenge
 *
 **************************************************/
void poly_challenge_decompress(poly *cp, const uint8_t c[68]) {
  unsigned int i;
  unsigned pos;
  uint64_t signs = 0;
  for(i = 0; i < N; i++)
    cp->coeffs[i] = 0;
  for(i = 0; i < 8; i++) {
    signs |= ((uint64_t)c[60 + i]) << (8 * i);
  }

  for(i = 0; i < TAU; i++) {
    pos = c[i];
    if(signs & 1) {
      cp->coeffs[pos] = -1;
    }
    else {
      cp->coeffs[pos] = 1;
    }
    signs >>= 1;
  }
}

/*************************************************
 * Name:        polyt0_unpack_idx
 *
 * Description: Unpack coefficient from t0 at specific index.
 *
 * Arguments:   - const uint8_t *t0: packed t0
 *              - unsigned idx: index of coefficient
 *
 **************************************************/
static inline int32_t polyt0_unpack_idx(const uint8_t *t0, unsigned idx) {
  int32_t coeff;
  // 8 coefficients are packed in 13 bytes
  t0 += 13 * (idx >> 3);

  if(idx % 8 == 0) {
    coeff = t0[0];
    coeff |= (uint32_t)t0[1] << 8;
  }
  else if(idx % 8 == 1) {
    coeff = t0[1] >> 5;
    coeff |= (uint32_t)t0[2] << 3;
    coeff |= (uint32_t)t0[3] << 11;
  }
  else if(idx % 8 == 2) {
    coeff = t0[3] >> 2;
    coeff |= (uint32_t)t0[4] << 6;
  }
  else if(idx % 8 == 3) {
    coeff = t0[4] >> 7;
    coeff |= (uint32_t)t0[5] << 1;
    coeff |= (uint32_t)t0[6] << 9;
  }
  else if(idx % 8 == 4) {
    coeff = t0[6] >> 4;
    coeff |= (uint32_t)t0[7] << 4;
    coeff |= (uint32_t)t0[8] << 12;
  }
  else if(idx % 8 == 5) {
    coeff = t0[8] >> 1;
    coeff |= (uint32_t)t0[9] << 7;
  }
  else if(idx % 8 == 6) {
    coeff = t0[9] >> 6;
    coeff |= (uint32_t)t0[10] << 2;
    coeff |= (uint32_t)t0[11] << 10;
  }
  else { // (idx % 8 == 7)
    coeff = t0[11] >> 3;
    coeff |= (uint32_t)t0[12] << 5;
  }
  coeff &= 0x1FFF;
  return (1 << (D - 1)) - coeff;
}

/*************************************************
 * Name:        polyt1_unpack_idx
 *
 * Description: Unpack coefficient from t1 at specific index.
 *
 * Arguments:   - const uint8_t *t1: packed t1
 *              - unsigned idx: index of coefficient
 *
 **************************************************/
static inline int32_t polyt1_unpack_idx(const uint8_t *t1, unsigned idx) {
  int32_t coeff;
  // 4 coefficients are packed in 5 bytes
  t1 += 5 * (idx >> 2);

  if(idx % 4 == 0) {
    coeff = (t1[0] >> 0);
    coeff |= ((uint32_t)t1[1] << 8);
  }
  else if(idx % 4 == 1) {
    coeff = (t1[1] >> 2);
    coeff |= ((uint32_t)t1[2] << 6);
  }
  else if(idx % 4 == 2) {
    coeff = (t1[2] >> 4);
    coeff |= ((uint32_t)t1[3] << 4);
  }
  else { // (idx % 4 == 3)
    coeff = (t1[3] >> 6);
    coeff |= ((uint32_t)t1[4] << 2);
  }
  coeff &= 0x3FF;
  return coeff;
}

/*************************************************
 * Name:        poly_schoolbook
 *
 * Description: Schoolbook multiplication between challenge and t0.
 *
 * Arguments:   - poly *c: Output polynomial
 *              - const uint8_t ccomp[]: First input, compressed challenge
 *              - const uint8_t *t0: Second input, packed t0
 *
 **************************************************/
void poly_schoolbook(poly *c, const uint8_t ccomp[68], const uint8_t *t0) {
  unsigned i, j, idx;
  uint64_t signs = 0;
  for(i = 0; i < N; i++)
    c->coeffs[i] = 0;
  for(i = 0; i < 8; i++) {
    signs |= ((uint64_t)ccomp[60 + i]) << (8 * i);
  }

  for(idx = 0; idx < TAU; idx++) {
    i = ccomp[idx];
    if(!(signs & 1)) {
      for(j = 0; i + j < N; j++) {
        c->coeffs[i + j] += polyt0_unpack_idx(t0, j);
      }
      for(j = N - i; j < N; j++) {
        c->coeffs[i + j - N] -= polyt0_unpack_idx(t0, j);
      }
    }
    else {
      for(j = 0; i + j < N; j++) {
        c->coeffs[i + j] -= polyt0_unpack_idx(t0, j);
      }
      for(j = N - i; j < N; j++) {
        c->coeffs[i + j - N] += polyt0_unpack_idx(t0, j);
      }
    }

    signs >>= 1;
  }
}

/*************************************************
 * Name:        poly_schoolbook_t1
 *
 * Description: Schoolbook multiplication between challenge and t1.
 *
 * Arguments:   - poly *c: Output polynomial
 *              - const uint8_t ccomp[]: First input, compressed challenge
 *              - const uint8_t *t1: Second input, packed t1
 *
 **************************************************/
void poly_schoolbook_t1(poly *c, const uint8_t ccomp[68], const uint8_t *t1) {
  unsigned i, j, idx;
  uint64_t signs = 0;
  for(i = 0; i < N; i++)
    c->coeffs[i] = 0;
  for(i = 0; i < 8; i++) {
    signs |= ((uint64_t)ccomp[60 + i]) << (8 * i);
  }

  for(idx = 0; idx < TAU; idx++) {
    i = ccomp[idx];
    if(!(signs & 1)) {
      for(j = 0; i + j < N; j++) {
        c->coeffs[i + j] += (polyt1_unpack_idx(t1, j) << D);
      }
      for(j = N - i; j < N; j++) {
        c->coeffs[i + j - N] -= (polyt1_unpack_idx(t1, j) << D);
      }
    }
    else {
      for(j = 0; i + j < N; j++) {
        c->coeffs[i + j] -= (polyt1_unpack_idx(t1, j) << D);
      }
      for(j = N - i; j < N; j++) {
        c->coeffs[i + j - N] += (polyt1_unpack_idx(t1, j) << D);
      }
    }

    signs >>= 1;
  }
}

/*************************************************
 * Name:        polyw_pack
 *
 * Description: Pack polynomial w.
 *
 * Arguments:   - uint8_t buf[]: buffer to hold compressed w
 *              - poly *w: input polynomial
 *
 **************************************************/
void polyw_pack(uint8_t buf[K * 768], poly *w) {
  unsigned int i;
  
  poly_reduce(w);
  poly_caddq(w);
  for(i = 0; i < N; i++) {
    buf[i * 3 + 0] = w->coeffs[i];
    buf[i * 3 + 1] = w->coeffs[i] >> 8;
    buf[i * 3 + 2] = w->coeffs[i] >> 16;
  }
}

/*************************************************
 * Name:        polyw_unpack
 *
 * Description: Unpack polynomial w.
 *
 * Arguments:   - poly *w: output polynomial
 *              - const uint8_t buf[]: buffer holding compressed w
 *
 **************************************************/
void polyw_unpack(poly *w, const uint8_t buf[K * 768]) {
  unsigned int i;
  for(i = 0; i < N; i++) {
    w->coeffs[i] = buf[i * 3 + 0];
    w->coeffs[i] |= (int32_t)buf[i * 3 + 1] << 8;
    w->coeffs[i] |= (int32_t)buf[i * 3 + 2] << 16;
  }
}

/*************************************************
 * Name:        polyw_add_idx
 *
 * Description: Add an integer to a coefficient in a compressed polynomial buffer.
 *
 * Arguments:   - uint8_t buf[]: buffer holding compressed polynomial coefficients
 *              - int32_t a: integer to add to the coefficient
 *              - unsigned int i: index of the coefficient to modify
 *
 **************************************************/
static void polyw_add_idx(uint8_t buf[K * 768], int32_t a, unsigned int i) {
  int32_t coeff;
  coeff = buf[i * 3 + 0];
  coeff |= (int32_t)buf[i * 3 + 1] << 8;
  coeff |= (int32_t)buf[i * 3 + 2] << 16;

  coeff += a;

  coeff = freeze(coeff);

  buf[i * 3 + 0] = coeff;
  buf[i * 3 + 1] = coeff >> 8;
  buf[i * 3 + 2] = coeff >> 16;
}

/*************************************************
 * Name:        polyw_sub
 *
 * Description: Subtract the coefficients of a polynomial from a compressed
                polynomial buffer and store the result in another polynomial.
 *
 * Arguments:   - poly *c: output polynomial to store the result
 *              - uint8_t buf[]: buffer holding compressed polynomial coefficients
 *              - poly *a: polynomial whose coefficients are to be subtracted
 *                from the buffer
 *
 **************************************************/
void polyw_sub(poly *c, const uint8_t buf[3 * 256], const poly *a) {
  unsigned int i;
  int32_t coeff;

  for(i = 0; i < N; i++) {
    coeff = buf[i * 3 + 0];
    coeff |= (int32_t)buf[i * 3 + 1] << 8;
    coeff |= (int32_t)buf[i * 3 + 2] << 16;

    c->coeffs[i] = coeff - a->coeffs[i];
  }
}

/*************************************************
 * Name:        highbits
 *
 * Description: Compute the high bits of an integer.
 *
 * Arguments:   - int32_t a: input integer whose high bits are to be computed
 *
 * Returns the high bits of the input as the result.
 **************************************************/
static int32_t highbits(int32_t a) {
  int32_t a1;

  a1 = (a + 127) >> 7;
#if GAMMA2 == (Q - 1) / 32
  a1 = (a1 * 1025 + (1 << 21)) >> 22;
  a1 &= 15;
#elif GAMMA2 == (Q - 1) / 88
  a1 = (a1 * 11275 + (1 << 23)) >> 24;
  a1 ^= ((43 - a1) >> 31) & a1;
#endif

  return a1;
}

/*************************************************
 * Name:        poly_highbits
 *
 * Description: Compute the high bits of each coefficient in a polynomial.
 *
 * Arguments:   - poly *a1: output polynomial to store the high bits of the coefficients
 *              - const poly *a: input polynomial whose coefficients' high bits
 *                are to be computed
 *
 **************************************************/
void poly_highbits(poly *a1, const poly *a) {
  unsigned int i;

  for(i = 0; i < N; ++i)
    a1->coeffs[i] = highbits(a->coeffs[i]);
}

/*************************************************
 * Name:        lowbits
 *
 * Description: Compute the low bits of an integer.
 *
 * Arguments:   - int32_t a: input integer whose low bits are to be computed
 *
 * Returns the low bits of the input as the result.
 **************************************************/
static int32_t lowbits(int32_t a) {
  int32_t a1;
  int32_t a0;

  a1 = (a + 127) >> 7;
#if GAMMA2 == (Q - 1) / 32
  a1 = (a1 * 1025 + (1 << 21)) >> 22;
  a1 &= 15;
#elif GAMMA2 == (Q - 1) / 88
  a1 = (a1 * 11275 + (1 << 23)) >> 24;
  a1 ^= ((43 - a1) >> 31) & a1;
#endif

  a0 = a - a1 * 2 * GAMMA2;
  a0 -= (((Q - 1) / 2 - a0) >> 31) & Q;
  return a0;
}

/*************************************************
 * Name:        poly_lowbits
 *
 * Description: Compute the low bits of each coefficient in a polynomial.
 *
 * Arguments:   - poly *a0: output polynomial to store the low bits of the coefficients
 *              - const poly *a: input polynomial whose coefficients' low bits
                  are to be computed
 *
 **************************************************/
void poly_lowbits(poly *a0, const poly *a) {
  unsigned int i;

  for(i = 0; i < N; ++i)
    a0->coeffs[i] = lowbits(a->coeffs[i]);
}

/*************************************************
 * Name:        unpack_sk_s1
 *
 * Description: Unpack only s1 from the secret key into a small polynomial.
 *
 * Arguments:   - smallpoly *a: output small polynomial to store the unpacked data
 *              - const uint8_t *sk: input secret key buffer
 *              - unsigned int idx: index specifying the polynomial to unpack
 *
 **************************************************/
void unpack_sk_s1(smallpoly *a, const uint8_t *sk, unsigned int idx) {
  small_polyeta_unpack(a, sk + 2 * SEEDBYTES + TRBYTES + idx * POLYETA_PACKEDBYTES);
}

/*************************************************
 * Name:        unpack_sk_s2
 *
 * Description: Unpack only s2 from the secret key into a small polynomial.
 *
 * Arguments:   - smallpoly *a: output small polynomial to store the unpacked data
 *              - const uint8_t *sk: input secret key buffer
 *              - unsigned int idx: index specifying the polynomial to unpack
 *
 **************************************************/
void unpack_sk_s2(smallpoly *a, const uint8_t *sk, unsigned int idx) {
  small_polyeta_unpack(a, sk + 2 * SEEDBYTES + TRBYTES + L * POLYETA_PACKEDBYTES + idx * POLYETA_PACKEDBYTES);
}

/* Note: Buffer size can potentially be increased */
#define POLY_UNIFORM_BUFFERSIZE 3
/*************************************************
 * Name:        poly_uniform_pointwise_montgomery_polywadd_lowram
 *
 * Description: Generate a uniform polynomial using a seed and nonce, 
 *              perform pointwise multiplication with another polynomial, 
 *              and add the result to a compressed polynomial buffer.
 *
 * Arguments:   - uint8_t wcomp[]: buffer to store the compressed polynomial
 *                coefficients
 *              - poly *b: input polynomial for pointwise multiplication
 *              - const uint8_t seed[]: seed for SHAKE128
 *              - uint16_t nonce: nonce for SHAKE128
 *              - keccak_state *state: state for the SHAKE128
 *
 **************************************************/
void poly_uniform_pointwise_montgomery_polywadd_lowram(uint8_t wcomp[3 * N], const poly *b, const uint8_t seed[SEEDBYTES], uint16_t nonce, keccak_state *state) {
  int32_t t;
  uint8_t buf[POLY_UNIFORM_BUFFERSIZE * 3];
  unsigned int ctr, pos;
  {
    ctr = 0;
    stream128_init(state, seed, nonce);

    do {
      shake128_squeeze(buf, sizeof buf, state);

      for(pos = 0; pos < sizeof buf && ctr < N; pos += 3) {
        t = buf[pos];
        t |= (uint32_t)buf[pos + 1] << 8;
        t |= (uint32_t)buf[pos + 2] << 16;
        t &= 0x7FFFFF;

        if(t < Q) {
          t = montgomery_reduce((int64_t)t * b->coeffs[ctr]);
          polyw_add_idx(wcomp, t, ctr);
          ctr++;
        }
      }
    } while(ctr < N);
  }
}

#define POLY_UNIFORM_GAMMA1_BUFFERSIZE 1
#if GAMMA1 == (1 << 17)
#define POLY_UNIFORM_GAMMA1_BUFFERSIZE_COEFFS (POLY_UNIFORM_GAMMA1_BUFFERSIZE * 4)
#define POLY_UNIFORM_GAMMA1_BUFFERSIZE_BYTES (POLY_UNIFORM_GAMMA1_BUFFERSIZE * 9)
#elif GAMMA1 == (1 << 19)
#define POLY_UNIFORM_GAMMA1_BUFFERSIZE_COEFFS (POLY_UNIFORM_GAMMA1_BUFFERSIZE * 2)
#define POLY_UNIFORM_GAMMA1_BUFFERSIZE_BYTES (POLY_UNIFORM_GAMMA1_BUFFERSIZE * 5)
#endif

/*************************************************
 * Name:        polyz_unpack_inplace
 *
 * Description: Unpack a compressed polynomial z in place.
 *
 * Arguments:   - int32_t *r: pointer to the array where the unpacked polynomial
 *                            coefficients will be stored which is also used as
 *                            the input
 *
 **************************************************/
static void polyz_unpack_inplace(int32_t *r) {
  uint8_t *a = (uint8_t *)r;

  unsigned int i, j;
#if GAMMA1 == (1 << 17)
  int32_t t0;
  for(j = 0; j < POLY_UNIFORM_GAMMA1_BUFFERSIZE; ++j) {
    i = POLY_UNIFORM_GAMMA1_BUFFERSIZE - 1 - j;

    r[4 * i + 3] = a[9 * i + 6] >> 6;
    r[4 * i + 3] |= (uint32_t)a[9 * i + 7] << 2;
    r[4 * i + 3] |= (uint32_t)a[9 * i + 8] << 10;
    r[4 * i + 3] &= 0x3FFFF;

    r[4 * i + 2] = a[9 * i + 4] >> 4;
    r[4 * i + 2] |= (uint32_t)a[9 * i + 5] << 4;
    r[4 * i + 2] |= (uint32_t)a[9 * i + 6] << 12;
    r[4 * i + 2] &= 0x3FFFF;

    r[4 * i + 1] = (uint32_t)a[9 * i + 4] << 14;
    r[4 * i + 1] |= a[9 * i + 2] >> 2;
    r[4 * i + 1] |= (uint32_t)a[9 * i + 3] << 6;
    r[4 * i + 1] &= 0x3FFFF;

    t0 = a[9 * i + 0];
    t0 |= (uint32_t)a[9 * i + 1] << 8;
    t0 |= (uint32_t)a[9 * i + 2] << 16;
    t0 &= 0x3FFFF;

    r[4 * i + 0] = GAMMA1 - t0;
    r[4 * i + 1] = GAMMA1 - r[4 * i + 1];
    r[4 * i + 2] = GAMMA1 - r[4 * i + 2];
    r[4 * i + 3] = GAMMA1 - r[4 * i + 3];
  }
#elif GAMMA1 == (1 << 19)
  int32_t tmp0, tmp1;
  for(j = 0; j < POLY_UNIFORM_GAMMA1_BUFFERSIZE; ++j) {
    i = POLY_UNIFORM_GAMMA1_BUFFERSIZE - 1 - j;

    tmp0 = a[5 * i + 2] >> 4;
    tmp0 |= (uint32_t)a[5 * i + 3] << 4;
    tmp0 |= (uint32_t)a[5 * i + 4] << 12;
    tmp0 &= 0xFFFFF;

    tmp1 = a[5 * i + 0];
    tmp1 |= (uint32_t)a[5 * i + 1] << 8;
    tmp1 |= (uint32_t)a[5 * i + 2] << 16;
    tmp1 &= 0xFFFFF;

    r[2 * i + 0] = GAMMA1 - tmp1;
    r[2 * i + 1] = GAMMA1 - tmp0;
  }
#endif
}

/*************************************************
 * Name:        poly_uniform_gamma1_lowram
 *
 * Description: Generate a uniform polynomial with coefficients in the range [-GAMMA1, GAMMA1].
 *
 * Arguments:   - poly *a: output polynomial to store the generated coefficients
 *              - const uint8_t seed[]: seed for SHAKE256
 *              - uint16_t nonce: nonce for SHAKE256
 *              - keccak_state *state: state for SHAKE256
 *
 **************************************************/
void poly_uniform_gamma1_lowram(poly *a, const uint8_t seed[CRHBYTES], uint16_t nonce, keccak_state *state) {
  unsigned int i, j;
  int32_t buf[POLY_UNIFORM_GAMMA1_BUFFERSIZE_COEFFS];

  stream256_init(state, seed, nonce);
  for(i = 0; i < N / POLY_UNIFORM_GAMMA1_BUFFERSIZE_COEFFS; i++) {
    shake256_squeeze((uint8_t *)buf, POLY_UNIFORM_GAMMA1_BUFFERSIZE_BYTES, state);
    polyz_unpack_inplace(buf);

    for(j = 0; j < POLY_UNIFORM_GAMMA1_BUFFERSIZE_COEFFS; j++) {
      a->coeffs[i * POLY_UNIFORM_GAMMA1_BUFFERSIZE_COEFFS + j] = buf[j];
    }
  }
}

/*************************************************
 * Name:        poly_uniform_gamma1_add_lowram
 *
 * Description: Generate a uniform polynomial with coefficients in the range [-GAMMA1, GAMMA1],
 *              and add it to another polynomial.
 *
 * Arguments:   - poly *a: output polynomial to store the result
 *              - poly *b: input polynomial whose coefficients are to be added
 *              - const uint8_t seed[]: seed for SHAKE256
 *              - uint16_t nonce: nonce for SHAKE256
 *              - keccak_state *state: state for SHAKE256
 *
 **************************************************/
void poly_uniform_gamma1_add_lowram(poly *a, const poly *b, const uint8_t seed[CRHBYTES], uint16_t nonce, keccak_state *state) {
  unsigned int i, j;
  int32_t buf[POLY_UNIFORM_GAMMA1_BUFFERSIZE_COEFFS];

  stream256_init(state, seed, nonce);
  for(i = 0; i < N / POLY_UNIFORM_GAMMA1_BUFFERSIZE_COEFFS; i++) {
    shake256_squeeze((uint8_t *)buf, POLY_UNIFORM_GAMMA1_BUFFERSIZE_BYTES, state);
    polyz_unpack_inplace(buf);

    for(j = 0; j < POLY_UNIFORM_GAMMA1_BUFFERSIZE_COEFFS; j++) {
      a->coeffs[i * POLY_UNIFORM_GAMMA1_BUFFERSIZE_COEFFS + j] = buf[j] + b->coeffs[i * POLY_UNIFORM_GAMMA1_BUFFERSIZE_COEFFS + j];
    }
  }
}

/*************************************************
* Name:        make_hint
*
* Description: Compute hint bit indicating whether the low bits of the
*              input element overflow into the high bits.
*
* Arguments:   - int32_t a0: low bits of input element
*              - int32_t a1: high bits of input element
*
* Returns 1 if overflow.
**************************************************/
static inline int32_t make_hint_lowram(int32_t z, int32_t r) {
  int32_t r1, v1;

  r1 = highbits(r);
  v1 = highbits(r + z);

  if(r1 != v1)
    return 1;
  return 0;
}

/*************************************************
 * Name:        poly_make_hint_lowram
 *
 * Description: Generate hint polynomial.
 *
 * Arguments:   - poly *a: output polynomial to store the generated hints
 *              - poly *t: input polynomial
 *              - uint8_t w[]: buffer holding compressed polynomial coefficients
 *
 * Returns the number of hints generated.
 **************************************************/
unsigned int poly_make_hint_lowram(poly *a, const poly *t, const uint8_t w[768]) {
  unsigned int i;
  int32_t coeff;
  unsigned int hints_n = 0;
  for(i = 0; i < N; i++) {
    // unpack coeff from w (contains w - cs2)
    coeff = w[i * 3 + 0];
    coeff |= (int32_t)w[i * 3 + 1] << 8;
    coeff |= (int32_t)w[i * 3 + 2] << 16;

    // compute w - cs2 + c*t0
    coeff = coeff + t->coeffs[i];

    a->coeffs[i] = make_hint_lowram(-t->coeffs[i], coeff);
    if(a->coeffs[i] == 1) {
      hints_n++;
    }
  }
  return hints_n;
}

/*************************************************
 * Name:        unpack_sig_h_indices
 *
 * Description: Unpack only h from signature sig = (c, z, h).
 *
 * Arguments:   - polyveck *h: pointer to output hint vector h
 *              - const uint8_t sig[]: byte array containing
 *                bit-packed signature
 *
 * Returns 1 in case of malformed signature; otherwise 0.
 **************************************************/
int unpack_sig_h_indices(uint8_t h_i[OMEGA], unsigned int *number_of_hints, unsigned int idx, const uint8_t sig[CRYPTO_BYTES]) {
  unsigned int j, k, hidx;
  
  sig += L * POLYZ_PACKEDBYTES;
  sig += CTILDEBYTES;
  /* Decode h */
  k = 0;
  hidx = 0;

  if(idx > 0) {
    k = sig[OMEGA + (idx - 1)];
  }

  if(sig[OMEGA + idx] < k || sig[OMEGA + idx] > OMEGA) {
    return 1;
  }

  for(j = k; j < sig[OMEGA + idx]; ++j) {
    /* Coefficients are ordered for strong unforgeability */
    if(j > k && sig[j] <= sig[j - 1]) {
      return 1;
    }
    h_i[hidx++] = sig[j];
  }

  *number_of_hints = hidx;

  k = sig[OMEGA + (K - 1)];
  /* Extra indices are zero for strong unforgeability */
  for(j = k; j < OMEGA; ++j) {
    if(sig[j]) {
      return 1;
    }
  }
  return 0;
}

/*************************************************
 * Name:        poly_use_hint_lowram
 *
 * Description: Use hint polynomial to correct the high bits of a polynomial.
 *
 * Arguments:   - poly *b: pointer to output polynomial with corrected high bits
 *              - const poly *a: pointer to input polynomial
 *              - const poly *h: pointer to input hint polynomial
 **************************************************/
void poly_use_hint_lowram(poly *b, const poly *a, const uint8_t h_i[OMEGA], unsigned int number_of_hints) {
  unsigned int i;
  unsigned int in_list;
  unsigned int hidx;

  for(i = 0; i < N; ++i) {
    in_list = 0;
    for(hidx = 0; hidx < number_of_hints; hidx++) {
      if(i == h_i[hidx]) {
        in_list = 1;
        break;
      }
    }
    if(in_list) {
      b->coeffs[i] = use_hint(a->coeffs[i], 1);
    }
    else {
      b->coeffs[i] = use_hint(a->coeffs[i], 0);
    }
  }
}

/*************************************************
 * Name:        pack_pk_rho
 *
 * Description: Bit-pack only rho in public key pk = (rho, t1).
 *
 * Arguments:   - uint8_t pk[]: output byte array
 *              - const uint8_t rho[]: byte array containing rho
 **************************************************/
void pack_pk_rho(uint8_t pk[CRYPTO_PUBLICKEYBYTES],
                 const uint8_t rho[SEEDBYTES]) {
  unsigned int i;
  
  for(i = 0; i < SEEDBYTES; ++i) {
    pk[i] = rho[i];
  }
}

/*************************************************
 * Name:        pack_pk_t1
 *
 * Description: Bit-pack only the t1 elem at idx in public key pk = (rho, t1).
 *
 * Arguments:   - uint8_t pk[]: output byte array
 *              - const polyveck *t1: pointer to vector t1
 *              - const unsigned int idx: index to the elem to pack
 **************************************************/
void pack_pk_t1(uint8_t pk[CRYPTO_PUBLICKEYBYTES],
                const poly *t1,
                const unsigned int idx) {
  pk += SEEDBYTES;
  polyt1_pack(pk + idx * POLYT1_PACKEDBYTES, t1);
}

/*************************************************
 * Name:        pack_sk_s1
 *
 * Description: Bit-pack only some element of s1 in secret key sk = (rho, key, tr, s1, s2, t0).
 *
 * Arguments:   - uint8_t sk[]: output byte array
 *              - const poly *s1_elem: pointer to vector element idx in s1
 *              - const unisgned int idx: index to the element of s1 that should
 *                be packed
 **************************************************/
void pack_sk_s1(uint8_t sk[CRYPTO_SECRETKEYBYTES],
                const poly *s1_elem,
                const unsigned int idx) {
  sk += 2 * SEEDBYTES + TRBYTES;
  polyeta_pack(sk + idx * POLYETA_PACKEDBYTES, s1_elem);
}

/*************************************************
 * Name:        pack_sk_s2
 *
 * Description: Bit-pack only some element of s2 in secret key sk = (rho, key, tr, s1, s2, t0).
 *
 * Arguments:   - uint8_t sk[]: output byte array
 *              - const poly *s2_elem: pointer to vector element idx in s2
 *              - const unsigned int idx: index to the element of s1 that should
 *                be packed
 **************************************************/
void pack_sk_s2(uint8_t sk[CRYPTO_SECRETKEYBYTES],
                const poly *s2_elem,
                const unsigned int idx) {
  sk += 2 * SEEDBYTES + TRBYTES + L * POLYETA_PACKEDBYTES;
  polyeta_pack(sk + idx * POLYETA_PACKEDBYTES, s2_elem);
}

/*************************************************
 * Name:        pack_sk_t0
 *
 * Description: Bit-pack only some element of t0 in secret key sk = (rho, key, tr, s1, s2, t0).
 *
 * Arguments:   - uint8_t sk[]: output byte array
 *              - const poly *t0_elem: pointer to vector element idx in s2
 *              - const unsigned int idx: index to the element of s1 that should
 *                be packed
 **************************************************/
void pack_sk_t0(uint8_t sk[CRYPTO_SECRETKEYBYTES],
                const poly *t0_elem,
                const unsigned int idx) {
  sk += 2 * SEEDBYTES + TRBYTES + L * POLYETA_PACKEDBYTES + K * POLYETA_PACKEDBYTES;
  polyt0_pack(sk + idx * POLYT0_PACKEDBYTES, t0_elem);
}

/*************************************************
 * Name:        pack_sk_rho
 *
 * Description: Bit-pack only rho in secret key sk = (rho, key, tr, s1, s2, t0).
 *
 * Arguments:   - uint8_t sk[]: output byte array
 *              - const uint8_t rho[]: byte array containing rho
 **************************************************/
void pack_sk_rho(uint8_t sk[CRYPTO_SECRETKEYBYTES],
                 const uint8_t rho[SEEDBYTES]) {
  unsigned int i;
  
  for(i = 0; i < SEEDBYTES; ++i) {
    sk[i] = rho[i];
  }
}

/*************************************************
 * Name:        pack_sk_key
 *
 * Description: Bit-pack only key in secret key sk = (rho, key, tr, s1, s2, t0).
 *
 * Arguments:   - uint8_t sk[]: output byte array
 *              - const uint8_t key[]: byte array containing key
 **************************************************/
void pack_sk_key(uint8_t sk[CRYPTO_SECRETKEYBYTES],
                 const uint8_t key[SEEDBYTES]) {
  unsigned int i;
  
  sk += SEEDBYTES;
  for(i = 0; i < SEEDBYTES; ++i) {
    sk[i] = key[i];
  }
}

/*************************************************
 * Name:        pack_sk_tr
 *
 * Description: Bit-pack only tr in secret key sk = (rho, key, tr, s1, s2, t0).
 *
 * Arguments:   - uint8_t sk[]: output byte array
 *              - const uint8_t tr[]: byte array containing tr
 **************************************************/
void pack_sk_tr(uint8_t sk[CRYPTO_SECRETKEYBYTES],
                const uint8_t tr[TRBYTES]) {
  unsigned int i;
  
  sk += 2 * SEEDBYTES;
  for(i = 0; i < TRBYTES; ++i) {
    sk[i] = tr[i];
  }
}

/*************************************************
 * Name:        challenge
 *
 * Description: Implementation of H. Samples polynomial with TAU nonzero
 *              coefficients in {-1,1} using the output stream of
 *              SHAKE256(seed). Memory optimized.
 *
 * Arguments:   - poly *c: pointer to output polynomial
 *              - const uint8_t mu[]: byte array containing seed of length SEEDBYTES
 **************************************************/
#define CHALLENGE_lowram_BUF_SIZE 8
void poly_challenge_lowram(poly *c, const uint8_t seed[CTILDEBYTES]) {
  unsigned int i, b, pos;
  uint64_t signs;
  uint8_t buf[CHALLENGE_lowram_BUF_SIZE];
  keccak_state state;

  shake256_init(&state);
  shake256_absorb(&state, seed, CTILDEBYTES);
  shake256_finalize(&state);
  shake256_squeeze(buf, CHALLENGE_lowram_BUF_SIZE, &state);
  signs = 0;
  for(i = 0; i < 8; ++i) {
    signs |= (uint64_t)buf[i] << 8 * i;
  }
  pos = 8;

  for(i = 0; i < N; ++i)
    c->coeffs[i] = 0;
  for(i = N - TAU; i < N; ++i) {
    do {
      if(pos >= CHALLENGE_lowram_BUF_SIZE) {
        shake256_squeeze(buf, CHALLENGE_lowram_BUF_SIZE, &state);
        pos = 0;
      }

      b = buf[pos++];
    } while(b > i);

    c->coeffs[i] = c->coeffs[b];
    c->coeffs[b] = 1 - 2 * (signs & 1);
    signs >>= 1;
  }
}

/*************************************************
 * Name:        poly_pointwise_acc_montgomery
 *
 * Description: Pointwise multiplication of polynomials in NTT domain
 *              representation and multiplication of resulting polynomial
 *              by 2^{-32} with accumulation.
 *
 * Arguments:   - poly *c: pointer to output/accumulator polynomial
 *              - const poly *a: pointer to first input polynomial
 *              - const poly *b: pointer to second input polynomial
 **************************************************/
void poly_pointwise_acc_montgomery(poly *c, const poly *a, const poly *b) {
  unsigned int i;

  for(i = 0; i < N; ++i)
    c->coeffs[i] += montgomery_reduce((int64_t)a->coeffs[i] * b->coeffs[i]);
}
