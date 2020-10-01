#include <stdint.h>
#include "params.h"
#include "sign.h"
#include "packing.h"
#include "polyvec.h"
#include "poly.h"
#include "randombytes.h"
#include "symmetric.h"
#include "fips202.h"
#ifdef DILITHIUM_USE_AES
#include "aes256ctr.h"
#endif

#ifndef DILITHIUM_USE_AES
static inline void polyvec_matrix_expand_row(polyvecl mat[K], const uint8_t rho[SEEDBYTES], unsigned int i) {
  if(i == 0) polyvec_matrix_expand_row0(mat, rho);
  if(i == 1) polyvec_matrix_expand_row1(mat, rho);
  if(i == 2) polyvec_matrix_expand_row2(mat, rho);
  if(i == 3) polyvec_matrix_expand_row3(mat, rho);
#if K > 4
  if(i == 4) polyvec_matrix_expand_row4(mat, rho);
  if(i == 5) polyvec_matrix_expand_row5(mat, rho);
#if K > 6
  if(i == 6) polyvec_matrix_expand_row6(mat, rho);
  if(i == 7) polyvec_matrix_expand_row7(mat, rho);
#endif
#endif
}
#endif

/*************************************************
* Name:        crypto_sign_keypair
*
* Description: Generates public and private key.
*
* Arguments:   - uint8_t *pk: pointer to output public key (allocated
*                             array of CRYPTO_PUBLICKEYBYTES bytes)
*              - uint8_t *sk: pointer to output private key (allocated
*                             array of CRYPTO_SECRETKEYBYTES bytes)
*
* Returns 0 (success)
**************************************************/
int crypto_sign_keypair(uint8_t *pk, uint8_t *sk) {
  unsigned int i;
  __attribute__((aligned(32)))
  uint8_t seedbuf[3*SEEDBYTES];
  __attribute__((aligned(32)))
  uint8_t tr[CRHBYTES];
  const uint8_t *rho, *rhoprime, *key;
  polyvecl mat[K], s1;
  polyveck s2;
  poly t1, t0;

  /* Get randomness for rho, rhoprime and key */
  randombytes(seedbuf, SEEDBYTES);
  shake256(seedbuf, 3*SEEDBYTES, seedbuf, SEEDBYTES);
  rho = seedbuf;
  rhoprime = seedbuf + SEEDBYTES;
  key = seedbuf + 2*SEEDBYTES;

  /* Store rho, key */
  for(i = 0; i < SEEDBYTES; ++i)
    pk[i] = rho[i];
  for(i = 0; i < SEEDBYTES; ++i)
    sk[i] = rho[i];
  for(i = 0; i < SEEDBYTES; ++i)
    sk[SEEDBYTES + i] = key[i];

  /* Sample short vectors s1 and s2 */
#ifdef DILITHIUM_USE_AES
  uint64_t nonce = 0;
  aes256ctr_ctx aesctx;
  aes256ctr_init(&aesctx, rhoprime, nonce++);
  for(i = 0; i < L; ++i) {
    poly_uniform_eta_preinit(&s1.vec[i], &aesctx);
    aesctx.n = _mm_loadl_epi64((__m128i *)&nonce);
    nonce++;
  }
  for(i = 0; i < K; ++i) {
    poly_uniform_eta_preinit(&s2.vec[i], &aesctx);
    aesctx.n = _mm_loadl_epi64((__m128i *)&nonce);
    nonce++;
  }
#elif K == 4 && L == 4
  poly_uniform_eta_4x(&s1.vec[0], &s1.vec[1], &s1.vec[2], &s1.vec[3], rhoprime, 0, 1, 2, 3);
  poly_uniform_eta_4x(&s2.vec[0], &s2.vec[1], &s2.vec[2], &s2.vec[3], rhoprime, 4, 5, 6, 7);
#elif K == 6 && L == 5
  poly_uniform_eta_4x(&s1.vec[0], &s1.vec[1], &s1.vec[2], &s1.vec[3], rhoprime, 0, 1, 2, 3);
  poly_uniform_eta_4x(&s1.vec[4], &s2.vec[0], &s2.vec[1], &s2.vec[2], rhoprime, 4, 5, 6, 7);
  poly_uniform_eta_4x(&s2.vec[3], &s2.vec[4], &s2.vec[5], &t0, rhoprime, 8, 9, 10, 11);
#elif K == 8 && L == 7
  poly_uniform_eta_4x(&s1.vec[0], &s1.vec[1], &s1.vec[2], &s1.vec[3], rhoprime, 0, 1, 2, 3);
  poly_uniform_eta_4x(&s1.vec[4], &s1.vec[5], &s1.vec[6], &s2.vec[0], rhoprime, 4, 5, 6, 7);
  poly_uniform_eta_4x(&s2.vec[1], &s2.vec[2], &s2.vec[3], &s2.vec[4], rhoprime, 8, 9, 10, 11);
  poly_uniform_eta_4x(&s2.vec[5], &s2.vec[6], &s2.vec[7], &t0, rhoprime, 12, 13, 14, 15);
#else
#error
#endif

  /* Pack secret vectors */
  for(i = 0; i < L; i++)
    polyeta_pack(sk + 2*SEEDBYTES + CRHBYTES + i*POLYETA_PACKEDBYTES, &s1.vec[i]);
  for(i = 0; i < K; i++)
    polyeta_pack(sk + 2*SEEDBYTES + CRHBYTES + (L + i)*POLYETA_PACKEDBYTES, &s2.vec[i]);

  /* Transform s1 */
  polyvecl_ntt(&s1);

  for(i = 0; i < K; i++) {
    /* Expand matrix row */
    polyvec_matrix_expand_row(mat, rho, i);

    /* Compute inner-product */
    polyvecl_pointwise_acc_montgomery(&t1, &mat[i], &s1);
    poly_invntt_tomont(&t1);

    /* Add error polynomial */
    poly_add(&t1, &t1, &s2.vec[i]);

    /* Round t and pack t1, t0 */
    poly_caddq(&t1);
    poly_power2round(&t1, &t0, &t1);
    polyt1_pack(pk + SEEDBYTES + i*POLYT1_PACKEDBYTES, &t1);
    polyt0_pack(sk + 2*SEEDBYTES + CRHBYTES + (L+K)*POLYETA_PACKEDBYTES + i*POLYT0_PACKEDBYTES, &t0);
  }

  /* Compute CRH(rho, t1) and store in secret key */
  crh(tr, pk, CRYPTO_PUBLICKEYBYTES);
  for(i = 0; i < CRHBYTES; ++i)
    sk[2*SEEDBYTES + i] = tr[i];

  return 0;
}

