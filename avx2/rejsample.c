#include <stdint.h>
#include <immintrin.h>
#include "params.h"
#include "test/cpucycles.h"

#ifdef DBENCH
extern const unsigned long long timing_overhead;
extern unsigned long long *tsample;
#endif

static const uint64_t idx[256] = {0, 0, 1, 256, 2, 512, 513, 131328, 3, 768, 769, 196864, 770, 197120, 197121, 50462976, 4, 1024, 1025, 262400, 1026, 262656, 262657, 67240192, 1027, 262912, 262913, 67305728, 262914, 67305984, 67305985, 17230332160, 5, 1280, 1281, 327936, 1282, 328192, 328193, 84017408, 1283, 328448, 328449, 84082944, 328450, 84083200, 84083201, 21525299456, 1284, 328704, 328705, 84148480, 328706, 84148736, 84148737, 21542076672, 328707, 84148992, 84148993, 21542142208, 84148994, 21542142464, 21542142465, 5514788471040, 6, 1536, 1537, 393472, 1538, 393728, 393729, 100794624, 1539, 393984, 393985, 100860160, 393986, 100860416, 100860417, 25820266752, 1540, 394240, 394241, 100925696, 394242, 100925952, 100925953, 25837043968, 394243, 100926208, 100926209, 25837109504, 100926210, 25837109760, 25837109761, 6614300098816, 1541, 394496, 394497, 100991232, 394498, 100991488, 100991489, 25853821184, 394499, 100991744, 100991745, 25853886720, 100991746, 25853886976, 25853886977, 6618595066112, 394500, 100992000, 100992001, 25853952256, 100992002, 25853952512, 25853952513, 6618611843328, 100992003, 25853952768, 25853952769, 6618611908864, 25853952770, 6618611909120, 6618611909121, 1694364648734976, 7, 1792, 1793, 459008, 1794, 459264, 459265, 117571840, 1795, 459520, 459521, 117637376, 459522, 117637632, 117637633, 30115234048, 1796, 459776, 459777, 117702912, 459778, 117703168, 117703169, 30132011264, 459779, 117703424, 117703425, 30132076800, 117703426, 30132077056, 30132077057, 7713811726592, 1797, 460032, 460033, 117768448, 460034, 117768704, 117768705, 30148788480, 460035, 117768960, 117768961, 30148854016, 117768962, 30148854272, 30148854273, 7718106693888, 460036, 117769216, 117769217, 30148919552, 117769218, 30148919808, 30148919809, 7718123471104, 117769219, 30148920064, 30148920065, 7718123536640, 30148920066, 7718123536896, 7718123536897, 1975839625445632, 1798, 460288, 460289, 117833984, 460290, 117834240, 117834241, 30165565696, 460291, 117834496, 117834497, 30165631232, 117834498, 30165631488, 30165631489, 7722401661184, 460292, 117834752, 117834753, 30165696768, 117834754, 30165697024, 30165697025, 7722418438400, 117834755, 30165697280, 30165697281, 7722418503936, 30165697282, 7722418504192, 7722418504193, 1976939137073408, 460293, 117835008, 117835009, 30165762304, 117835010, 30165762560, 30165762561, 7722435215616, 117835011, 30165762816, 30165762817, 7722435281152, 30165762818, 7722435281408, 7722435281409, 1976943432040704, 117835012, 30165763072, 30165763073, 7722435346688, 30165763074, 7722435346944, 7722435346945, 1976943448817920, 30165763075, 7722435347200, 7722435347201, 1976943448883456, 7722435347202, 1976943448883712, 1976943448883713, 506097522914230528};

