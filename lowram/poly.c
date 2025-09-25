#include <stdint.h>
#include "params.h"
#include "poly.h"
#include "ntt.h"
#include "reduce.h"
#include "rounding.h"
#include "symmetric.h"

#ifdef DBENCH
#include "test/cpucycles.h"
extern const uint64_t timing_overhead;
extern uint64_t *tred, *tadd, *tmul, *tround, *tsample, *tpack;
#define DBENCH_START() uint64_t time = cpucycles()
#define DBENCH_STOP(t) t += cpucycles() - time - timing_overhead
#else
#define DBENCH_START()
#define DBENCH_STOP(t)
#endif

/*************************************************
* Name:        poly_reduce
*
* Description: Inplace reduction of all coefficients of polynomial to
*              representative in [-6283008,6283008].
*
* Arguments:   - poly *a: pointer to input/output polynomial
**************************************************/
void poly_reduce(poly *a) {
  unsigned int i;
  DBENCH_START();

  for(i = 0; i < N; ++i)
    a->coeffs[i] = reduce32(a->coeffs[i]);

  DBENCH_STOP(*tred);
}

/*************************************************
* Name:        poly_caddq
*
* Description: For all coefficients of in/out polynomial add Q if
*              coefficient is negative.
*
* Arguments:   - poly *a: pointer to input/output polynomial
**************************************************/
void poly_caddq(poly *a) {
  unsigned int i;
  DBENCH_START();

  for(i = 0; i < N; ++i)
    a->coeffs[i] = caddq(a->coeffs[i]);

  DBENCH_STOP(*tred);
}

/*************************************************
* Name:        poly_add
*
* Description: Add polynomials. No modular reduction is performed.
*
* Arguments:   - poly *c: pointer to output polynomial
*              - const poly *a: pointer to first summand
*              - const poly *b: pointer to second summand
**************************************************/
void poly_add(poly *c, const poly *a, const poly *b)  {
  unsigned int i;
  DBENCH_START();

  for(i = 0; i < N; ++i)
    c->coeffs[i] = a->coeffs[i] + b->coeffs[i];

  DBENCH_STOP(*tadd);
}



/*************************************************
* Name:        poly_ntt
*
* Description: Inplace forward NTT. Coefficients can grow by
*              8*Q in absolute value.
*
* Arguments:   - poly *a: pointer to input/output polynomial
**************************************************/
void poly_ntt(poly *a) {
  DBENCH_START();

  ntt(a->coeffs);

  DBENCH_STOP(*tmul);
}

/*************************************************
* Name:        poly_invntt_tomont
*
* Description: Inplace inverse NTT and multiplication by 2^{32}.
*              Input coefficients need to be less than Q in absolute
*              value and output coefficients are again bounded by Q.
*
* Arguments:   - poly *a: pointer to input/output polynomial
**************************************************/
void poly_invntt_tomont(poly *a) {
  DBENCH_START();

  invntt_tomont(a->coeffs);

  DBENCH_STOP(*tmul);
}

/*************************************************
* Name:        poly_pointwise_montgomery
*
* Description: Pointwise multiplication of polynomials in NTT domain
*              representation and multiplication of resulting polynomial
*              by 2^{-32}.
*
* Arguments:   - poly *c: pointer to output polynomial
*              - const poly *a: pointer to first input polynomial
*              - const poly *b: pointer to second input polynomial
**************************************************/
void poly_pointwise_montgomery(poly *c, const poly *a, const poly *b) {
  unsigned int i;
  DBENCH_START();

  for(i = 0; i < N; ++i)
    c->coeffs[i] = montgomery_reduce((int64_t)a->coeffs[i] * b->coeffs[i]);

  DBENCH_STOP(*tmul);
}