/*************************************************
* Name:        crypto_sign_signature
*
* Description: Computes signature.
*
* Arguments:   - uint8_t *sig: pointer to output signature (of length CRYPTO_BYTES)
*              - size_t *siglen: pointer to output length of signature
*              - uint8_t *m: pointer to message to be signed
*              - size_t mlen: length of message
*              - uint8_t *sk: pointer to bit-packed secret key
*
* Returns 0 (success)
**************************************************/
int crypto_sign_signature(uint8_t *sig,
                          size_t *siglen,
                          const uint8_t *m,
                          size_t mlen,
                          const uint8_t *sk)
{
  unsigned int i, j, n, pos;
  __attribute__((aligned(32)))
  uint8_t seedbuf[2*SEEDBYTES + 3*CRHBYTES];
  uint8_t *rho, *tr, *key, *mu, *rhoprime;
  uint8_t *hint = sig + SEEDBYTES + L*POLYZ_PACKEDBYTES;
  uint64_t nonce = 0;
  polyvecl mat[K], s1, y, z;
  polyveck t0, s2, w1, w0;
  poly cp, h;
  keccak_state state;

  rho = seedbuf;
  tr = rho + SEEDBYTES;
  key = tr + CRHBYTES;
  mu = key + SEEDBYTES;
  rhoprime = mu + CRHBYTES;
  unpack_sk(rho, tr, key, &t0, &s1, &s2, sk);

  /* Compute CRH(tr, msg) */
  shake256_init(&state);
  shake256_absorb(&state, tr, CRHBYTES);
  shake256_absorb(&state, m, mlen);
  shake256_finalize(&state);
  shake256_squeeze(mu, CRHBYTES, &state);

#ifdef DILITHIUM_RANDOMIZED_SIGNING
  randombytes(rhoprime, CRHBYTES);
#else
  crh(rhoprime, key, SEEDBYTES + CRHBYTES);
#endif

  /* Expand matrix and transform vectors */
  polyvec_matrix_expand(mat, rho);
  polyvecl_ntt(&s1);
  polyveck_ntt(&s2);
  polyveck_ntt(&t0);

#ifdef DILITHIUM_USE_AES
  aes256ctr_ctx aesctx;
  aes256ctr_init(&aesctx, rhoprime, nonce++);
#endif

rej:
  /* Sample intermediate vector y */
#ifdef DILITHIUM_USE_AES
  for(i = 0; i < L; ++i) {
    poly_uniform_gamma1_preinit(&y.vec[i], &aesctx);
    aesctx.n = _mm_loadl_epi64((__m128i *)&nonce);
    nonce++;
  }
#elif L == 4
  poly_uniform_gamma1_4x(&y.vec[0], &y.vec[1], &y.vec[2], &y.vec[3],
                         rhoprime, nonce, nonce + 1, nonce + 2, nonce + 3);
  nonce += 4;
#elif L == 5
  poly_uniform_gamma1_4x(&y.vec[0], &y.vec[1], &y.vec[2], &y.vec[3],
                         rhoprime, nonce, nonce + 1, nonce + 2, nonce + 3);
  poly_uniform_gamma1(&y.vec[4], rhoprime, nonce + 4);
  nonce += 5;
#elif L == 7
  poly_uniform_gamma1_4x(&y.vec[0], &y.vec[1], &y.vec[2], &y.vec[3],
                         rhoprime, nonce, nonce + 1, nonce + 2, nonce + 3);
  poly_uniform_gamma1_4x(&y.vec[4], &y.vec[5], &y.vec[6], &h,
                         rhoprime, nonce + 4, nonce + 5, nonce + 6, 0);
  nonce += 7;
#else
#error
#endif

  /* Save y and transform it */
  z = y;
  polyvecl_ntt(&y);

  for(i = 0; i < K; i++) {
    /* Compute inner-product */
    polyvecl_pointwise_acc_montgomery(&w1.vec[i], &mat[i], &y);
    poly_invntt_tomont(&w1.vec[i]);

    /* Decompose w and use sig as temporary buffer for packed w1 */
    poly_caddq(&w1.vec[i]);
    poly_decompose(&w1.vec[i], &w0.vec[i], &w1.vec[i]);
    polyw1_pack(sig + i*POLYW1_PACKEDBYTES, &w1.vec[i]);
  }

  /* Call the random oracle */
  shake256_init(&state);
  shake256_absorb(&state, mu, CRHBYTES);
  shake256_absorb(&state, sig, K*POLYW1_PACKEDBYTES);
  shake256_finalize(&state);
  shake256_squeeze(sig, SEEDBYTES, &state);
  poly_challenge(&cp, sig);
  poly_ntt(&cp);

  /* Compute z, reject if it reveals secret */
  for(i = 0; i < L; i++) {
    poly_pointwise_montgomery(&h, &cp, &s1.vec[i]);
    poly_invntt_tomont(&h);
    poly_add(&z.vec[i], &z.vec[i], &h);
    poly_reduce(&z.vec[i]);
    if(poly_chknorm(&z.vec[i], GAMMA1 - BETA))
      goto rej;
  }

  /* Zero hint in signature */
  n = pos = 0;
  for(i = 0; i < OMEGA + K; i++)
    hint[i] = 0;

  for(i = 0; i < K; i++) {
    /* Check that subtracting cs2 does not change high bits of w and low bits
     * do not reveal secret information */
    poly_pointwise_montgomery(&h, &cp, &s2.vec[i]);
    poly_invntt_tomont(&h);
    poly_sub(&w0.vec[i], &w0.vec[i], &h);
    poly_reduce(&w0.vec[i]);
    if(poly_chknorm(&w0.vec[i], GAMMA2 - BETA))
      goto rej;

    /* Compute hints */
    poly_pointwise_montgomery(&h, &cp, &t0.vec[i]);
    poly_invntt_tomont(&h);
    poly_reduce(&h);
    if(poly_chknorm(&h, GAMMA2))
      goto rej;

    poly_add(&w0.vec[i], &w0.vec[i], &h);
    poly_caddq(&w0.vec[i]);
    n += poly_make_hint(&h, &w0.vec[i], &w1.vec[i]);
    if(n > OMEGA)
      goto rej;

    /* Store hints in signature */
    for(j = 0; j < N; ++j)
      if(h.coeffs[j] != 0)
        hint[pos++] = j;
    hint[OMEGA + i] = pos;
  }

  /* Pack z into signature */
  for(i = 0; i < L; i++)
    polyz_pack(sig + SEEDBYTES + i*POLYZ_PACKEDBYTES, &z.vec[i]);

  *siglen = CRYPTO_BYTES;
  return 0;
}