unsigned int rej_uniform(uint32_t *r,
                         unsigned int len,
                         const unsigned char *buf,
                         unsigned int buflen)
{
  unsigned int i, ctr, pos;
  uint32_t vec[8];
  __m256i tmp0, tmp1;
  uint32_t good = 0;
  const __m256i bound = _mm256_set1_epi32(Q);
  DBENCH_START();

  ctr = pos = 0;
  while(ctr + 8 <= len && pos + 24 <= buflen) {
    for(i = 0; i < 8; i++) {
      vec[i]  = buf[pos++];
      vec[i] |= (uint32_t)buf[pos++] << 8;
      vec[i] |= (uint32_t)(buf[pos++] & 0x7F) << 16;
    }

    tmp0 = _mm256_loadu_si256((__m256i *)vec);
    tmp1 = _mm256_cmpgt_epi32(bound, tmp0);
    good = _mm256_movemask_ps((__m256)tmp1);

    __m128i rid = _mm_loadl_epi64((__m128i *)&idx[good]);
    tmp1 = _mm256_cvtepu8_epi32(rid);
    tmp0 = _mm256_permutevar8x32_epi32(tmp0, tmp1);
    _mm256_storeu_si256((__m256i *)&r[ctr], tmp0);
    ctr += __builtin_popcount(good);
  }

  while(ctr < len && pos + 3 <= buflen) {
    vec[0]  = buf[pos++];
    vec[0] |= (uint32_t)buf[pos++] << 8;
    vec[0] |= ((uint32_t)buf[pos++] & 0x7F) << 16;

    if(vec[0] < Q)
      r[ctr++] = vec[0];
  }

  DBENCH_STOP(*tsample);
  return ctr;
}

unsigned int rej_eta(uint32_t *r,
                     unsigned int len,
                     const unsigned char *buf,
                     unsigned int buflen)
{
  unsigned int i, ctr, pos;
  uint8_t vec[32];
  __m256i tmp0, tmp1;
  __m128i d0, d1, rid;
  uint32_t good = 0;
  const __m256i bound = _mm256_set1_epi8(2*ETA + 1);
  const __m256i off = _mm256_set1_epi32(Q + ETA);
  DBENCH_START();

  ctr = pos = 0;
  while(ctr + 32 <= len && pos + 16 <= buflen) {
    for(i = 0; i < 16; i++) {
#if ETA <= 3
      vec[2*i+0] = buf[pos] & 0x07;
      vec[2*i+1] = buf[pos++] >> 5;
#else
      vec[2*i+0] = buf[pos] & 0x0F;
      vec[2*i+1] = buf[pos++] >> 4;
#endif
    }

    tmp0 = _mm256_loadu_si256((__m256i *)vec);
    tmp1 = _mm256_cmpgt_epi8(bound, tmp0);
    good = _mm256_movemask_epi8(tmp1);

    d0 = _mm256_castsi256_si128(tmp0);
    rid = _mm_loadl_epi64((__m128i *)&idx[good & 0xFF]);
    d1 = _mm_shuffle_epi8(d0, rid);
    tmp1 = _mm256_cvtepu8_epi32(d1);
    tmp1 = _mm256_sub_epi32(off, tmp1);
    _mm256_storeu_si256((__m256i *)&r[ctr], tmp1);
    ctr += __builtin_popcount(good & 0xFF);

    d0 = _mm_bsrli_si128(d0, 8);
    rid = _mm_loadl_epi64((__m128i *)&idx[(good >> 8) & 0xFF]);
    d1 = _mm_shuffle_epi8(d0, rid);
    tmp1 = _mm256_cvtepu8_epi32(d1);
    tmp1 = _mm256_sub_epi32(off, tmp1);
    _mm256_storeu_si256((__m256i *)&r[ctr], tmp1);
    ctr += __builtin_popcount((good >> 8) & 0xFF);

    d0 = _mm256_extracti128_si256(tmp0, 1);
    rid = _mm_loadl_epi64((__m128i *)&idx[(good >> 16) & 0xFF]);
    d1 = _mm_shuffle_epi8(d0, rid);
    tmp1 = _mm256_cvtepu8_epi32(d1);
    tmp1 = _mm256_sub_epi32(off, tmp1);
    _mm256_storeu_si256((__m256i *)&r[ctr], tmp1);
    ctr += __builtin_popcount((good >> 16) & 0xFF);

    d0 = _mm_bsrli_si128(d0, 8);
    rid = _mm_loadl_epi64((__m128i *)&idx[(good >> 24) & 0xFF]);
    d1 = _mm_shuffle_epi8(d0, rid);
    tmp1 = _mm256_cvtepu8_epi32(d1);
    tmp1 = _mm256_sub_epi32(off, tmp1);
    _mm256_storeu_si256((__m256i *)&r[ctr], tmp1);
    ctr += __builtin_popcount((good >> 24) & 0xFF);
  }

  while(ctr < len && pos < buflen) {
#if ETA <= 3
    vec[0] = buf[pos] & 0x07;
    vec[1] = buf[pos++] >> 5;
#else
    vec[0] = buf[pos] & 0x0F;
    vec[1] = buf[pos++] >> 4;
#endif

    if(vec[0] <= 2*ETA)
      r[ctr++] = Q + ETA - vec[0];
    if(vec[1] <= 2*ETA && ctr < len)
      r[ctr++] = Q + ETA - vec[1];
  }

  DBENCH_STOP(*tsample);
  return ctr;
}

