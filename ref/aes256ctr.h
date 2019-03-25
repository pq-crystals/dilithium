#ifndef AES256CTR_H
#define AES256CTR_H

#include <stdint.h>

typedef struct {
  uint64_t sk_exp[120];
  uint32_t ivw[16];
} aes256ctr_ctx;

void aes256ctr_init(aes256ctr_ctx *s, const unsigned char *key, uint16_t nonce);
void aes256ctr_squeezeblocks(unsigned char *out, unsigned long long nblocks, aes256ctr_ctx *s);

#endif
