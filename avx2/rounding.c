#include <stdint.h>

extern uint32_t power2round(const uint32_t a, uint32_t *a0);
extern uint32_t decompose(uint32_t a, uint32_t *a0);
extern unsigned int make_hint(const uint32_t a0, const uint32_t a1);
extern uint32_t use_hint(const uint32_t a, const unsigned int hint);
