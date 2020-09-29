#ifndef REDUCE_H
#define REDUCE_H

#include <stdint.h>
#include "params.h"

#define MONT -4186625 // 2^32 % Q
#define QINV 58728449 // q^(-1) mod 2^32

/* a <= Q*2^32 => r < 2*Q */
#define montgomery_reduce DILITHIUM_NAMESPACE(_montgomery_reduce)
int32_t montgomery_reduce(int64_t a);

/* r < 2*Q */
#define reduce32 DILITHIUM_NAMESPACE(_reduce32)
int32_t reduce32(int32_t a);

/* a < 2*Q => r < Q */
#define caddq DILITHIUM_NAMESPACE(_caddq)
int32_t caddq(int32_t a);

/* r < Q */
#define freeze DILITHIUM_NAMESPACE(_freeze)
int32_t freeze(int32_t a);

#endif
