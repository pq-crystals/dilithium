#ifndef FIPS202_H
#define FIPS202_H

#include <stdint.h>

#define SHAKE128_RATE 168
#define SHAKE256_RATE 136

typedef struct {
  uint64_t s[25];
} keccak_state;

void shake128_absorb(keccak_state *state,
                     const unsigned char *input,
                     unsigned long long inlen);

void shake128_stream_init(keccak_state *sate,
                          const unsigned char *seed,
                          uint16_t nonce);

void shake128_squeezeblocks(unsigned char *output,
                            unsigned long nblocks,
                            keccak_state *state);

void shake256_absorb(keccak_state *state,
                     const unsigned char *input,
                     unsigned long long inlen);

void shake256_stream_init(keccak_state *state,
                          const unsigned char *seed,
                          uint16_t nonce);

void shake256_squeezeblocks(unsigned char *output,
                            unsigned long nblocks,
                            keccak_state *state);

void shake128(unsigned char *output,
              unsigned long long outlen,
              const unsigned char *input,
              unsigned long long inlen);

void shake256(unsigned char *output,
              unsigned long long outlen,
              const unsigned char *input,
              unsigned long long inlen);

#endif
