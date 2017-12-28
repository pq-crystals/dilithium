#include <stdint.h>
#include "fips202.h"
#include "params.h"
#include "reduce.h"
#include "ntt.h"
#include "poly.h"

void poly_freeze(poly *a) {
  unsigned int i;

  for(i = 0; i < N; ++i)
    a->coeffs[i] = freeze(a->coeffs[i]);
}

void poly_add(poly *c, const poly *a, const poly *b)  {
  unsigned int i;

  for(i = 0; i < N; ++i)
    c->coeffs[i] = a->coeffs[i] + b->coeffs[i];
}

/* Assumes input coefficients to be less than 2*Q */
void poly_sub(poly *c, const poly *a, const poly *b) {
  unsigned int i;

  for(i = 0; i < N; ++i)
    c->coeffs[i] = a->coeffs[i] + 2*Q - b->coeffs[i];
}

/* Assumes input coefficients to be less than 2*Q */
void poly_neg(poly *a) {
  unsigned int i;

  for(i = 0; i < N; ++i)
    a->coeffs[i] = 2*Q - a->coeffs[i];
}

void poly_shiftl(poly *a, unsigned int k) {
  unsigned int i;

  for(i = 0; i < N; ++i)
    a->coeffs[i] <<= k;
}

void poly_ntt(poly *a) {
  ntt(a->coeffs);
}

/* Input coefficients are assumed to include Montgomery factor */
void poly_invntt_montgomery(poly *a) {
  invntt_frominvmont(a->coeffs);
}

void poly_pointwise_invmontgomery(poly *c, const poly *a, const poly *b) {
  unsigned int i;

  for(i = 0; i < N; ++i)
    c->coeffs[i] = montgomery_reduce((uint64_t)a->coeffs[i] * b->coeffs[i]);
}

/* Assumes input coefficients to be frozen */
int poly_chknorm(const poly *a, uint32_t B) {
  unsigned int i;
  int32_t t;

  /* It is ok to leak which coefficient violates the bound since
     the probability for each coefficient is independent of secret
     data but we must not leak the sign of the centralized representative. */
  for(i = 0; i < N; ++i) {
    /* Absolute value of centralized representative */
    t = (Q-1)/2 - a->coeffs[i];
    t ^= (t >> 31);
    t = (Q-1)/2 - t;

    if((uint32_t)t >= B)
      return 1;
  }

  return 0;
}

/* Assumes that buf contains enough randomness.
   For example 5*SHAKE128_RATE bytes. */
void poly_uniform(poly *a, unsigned char *buf) {
  unsigned int ctr, pos;
  uint32_t t;

  ctr = pos = 0;
  while(ctr < N) {
    t  = buf[pos++];
    t |= (uint32_t)buf[pos++] << 8;
    t |= (uint32_t)buf[pos++] << 16;
    t &= 0x7FFFFF;

    if(t < Q)
      a->coeffs[ctr++] = t;
  }
}

#if ETA > 7
#error "rej_eta() assumes ETA <= 7"
#endif

static unsigned int rej_eta(uint32_t *a,
                            unsigned int len,
                            const unsigned char *buf,
                            unsigned int buflen)
{
  unsigned int ctr, pos;
  unsigned char t0, t1;

  ctr = pos = 0;
  while(ctr < len) {
#if ETA <= 3
    t0 = buf[pos] & 0x07;
    t1 = buf[pos++] >> 5;
#else
    t0 = buf[pos] & 0x0F;
    t1 = buf[pos++] >> 4;
#endif

    if(t0 <= 2*ETA)
      a[ctr++] = Q + ETA - t0;
    if(t1 <= 2*ETA && ctr < N)
      a[ctr++] = Q + ETA - t1;

    if(pos >= buflen)
      break;
  }

  return ctr;
}

