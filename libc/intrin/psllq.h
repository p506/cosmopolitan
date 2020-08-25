#ifndef COSMOPOLITAN_LIBC_INTRIN_PSLLQ_H_
#define COSMOPOLITAN_LIBC_INTRIN_PSLLQ_H_
#include "libc/intrin/macros.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

void psllq(uint64_t[2], const uint64_t[2], unsigned char);
void psllqv(uint64_t[2], const uint64_t[2], const uint64_t[2]);

#define psllq(A, B, I) INTRIN_SSEVEX_X_I_(psllq, SSE2, "psllq", A, B, I)
#define psllqv(A, B, C) \
  INTRIN_SSEVEX_X_X_X_(psllqv, SSE2, "psllq", INTRIN_NONCOMMUTATIVE, A, B, C)

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_INTRIN_PSLLQ_H_ */