/*************************************************
* Name:        poly_power2round
*
* Description: For all coefficients c of the input polynomial,
*              compute c0, c1 such that c mod Q = c1*2^D + c0
*              with -2^{D-1} < c0 <= 2^{D-1}. Assumes coefficients to be
*              standard representatives.
*
* Arguments:   - poly *a1: pointer to output polynomial with coefficients c1
*              - poly *a0: pointer to output polynomial with coefficients c0
*              - const poly *a: pointer to input polynomial
**************************************************/
void poly_power2round(poly *a1, poly *a0, const poly *a) {
  unsigned int i;
  DBENCH_START();

  for(i = 0; i < N; ++i)
    a1->coeffs[i] = power2round(&a0->coeffs[i], a->coeffs[i]);

  DBENCH_STOP(*tround);
}




/*************************************************
* Name:        poly_chknorm
*
* Description: Check infinity norm of polynomial against given bound.
*              Assumes input coefficients were reduced by reduce32().
*
* Arguments:   - const poly *a: pointer to polynomial
*              - int32_t B: norm bound
*
* Returns 0 if norm is strictly smaller than B <= (Q-1)/8 and 1 otherwise.
**************************************************/
int poly_chknorm(const poly *a, int32_t B) {
  unsigned int i;
  int32_t t;
  DBENCH_START();

  if(B > (Q-1)/8)
    return 1;

  /* It is ok to leak which coefficient violates the bound since
     the probability for each coefficient is independent of secret
     data but we must not leak the sign of the centralized representative. */
  for(i = 0; i < N; ++i) {
    /* Absolute value */
    t = a->coeffs[i] >> 31;
    t = a->coeffs[i] - (t & 2*a->coeffs[i]);

    if(t >= B) {
      DBENCH_STOP(*tsample);
      return 1;
    }
  }

  DBENCH_STOP(*tsample);
  return 0;
}

/*************************************************
* Name:        rej_uniform
*
* Description: Sample uniformly random coefficients in [0, Q-1] by
*              performing rejection sampling on array of random bytes.
*
* Arguments:   - int32_t *a: pointer to output array (allocated)
*              - unsigned int len: number of coefficients to be sampled
*              - const uint8_t *buf: array of random bytes
*              - unsigned int buflen: length of array of random bytes
*
* Returns number of sampled coefficients. Can be smaller than len if not enough
* random bytes were given.
**************************************************/
static unsigned int rej_uniform(int32_t *a,
                                unsigned int len,
                                const uint8_t *buf,
                                unsigned int buflen)
{
  unsigned int ctr, pos;
  uint32_t t;
  DBENCH_START();

  ctr = pos = 0;
  while(ctr < len && pos + 3 <= buflen) {
    t  = buf[pos++];
    t |= (uint32_t)buf[pos++] << 8;
    t |= (uint32_t)buf[pos++] << 16;
    t &= 0x7FFFFF;

    if(t < Q)
      a[ctr++] = t;
  }

  DBENCH_STOP(*tsample);
  return ctr;
}

/*************************************************
* Name:        poly_uniform
*
* Description: Sample polynomial with uniformly random coefficients
*              in [0,Q-1] by performing rejection sampling on the
*              output stream of SHAKE128(seed|nonce)
*
* Arguments:   - poly *a: pointer to output polynomial
*              - const uint8_t seed[]: byte array with seed of length SEEDBYTES
*              - uint16_t nonce: 2-byte nonce
**************************************************/
#define POLY_UNIFORM_NBLOCKS ((768 + STREAM128_BLOCKBYTES - 1)/STREAM128_BLOCKBYTES)
void poly_uniform(poly *a,
                  const uint8_t seed[SEEDBYTES],
                  uint16_t nonce)
{
  unsigned int i, ctr, off;
  unsigned int buflen = POLY_UNIFORM_NBLOCKS*STREAM128_BLOCKBYTES;
  uint8_t buf[POLY_UNIFORM_NBLOCKS*STREAM128_BLOCKBYTES + 2];
  stream128_state state;

  stream128_init(&state, seed, nonce);
  stream128_squeezeblocks(buf, POLY_UNIFORM_NBLOCKS, &state);

  ctr = rej_uniform(a->coeffs, N, buf, buflen);

  while(ctr < N) {
    off = buflen % 3;
    for(i = 0; i < off; ++i)
      buf[i] = buf[buflen - off + i];

    stream128_squeezeblocks(buf + off, 1, &state);
    buflen = STREAM128_BLOCKBYTES + off;
    ctr += rej_uniform(a->coeffs + ctr, N - ctr, buf, buflen);
  }
}

