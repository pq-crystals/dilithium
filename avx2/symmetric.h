#ifndef SYMMETRIC_H
#define SYMMETRIC_H

#include "params.h"

#ifdef DILITHIUM_90S

#include "aes256ctr.h"

#define crh(OUT, IN, INBYTES) shake256(OUT, CRHBYTES, IN, INBYTES)
#define stream128_init(STATE, SEED, NONCE) aes256ctr_init(STATE, SEED, NONCE)
#define stream128_squeezeblocks(OUT, OUTBLOCKS, STATE) aes256ctr_squeezeblocks(OUT, OUTBLOCKS, STATE)
#define stream256_init(STATE, SEED, NONCE) aes256ctr_init(STATE, SEED, NONCE)
#define stream256_squeezeblocks(OUT, OUTBLOCKS, STATE) aes256ctr_squeezeblocks(OUT, OUTBLOCKS, STATE)

#define STREAM128_BLOCKBYTES 128
#define STREAM256_BLOCKBYTES 128

typedef aes256ctr_ctx stream128_state;
typedef aes256ctr_ctx stream256_state;

#else

#include "fips202.h"
#include "fips202x4.h"

#define crh(OUT, IN, INBYTES) shake256(OUT, CRHBYTES, IN, INBYTES)
#define stream128_init(STATE, SEED, NONCE) shake128_stream_init(STATE, SEED, NONCE)
#define stream128_squeezeblocks(OUT, OUTBLOCKS, STATE) shake128_squeezeblocks(OUT, OUTBLOCKS, STATE)
#define stream256_init(STATE, SEED, NONCE) shake256_stream_init(STATE, SEED, NONCE)
#define stream256_squeezeblocks(OUT, OUTBLOCKS, STATE) shake256_squeezeblocks(OUT, OUTBLOCKS, STATE)

#define STREAM128_BLOCKBYTES SHAKE128_RATE
#define STREAM256_BLOCKBYTES SHAKE256_RATE

typedef keccak_state stream128_state;
typedef keccak_state stream256_state;

#endif

#endif