void poly_uniform_eta(poly *a,
                     const unsigned char seed[SEEDBYTES], 
                     unsigned char nonce)
{
  unsigned int i, ctr;
  unsigned char inbuf[SEEDBYTES + 1];
  /* Probability that we need more than 2 blocks: < 2^{-84} */
  unsigned char outbuf[2*SHAKE256_RATE];
  uint64_t state[25];

  for(i= 0; i < SEEDBYTES; ++i)
    inbuf[i] = seed[i];
  inbuf[SEEDBYTES] = nonce;

  shake256_absorb(state, inbuf, SEEDBYTES + 1);
  shake256_squeezeblocks(outbuf, 2, state);

  ctr = rej_eta(a->coeffs, N, outbuf, 2*SHAKE256_RATE);
  if(ctr < N) {
    shake256_squeezeblocks(outbuf, 1, state);
    rej_eta(a->coeffs + ctr, N - ctr, outbuf, SHAKE256_RATE);
  }
}

#if GAMMA1 > (1 << 19)
#error "rej_gamma1m1() assumes GAMMA1 - 1 fits in 19 bits"
#endif

static unsigned int rej_gamma1m1(uint32_t *a,
                                 unsigned int len,
                                 const unsigned char *buf,
                                 unsigned int buflen)
{
  unsigned int ctr, pos;
  uint32_t t0,t1;

  ctr = pos = 0;
  while(ctr < len) {
    t0  = buf[pos];
    t0 |= (uint32_t)buf[pos + 1] << 8;
    t0 |= (uint32_t)buf[pos + 2] << 16;
    t0 &= 0xFFFFF;

    t1  = buf[pos + 2] >> 4;
    t1 |= (uint32_t)buf[pos + 3] << 4;
    t1 |= (uint32_t)buf[pos + 4] << 12;

    pos += 5;

    if(t0 <= 2*GAMMA1 - 2)
      a[ctr++] = Q + GAMMA1 - 1 - t0;
    if(t1 <= 2*GAMMA1 - 2 && ctr < len)
      a[ctr++] = Q + GAMMA1 - 1 - t1;

    if(pos > buflen - 5)
      break;
  }

  return ctr;
}

void poly_uniform_gamma1m1(poly *a,
                          const unsigned char seed[SEEDBYTES + CRHBYTES],
                          uint16_t nonce)
{
  unsigned int i, ctr;
  unsigned char inbuf[SEEDBYTES + CRHBYTES + 2];
  /* Probability that we need more than 5 blocks: < 2^{-81}
     Probability that we need more than 6 blocks: < ... */
  unsigned char outbuf[5*SHAKE256_RATE];
  uint64_t state[25];

  for(i = 0; i < SEEDBYTES + CRHBYTES; ++i)
    inbuf[i] = seed[i];
  inbuf[SEEDBYTES + CRHBYTES] = nonce & 0xFF;
  inbuf[SEEDBYTES + CRHBYTES + 1] = nonce >> 8;

  shake256_absorb(state, inbuf, SEEDBYTES + CRHBYTES + 2);
  shake256_squeezeblocks(outbuf, 5, state);

  ctr = rej_gamma1m1(a->coeffs, N, outbuf, 5*SHAKE256_RATE);
  if(ctr < N) {
    /* There are no bytes left in outbuf
       since 5*SHAKE256_RATE is divisible by 5 */
    shake256_squeezeblocks(outbuf, 1, state);
    rej_gamma1m1(a->coeffs + ctr, N - ctr, outbuf, SHAKE256_RATE);
  }
}
/* Assumes input coefficients to be frozen */
void poly_pack(unsigned char *r, const poly *a) {
  unsigned int i;
 
  for(i = 0; i < N/8; ++i) {
    r[23*i+ 0]  =  a->coeffs[8*i+0] & 0xFF;
    r[23*i+ 1]  = (a->coeffs[8*i+0] >> 8) & 0xFF;
    r[23*i+ 2]  = (a->coeffs[8*i+0] >> 16) & 0x7F;
    r[23*i+ 2] |= (a->coeffs[8*i+1] & 0x01) << 7;
    r[23*i+ 3]  = (a->coeffs[8*i+1] >>  1) & 0xFF;
    r[23*i+ 4]  = (a->coeffs[8*i+1] >>  9) & 0xFF;
    r[23*i+ 5]  = (a->coeffs[8*i+1] >> 17) & 0x3F;
    r[23*i+ 5] |= (a->coeffs[8*i+2] & 0x03) << 6;
    r[23*i+ 6]  = (a->coeffs[8*i+2] >>  2) & 0xFF;
    r[23*i+ 7]  = (a->coeffs[8*i+2] >> 10) & 0xFF;
    r[23*i+ 8]  = (a->coeffs[8*i+2] >> 18) & 0x1F;
    r[23*i+ 8] |= (a->coeffs[8*i+3] & 0x07) << 5;
    r[23*i+ 9]  = (a->coeffs[8*i+3] >>  3) & 0xFF;
    r[23*i+10]  = (a->coeffs[8*i+3] >> 11) & 0xFF;
    r[23*i+11]  = (a->coeffs[8*i+3] >> 19) & 0x0F;
    r[23*i+11] |= (a->coeffs[8*i+4] & 0x0F) << 4;
    r[23*i+12]  = (a->coeffs[8*i+4] >>  4) & 0xFF;
    r[23*i+13]  = (a->coeffs[8*i+4] >> 12) & 0xFF;
    r[23*i+14]  = (a->coeffs[8*i+4] >> 20) & 0x07;
    r[23*i+14] |= (a->coeffs[8*i+5] & 0x1F) << 3;
    r[23*i+15]  = (a->coeffs[8*i+5] >>  5) & 0xFF;
    r[23*i+16]  = (a->coeffs[8*i+5] >> 13) & 0xFF;
    r[23*i+17]  = (a->coeffs[8*i+5] >> 21) & 0x03;
    r[23*i+17] |= (a->coeffs[8*i+6] & 0x3F) << 2;
    r[23*i+18]  = (a->coeffs[8*i+6] >>  6) & 0xFF;
    r[23*i+19]  = (a->coeffs[8*i+6] >> 14) & 0xFF;
    r[23*i+20]  = (a->coeffs[8*i+6] >> 22) & 0x01;
    r[23*i+20] |= (a->coeffs[8*i+7] & 0x7F) << 1;
    r[23*i+21]  = (a->coeffs[8*i+7] >>  7) & 0xFF;
    r[23*i+22]  = (a->coeffs[8*i+7] >> 15) & 0xFF;
  }
}