unsigned int rej_gamma1m1(uint32_t *r,
                          unsigned int len,
                          const unsigned char *buf,
                          unsigned int buflen)
{
  unsigned int i, ctr, pos;
  uint32_t vec[8];
  __m256i tmp0, tmp1;
  uint32_t good = 0;
  const __m256i bound = _mm256_set1_epi32(2*GAMMA1 - 1);
  const __m256i off = _mm256_set1_epi32(Q + GAMMA1 - 1);
  DBENCH_START();

  ctr = pos = 0;
  while(ctr + 8 <= len && pos + 20 <= buflen) {
    for(i = 0; i < 4; i++) {
      vec[2*i+0]  = buf[pos + 0];
      vec[2*i+0] |= (uint32_t)buf[pos + 1] << 8;
      vec[2*i+0] |= (uint32_t)buf[pos + 2] << 16;
      vec[2*i+0] &= 0xFFFFF;

      vec[2*i+1]  = buf[pos + 2] >> 4;
      vec[2*i+1] |= (uint32_t)buf[pos + 3] << 4;
      vec[2*i+1] |= (uint32_t)buf[pos + 4] << 12;

      pos += 5;
    }

    tmp0 = _mm256_loadu_si256((__m256i *)vec);
    tmp1 = _mm256_cmpgt_epi32(bound, tmp0);
    good = _mm256_movemask_ps((__m256)tmp1);
    tmp0 = _mm256_sub_epi32(off, tmp0);

    __m128i rid = _mm_loadl_epi64((__m128i *)&idx[good]);
    tmp1 = _mm256_cvtepu8_epi32(rid);
    tmp0 = _mm256_permutevar8x32_epi32(tmp0, tmp1);
    _mm256_storeu_si256((__m256i *)&r[ctr], tmp0);
    ctr += __builtin_popcount(good);
  }

  while(ctr < len && pos + 5 <= buflen) {
    vec[0]  = buf[pos + 0];
    vec[0] |= (uint32_t)buf[pos + 1] << 8;
    vec[0] |= (uint32_t)buf[pos + 2] << 16;
    vec[0] &= 0xFFFFF;

    vec[1]  = buf[pos + 2] >> 4;
    vec[1] |= (uint32_t)buf[pos + 3] << 4;
    vec[1] |= (uint32_t)buf[pos + 4] << 12;

    pos += 5;
    
    if(vec[0] <= 2*GAMMA1 - 2)
      r[ctr++] = Q + GAMMA1 - 1 - vec[0];
    if(vec[1] <= 2*GAMMA1 - 2 && ctr < len)
      r[ctr++] = Q + GAMMA1 - 1 - vec[1];
  }

  DBENCH_STOP(*tsample);
  return ctr;
}
