/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ This program is free software; you can redistribute it and/or modify         │
│ it under the terms of the GNU General Public License as published by         │
│ the Free Software Foundation; version 2 of the License.                      │
│                                                                              │
│ This program is distributed in the hope that it will be useful, but          │
│ WITHOUT ANY WARRANTY; without even the implied warranty of                   │
│ MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU             │
│ General Public License for more details.                                     │
│                                                                              │
│ You should have received a copy of the GNU General Public License            │
│ along with this program; if not, write to the Free Software                  │
│ Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA                │
│ 02110-1301 USA                                                               │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/assert.h"
#include "libc/limits.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/testlib/testlib.h"
#include "test/tool/build/lib/optest.h"
#include "tool/build/lib/alu.h"
#include "tool/build/lib/flags.h"

#define NATIVE_ALU2(MODE, INSTRUCTION)                     \
  asm("pushf\n\t"                                          \
      "andl\t%3,(%%rsp)\n\t"                               \
      "orl\t%4,(%%rsp)\n\t"                                \
      "popf\n\t" INSTRUCTION "\t%" MODE "2,%" MODE "0\n\t" \
      "pushf\n\t"                                          \
      "pop\t%q1"                                           \
      : "+r"(x), "=rm"(*f)                                 \
      : "r"(y), "i"(~FMASK), "r"(*f & FMASK)               \
      : "cc")

#define NATIVE_ALU2_ANYBITS(INSTRUCTION, MUTATING) \
  switch (w) {                                     \
    case 0:                                        \
      NATIVE_ALU2("b", INSTRUCTION);               \
      if (MUTATING) x &= 0xff;                     \
      return x;                                    \
    case 1:                                        \
      NATIVE_ALU2("w", INSTRUCTION);               \
      if (MUTATING) x &= 0xffff;                   \
      return x;                                    \
    case 2:                                        \
      NATIVE_ALU2("k", INSTRUCTION);               \
      return x;                                    \
    case 3:                                        \
      NATIVE_ALU2("q", INSTRUCTION);               \
      return x;                                    \
    default:                                       \
      abort();                                     \
  }

int64_t RunGolden(char w, int h, uint64_t x, uint64_t y, uint32_t *f) {
  bool rw;
  rw = !(h & ALU_TEST);
  switch (h & 7) {
    case ALU_OR:
      NATIVE_ALU2_ANYBITS("or", rw);
    case ALU_AND:
      if (rw) {
        NATIVE_ALU2_ANYBITS("and", rw);
      } else {
        NATIVE_ALU2_ANYBITS("test", rw);
      }
    case ALU_XOR:
      NATIVE_ALU2_ANYBITS("xor", rw);
    case ALU_SBB:
      NATIVE_ALU2_ANYBITS("sbb", rw);
    case ALU_CMP:
      rw = false;
      NATIVE_ALU2_ANYBITS("cmp", rw);
    case ALU_SUB:
      NATIVE_ALU2_ANYBITS("sub", rw);
    case ALU_ADC:
      NATIVE_ALU2_ANYBITS("adc", rw);
    case ALU_ADD:
      NATIVE_ALU2_ANYBITS("add", rw);
    default:
      abort();
  }
}

const uint8_t kAluOps[] = {
    ALU_ADD, ALU_OR, ALU_ADC, ALU_SBB, ALU_AND, ALU_SUB, ALU_XOR, ALU_CMP, ALU_AND | ALU_TEST,
};

const char *const kAluNames[] = {
    [ALU_ADD] = "add", [ALU_OR] = "or",   [ALU_ADC] = "adc",
    [ALU_SBB] = "sbb", [ALU_AND] = "and", [ALU_SUB] = "sub",
    [ALU_XOR] = "xor", [ALU_CMP] = "cmp", [ALU_AND | ALU_TEST] = "test",
};

int64_t RunOpTest(char w, int h, uint64_t x, uint64_t y, uint32_t *f) {
  return Alu(w, h, x, y, f);
}

TEST(alu, test) {
  RunOpTests(kAluOps, ARRAYLEN(kAluOps), kAluNames);
}
