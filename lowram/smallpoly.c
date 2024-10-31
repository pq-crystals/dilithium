#include "smallpoly.h"
#include "smallntt.h"

void poly_small_ntt_copy(smallpoly *out, poly *in)
{
  for (int i = N - 1; i >= 0; i--)
  {
    out->coeffs[i] = in->coeffs[i];
  }
  small_ntt(out->coeffs);
}

void polyvecl_small_ntt(smallpoly v[L])
{
  unsigned int i;

  for (i = 0; i < L; ++i)
    small_ntt(v[i].coeffs);
}

void polyveck_small_ntt(smallpoly v[K])
{
  unsigned int i;

  for (i = 0; i < K; ++i)
    small_ntt(v[i].coeffs);
}

void poly_small_basemul(int16_t r[N], const int16_t a[N], const int16_t b[N])
{
  unsigned int i;
  for (i = 0; i < N / 4; i++)
  {
    small_basemul(&r[4 * i], &a[4 * i], &b[4 * i], small_zetas[64 + i]);
    small_basemul(&r[4 * i + 2], &a[4 * i + 2], &b[4 * i + 2], -small_zetas[64 + i]);
  }
}

void poly_small_basemul_invntt(poly *r, const smallpoly *a, const smallpoly *b)
{
  // re-use the buffer
  smallpoly *tmp = (smallpoly *)r;
  poly_small_basemul(tmp->coeffs, a->coeffs, b->coeffs);

  small_invntt_tomont(tmp->coeffs);

  int j;
  // buffer is the same, so we neeed to be careful
  for (j = N - 1; j >= 0; j--)
  {
    r->coeffs[j] = tmp->coeffs[j];
  }
}

void polyvecl_small_basemul_invntt(polyvecl *r, const smallpoly *a, const smallpoly b[L])
{
  unsigned int i;
  for (i = 0; i < L; i++)
  {
    poly_small_basemul_invntt(&r->vec[i], a, &b[i]);
  }
}

void small_polyeta_unpack(smallpoly *r, const uint8_t *a)
{
  unsigned int i;

#if ETA == 2
  for (i = 0; i < N / 8; ++i)
  {
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
  for (i = 0; i < N / 2; ++i)
  {
    r->coeffs[2 * i + 0] = a[i] & 0x0F;
    r->coeffs[2 * i + 1] = a[i] >> 4;
    r->coeffs[2 * i + 0] = ETA - r->coeffs[2 * i + 0];
    r->coeffs[2 * i + 1] = ETA - r->coeffs[2 * i + 1];
  }
#endif
}
