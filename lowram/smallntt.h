/**
 * Copyright (c) 2023 Junhao Huang (jhhuang_nuaa@126.com)
 *
 * Licensed under the Apache License, Version 2.0(the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef SMALLNTT_H
#define SMALLNTT_H

#include <stdint.h>
#include "params.h"
#include "poly.h"

/* We use the Kyber prime 3329 as the modulus for the small NTT. Other choices
such as 769 for all parameter sets or 257 for Dilithium2 and Dilithium5 are also
viable. */
#define SMALL_Q 3329
#define Q_INV_SMALL -3327

extern const int16_t small_zetas[128];

void small_ntt(int16_t r[N]);
void small_invntt_tomont(int16_t r[N]);
void small_basemul(int16_t r[2], const int16_t a[2], const int16_t b[2], int16_t zeta);

#endif