void poly_unpack(poly *r, const unsigned char *a) {
  unsigned int i;

  for(i = 0; i < N/8; ++i) {
    r->coeffs[8*i+0]  = a[23*i+0];
    r->coeffs[8*i+0] |= (uint32_t)a[23*i+1] << 8;
    r->coeffs[8*i+0] |= (uint32_t)(a[23*i+2] & 0x7F) << 16;

    r->coeffs[8*i+1]  = a[23*i+2] >> 7;
    r->coeffs[8*i+1] |= (uint32_t)a[23*i+3] << 1;
    r->coeffs[8*i+1] |= (uint32_t)a[23*i+4] << 9;
    r->coeffs[8*i+1] |= (uint32_t)(a[23*i+5] & 0x3F) << 17;

    r->coeffs[8*i+2]  = a[23*i+5] >> 6;
    r->coeffs[8*i+2] |= (uint32_t)a[23*i+6] << 2;
    r->coeffs[8*i+2] |= (uint32_t)a[23*i+7] << 10;
    r->coeffs[8*i+2] |= (uint32_t)(a[23*i+8] & 0x1F) << 18;

    r->coeffs[8*i+3]  = a[23*i+8] >> 5;
    r->coeffs[8*i+3] |= (uint32_t)a[23*i+9] << 3;
    r->coeffs[8*i+3] |= (uint32_t)a[23*i+10] << 11;
    r->coeffs[8*i+3] |= (uint32_t)(a[23*i+11] & 0x0F) << 19;

    r->coeffs[8*i+4]  = a[23*i+11] >> 4;
    r->coeffs[8*i+4] |= (uint32_t)a[23*i+12] << 4;
    r->coeffs[8*i+4] |= (uint32_t)a[23*i+13] << 12;
    r->coeffs[8*i+4] |= (uint32_t)(a[23*i+14] & 0x07) << 20;

    r->coeffs[8*i+5]  = a[23*i+14] >> 3;
    r->coeffs[8*i+5] |= (uint32_t)a[23*i+15] << 5;
    r->coeffs[8*i+5] |= (uint32_t)a[23*i+16] << 13;
    r->coeffs[8*i+5] |= (uint32_t)(a[23*i+17] & 0x03) << 21;

    r->coeffs[8*i+6]  = a[23*i+17] >> 2;
    r->coeffs[8*i+6] |= (uint32_t)a[23*i+18] << 6;
    r->coeffs[8*i+6] |= (uint32_t)a[23*i+19] << 14;
    r->coeffs[8*i+6] |= (uint32_t)(a[23*i+20] & 0x01) << 22;

    r->coeffs[8*i+7]  = a[23*i+20] >> 1;
    r->coeffs[8*i+7] |= (uint32_t)a[23*i+21] << 7;
    r->coeffs[8*i+7] |= (uint32_t)a[23*i+22] << 15;
  }
}