/*************************************************
* Name:        rej_eta
*
* Description: Sample uniformly random coefficients in [-ETA, ETA] by
*              performing rejection sampling on array of random bytes.
*
* Arguments:   - int32_t *a: pointer to output array (allocated)
*              - unsigned int len: number of coefficients to be sampled
*              - const uint8_t *buf: array of random bytes
*              - unsigned int buflen: length of array of random bytes
*
* Returns number of sampled coefficients. Can be smaller than len if not enough
* random bytes were given.
**************************************************/
static unsigned int rej_eta(int32_t *a,
                            unsigned int len,
                            const uint8_t *buf,
                            unsigned int buflen)
{
  unsigned int ctr, pos;
  uint32_t t0, t1;
  DBENCH_START();

  ctr = pos = 0;
  while(ctr < len && pos < buflen) {
    t0 = buf[pos] & 0x0F;
    t1 = buf[pos++] >> 4;

#if ETA == 2
    if(t0 < 15) {
      t0 = t0 - (205*t0 >> 10)*5;
      a[ctr++] = 2 - t0;
    }
    if(t1 < 15 && ctr < len) {
      t1 = t1 - (205*t1 >> 10)*5;
      a[ctr++] = 2 - t1;
    }
#elif ETA == 4
    if(t0 < 9)
      a[ctr++] = 4 - t0;
    if(t1 < 9 && ctr < len)
      a[ctr++] = 4 - t1;
#endif
  }

  DBENCH_STOP(*tsample);
  return ctr;
}

/*************************************************
* Name:        poly_uniform_eta
*
* Description: Sample polynomial with uniformly random coefficients
*              in [-ETA,ETA] by performing rejection sampling on the
*              output stream from SHAKE256(seed|nonce)
*
* Arguments:   - poly *a: pointer to output polynomial
*              - const uint8_t seed[]: byte array with seed of length CRHBYTES
*              - uint16_t nonce: 2-byte nonce
**************************************************/
#if ETA == 2
#define POLY_UNIFORM_ETA_NBLOCKS ((136 + STREAM256_BLOCKBYTES - 1)/STREAM256_BLOCKBYTES)
#elif ETA == 4
#define POLY_UNIFORM_ETA_NBLOCKS ((227 + STREAM256_BLOCKBYTES - 1)/STREAM256_BLOCKBYTES)
#endif
void poly_uniform_eta(poly *a,
                      const uint8_t seed[CRHBYTES],
                      uint16_t nonce)
{
  unsigned int ctr;
  unsigned int buflen = POLY_UNIFORM_ETA_NBLOCKS*STREAM256_BLOCKBYTES;
  uint8_t buf[POLY_UNIFORM_ETA_NBLOCKS*STREAM256_BLOCKBYTES];
  stream256_state state;

  stream256_init(&state, seed, nonce);
  stream256_squeezeblocks(buf, POLY_UNIFORM_ETA_NBLOCKS, &state);

  ctr = rej_eta(a->coeffs, N, buf, buflen);

  while(ctr < N) {
    stream256_squeezeblocks(buf, 1, &state);
    ctr += rej_eta(a->coeffs + ctr, N - ctr, buf, STREAM256_BLOCKBYTES);
  }
}