/*************************************************
* Name:        crypto_sign
*
* Description: Compute signed message.
*
* Arguments:   - uint8_t *sm: pointer to output signed message (allocated
*                             array with CRYPTO_BYTES + mlen bytes),
*                             can be equal to m
*              - size_t *smlen: pointer to output length of signed
*                               message
*              - const uint8_t *m: pointer to message to be signed
*              - size_t mlen: length of message
*              - const uint8_t *sk: pointer to bit-packed secret key
*
* Returns 0 (success)
**************************************************/
int crypto_sign(uint8_t *sm,
                size_t *smlen,
                const uint8_t *m,
                size_t mlen,
                const uint8_t *sk)
{
  size_t i;

  for(i = 0; i < mlen; ++i)
    sm[CRYPTO_BYTES + mlen - 1 - i] = m[mlen - 1 - i];
  crypto_sign_signature(sm, smlen, sm + CRYPTO_BYTES, mlen, sk);
  *smlen += mlen;
  return 0;
}

/*************************************************
* Name:        crypto_sign_verify
*
* Description: Verifies signature.
*
* Arguments:   - uint8_t *m: pointer to input signature
*              - size_t siglen: length of signature
*              - const uint8_t *m: pointer to message
*              - size_t mlen: length of message
*              - const uint8_t *pk: pointer to bit-packed public key
*
* Returns 0 if signature could be verified correctly and -1 otherwise
**************************************************/
int crypto_sign_verify(const uint8_t *sig,
                       size_t siglen,
                       const uint8_t *m,
                       size_t mlen,
                       const uint8_t *pk)
{
  unsigned int i, j, pos = 0;
  __attribute__((aligned(32)))
  uint8_t buf[K*POLYW1_PACKEDBYTES];
  uint8_t mu[CRHBYTES];
  uint8_t c[SEEDBYTES];
  const uint8_t *hint = sig + SEEDBYTES + L*POLYZ_PACKEDBYTES;
  polyvecl mat[K], z;
  poly cp, w1, t1, h;
  keccak_state state;

  if(siglen != CRYPTO_BYTES)
    return -1;

  /* Compute CRH(CRH(rho, t1), msg) */
  crh(mu, pk, CRYPTO_PUBLICKEYBYTES);
  shake256_init(&state);
  shake256_absorb(&state, mu, CRHBYTES);
  shake256_absorb(&state, m, mlen);
  shake256_finalize(&state);
  shake256_squeeze(mu, CRHBYTES, &state);

  /* Expand challenge */
  poly_challenge(&cp, sig);
  poly_ntt(&cp);

  /* Unpack z; shortness follows from unpacking */
  for(i = 0; i < L; i++) {
    polyz_unpack(&z.vec[i], sig + SEEDBYTES + i*POLYZ_PACKEDBYTES);
    poly_ntt(&z.vec[i]);
  }

  for(i = 0; i < K; i++) {
    /* Expand matrix row */
    polyvec_matrix_expand_row(mat, pk, i);

    /* Compute i-th row of Az - c2^Dt1 */
    polyvecl_pointwise_acc_montgomery(&w1, &mat[i], &z);

    polyt1_unpack(&t1, pk + SEEDBYTES + i*POLYT1_PACKEDBYTES);
    poly_shiftl(&t1);
    poly_ntt(&t1);
    poly_pointwise_montgomery(&t1, &cp, &t1);

    poly_sub(&w1, &w1, &t1);
    poly_reduce(&w1);
    poly_invntt_tomont(&w1);

    /* Get hint polynomial and reconstruct w1 */
    for(j = 0; j < N; ++j)
      h.coeffs[j] = 0;

    if(hint[OMEGA + i] < pos || hint[OMEGA + i] > OMEGA)
      return -1;

    for(j = pos; j < hint[OMEGA + i]; ++j) {
      /* Coefficients are ordered for strong unforgeability */
      if(j > pos && hint[j] <= hint[j-1]) return -1;
      h.coeffs[hint[j]] = 1;
    }
    pos = hint[OMEGA + i];

    poly_caddq(&w1);
    poly_use_hint(&w1, &w1, &h);
    polyw1_pack(buf + i*POLYW1_PACKEDBYTES, &w1);
  }

  /* Extra indices are zero for strong unforgeability */
  for(j = pos; j < OMEGA; ++j)
    if(hint[j]) return -1;

  /* Call random oracle and verify challenge */
  shake256_init(&state);
  shake256_absorb(&state, mu, CRHBYTES);
  shake256_absorb(&state, buf, K*POLYW1_PACKEDBYTES);
  shake256_finalize(&state);
  shake256_squeeze(c, SEEDBYTES, &state);
  for(i = 0; i < SEEDBYTES; ++i)
    if(c[i] != sig[i])
      return -1;

  return 0;
}