#if ETA > 7
#error "polyeta_pack() assumes ETA <= 7"
#endif

void polyeta_pack(unsigned char *r, const poly *a) {
  unsigned int i;
  unsigned char t[8];

#if ETA <= 3
  for(i = 0; i < N/8; ++i) {
    t[0] = Q + ETA - a->coeffs[8*i+0];
    t[1] = Q + ETA - a->coeffs[8*i+1];
    t[2] = Q + ETA - a->coeffs[8*i+2];
    t[3] = Q + ETA - a->coeffs[8*i+3];
    t[4] = Q + ETA - a->coeffs[8*i+4];
    t[5] = Q + ETA - a->coeffs[8*i+5];
    t[6] = Q + ETA - a->coeffs[8*i+6];
    t[7] = Q + ETA - a->coeffs[8*i+7];

    r[3*i+0]  = t[0];
    r[3*i+0] |= t[1] << 3;
    r[3*i+0] |= t[2] << 6;
    r[3*i+1]  = t[2] >> 2;
    r[3*i+1] |= t[3] << 1;
    r[3*i+1] |= t[4] << 4;
    r[3*i+1] |= t[5] << 7;
    r[3*i+2]  = t[5] >> 1;
    r[3*i+2] |= t[6] << 2;
    r[3*i+2] |= t[7] << 5;
  }
#else
  for(i = 0; i < N/2; ++i) {
    t[0] = Q + ETA - a->coeffs[2*i+0];
    t[1] = Q + ETA - a->coeffs[2*i+1];
    r[i] = t[0] | (t[1] << 4);
  }
#endif
}

void polyeta_unpack(poly *r, const unsigned char *a) {
  unsigned int i;

#if ETA <= 3
  for(i = 0; i < N/8; ++i) {
    r->coeffs[8*i+0] = a[3*i+0] & 0x07;
    r->coeffs[8*i+1] = (a[3*i+0] >> 3) & 0x07;
    r->coeffs[8*i+2] = (a[3*i+0] >> 6) | ((a[3*i+1] & 0x01) << 2);
    r->coeffs[8*i+3] = (a[3*i+1] >> 1) & 0x07;
    r->coeffs[8*i+4] = (a[3*i+1] >> 4) & 0x07;
    r->coeffs[8*i+5] = (a[3*i+1] >> 7) | ((a[3*i+2] & 0x03) << 1);
    r->coeffs[8*i+6] = (a[3*i+2] >> 2) & 0x07;
    r->coeffs[8*i+7] = (a[3*i+2] >> 5);

    r->coeffs[8*i+0] = Q + ETA - r->coeffs[8*i+0];
    r->coeffs[8*i+1] = Q + ETA - r->coeffs[8*i+1];
    r->coeffs[8*i+2] = Q + ETA - r->coeffs[8*i+2];
    r->coeffs[8*i+3] = Q + ETA - r->coeffs[8*i+3];
    r->coeffs[8*i+4] = Q + ETA - r->coeffs[8*i+4];
    r->coeffs[8*i+5] = Q + ETA - r->coeffs[8*i+5];
    r->coeffs[8*i+6] = Q + ETA - r->coeffs[8*i+6];
    r->coeffs[8*i+7] = Q + ETA - r->coeffs[8*i+7];
  }
#else
  for(i = 0; i < N/2; ++i) {
    r->coeffs[2*i+0] = a[i] & 0x0F;
    r->coeffs[2*i+1] = a[i] >> 4;
    r->coeffs[2*i+0] = Q + ETA - r->coeffs[2*i+0];
    r->coeffs[2*i+1] = Q + ETA - r->coeffs[2*i+1];
  }
#endif
}