/*************************************************
* Name:        challenge
*
* Description: Implementation of H. Samples polynomial with TAU nonzero
*              coefficients in {-1,1} using the output stream of
*              SHAKE256(seed).
*
* Arguments:   - poly *c: pointer to output polynomial
*              - const uint8_t mu[]: byte array containing seed of length CTILDEBYTES
**************************************************/
void poly_challenge(poly *c, const uint8_t seed[CTILDEBYTES]) {
  unsigned int i, b, pos;
  uint64_t signs;
  uint8_t buf[SHAKE256_RATE];
  keccak_state state;

  shake256_init(&state);
  shake256_absorb(&state, seed, CTILDEBYTES);
  shake256_finalize(&state);
  shake256_squeezeblocks(buf, 1, &state);

  signs = 0;
  for(i = 0; i < 8; ++i)
    signs |= (uint64_t)buf[i] << 8*i;
  pos = 8;

  for(i = 0; i < N; ++i)
    c->coeffs[i] = 0;
  for(i = N-TAU; i < N; ++i) {
    do {
      if(pos >= SHAKE256_RATE) {
        shake256_squeezeblocks(buf, 1, &state);
        pos = 0;
      }

      b = buf[pos++];
    } while(b > i);

    c->coeffs[i] = c->coeffs[b];
    c->coeffs[b] = 1 - 2*(signs & 1);
    signs >>= 1;
  }
}

/*************************************************
* Name:        polyeta_pack
*
* Description: Bit-pack polynomial with coefficients in [-ETA,ETA].
*
* Arguments:   - uint8_t *r: pointer to output byte array with at least
*                            POLYETA_PACKEDBYTES bytes
*              - const poly *a: pointer to input polynomial
**************************************************/
void polyeta_pack(uint8_t *r, const poly *a) {
  unsigned int i;
  uint8_t t[8];
  DBENCH_START();

#if ETA == 2
  for(i = 0; i < N/8; ++i) {
    t[0] = ETA - a->coeffs[8*i+0];
    t[1] = ETA - a->coeffs[8*i+1];
    t[2] = ETA - a->coeffs[8*i+2];
    t[3] = ETA - a->coeffs[8*i+3];
    t[4] = ETA - a->coeffs[8*i+4];
    t[5] = ETA - a->coeffs[8*i+5];
    t[6] = ETA - a->coeffs[8*i+6];
    t[7] = ETA - a->coeffs[8*i+7];

    r[3*i+0]  = (t[0] >> 0) | (t[1] << 3) | (t[2] << 6);
    r[3*i+1]  = (t[2] >> 2) | (t[3] << 1) | (t[4] << 4) | (t[5] << 7);
    r[3*i+2]  = (t[5] >> 1) | (t[6] << 2) | (t[7] << 5);
  }
#elif ETA == 4
  for(i = 0; i < N/2; ++i) {
    t[0] = ETA - a->coeffs[2*i+0];
    t[1] = ETA - a->coeffs[2*i+1];
    r[i] = t[0] | (t[1] << 4);
  }
#endif

  DBENCH_STOP(*tpack);
}


/*************************************************
* Name:        polyt1_pack
*
* Description: Bit-pack polynomial t1 with coefficients fitting in 10 bits.
*              Input coefficients are assumed to be standard representatives.
*
* Arguments:   - uint8_t *r: pointer to output byte array with at least
*                            POLYT1_PACKEDBYTES bytes
*              - const poly *a: pointer to input polynomial
**************************************************/
void polyt1_pack(uint8_t *r, const poly *a) {
  unsigned int i;
  DBENCH_START();

  for(i = 0; i < N/4; ++i) {
    r[5*i+0] = (a->coeffs[4*i+0] >> 0);
    r[5*i+1] = (a->coeffs[4*i+0] >> 8) | (a->coeffs[4*i+1] << 2);
    r[5*i+2] = (a->coeffs[4*i+1] >> 6) | (a->coeffs[4*i+2] << 4);
    r[5*i+3] = (a->coeffs[4*i+2] >> 4) | (a->coeffs[4*i+3] << 6);
    r[5*i+4] = (a->coeffs[4*i+3] >> 2);
  }

  DBENCH_STOP(*tpack);
}


