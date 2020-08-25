#ifndef COSMOPOLITAN_LIBC_INTRIN_PMINSW_H_
#define COSMOPOLITAN_LIBC_INTRIN_PMINSW_H_
#include "libc/intrin/macros.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

void pminsw(int16_t[8], const int16_t[8], const int16_t[8]);

#define pminsw(A, B, C) \
  INTRIN_SSEVEX_X_X_X_(pminsw, SSE2, "pminsw", INTRIN_COMMUTATIVE, A, B, C)

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_INTRIN_PMINSW_H_ */
