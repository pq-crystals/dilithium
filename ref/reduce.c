#include <stdint.h>
#include "params.h"
#include "reduce.h"

// a <= Q*2^32 = > r < 2*Q
uint32_t montgomery_reduce(uint64_t a) {
  const uint64_t qinv = QINV;
  uint64_t t;

  t = a * qinv;
  t &= (1UL << 32) - 1;
  t *= Q;
  t = a + t;
  return t >> 32;
}

// r < 2*Q
uint32_t reduce32(uint32_t a) {
  uint32_t t;

  t = a & 0x7FFFFF;
  a >>= 23;
  t += ((a << 13) - a);
  return t;
}

// r < Q
uint32_t freeze(uint32_t a) {
  a = reduce32(a);
  a -= Q;
  a += ((int32_t)a >> 31) & Q;
  return a;
}
