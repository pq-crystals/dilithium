#include "params.h"
#include "poly.h"
#include "polyvec.h"
#include "packing.h"

void pack_pk(unsigned char pk[PK_SIZE_PACKED],
             const unsigned char rho[SEEDBYTES],
             const polyveck *t1)
{
  unsigned int i;

  for(i = 0; i < SEEDBYTES; ++i)
    pk[i] = rho[i];
  pk += SEEDBYTES;

  for(i = 0; i < K; ++i)
    polyt1_pack(pk + i*POLT1_SIZE_PACKED, t1->vec+i);
}

void unpack_pk(unsigned char rho[SEEDBYTES], polyveck *t1,
               const unsigned char pk[PK_SIZE_PACKED])
{
  unsigned int i;

  for(i = 0; i < SEEDBYTES; ++i)
    rho[i] = pk[i];
  pk += SEEDBYTES;

  for(i = 0; i < K; ++i)
    polyt1_unpack(t1->vec+i, pk + i*POLT1_SIZE_PACKED);
}

void pack_sk(unsigned char sk[SK_SIZE_PACKED],
             const unsigned char rho[SEEDBYTES],
             const unsigned char key[SEEDBYTES],
             const unsigned char tr[CRHBYTES],
             const polyvecl *s1,
             const polyveck *s2,
             const polyveck *t0)
{
  unsigned int i;

  for(i = 0; i < SEEDBYTES; ++i)
    sk[i] = rho[i];
  sk += SEEDBYTES;

  for(i = 0; i < SEEDBYTES; ++i)
    sk[i] = key[i];
  sk += SEEDBYTES;

  for(i = 0; i < CRHBYTES; ++i)
    sk[i] = tr[i];
  sk += CRHBYTES;

  for(i = 0; i < L; ++i)
    polyeta_pack(sk + i*POLETA_SIZE_PACKED, s1->vec+i);
  sk += L*POLETA_SIZE_PACKED;

  for(i = 0; i < K; ++i)
    polyeta_pack(sk + i*POLETA_SIZE_PACKED, s2->vec+i);
  sk += K*POLETA_SIZE_PACKED;

  for(i = 0; i < K; ++i)
    polyt0_pack(sk + i*POLT0_SIZE_PACKED, t0->vec+i);
}

void unpack_sk(unsigned char rho[SEEDBYTES],
               unsigned char key[SEEDBYTES],
               unsigned char tr[CRHBYTES],
               polyvecl *s1,
               polyveck *s2,
               polyveck *t0,
               const unsigned char *sk)
{
  unsigned int i;

  for(i = 0; i < SEEDBYTES; ++i)
    rho[i] = sk[i];
  sk += SEEDBYTES;

  for(i = 0; i < SEEDBYTES; ++i)
    key[i] = sk[i];
  sk += SEEDBYTES;

  for(i = 0; i < CRHBYTES; ++i)
    tr[i] = sk[i];
  sk += CRHBYTES;

  for(i=0; i < L; ++i)
    polyeta_unpack(s1->vec+i, sk + i*POLETA_SIZE_PACKED);
  sk += L*POLETA_SIZE_PACKED;
  
  for(i=0; i < K; ++i)
    polyeta_unpack(s2->vec+i, sk + i*POLETA_SIZE_PACKED);
  sk += K*POLETA_SIZE_PACKED;

  for(i=0; i < K; ++i)
    polyt0_unpack(t0->vec+i, sk + i*POLT0_SIZE_PACKED);
}

void pack_sig(unsigned char sig[SIG_SIZE_PACKED],
              const polyvecl *z, const polyveck *h, const poly *c)
{
  unsigned int i, j, k;
  uint64_t signs, mask;

  for(i = 0; i < L; ++i)
    polyz_pack(sig + i*POLZ_SIZE_PACKED, z->vec+i);
  sig += L*POLZ_SIZE_PACKED;

  /* Encode h */
  k = 0;
  for(i = 0; i < K; ++i) {
    for(j = 0; j < N; ++j)
      if(h->vec[i].coeffs[j] == 1)
        sig[k++] = j;

    sig[OMEGA + i] = k;
  }
  while(k < OMEGA) sig[k++] = 0;
  sig += OMEGA + K;
  
  /* Encode c */
  signs = 0;
  mask = 1;
  for(i = 0; i < N/8; ++i) {
    sig[i] = 0;
    for(j = 0; j < 8; ++j) {
      if(c->coeffs[8*i+j] != 0) {
        sig[i] |= (1 << j);
        if(c->coeffs[8*i+j] == (Q - 1)) signs |= mask;
        mask <<= 1;
      }
    }
  }
  sig += N/8;
  for(i = 0; i < 8; ++i)
    sig[i] = signs >> 8*i;
}

void unpack_sig(polyvecl *z, polyveck *h, poly *c,
                const unsigned char sig[SIG_SIZE_PACKED])
{
  unsigned int i, j, k;
  uint64_t signs, mask;

  for(i = 0; i < L; ++i)
    polyz_unpack(z->vec+i, sig + i*POLZ_SIZE_PACKED);
  sig += L*POLZ_SIZE_PACKED;

  /* Decode h */
  k = 0;
  for(i = 0; i < K; ++i) {
    for(j = 0; j < N; ++j)
      h->vec[i].coeffs[j] = 0;

    for(j = k; j < sig[OMEGA + i]; ++j)
      h->vec[i].coeffs[sig[j]] = 1;

    k = sig[OMEGA + i];
  }
  sig += OMEGA + K;

  /* Decode c */
  for(i = 0; i < N; ++i)
    c->coeffs[i] = 0;

  signs = 0;
  for(i = 0; i < 8; ++i)
    signs |= (uint64_t)sig[N/8+i] << 8*i;

  mask = 1;
  for(i = 0; i < N/8; ++i) {
    for(j = 0; j < 8; ++j) {
      if((sig[i] >> j) & 0x01) {
        c->coeffs[8*i+j] = (signs & mask) ? Q - 1 : 1;
        mask <<= 1;
      }
    }
  }
}
