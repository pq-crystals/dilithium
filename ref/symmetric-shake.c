#include <stdint.h>
#include "params.h"
#include "symmetric.h"
#include "fips202.h"

void dilithium_shake128_stream_init(keccak_state *state,
                                    const uint8_t seed[SEEDBYTES],
                                    uint16_t nonce)
{
  unsigned int i;
  uint8_t buf[SEEDBYTES + 2];

  for(i = 0; i < SEEDBYTES; ++i)
    buf[i] = seed[i];
  buf[SEEDBYTES] = nonce;
  buf[SEEDBYTES+1] = nonce >> 8;

  shake128_absorb(state, buf, sizeof(buf));
}

void dilithium_shake256_stream_init(keccak_state *state,
                                    const uint8_t seed[CRHBYTES],
                                    uint16_t nonce)
{
  unsigned int i;
  uint8_t buf[CRHBYTES + 2];

  for(i = 0; i < CRHBYTES; ++i)
    buf[i] = seed[i];
  buf[CRHBYTES] = nonce;
  buf[CRHBYTES+1] = nonce >> 8;

  shake256_absorb(state, buf, sizeof(buf));
}
