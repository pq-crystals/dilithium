#ifndef CONSTS_H
#define CONSTS_H

#include "params.h"

#define _8XQINV      0
#define _8XQ         8
#define _8X2Q       16
#define _8X256Q     24
#define _MASK       32
#define _8X23ONES   40
#define _8XDIV      48
#define _ZETAS      56
#define _ZETAS_INV 312

/* The C ABI on MacOS exports all symbols with a leading
 * underscore. This means that any symbols we refer to from
 * C files (functions) can't be found, and all symbols we
 * refer to from ASM also can't be found.
 *
 * This define helps us get around this
 */
#if defined(__WIN32__) || defined(__APPLE__)
#define decorate(s) _##s
#define cdecl2(s) decorate(s)
#define cdecl(s) cdecl2(DILITHIUM_NAMESPACE(##s))
#else
#define cdecl(s) DILITHIUM_NAMESPACE(##s)
#endif

#ifndef __ASSEMBLER__
#define qdata DILITHIUM_NAMESPACE(qdata)
extern const uint32_t qdata[];
#endif

#endif