/*************************************************
* Name:        polyt0_pack
*
* Description: Bit-pack polynomial t0 with coefficients in ]-2^{D-1}, 2^{D-1}].
*
* Arguments:   - uint8_t *r: pointer to output byte array with at least
*                            POLYT0_PACKEDBYTES bytes
*              - const poly *a: pointer to input polynomial
**************************************************/
void polyt0_pack(uint8_t *r, const poly *a) {
  unsigned int i;
  uint32_t t[8];
  DBENCH_START();

  for(i = 0; i < N/8; ++i) {
    t[0] = (1 << (D-1)) - a->coeffs[8*i+0];
    t[1] = (1 << (D-1)) - a->coeffs[8*i+1];
    t[2] = (1 << (D-1)) - a->coeffs[8*i+2];
    t[3] = (1 << (D-1)) - a->coeffs[8*i+3];
    t[4] = (1 << (D-1)) - a->coeffs[8*i+4];
    t[5] = (1 << (D-1)) - a->coeffs[8*i+5];
    t[6] = (1 << (D-1)) - a->coeffs[8*i+6];
    t[7] = (1 << (D-1)) - a->coeffs[8*i+7];

    r[13*i+ 0]  =  t[0];
    r[13*i+ 1]  =  t[0] >>  8;
    r[13*i+ 1] |=  t[1] <<  5;
    r[13*i+ 2]  =  t[1] >>  3;
    r[13*i+ 3]  =  t[1] >> 11;
    r[13*i+ 3] |=  t[2] <<  2;
    r[13*i+ 4]  =  t[2] >>  6;
    r[13*i+ 4] |=  t[3] <<  7;
    r[13*i+ 5]  =  t[3] >>  1;
    r[13*i+ 6]  =  t[3] >>  9;
    r[13*i+ 6] |=  t[4] <<  4;
    r[13*i+ 7]  =  t[4] >>  4;
    r[13*i+ 8]  =  t[4] >> 12;
    r[13*i+ 8] |=  t[5] <<  1;
    r[13*i+ 9]  =  t[5] >>  7;
    r[13*i+ 9] |=  t[6] <<  6;
    r[13*i+10]  =  t[6] >>  2;
    r[13*i+11]  =  t[6] >> 10;
    r[13*i+11] |=  t[7] <<  3;
    r[13*i+12]  =  t[7] >>  5;
  }

  DBENCH_STOP(*tpack);
}


/*************************************************
* Name:        polyz_pack
*
* Description: Bit-pack polynomial with coefficients
*              in [-(GAMMA1 - 1), GAMMA1].
*
* Arguments:   - uint8_t *r: pointer to output byte array with at least
*                            POLYZ_PACKEDBYTES bytes
*              - const poly *a: pointer to input polynomial
**************************************************/
void polyz_pack(uint8_t *r, const poly *a) {
  unsigned int i;
  uint32_t t[4];
  DBENCH_START();

#if GAMMA1 == (1 << 17)
  for(i = 0; i < N/4; ++i) {
    t[0] = GAMMA1 - a->coeffs[4*i+0];
    t[1] = GAMMA1 - a->coeffs[4*i+1];
    t[2] = GAMMA1 - a->coeffs[4*i+2];
    t[3] = GAMMA1 - a->coeffs[4*i+3];

    r[9*i+0]  = t[0];
    r[9*i+1]  = t[0] >> 8;
    r[9*i+2]  = t[0] >> 16;
    r[9*i+2] |= t[1] << 2;
    r[9*i+3]  = t[1] >> 6;
    r[9*i+4]  = t[1] >> 14;
    r[9*i+4] |= t[2] << 4;
    r[9*i+5]  = t[2] >> 4;
    r[9*i+6]  = t[2] >> 12;
    r[9*i+6] |= t[3] << 6;
    r[9*i+7]  = t[3] >> 2;
    r[9*i+8]  = t[3] >> 10;
  }
#elif GAMMA1 == (1 << 19)
  for(i = 0; i < N/2; ++i) {
    t[0] = GAMMA1 - a->coeffs[2*i+0];
    t[1] = GAMMA1 - a->coeffs[2*i+1];

    r[5*i+0]  = t[0];
    r[5*i+1]  = t[0] >> 8;
    r[5*i+2]  = t[0] >> 16;
    r[5*i+2] |= t[1] << 4;
    r[5*i+3]  = t[1] >> 4;
    r[5*i+4]  = t[1] >> 12;
  }
#endif

  DBENCH_STOP(*tpack);
}

