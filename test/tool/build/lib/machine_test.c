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
#include "libc/bits/progn.h"
#include "libc/fmt/bing.h"
#include "libc/math.h"
#include "libc/mem/mem.h"
#include "libc/runtime/gc.h"
#include "libc/stdio/stdio.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/testlib.h"
#include "libc/x/x.h"
#include "tool/build/lib/endian.h"
#include "tool/build/lib/fpu.h"
#include "tool/build/lib/machine.h"
#include "tool/build/lib/memory.h"

const uint8_t kPi80[] = {
    0xd9, 0xe8,                          // fld1
    0xb8, 0x0a, 0x00, 0x00, 0x00,        // mov    $0xa,%eax
    0x31, 0xd2,                          // xor    %edx,%edx
    0xd9, 0xee,                          // fldz
    0x48, 0x98,                          // cltq
    0x48, 0x39, 0xc2,                    // cmp    %rax,%rdx
    0xd9, 0x05, 0x1a, 0x00, 0x00, 0x00,  // flds   0x1a(%rip)
    0x7d, 0x13,                          // jge    2b <pi80+0x2b>
    0xde, 0xc1,                          // faddp
    0x48, 0xff, 0xc2,                    // inc    %rdx
    0xd9, 0xfa,                          // fsqrt
    0xd9, 0x05, 0x0f, 0x00, 0x00, 0x00,  // flds   15(%rip)
    0xd8, 0xc9,                          // fmul   %st(1),%st
    0xde, 0xca,                          // fmulp  %st,%st(2)
    0xeb, 0xe2,                          // jmp    d <pi80+0xd>
    0xdd, 0xd9,                          // fstp   %st(1)
    0xde, 0xf1,                          // fdivp
    0xf4,                                // hlt
    0x00, 0x00, 0x00, 0x40,              // .float 2.0
    0x00, 0x00, 0x00, 0x3f,              // .float 0.5
};

const uint8_t kTenthprime[] = {
    0x31, 0xd2,                    // xor    %edx,%edx
    0x45, 0x31, 0xc0,              // xor    %r8d,%r8d
    0x31, 0xc9,                    // xor    %ecx,%ecx
    0xbe, 0x03, 0x00, 0x00, 0x00,  // mov    $0x3,%esi
    0x41, 0xff, 0xc0,              // inc    %r8d
    0x44, 0x89, 0xc0,              // mov    %r8d,%eax
    0x83, 0xf9, 0x0a,              // cmp    $0xa,%ecx
    0x74, 0x0b,                    // je     20
    0x99,                          // cltd
    0xf7, 0xfe,                    // idiv   %esi
    0x83, 0xfa, 0x01,              // cmp    $0x1,%edx
    0x83, 0xd9, 0xff,              // sbb    $-1,%ecx
    0xeb, 0xea,                    // jmp    a
    0xf4,                          // hlt
};

const uint8_t kTenthprime2[] = {
    0xE8, 0x11, 0x00, 0x00, 0x00,  //
    0xF4,                          //
    0x89, 0xF8,                    //
    0xB9, 0x03, 0x00, 0x00, 0x00,  //
    0x99,                          //
    0xF7, 0xF9,                    //
    0x85, 0xD2,                    //
    0x0F, 0x95, 0xC0,              //
    0xC3,                          //
    0x55,                          //
    0x48, 0x89, 0xE5,              //
    0x31, 0xF6,                    //
    0x45, 0x31, 0xC0,              //
    0x44, 0x89, 0xC7,              //
    0xE8, 0xDF, 0xFF, 0xFF, 0xFF,  //
    0x0F, 0xB6, 0xC0,              //
    0x66, 0x83, 0xF8, 0x01,        //
    0x83, 0xDE, 0xFF,              //
    0x41, 0xFF, 0xC0,              //
    0x83, 0xFE, 0x0A,              //
    0x75, 0xE6,                    //
    0x44, 0x89, 0xC0,              //
    0x5D,                          //
    0xC3,                          //
};

int64_t base;
uint8_t *real;
size_t realsize;
struct Machine *m;

