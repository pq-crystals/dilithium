#ifndef REJSAMPLE_H
#define REJSAMPLE_H

#include "params.h"
#include <stdint.h>

#define rej_uniform_avx NAMESPACE(rej_uniform_avx)
unsigned int rej_uniform_avx(uint32_t *r,
                             unsigned int len,
                             const uint8_t *buf,
                             unsigned int buflen);


#define rej_eta_avx NAMESPACE(rej_eta_avx)
unsigned int rej_eta_avx(uint32_t *r,
                         unsigned int len,
                         const uint8_t *buf,
                         unsigned int buflen);

#define rej_gamma1m1_avx NAMESPACE(rej_uniform_gamma1m1_avx)
unsigned int rej_gamma1m1_avx(uint32_t *r,
                              unsigned int len,
                              const uint8_t *buf,
                              unsigned int buflen);

#endif
