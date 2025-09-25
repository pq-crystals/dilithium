#include "params.h"
#include "packing.h"
#include "polyvec.h"
#include "poly.h"









/*************************************************
* Name:        pack_sig_h
*
* Description: Pack only h into signature.
*              This function is part of the lowram implementation.
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
*              This function is part of the lowram implementation.
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
 * Name:        unpack_sig_h_indices
 *
 * Description: Unpack only h from signature sig = (c, z, h).
 *              This function is part of the lowram implementation.
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
 * Name:        pack_pk_rho
 *
 * Description: Bit-pack only rho in public key pk = (rho, t1).
 *              This function is part of the lowram implementation.
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
 *              This function is part of the lowram implementation.
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
 *              This function is part of the lowram implementation.
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
 *              This function is part of the lowram implementation.
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
 *              This function is part of the lowram implementation.
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
 *              This function is part of the lowram implementation.
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
 *              This function is part of the lowram implementation.
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
 * Name:        unpack_sk_s1
 *
 * Description: Unpack only s1 from the secret key into a small polynomial.
 *              This function is part of the lowram implementation.
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
 *              This function is part of the lowram implementation.
 *
 * Arguments:   - smallpoly *a: output small polynomial to store the unpacked data
 *              - const uint8_t *sk: input secret key buffer
 *              - unsigned int idx: index specifying the polynomial to unpack
 *
 **************************************************/
void unpack_sk_s2(smallpoly *a, const uint8_t *sk, unsigned int idx) {
  small_polyeta_unpack(a, sk + 2 * SEEDBYTES + TRBYTES + L * POLYETA_PACKEDBYTES + idx * POLYETA_PACKEDBYTES);
}