void SetUp(void) {
  base = 0;
  m = NewMachine();
  realsize = 0x10000;
  real = tmemalign(PAGESIZE, ROUNDUP(realsize, PAGESIZE));
  RegisterMemory(m, base, real, realsize);
  m->ip = base;
  Write64(m->sp, m->ip + realsize);
}

void TearDown(void) {
  ResetRam(m);
  tfree(real);
  free(m);
}

int ExecuteUntilHalt(struct Machine *m) {
  int rc;
  if (!(rc = setjmp(m->onhalt))) {
    for (;;) {
      LoadInstruction(m);
      ExecuteInstruction(m);
    }
  } else {
    return rc;
  }
}

TEST(machine, test) {
  memcpy(real, kTenthprime, sizeof(kTenthprime));
  ASSERT_EQ(kMachineHalt, ExecuteUntilHalt(m));
  ASSERT_EQ(15, Read32(m->ax));
}

TEST(machine, testFpu) {
  memcpy(real, kPi80, sizeof(kPi80));
  ASSERT_EQ(kMachineHalt, ExecuteUntilHalt(m));
  ASSERT_TRUE(fabs(3.14159 - FpuPop(m)) < 0.0001);
  m->ip = base;
  ASSERT_EQ(kMachineHalt, ExecuteUntilHalt(m));
  ASSERT_TRUE(fabs(3.14159 - FpuPop(m)) < 0.0001);
}

BENCH(machine, benchPrimeNumberPrograms) {
  memcpy(real, kTenthprime2, sizeof(kTenthprime2));
  EZBENCH2("tenthprime2", m->ip = base, ExecuteUntilHalt(m));
  ASSERT_EQ(15, Read32(m->ax));
  memcpy(real, kTenthprime, sizeof(kTenthprime));
  EZBENCH2("tenthprime", m->ip = base, ExecuteUntilHalt(m));
  ASSERT_EQ(15, Read32(m->ax));
}

BENCH(machine, benchFpu) {
  memcpy(real, kPi80, sizeof(kPi80));
  EZBENCH2("pi80", m->ip = base, PROGN(ExecuteUntilHalt(m), FpuPop(m)));
}

BENCH(machine, benchLoadExec2) {
  uint8_t kMovCode[] = {0xbe, 0x03, 0x00, 0x00, 0x00};
  memcpy(real, kMovCode, sizeof(kMovCode));
  LoadInstruction(m);
  EZBENCH2("mov", m->ip = base, ExecuteInstruction(m));
}

BENCH(machine, benchLoadExec3) {
  uint8_t kMovdCode[] = {0x66, 0x0f, 0x6e, 0xc0};
  Write64(m->ax, 0);
  memcpy(real, kMovdCode, sizeof(kMovdCode));
  LoadInstruction(m);
  EZBENCH2("movd", m->ip = base, ExecuteInstruction(m));
}

BENCH(machine, benchLoadExec4) {
  uint8_t kAddpsRegregCode[] = {0x0f, 0x58, 0xC0};
  uint8_t kAddpsMemregCode[] = {0x0f, 0x58, 0x00};
  Write64(m->ax, 0);
  memcpy(real, kAddpsRegregCode, sizeof(kAddpsRegregCode));
  LoadInstruction(m);
  EZBENCH2("addps reg reg", m->ip = base, ExecuteInstruction(m));
  memcpy(real, kAddpsMemregCode, sizeof(kAddpsMemregCode));
  LoadInstruction(m);
  EZBENCH2("addps mem reg", m->ip = base, ExecuteInstruction(m));
}

BENCH(machine, benchLoadExec5) {
  uint8_t kPaddwRegregCode[] = {0x66, 0x0F, 0xFD, 0xC0};
  uint8_t kPaddwMemregCode[] = {0x66, 0x0F, 0xFD, 0x00};
  Write64(m->ax, 0);
  memcpy(real, kPaddwRegregCode, sizeof(kPaddwRegregCode));
  LoadInstruction(m);
  EZBENCH2("paddw", m->ip = base, ExecuteInstruction(m));
  memcpy(real, kPaddwMemregCode, sizeof(kPaddwMemregCode));
  LoadInstruction(m);
  EZBENCH2("paddw mem", m->ip = base, ExecuteInstruction(m));
}

