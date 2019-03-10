#ifndef REJSAMPLE_H
#define REJSAMPLE_H

#include <stdint.h>

unsigned int rej_uniform(uint32_t *r,
                         unsigned int len,
                         const unsigned char *buf,
                         unsigned int buflen);
unsigned int rej_eta(uint32_t *r,
                     unsigned int len,
                     const unsigned char *buf,
                     unsigned int buflen);
unsigned int rej_gamma1m1(uint32_t *r,
                          unsigned int len,
                          const unsigned char *buf,
                          unsigned int buflen);

#endif
