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

#ifndef __ASSEMBLER__

#define qdata NAMESPACE(qdata)
extern const uint32_t qdata[];

#endif

#endif