#if D != 14
#error "polyt1_pack() assumes D == 14"
#endif

void polyt1_pack(unsigned char *r, const poly *a) {
  unsigned int i;

  for(i = 0; i < N/8; ++i) {
    r[9*i+0]  =  a->coeffs[8*i+0] & 0xFF;
    r[9*i+1]  = (a->coeffs[8*i+0] >> 8) | ((a->coeffs[8*i+1] & 0x7F) << 1);
    r[9*i+2]  = (a->coeffs[8*i+1] >> 7) | ((a->coeffs[8*i+2] & 0x3F) << 2);
    r[9*i+3]  = (a->coeffs[8*i+2] >> 6) | ((a->coeffs[8*i+3] & 0x1F) << 3);
    r[9*i+4]  = (a->coeffs[8*i+3] >> 5) | ((a->coeffs[8*i+4] & 0x0F) << 4);
    r[9*i+5]  = (a->coeffs[8*i+4] >> 4) | ((a->coeffs[8*i+5] & 0x07) << 5);
    r[9*i+6]  = (a->coeffs[8*i+5] >> 3) | ((a->coeffs[8*i+6] & 0x03) << 6);
    r[9*i+7]  = (a->coeffs[8*i+6] >> 2) | ((a->coeffs[8*i+7] & 0x01) << 7);
    r[9*i+8]  =  a->coeffs[8*i+7] >> 1;
  }
}

void polyt1_unpack(poly *r, const unsigned char *a) {
  unsigned int i;

  for(i = 0; i < N/8; ++i) {
    r->coeffs[8*i+0] =  a[9*i+0]       | ((uint32_t)(a[9*i+1] & 0x01) << 8);
    r->coeffs[8*i+1] = (a[9*i+1] >> 1) | ((uint32_t)(a[9*i+2] & 0x03) << 7);
    r->coeffs[8*i+2] = (a[9*i+2] >> 2) | ((uint32_t)(a[9*i+3] & 0x07) << 6);
    r->coeffs[8*i+3] = (a[9*i+3] >> 3) | ((uint32_t)(a[9*i+4] & 0x0F) << 5);
    r->coeffs[8*i+4] = (a[9*i+4] >> 4) | ((uint32_t)(a[9*i+5] & 0x1F) << 4);
    r->coeffs[8*i+5] = (a[9*i+5] >> 5) | ((uint32_t)(a[9*i+6] & 0x3F) << 3);
    r->coeffs[8*i+6] = (a[9*i+6] >> 6) | ((uint32_t)(a[9*i+7] & 0x7F) << 2);
    r->coeffs[8*i+7] = (a[9*i+7] >> 7) | ((uint32_t)(a[9*i+8] & 0xFF) << 1);
  }
}

void polyt0_pack(unsigned char *r, const poly *a) {
  unsigned int i;
  uint32_t t[4];

  for(i = 0; i < N/4; ++i) {
    t[0] = Q + (1 << (D-1)) - a->coeffs[4*i+0];
    t[1] = Q + (1 << (D-1)) - a->coeffs[4*i+1];
    t[2] = Q + (1 << (D-1)) - a->coeffs[4*i+2];
    t[3] = Q + (1 << (D-1)) - a->coeffs[4*i+3];

    r[7*i+0]  =  t[0];
    r[7*i+1]  =  t[0] >> 8;
    r[7*i+1] |=  t[1] << 6;
    r[7*i+2]  =  t[1] >> 2;
    r[7*i+3]  =  t[1] >> 10;
    r[7*i+3] |=  t[2] << 4;
    r[7*i+4]  =  t[2] >> 4;
    r[7*i+5]  =  t[2] >> 12;
    r[7*i+5] |=  t[3] << 2;
    r[7*i+6]  =  t[3] >> 6;
  }
}

