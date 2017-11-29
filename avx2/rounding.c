#include <stdint.h>
#include "params.h"
#include "reduce.h"

uint32_t power2round(uint32_t a, uint32_t *a0)  {
  int32_t t;
  /* Centralized remainder mod 2^D */
  t = a & ((1 << D) - 1);
  t -= (1 << (D-1)) + 1;
  t += (t >> 31) & (1 << D);
  t -= (1 << (D-1)) - 1;
  *a0 = Q + t;
  a = (a - t) >> D;
  return a;
}

#if GAMMA1 != (Q-1)/16
#error "decompose assumes GAMMA1 == (Q-1)/16"
#endif

uint32_t decompose(uint32_t a, uint32_t *a0) {
  int32_t t, u;
  /* Centralized remainder mod ALPHA */
  t = a & 0x7FFFF;
  t += (a >> 19) << 9;
  t -= ALPHA/2 + 1;
  t += (t >> 31) & ALPHA;
  t -= ALPHA/2 - 1;
  a -= t;

  /* Divide by ALPHA (possible to avoid) */
  u = a - 1;
  u >>= 31;
  a = (a >> 19) + 1;
  a -= u & 1;

  /* Border case */
  *a0 = Q + t - (a >> 4);
  a &= 0xF;
  return a;
}

unsigned int make_hint(const uint32_t a, const uint32_t b) {
  uint32_t t;
  return decompose(a, &t) != decompose(freeze(a + b), &t);
}

uint32_t use_hint(const uint32_t a, const unsigned int hint) {
  uint32_t a0, a1;
  
  a1 = decompose(a, &a0);
  if(hint == 0)
    return a1;
  else if(a0 > Q)
    return (a1 == (Q - 1)/ALPHA - 1) ? 0 : a1 + 1;
  else
    return (a1 == 0) ? (Q - 1)/ALPHA - 1 : a1 - 1;

  /* If decompose does not divide out ALPHA: 
  if(hint == 0)
    return a1;
  else if(a0 > Q)
    return (a1 == Q - 1 - ALPHA) ? 0 : a1 + ALPHA;
  else
    return (a1 == 0) ? Q - 1 - ALPHA : a1 - ALPHA;
  */
}