/*************************************************
* Name:        polyz_unpack
*
* Description: Unpack polynomial z with coefficients
*              in [-(GAMMA1 - 1), GAMMA1].
*
* Arguments:   - poly *r: pointer to output polynomial
*              - const uint8_t *a: byte array with bit-packed polynomial
**************************************************/
void polyz_unpack(poly *r, const uint8_t *a) {
  unsigned int i;
  DBENCH_START();

#if GAMMA1 == (1 << 17)
  for(i = 0; i < N/4; ++i) {
    r->coeffs[4*i+0]  = a[9*i+0];
    r->coeffs[4*i+0] |= (uint32_t)a[9*i+1] << 8;
    r->coeffs[4*i+0] |= (uint32_t)a[9*i+2] << 16;
    r->coeffs[4*i+0] &= 0x3FFFF;

    r->coeffs[4*i+1]  = a[9*i+2] >> 2;
    r->coeffs[4*i+1] |= (uint32_t)a[9*i+3] << 6;
    r->coeffs[4*i+1] |= (uint32_t)a[9*i+4] << 14;
    r->coeffs[4*i+1] &= 0x3FFFF;

    r->coeffs[4*i+2]  = a[9*i+4] >> 4;
    r->coeffs[4*i+2] |= (uint32_t)a[9*i+5] << 4;
    r->coeffs[4*i+2] |= (uint32_t)a[9*i+6] << 12;
    r->coeffs[4*i+2] &= 0x3FFFF;

    r->coeffs[4*i+3]  = a[9*i+6] >> 6;
    r->coeffs[4*i+3] |= (uint32_t)a[9*i+7] << 2;
    r->coeffs[4*i+3] |= (uint32_t)a[9*i+8] << 10;
    r->coeffs[4*i+3] &= 0x3FFFF;

    r->coeffs[4*i+0] = GAMMA1 - r->coeffs[4*i+0];
    r->coeffs[4*i+1] = GAMMA1 - r->coeffs[4*i+1];
    r->coeffs[4*i+2] = GAMMA1 - r->coeffs[4*i+2];
    r->coeffs[4*i+3] = GAMMA1 - r->coeffs[4*i+3];
  }
#elif GAMMA1 == (1 << 19)
  for(i = 0; i < N/2; ++i) {
    r->coeffs[2*i+0]  = a[5*i+0];
    r->coeffs[2*i+0] |= (uint32_t)a[5*i+1] << 8;
    r->coeffs[2*i+0] |= (uint32_t)a[5*i+2] << 16;
    r->coeffs[2*i+0] &= 0xFFFFF;

    r->coeffs[2*i+1]  = a[5*i+2] >> 4;
    r->coeffs[2*i+1] |= (uint32_t)a[5*i+3] << 4;
    r->coeffs[2*i+1] |= (uint32_t)a[5*i+4] << 12;
    /* r->coeffs[2*i+1] &= 0xFFFFF; */ /* No effect, since we're anyway at 20 bits */

    r->coeffs[2*i+0] = GAMMA1 - r->coeffs[2*i+0];
    r->coeffs[2*i+1] = GAMMA1 - r->coeffs[2*i+1];
  }
#endif

  DBENCH_STOP(*tpack);
}