BENCH(machine, benchLoadExec6) {
  uint8_t kPsubqRegregCode[] = {0x66, 0x0F, 0xFB, 0xC0};
  uint8_t kPsubqMemregCode[] = {0x66, 0x0F, 0xFB, 0x00};
  Write64(m->ax, 0);
  memcpy(real, kPsubqRegregCode, sizeof(kPsubqRegregCode));
  LoadInstruction(m);
  EZBENCH2("psubq", m->ip = base, ExecuteInstruction(m));
  memcpy(real, kPsubqMemregCode, sizeof(kPsubqMemregCode));
  LoadInstruction(m);
  EZBENCH2("psubq mem", m->ip = base, ExecuteInstruction(m));
}

BENCH(machine, benchAddqMem) {
  uint8_t kAddMemregCode[] = {0x48, 0x03, 0x08};
  Write64(m->ax, 0);
  memcpy(real, kAddMemregCode, sizeof(kAddMemregCode));
  LoadInstruction(m);
  EZBENCH2("addq mem", m->ip = base, ExecuteInstruction(m));
}

BENCH(machine, benchAddlMem) {
  uint8_t kAddMemregCode[] = {0x03, 0x08};
  Write64(m->ax, 0);
  memcpy(real, kAddMemregCode, sizeof(kAddMemregCode));
  LoadInstruction(m);
  EZBENCH2("addl mem", m->ip = base, ExecuteInstruction(m));
}

BENCH(machine, benchAddq) {
  uint8_t kAddqCode[] = {0x48, 0x01, 0xd8};
  Write64(m->ax, 0);
  memcpy(real, kAddqCode, sizeof(kAddqCode));
  LoadInstruction(m);
  EZBENCH2("addq", m->ip = base, ExecuteInstruction(m));
}

BENCH(machine, benchAddb) {
  uint8_t kAddbCode[] = {0x00, 0xd8};
  Write64(m->ax, 0);
  memcpy(real, kAddbCode, sizeof(kAddbCode));
  LoadInstruction(m);
  EZBENCH2("addb", m->ip = base, ExecuteInstruction(m));
}

BENCH(machine, benchXorReg) {
  memcpy(real, kTenthprime, sizeof(kTenthprime));
  LoadInstruction(m);
  EZBENCH2("xor", m->ip = base, ExecuteInstruction(m));
}

BENCH(machine, benchLoadExec8) {
  uint8_t kFchsCode[] = {0xd9, 0xe0};
  Write64(m->ax, 0);
  OpFinit(m);
  *FpuSt(m, 0) = M_PI;
  FpuSetTag(m, 0, kFpuTagValid);
  memcpy(real, kFchsCode, sizeof(kFchsCode));
  LoadInstruction(m);
  EZBENCH2("fchs", m->ip = base, ExecuteInstruction(m));
}

BENCH(machine, benchPushpop) {
  uint8_t kPushpop[] = {0x50, 0x58};
  Write64(m->ax, 0);
  memcpy(real, kPushpop, sizeof(kPushpop));
  EZBENCH2("pushpop", m->ip = base,
           PROGN(LoadInstruction(m), ExecuteInstruction(m), LoadInstruction(m),
                 ExecuteInstruction(m)));
}

BENCH(machine, benchPause) {
  uint8_t kPause[] = {0xf3, 0x90};
  Write64(m->ax, 0);
  memcpy(real, kPause, sizeof(kPause));
  LoadInstruction(m);
  EZBENCH2("pause", m->ip = base, ExecuteInstruction(m));
}

BENCH(machine, benchClc) {
  uint8_t kClc[] = {0xf8};
  Write64(m->ax, 0);
  memcpy(real, kClc, sizeof(kClc));
  LoadInstruction(m);
  EZBENCH2("clc", m->ip = base, ExecuteInstruction(m));
}

BENCH(machine, benchNop) {
  uint8_t kNop[] = {0x90};
  Write64(m->ax, 0);
  memcpy(real, kNop, sizeof(kNop));
  LoadInstruction(m);
  EZBENCH2("nop", m->ip = base, ExecuteInstruction(m));
}

TEST(machine, sizeIsReasonable) {
  ASSERT_LE(sizeof(struct Machine), 65536);
}