void polyt0_unpack(poly *r, const unsigned char *a) {
  unsigned int i;

  for(i = 0; i < N/4; ++i) {
    r->coeffs[4*i+0]  = a[7*i+0];
    r->coeffs[4*i+0] |= (uint32_t)(a[7*i+1] & 0x3F) << 8;

    r->coeffs[4*i+1]  = a[7*i+1] >> 6;
    r->coeffs[4*i+1] |= (uint32_t)a[7*i+2] << 2;
    r->coeffs[4*i+1] |= (uint32_t)(a[7*i+3] & 0x0F) << 10;
    
    r->coeffs[4*i+2]  = a[7*i+3] >> 4;
    r->coeffs[4*i+2] |= (uint32_t)a[7*i+4] << 4;
    r->coeffs[4*i+2] |= (uint32_t)(a[7*i+5] & 0x03) << 12;

    r->coeffs[4*i+3]  = a[7*i+5] >> 2;
    r->coeffs[4*i+3] |= (uint32_t)a[7*i+6] << 6;

    r->coeffs[4*i+0] = Q + (1 << (D-1)) - r->coeffs[4*i+0];
    r->coeffs[4*i+1] = Q + (1 << (D-1)) - r->coeffs[4*i+1];
    r->coeffs[4*i+2] = Q + (1 << (D-1)) - r->coeffs[4*i+2];
    r->coeffs[4*i+3] = Q + (1 << (D-1)) - r->coeffs[4*i+3];
  }
}

#if GAMMA1 > (1 << 19)
#error "polyz_pack() assumes GAMMA1 <= 2^{19}"
#endif

/* Assumes coefficients to be frozen */
void polyz_pack(unsigned char *r, const poly *a) {
  unsigned int i;
  uint32_t t[2];

  for(i = 0; i < N/2; ++i) {
    /* Map to {0,...,2*GAMMA1 - 2} */
    t[0] = GAMMA1 - 1 - a->coeffs[2*i+0];
    t[0] += ((int32_t)t[0] >> 31) & Q;
    t[1] = GAMMA1 - 1 - a->coeffs[2*i+1];
    t[1] += ((int32_t)t[1] >> 31) & Q;

    r[5*i+0]  = t[0];
    r[5*i+1]  = t[0] >> 8;
    r[5*i+2]  = t[0] >> 16;
    r[5*i+2] |= t[1] << 4;
    r[5*i+3]  = t[1] >> 4;
    r[5*i+4]  = t[1] >> 12;
  }
}

void polyz_unpack(poly *r, const unsigned char *a) {
  unsigned int i;

  for(i = 0; i < N/2; ++i) {
    r->coeffs[2*i+0]  = a[5*i+0];
    r->coeffs[2*i+0] |= (uint32_t)a[5*i+1] << 8;
    r->coeffs[2*i+0] |= (uint32_t)(a[5*i+2] & 0x0F) << 16;

    r->coeffs[2*i+1]  = a[5*i+2] >> 4;
    r->coeffs[2*i+1] |= (uint32_t)a[5*i+3] << 4;
    r->coeffs[2*i+1] |= (uint32_t)a[5*i+4] << 12;

    r->coeffs[2*i+0] = GAMMA1 - 1 - r->coeffs[2*i+0];
    r->coeffs[2*i+0] += ((int32_t)r->coeffs[2*i+0] >> 31) & Q;
    r->coeffs[2*i+1] = GAMMA1 - 1 - r->coeffs[2*i+1];
    r->coeffs[2*i+1] += ((int32_t)r->coeffs[2*i+1] >> 31) & Q;
  }
}

void polyw1_pack(unsigned char *r, const poly *a) {
  unsigned int i;

  for(i = 0; i < N/2; ++i)
    r[i] = a->coeffs[2*i+0] | (a->coeffs[2*i+1] << 4);
}