/*************************************************
* Name:        polyw1_pack
*
* Description: Bit-pack polynomial w1 with coefficients in [0,15] or [0,43].
*              Input coefficients are assumed to be standard representatives.
*
* Arguments:   - uint8_t *r: pointer to output byte array with at least
*                            POLYW1_PACKEDBYTES bytes
*              - const poly *a: pointer to input polynomial
**************************************************/
void polyw1_pack(uint8_t *r, const poly *a) {
  unsigned int i;
  DBENCH_START();

#if GAMMA2 == (Q-1)/88
  for(i = 0; i < N/4; ++i) {
    r[3*i+0]  = a->coeffs[4*i+0];
    r[3*i+0] |= a->coeffs[4*i+1] << 6;
    r[3*i+1]  = a->coeffs[4*i+1] >> 2;
    r[3*i+1] |= a->coeffs[4*i+2] << 4;
    r[3*i+2]  = a->coeffs[4*i+2] >> 4;
    r[3*i+2] |= a->coeffs[4*i+3] << 2;
  }
#elif GAMMA2 == (Q-1)/32
  for(i = 0; i < N/2; ++i)
    r[i] = a->coeffs[2*i+0] | (a->coeffs[2*i+1] << 4);
#endif

  DBENCH_STOP(*tpack);
}

/*************************************************
 * Name:        poly_challenge_compress
 *
 * Description: Compress the challenge polynomial.
 *              This function is part of the lowram implementation.
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
 *              This function is part of the lowram implementation.
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
 *              This function is part of the lowram implementation.
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
 *              This function is part of the lowram implementation.
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
 *              This function is part of the lowram implementation.
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
 *                polynomial buffer and store the result in another polynomial.
 *              This function is part of the lowram implementation.
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

/*************************************************
 * Name:        poly_lowbits
 *
 * Description: Compute the low bits of each coefficient in a polynomial.
 *
 * Arguments:   - poly *a0: output polynomial to store the low bits of the coefficients
 *              - const poly *a: input polynomial whose coefficients' low bits
 *                  are to be computed
 *
 **************************************************/
void poly_lowbits(poly *a0, const poly *a) {
  unsigned int i;

  for(i = 0; i < N; ++i)
    a0->coeffs[i] = lowbits(a->coeffs[i]);
}

#define POLY_UNIFORM_BUFFERSIZE 3
/*************************************************
 * Name:        poly_uniform_pointwise_montgomery_polywadd_lowram
 *
 * Description: Generate a uniform polynomial using a seed and nonce, 
 *              perform pointwise multiplication with another polynomial, 
 *              and add the result to a compressed polynomial buffer.
 *              This function is part of the lowram implementation.
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
 *              This function is part of the lowram implementation.
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
 *              This function is part of the lowram implementation.
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

/*************************************************
 * Name:        poly_make_hint_lowram
 *
 * Description: Generate hint polynomial.
 *              This function is part of the lowram implementation.
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
 * Name:        poly_use_hint_lowram
 *
 * Description: Use hint polynomial to correct the high bits of a polynomial.
 *              This function is part of the lowram implementation.
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
 * Name:        challenge
 *
 * Description: Implementation of H. Samples polynomial with TAU nonzero
 *              coefficients in {-1,1} using the output stream of
 *              SHAKE256(seed). Memory optimized.
 *              This function is part of the lowram implementation.
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
 *              representation and multiplication of resulting polynomial.
 *              This function is part of the lowram implementation.
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


/* Small polynomial functions for 16-bit coefficients - part of lowram implementation */

/*************************************************
* Name:        poly_small_ntt_copy
*
* Description: Copy a polynomial to a small polynomial and apply NTT.
*              This function is part of the lowram implementation.
*
* Arguments:   - smallpoly *out: pointer to output small polynomial
*              - const poly *in: pointer to input polynomial
**************************************************/
void poly_small_ntt_copy(smallpoly *out, const poly *in) {
  int i;
  
  for(i = N - 1; i >= 0; i--) {
    out->coeffs[i] = in->coeffs[i];
  }
  poly_small_ntt(out);
}




