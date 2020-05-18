#include <stdint.h>
#include "params.h"
#include "symmetric.h"
#include "fips202.h"

void dilithium_shake128_stream_init(keccak_state *state,
                                    const uint8_t seed[SEEDBYTES],
                                    uint16_t nonce)
{
  shake128_init(state);
  shake128_absorb(state, seed, SEEDBYTES);
  shake128_absorb(state, (uint8_t *)&nonce, 2);
  shake128_finalize(state);
}

void dilithium_shake256_stream_init(keccak_state *state,
                                    const uint8_t seed[CRHBYTES],
                                    uint16_t nonce)
{
  shake256_init(state);
  shake256_absorb(state, seed, CRHBYTES);
  shake256_absorb(state, (uint8_t *)&nonce, 2);
  shake256_finalize(state);
}