/*************************************************
* Name:        crypto_sign_open
*
* Description: Verify signed message.
*
* Arguments:   - uint8_t *m: pointer to output message (allocated
*                            array with smlen bytes), can be equal to sm
*              - size_t *mlen: pointer to output length of message
*              - const uint8_t *sm: pointer to signed message
*              - size_t smlen: length of signed message
*              - const uint8_t *pk: pointer to bit-packed public key
*
* Returns 0 if signed message could be verified correctly and -1 otherwise
**************************************************/
int crypto_sign_open(uint8_t *m,
                     size_t *mlen,
                     const uint8_t *sm,
                     size_t smlen,
                     const uint8_t *pk)
{
  size_t i;

  if(smlen < CRYPTO_BYTES)
    goto badsig;

  *mlen = smlen - CRYPTO_BYTES;
  if(crypto_sign_verify(sm, CRYPTO_BYTES, sm + CRYPTO_BYTES, *mlen, pk))
    goto badsig;
  else {
    /* All good, copy msg, return 0 */
    for(i = 0; i < *mlen; ++i)
      m[i] = sm[CRYPTO_BYTES + i];
    return 0;
  }

badsig:
  /* Signature verification failed */
  *mlen = -1;
  for(i = 0; i < smlen; ++i)
    m[i] = 0;

  return -1;
}