/*************************************************
* Name:        poly_small_basemul_invntt
*
* Description: Multiplication of polynomials in Zq[X]/(X^n+1) and inverse NTT.
*              This function is part of the lowram implementation.
*
* Arguments:   - poly *r: pointer to output polynomial
*              - const smallpoly *a: pointer to first input small polynomial
*              - const smallpoly *b: pointer to second input small polynomial
**************************************************/
void poly_small_basemul_invntt(poly *r, const smallpoly *a, const smallpoly *b) {
  int j;
  unsigned int i;
  
  // re-use the buffer
  smallpoly *tmp = (smallpoly *)r;
  
  // Inline implementation of polynomial basemul
  for(i = 0; i < N / 4; i++) {
    small_basemul(&tmp->coeffs[4 * i], &a->coeffs[4 * i], &b->coeffs[4 * i], small_zetas[64 + i]);
    small_basemul(&tmp->coeffs[4 * i + 2], &a->coeffs[4 * i + 2], &b->coeffs[4 * i + 2], -small_zetas[64 + i]);
  }

  small_invntt_tomont(tmp->coeffs);
  // buffer is the same, so we neeed to be careful
  for(j = N - 1; j >= 0; j--) {
    r->coeffs[j] = tmp->coeffs[j];
  }
}


/*************************************************
* Name:        small_polyeta_unpack
*
* Description: Unpack polynomial from byte array with bits coefficients.
*              This function is part of the lowram implementation.
*
* Arguments:   - smallpoly *r: pointer to output small polynomial
*              - const uint8_t *a: pointer to input byte array
**************************************************/
void small_polyeta_unpack(smallpoly *r, const uint8_t *a) {
  unsigned int i;

#if ETA == 2
  for(i = 0; i < N / 8; ++i) {
    r->coeffs[8 * i + 0] = (a[3 * i + 0] >> 0) & 7;
    r->coeffs[8 * i + 1] = (a[3 * i + 0] >> 3) & 7;
    r->coeffs[8 * i + 2] = ((a[3 * i + 0] >> 6) | (a[3 * i + 1] << 2)) & 7;
    r->coeffs[8 * i + 3] = (a[3 * i + 1] >> 1) & 7;
    r->coeffs[8 * i + 4] = (a[3 * i + 1] >> 4) & 7;
    r->coeffs[8 * i + 5] = ((a[3 * i + 1] >> 7) | (a[3 * i + 2] << 1)) & 7;
    r->coeffs[8 * i + 6] = (a[3 * i + 2] >> 2) & 7;
    r->coeffs[8 * i + 7] = (a[3 * i + 2] >> 5) & 7;

    r->coeffs[8 * i + 0] = ETA - r->coeffs[8 * i + 0];
    r->coeffs[8 * i + 1] = ETA - r->coeffs[8 * i + 1];
    r->coeffs[8 * i + 2] = ETA - r->coeffs[8 * i + 2];
    r->coeffs[8 * i + 3] = ETA - r->coeffs[8 * i + 3];
    r->coeffs[8 * i + 4] = ETA - r->coeffs[8 * i + 4];
    r->coeffs[8 * i + 5] = ETA - r->coeffs[8 * i + 5];
    r->coeffs[8 * i + 6] = ETA - r->coeffs[8 * i + 6];
    r->coeffs[8 * i + 7] = ETA - r->coeffs[8 * i + 7];
  }
#elif ETA == 4
  for(i = 0; i < N / 2; ++i) {
    r->coeffs[2 * i + 0] = a[i] & 0x0F;
    r->coeffs[2 * i + 1] = a[i] >> 4;
    r->coeffs[2 * i + 0] = ETA - r->coeffs[2 * i + 0];
    r->coeffs[2 * i + 1] = ETA - r->coeffs[2 * i + 1];
  }
#endif
}

/*************************************************
* Name:        poly_small_ntt
*
* Description: Inplace number-theoretic transform (NTT) for small polynomial.
*              This function is part of the lowram implementation.
*
* Arguments:   - smallpoly *a: pointer to input/output polynomial
**************************************************/
void poly_small_ntt(smallpoly *a) {
  small_ntt(a->coeffs);
}

