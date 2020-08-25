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
#include "libc/str/str.h"
#include "third_party/xed/x86.h"
#include "tool/build/lib/case.h"
#include "tool/build/lib/modrm.h"

#define UNKNOWN     "wut"
#define RCASE(x, y) CASE(x, return y)

static const char kFpuName[][8][8] = {
    {"fadd", "fmul", "fcom", "fcomp", "fsub", "fsubr", "fdiv", "fdivr"},
    {"fchs", "fabs", "ftst", "fxam"},
    {"fld1", "fldl2t", "fldl2e", "fldpi", "fldlg2", "fldln2", "fldz"},
    {"f2xm1", "fyl2x", "fptan", "fpatan", "fxtract", "fprem1", "fdecstp",
     "fincstp"},
    {"fprem", "fyl2xp1", "fsqrt", "fsincos", "frndint", "fscale", "fsin",
     "fcos"},
    {"fneni", "fndisi", "fnclex", "fninit", "fnsetpm"},
};

char *DisOpFpu0(struct XedDecodedInst *x, int group) {
  const char *s;
  s = kFpuName[group][ModrmRm(x)];
  return *s ? s : UNKNOWN;
}

char *DisOpFpu1(struct XedDecodedInst *x, char *p, const char *extra) {
  stpcpy(stpcpy(p, kFpuName[0][ModrmReg(x)]), extra);
  return p;
}

char *DisOp66(struct XedDecodedInst *x, char *p, const char *s, const char *a,
              const char *b) {
  stpcpy(stpcpy(p, s), !Prefix66(x) ? a : b);
  return p;
}

char *DisOpVpsWpsVssWss(struct XedDecodedInst *x, char *p, const char *s) {
  return DisOp66(x, p, s, "ps %Vps Wps", "ss %Vss Wss");
}

char *DisOpVpdWpdVpsWps(struct XedDecodedInst *x, char *p, const char *s) {
  return DisOp66(x, p, s, "ps %Vps Wps", "pd %Vpd Wpd");
}

char *DisOpPqQqVdqWdq(struct XedDecodedInst *x, char *p, const char *s) {
  return DisOp66(x, p, s, " %Pq Qq", " %Vdq Wdq");
}

char *DisOpPqQqIbVdqWdqIb(struct XedDecodedInst *x, char *p, const char *s) {
  return DisOp66(x, p, s, " %Pq Qq Ib", " %Vdq Wdq Ib");
}

char *DisOpNqIbUdqIb(struct XedDecodedInst *x, char *p, const char *s) {
  return DisOp66(x, p, s, " %Nq Ib", " %Udq Ib");
}

char *DisOpVpsWpsVssWssVpdWpdVsdWsd(struct XedDecodedInst *x, char *p,
                                    const char *s) {
  char *q = stpcpy(p, s);
  if (x->op.ild_f3) {
    stpcpy(q, "ss %Vss Wss");
  } else if (x->op.ild_f2) {
    stpcpy(q, "sd %Vsd Wsd");
  } else if (Prefix66(x)) {
    stpcpy(q, "pd %Vpd Wpd");
  } else {
    stpcpy(q, "ps %Vps Wps");
  }
  return p;
}

const char *DisSpecMap0(struct XedDecodedInst *x, char *p) {
  switch (x->op.opcode & 0xff) {
    RCASE(0x2F, "das");
    RCASE(0x37, "aaa");
    RCASE(0x3F, "aas");
    RCASE(0xD6, "salc");
    RCASE(0x61, "popa");
    RCASE(0x60, "pusha");
    RCASE(0x62, "bound");
    RCASE(0xD4, "aam Ib");
    RCASE(0xD5, "aad Ib");
    RCASE(0x00, "add Eb %Gb");
    RCASE(0x01, "add Evqp %Gvqp");
    RCASE(0x02, "add %Gb Eb");
    RCASE(0x03, "add %Gvqp Evqp");
    RCASE(0x04, "add %al Ib");
    RCASE(0x05, "add %rAX Ivds");
    RCASE(0x06, "push %es");
    RCASE(0x07, "pop %es");
    RCASE(0x08, "or Eb %Gb");
    RCASE(0x09, "or Evqp %Gvqp");
    RCASE(0x0a, "or %Gb Eb");
    RCASE(0x0b, "or %Gvqp Evqp");
    RCASE(0x0c, "or %al Ib");
    RCASE(0x0d, "or %rAX Ivds");
    RCASE(0x0e, "push %cs");
    RCASE(0x0f, "pop %cs");
    RCASE(0x10, "adc Eb %Gb");
    RCASE(0x11, "adc Evqp %Gvqp");
    RCASE(0x12, "adc %Gb Eb");
    RCASE(0x13, "adc %Gvqp Evqp");
    RCASE(0x14, "adc %al Ib");
    RCASE(0x15, "adc %rAX Ivds");
    RCASE(0x16, "push %ss");
    RCASE(0x17, "pop %ss");
    RCASE(0x18, "sbb Eb %Gb");
    RCASE(0x19, "sbb Evqp %Gvqp");
    RCASE(0x1a, "sbb %Gb Eb");
    RCASE(0x1b, "sbb %Gvqp Evqp");
    RCASE(0x1c, "sbb %al Ib");
    RCASE(0x1d, "sbb %rAX Ivds");
    RCASE(0x1e, "push %ds");
    RCASE(0x1f, "pop %ds");
    RCASE(0x20, "and Eb %Gb");
    RCASE(0x21, "and Evqp %Gvqp");
    RCASE(0x22, "and %Gb Eb");
    RCASE(0x23, "and %Gvqp Evqp");
    RCASE(0x24, "and %al Ib");
    RCASE(0x25, "and %rAX Ivds");
    RCASE(0x26, "push %es");
    RCASE(0x27, "pop %es");
    RCASE(0x28, "sub Eb %Gb");
    RCASE(0x29, "sub Evqp %Gvqp");
    RCASE(0x2a, "sub %Gb Eb");
    RCASE(0x2b, "sub %Gvqp Evqp");
    RCASE(0x2c, "sub %al Ib");
    RCASE(0x2d, "sub %rAX Ivds");
    RCASE(0x30, "xor Eb %Gb");
    RCASE(0x31, "xor Evqp %Gvqp");
    RCASE(0x32, "xor %Gb Eb");
    RCASE(0x33, "xor %Gvqp Evqp");
    RCASE(0x34, "xor %al Ib");
    RCASE(0x35, "xor %rAX Ivds");
    RCASE(0x38, "cmp Eb %Gb");
    RCASE(0x39, "cmp Evqp %Gvqp");
    RCASE(0x3A, "cmp %Gb Eb");
    RCASE(0x3B, "cmp %Gvqp Evqp");
    RCASE(0x3C, "cmp %al Ib");
    RCASE(0x3D, "cmp %rAX Ivds");
    RCASE(0x50 ... 0x57, "pushWQ %Zvq");
    RCASE(0x58 ... 0x5f, "popWQ %Zvq");
    RCASE(0x63, "movslLQ %Gdqp Ed");
    RCASE(0x68, "pushWQ Ivs");
    RCASE(0x69, "imul %Gvqp Evqp Ivds");
    RCASE(0x6A, "pushWQ Ibss");
    RCASE(0x6B, "imul %Gvqp Evqp Ibs");
    RCASE(0x6C, "insb Yb %dx");
    RCASE(0x6D, "insWL Yv %dx");
    RCASE(0x6E, "outsb %dx Xb");
    RCASE(0x6F, "outsWL %dx Xv");
    RCASE(0x70, "joBT Jbs");
    RCASE(0x71, "jnoBT Jbs");
    RCASE(0x72, "jbBT Jbs");
    RCASE(0x73, "jaeBT Jbs");
    RCASE(0x74, "jeBT Jbs");
    RCASE(0x75, "jneBT Jbs");
    RCASE(0x76, "jbeBT Jbs");
    RCASE(0x77, "jaBT Jbs");
    RCASE(0x78, "jsBT Jbs");
    RCASE(0x79, "jnsBT Jbs");
    RCASE(0x7a, "jpBT Jbs");
    RCASE(0x7b, "jnpBT Jbs");
    RCASE(0x7c, "jlBT Jbs");
    RCASE(0x7d, "jgeBT Jbs");
    RCASE(0x7e, "jleBT Jbs");
    RCASE(0x7f, "jgBT Jbs");
    RCASE(0x80, "ALU Eb Ib");
    RCASE(0x81, "ALU Evqp Ivds");
    RCASE(0x82, "ALU Eb Ib");
    RCASE(0x83, "ALU Evqp Ibs");
    RCASE(0x84, "test Eb %Gb");
    RCASE(0x85, "test %Gvqp Evqp");
    RCASE(0x86, "xchg %Gb Eb");
    RCASE(0x87, "xchg %Gvqp Evqp");
    RCASE(0x88, "mov Eb %Gb");
    RCASE(0x89, "mov Evqp %Gvqp");
    RCASE(0x8A, "mov %Gb Eb");
    RCASE(0x8B, "mov %Gvqp Evqp");
    RCASE(0x8C, "mov Evqp %Sw");
    RCASE(0x8D, "lea %Gvqp M");
    RCASE(0x8E, "mov %Sw Evqp");
    RCASE(0x90, "nop");
    RCASE(0x91 ... 0x97, "xchg %Zvqp %rAX");
    RCASE(0x98, "cwtl");
    RCASE(0x99, "cltd");
    RCASE(0x9B, "fwait");
    RCASE(0x9C, "pushfWQ");
    RCASE(0x9D, "popfWQ");
    RCASE(0x9E, "sahf");
    RCASE(0x9F, "lahf");
    RCASE(0xA0, "movABS %al Ob");
    RCASE(0xA1, "movABS %rAX Ovqp");
    RCASE(0xA2, "movABS Ob %al");
    RCASE(0xA3, "movABS Ovqp %rAX");
    RCASE(0xA4, "movsb Yb Xb");
    RCASE(0xA5, "movsWLQ Yvqp Xvqp");
    RCASE(0xA6, "cmpsb Yb Xb");
    RCASE(0xA7, "cmpsWLQ Yvqp Xvqp");
    RCASE(0xA8, "test %al Ib");
    RCASE(0xA9, "test %rAX Ivds");
    RCASE(0xAA, "stosb Yb %al");
    RCASE(0xAB, "stosWLQ Yvqp %rAX");
    RCASE(0xAC, "lodsb %al Xb");
    RCASE(0xAD, "lodsWLQ %rAX Xvqp");
    RCASE(0xAE, "scasb %al Yb");
    RCASE(0xAF, "scasWLQ %rAX Yvqp");
    RCASE(0xB0 ... 0xb7, "mov %Zb Ib");
    RCASE(0xB8 ... 0xbf, "movABS %Zvqp Ivqp");
    RCASE(0xC0, "BIT Eb Ib");
    RCASE(0xC1, "BIT Evqp Ib");
    RCASE(0xC2, "ret Iw");
    RCASE(0xC3, "ret");
    RCASE(0xC6, "mov Eb Ib");
    RCASE(0xC7, "mov Evqp Ivds");
    RCASE(0xC9, "leave");
    RCASE(0xCC, "int3");
    RCASE(0xCD, "int Ib");
    RCASE(0xD0, "BIT Eb $1");
    RCASE(0xD1, "BIT Evqp $1");
    RCASE(0xD2, "BIT Evqp %cl");
    RCASE(0xD3, "BIT Evqp %cl");
    RCASE(0xD7, "xlat BBb");
    RCASE(0xE0, "loopne Jbs");
    RCASE(0xE1, "loope Jbs");
    RCASE(0xE2, "loop Jbs");
    RCASE(0xE3, "jcxz Jbs");
    RCASE(0xE4, "in %al Ib");
    RCASE(0xE5, "in %eAX Ib");
    RCASE(0xE6, "out Ib %al");
    RCASE(0xE7, "out Ib %eAX");
    RCASE(0xE8, "call Jvds");
    RCASE(0xE9, "jmp Jvds");
    RCASE(0xEB, "jmp Jbs");
    RCASE(0xEC, "in %al %dx");
    RCASE(0xED, "in %eAX %dx");
    RCASE(0xEE, "out %dx %al");
    RCASE(0xEF, "out %dx %eAX");
    RCASE(0xF1, "int1");
    RCASE(0xF4, "hlt");
    RCASE(0xF5, "cmc");
    RCASE(0xF8, "clc");
    RCASE(0xF9, "stc");
    RCASE(0xFA, "cli");
    RCASE(0xFB, "sti");
    RCASE(0xFC, "cld");
    RCASE(0xFD, "std");
    case 0x8F:
      switch (ModrmReg(x)) {
        RCASE(0, "popWQ Evq");
        default:
          break;
      }
      break;
    case 0xD9:
      switch (ModrmReg(x)) {
        RCASE(1, "fxch EST1");
        RCASE(3, "fstps Msr %st");
        case 0:
          if (IsModrmRegister(x)) {
            return "fld EST";
          } else {
            return "flds Msr";
          }
          break;
        case 2:
          if (IsModrmRegister(x)) {
            return "fnop";
          } else {
            return "fsts Msr %st";
          }
          break;
        case 4:
          if (IsModrmRegister(x)) {
            return DisOpFpu0(x, 1);
          } else {
            return "fldenv Me";
          }
          break;
        case 5:
          if (IsModrmRegister(x)) {
            return DisOpFpu0(x, 2);
          } else {
            return "fldcw Mw";
          }
          break;
        case 6:
          if (IsModrmRegister(x)) {
            return DisOpFpu0(x, 3);
          } else {
            return "fnstenv M";
          }
          break;
        case 7:
          if (IsModrmRegister(x)) {
            return DisOpFpu0(x, 4);
          } else {
            return "fnstcw Mw";
          }
          break;
      }
      break;
    case 0xDA:
      switch (ModrmReg(x)) {
        case 0:
          if (!IsModrmRegister(x)) {
            return "fiaddl Mdi";
          } else {
            return "fcmovb %st EST";
          }
          break;
        case 1:
          if (!IsModrmRegister(x)) {
            return "fimull Mdi";
          } else {
            return "fcmove %st EST";
          }
          break;
        case 2:
          if (!IsModrmRegister(x)) {
            return "ficoml Mdi";
          } else {
            return "fcmovbe %st EST";
          }
          break;
        case 3:
          if (!IsModrmRegister(x)) {
            return "ficompl Mdi";
          } else {
            return "fcmovu %st EST";
          }
          break;
        case 4:
          if (!IsModrmRegister(x)) {
            return "fisubl Mdi";
          } else {
            return "fisubr Mdi";
          }
          break;
        case 5:
          if (!IsModrmRegister(x)) {
            return "fisubrl Mdi";
          } else {
            return "fucompp";
          }
          break;
        case 6:
          if (!IsModrmRegister(x)) {
            return "fidivl Mdi";
          } else {
            return UNKNOWN;
          }
          break;
        case 7:
          if (!IsModrmRegister(x)) {
            return "fidivrl Mdi";
          } else {
            return UNKNOWN;
          }
          break;
      }
      break;
    case 0xDB:
      switch (ModrmReg(x)) {
        case 0:
          if (!IsModrmRegister(x)) {
            return "fildl Mdi";
          } else {
            return "fcmovnb %st EST";
          }
          break;
        case 1:
          if (!IsModrmRegister(x)) {
            return "fisttpl Mdi";
          } else {
            return "fcmovne %st EST";
          }
          break;
        case 2:
          if (!IsModrmRegister(x)) {
            return "fistl Mdi";
          } else {
            return "fcmovnbe %st EST";
          }
          break;
        case 3:
          if (!IsModrmRegister(x)) {
            return "fistpl Mdi";
          } else {
            return "fcmovnu %st EST";
          }
          break;
        case 4:
          return DisOpFpu0(x, 5);
        case 5:
          if (!IsModrmRegister(x)) {
            return "fldt Mer";
          } else {
            return "fucomi %st EST";
          }
          break;
        case 6:
          if (IsModrmRegister(x)) {
            return "fcomi %st EST";
          } else {
            return UNKNOWN;
          }
          break;
        case 7:
          if (!IsModrmRegister(x)) {
            return "fstpt Mer";
          } else {
            return UNKNOWN;
          }
          break;
      }
      break;
    case 0xD8:
      return DisOpFpu1(x, p, !IsModrmRegister(x) ? "s Msr" : " EST1");
    case 0xDC:
      if (!IsModrmRegister(x)) {
        return DisOpFpu1(x, p, "l Mdr");
      } else {
        switch (ModrmReg(x)) {
          RCASE(0, "fadd EST %st");
          RCASE(1, "fmul EST %st");
          RCASE(2, "fcom %st EST");
          RCASE(3, "fcomp %st EST");
          RCASE(4, "fsub EST %st");
          RCASE(5, "fsubr EST %st");
          RCASE(6, "fdiv EST %st");
          RCASE(7, "fdivr EST %st");
        }
      }
      break;
    case 0xDD:
      if (!IsModrmRegister(x)) {
        switch (ModrmReg(x)) {
          RCASE(0, "fldl Mdr");
          RCASE(1, "fisttpll Mqi");
          RCASE(2, "fstl Mdr");
          RCASE(3, "fstpl Mdr");
          RCASE(4, "frstor Mdr");
          RCASE(6, "fnsave Mst");
          RCASE(7, "fnstsw Mst");
        }
      } else {
        switch (ModrmReg(x)) {
          RCASE(0, "ffree EST");
          RCASE(1, "fxch EST");
          RCASE(2, "fst EST");
          RCASE(3, "fstp EST");
          RCASE(4, "fucom EST1");
          RCASE(5, "fucomp EST1");
        }
      }
      break;
    case 0xDE:
      if (!IsModrmRegister(x)) {
        switch (ModrmReg(x)) {
          RCASE(0, "fiadds Mwi");
          RCASE(1, "fimuls Mwi");
          RCASE(2, "ficoms Mwi");
          RCASE(3, "ficomps Mwi");
          RCASE(4, "fisubs Mwi");
          RCASE(5, "fisubrs Mwi");
          RCASE(6, "fidivs Mwi");
          RCASE(7, "fidivrs Mwi");
        }
      } else {
        switch (ModrmReg(x)) {
          RCASE(0, "faddp EST1");
          RCASE(1, "fmulp EST1");
          RCASE(2, "fcomp EST1");
          RCASE(3, "fcompp");
          RCASE(4, "fsubp EST1");
          RCASE(5, "fsubrp EST1");
          RCASE(6, "fdivp EST1");
          RCASE(7, "fdivrp EST1");
        }
      }
      break;
    case 0xDF:
      if (!IsModrmRegister(x)) {
        switch (ModrmReg(x)) {
          RCASE(0, "filds Mwi");
          RCASE(1, "fisttps Mwi");
          RCASE(2, "fists Mwi");
          RCASE(3, "fistps Mwi");
          RCASE(4, "fbld");
          RCASE(5, "fildll Mqi");
          RCASE(6, "fbstp");
          RCASE(7, "fistpll Mqi");
        }
      } else {
        switch (ModrmReg(x)) {
          RCASE(0, "ffreep EST");
          RCASE(1, "fxch");
          RCASE(2, "fstp EST");
          RCASE(3, "fstp EST");
          RCASE(4, "fnstsw");
          RCASE(5, "fucomip EST");
          RCASE(6, "fcomip EST");
        }
      }
      break;
    case 0xF6:
      switch (ModrmReg(x)) {
        RCASE(0, "test Eb Ib");
        RCASE(1, "test Eb Ib");
        RCASE(2, "not Eb");
        RCASE(3, "neg Eb");
        RCASE(4, "mulb Eb");
        RCASE(5, "imulb Eb");
        RCASE(6, "divb Eb");
        RCASE(7, "idivb Eb");
      }
      break;
    case 0xF7:
      switch (ModrmReg(x)) {
        RCASE(0, "test Evqp Ivds");
        RCASE(1, "test Evqp Ivds");
        RCASE(2, "not Evqp");
        RCASE(3, "neg Evqp");
        RCASE(4, "mul Evqp");
        RCASE(5, "imul Evqp");
        RCASE(6, "div Evqp");
        RCASE(7, "idiv Evqp");
      }
      break;
    case 0xFE:
      switch (ModrmReg(x)) {
        RCASE(0, "inc Eb");
        RCASE(1, "dec Eb");
      }
      break;
    case 0xFF:
      switch (ModrmReg(x)) {
        RCASE(0, "inc Evqp");
        RCASE(1, "dec Evqp");
        RCASE(2, "CALL Eq");
        RCASE(4, "JMP Eq");
        RCASE(6, "pushWQ Evq");
      }
      break;
  }
  return UNKNOWN;
}

const char *DisSpecMap1(struct XedDecodedInst *x, char *p) {
  switch (x->op.opcode & 0xff) {
    RCASE(0x05, "syscall");
    RCASE(0x0B, "ud2");
    RCASE(0x0D, "nop Ev");
    RCASE(0x18, "nop Ev");
    RCASE(0x19, "nop Ev");
    RCASE(0x1A, "nop Ev");
    RCASE(0x1B, "nop Ev");
    RCASE(0x1C, "nop Ev");
    RCASE(0x1D, "nop Ev");
    RCASE(0x28, "movapSD %Vps Wps");
    RCASE(0x29, "movapSD Wps %Vps");
    RCASE(0x2B, "movntpSD Mps %Vps");
    RCASE(0x2E, Prefix66(x) ? "ucomisd %Vsd Wsd" : "ucomiss %Vss Wss");
    RCASE(0x2F, Prefix66(x) ? "comisd %Vsd Wsd" : "comiss %Vss Wss");
    RCASE(0x31, "rdtsc");
    RCASE(0x40, "cmovo %Gvqp Evqp");
    RCASE(0x41, "cmovno %Gvqp Evqp");
    RCASE(0x42, "cmovb %Gvqp Evqp");
    RCASE(0x43, "cmovae %Gvqp Evqp");
    RCASE(0x44, "cmove %Gvqp Evqp");
    RCASE(0x45, "cmovne %Gvqp Evqp");
    RCASE(0x46, "cmovbe %Gvqp Evqp");
    RCASE(0x47, "cmova %Gvqp Evqp");
    RCASE(0x48, "cmovs %Gvqp Evqp");
    RCASE(0x49, "cmovns %Gvqp Evqp");
    RCASE(0x4a, "cmovp %Gvqp Evqp");
    RCASE(0x4b, "cmovnp %Gvqp Evqp");
    RCASE(0x4c, "cmovl %Gvqp Evqp");
    RCASE(0x4d, "cmovge %Gvqp Evqp");
    RCASE(0x4e, "cmovle %Gvqp Evqp");
    RCASE(0x4f, "cmovg %Gvqp Evqp");
    RCASE(0x52, DisOpVpsWpsVssWss(x, p, "rsqrt"));
    RCASE(0x53, DisOpVpsWpsVssWss(x, p, "rcp"));
    RCASE(0x54, DisOpVpdWpdVpsWps(x, p, "and"));
    RCASE(0x55, DisOpVpdWpdVpsWps(x, p, "andn"));
    RCASE(0x56, DisOpVpdWpdVpsWps(x, p, "or"));
    RCASE(0x57, DisOpVpdWpdVpsWps(x, p, "xor"));
    RCASE(0x58, DisOpVpsWpsVssWssVpdWpdVsdWsd(x, p, "add"));
    RCASE(0x59, DisOpVpsWpsVssWssVpdWpdVsdWsd(x, p, "mul"));
    RCASE(0x5C, DisOpVpsWpsVssWssVpdWpdVsdWsd(x, p, "sub"));
    RCASE(0x5D, DisOpVpsWpsVssWssVpdWpdVsdWsd(x, p, "min"));
    RCASE(0x5E, DisOpVpsWpsVssWssVpdWpdVsdWsd(x, p, "div"));
    RCASE(0x5F, DisOpVpsWpsVssWssVpdWpdVsdWsd(x, p, "max"));
    RCASE(0x60, DisOpPqQqVdqWdq(x, p, "punpcklbw"));
    RCASE(0x61, DisOpPqQqVdqWdq(x, p, "punpcklwd"));
    RCASE(0x62, DisOpPqQqVdqWdq(x, p, "punpckldq"));
    RCASE(0x63, DisOpPqQqVdqWdq(x, p, "packsswb"));
    RCASE(0x64, DisOpPqQqVdqWdq(x, p, "pcmpgtb"));
    RCASE(0x65, DisOpPqQqVdqWdq(x, p, "pcmpgtw"));
    RCASE(0x66, DisOpPqQqVdqWdq(x, p, "pcmpgtd"));
    RCASE(0x67, DisOpPqQqVdqWdq(x, p, "packuswb"));
    RCASE(0x68, DisOpPqQqVdqWdq(x, p, "punpckhbw"));
    RCASE(0x69, DisOpPqQqVdqWdq(x, p, "punpckhwd"));
    RCASE(0x6A, DisOpPqQqVdqWdq(x, p, "punpckhdq"));
    RCASE(0x6B, DisOpPqQqVdqWdq(x, p, "packssdw"));
    RCASE(0x6C, DisOpPqQqVdqWdq(x, p, "punpcklqdq"));
    RCASE(0x6D, DisOpPqQqVdqWdq(x, p, "punpckhqdq"));
    RCASE(0x74, DisOpPqQqVdqWdq(x, p, "pcmpeqb"));
    RCASE(0x75, DisOpPqQqVdqWdq(x, p, "pcmpeqw"));
    RCASE(0x76, DisOpPqQqVdqWdq(x, p, "pcmpeqd"));
    RCASE(0x80, "jo Jvds");
    RCASE(0x81, "jno Jvds");
    RCASE(0x82, "jb Jvds");
    RCASE(0x83, "jae Jvds");
    RCASE(0x84, "je Jvds");
    RCASE(0x85, "jne Jvds");
    RCASE(0x86, "jbe Jvds");
    RCASE(0x87, "ja Jvds");
    RCASE(0x88, "js Jvds");
    RCASE(0x89, "jns Jvds");
    RCASE(0x8a, "jp Jvds");
    RCASE(0x8b, "jnp Jvds");
    RCASE(0x8c, "jl Jvds");
    RCASE(0x8d, "jge Jvds");
    RCASE(0x8e, "jle Jvds");
    RCASE(0x8f, "jg Jvds");
    RCASE(0x90, "seto Eb");
    RCASE(0x91, "setno Eb");
    RCASE(0x92, "setb Eb");
    RCASE(0x93, "setnb Eb");
    RCASE(0x94, "sete Eb");
    RCASE(0x95, "setne Eb");
    RCASE(0x96, "setbe Eb");
    RCASE(0x97, "seta Eb");
    RCASE(0x98, "sets Eb");
    RCASE(0x99, "setns Eb");
    RCASE(0x9A, "setp Eb");
    RCASE(0x9B, "setnp Eb");
    RCASE(0x9C, "setl Eb");
    RCASE(0x9D, "setge Eb");
    RCASE(0x9E, "setle Eb");
    RCASE(0x9F, "setg Eb");
    RCASE(0xA0, "push %fs");
    RCASE(0xA1, "pop %fs");
    RCASE(0xA2, "cpuid");
    RCASE(0xA3, "bt Evqp %Gvqp");
    RCASE(0xA4, "shld Evqp %Gvqp Ib");
    RCASE(0xA5, "shld Evqp %Gvqp %cl");
    RCASE(0xA8, "push %gs");
    RCASE(0xA9, "pop %gs");
    RCASE(0xAB, "bts Evqp %Gvqp");
    RCASE(0xAC, "shrd Evqp %Gvqp Ib");
    RCASE(0xAD, "shrd Evqp %Gvqp %cl");
    RCASE(0xAF, "imul %Gvqp Evqp");
    RCASE(0xB0, "cmpxchg Eb %Gb");
    RCASE(0xB1, "cmpxchg Evqp %Gvqp");
    RCASE(0xB3, "btr Evqp %Gvqp");
    RCASE(0xB6, "movzbWLQ %Gvqp Eb");
    RCASE(0xB7, "movzwWLQ %Gvqp Ew");
    RCASE(0xB9, "ud %Gvqp Evqp");
    RCASE(0xBB, "btc Evqp %Gvqp");
    RCASE(0xBC, "bsf %Gvqp Evqp");
    RCASE(0xBD, "bsr %Gvqp Evqp");
    RCASE(0xBE, "movsbWLQ %Gvqp Eb");
    RCASE(0xBF, "movswWLQ %Gvqp Ew");
    RCASE(0xC0, "xadd Eb %Gb");
    RCASE(0xC1, "xadd Evqp %Gvqp");
    RCASE(0xC2, DisOpVpsWpsVssWssVpdWpdVsdWsd(x, p, "cmp"));
    RCASE(0xC3, "movnti Mdqp %Gdqp");
    RCASE(0xC8 ... 0xCF, "bswap %Zvqp");
    RCASE(0xD1, DisOpPqQqVdqWdq(x, p, "psrlw"));
    RCASE(0xD2, DisOpPqQqVdqWdq(x, p, "psrld"));
    RCASE(0xD3, DisOpPqQqVdqWdq(x, p, "psrlq"));
    RCASE(0xD4, DisOpPqQqVdqWdq(x, p, "paddq"));
    RCASE(0xD5, DisOpPqQqVdqWdq(x, p, "pmullw"));
    RCASE(0xD7, Prefix66(x) ? "pmovmskb %Gdqp %Udq" : "pmovmskb %Gdqp %Nq");
    RCASE(0xD8, DisOpPqQqVdqWdq(x, p, "psubusb"));
    RCASE(0xD9, DisOpPqQqVdqWdq(x, p, "psubusw"));
    RCASE(0xDA, DisOpPqQqVdqWdq(x, p, "pminub"));
    RCASE(0xDB, DisOpPqQqVdqWdq(x, p, "pand"));
    RCASE(0xDC, DisOpPqQqVdqWdq(x, p, "paddusb"));
    RCASE(0xDD, DisOpPqQqVdqWdq(x, p, "paddusw"));
    RCASE(0xDE, DisOpPqQqVdqWdq(x, p, "pmaxub"));
    RCASE(0xDF, DisOpPqQqVdqWdq(x, p, "pandn"));
    RCASE(0xE0, DisOpPqQqVdqWdq(x, p, "pavgb"));
    RCASE(0xE1, DisOpPqQqVdqWdq(x, p, "psrawv"));
    RCASE(0xE2, DisOpPqQqVdqWdq(x, p, "psradv"));
    RCASE(0xE3, DisOpPqQqVdqWdq(x, p, "pavgw"));
    RCASE(0xE4, DisOpPqQqVdqWdq(x, p, "pmulhuw"));
    RCASE(0xE5, DisOpPqQqVdqWdq(x, p, "pmulhw"));
    RCASE(0xE7, Prefix66(x) ? "movntdq Mdq %Vdq" : "movntq Mq %Pq");
    RCASE(0xE8, DisOpPqQqVdqWdq(x, p, "psubsb"));
    RCASE(0xE9, DisOpPqQqVdqWdq(x, p, "psubsw"));
    RCASE(0xEA, DisOpPqQqVdqWdq(x, p, "pminsw"));
    RCASE(0xEB, DisOpPqQqVdqWdq(x, p, "por"));
    RCASE(0xEC, DisOpPqQqVdqWdq(x, p, "paddsb"));
    RCASE(0xED, DisOpPqQqVdqWdq(x, p, "paddsw"));
    RCASE(0xEE, DisOpPqQqVdqWdq(x, p, "pmaxsw"));
    RCASE(0xEF, DisOpPqQqVdqWdq(x, p, "pxor"));
    RCASE(0xF0, "lddqu %Vdq Mdq");
    RCASE(0xF1, DisOpPqQqVdqWdq(x, p, "psllwv"));
    RCASE(0xF2, DisOpPqQqVdqWdq(x, p, "pslldv"));
    RCASE(0xF3, DisOpPqQqVdqWdq(x, p, "psllqv"));
    RCASE(0xF4, DisOpPqQqVdqWdq(x, p, "pmuludq"));
    RCASE(0xF5, DisOpPqQqVdqWdq(x, p, "pmaddwd"));
    RCASE(0xF6, DisOpPqQqVdqWdq(x, p, "psadbw"));
    RCASE(0xF8, DisOpPqQqVdqWdq(x, p, "psubb"));
    RCASE(0xF9, DisOpPqQqVdqWdq(x, p, "psubw"));
    RCASE(0xFA, DisOpPqQqVdqWdq(x, p, "psubd"));
    RCASE(0xFB, DisOpPqQqVdqWdq(x, p, "psubq"));
    RCASE(0xFC, DisOpPqQqVdqWdq(x, p, "paddb"));
    RCASE(0xFD, DisOpPqQqVdqWdq(x, p, "paddw"));
    RCASE(0xFE, DisOpPqQqVdqWdq(x, p, "paddd"));
    RCASE(0xFF, "ud0 %Gvqp Evqp");
    case 0x1F:
      if (x->op.modrm == 0x45) {
        return "bofram Jb";
      } else {
        return "nop Ev";
      }
      break;
    case 0x70:
      switch (x->op.rep | Prefix66(x)) {
        RCASE(0, "pshufw %Pq Qq Ib");
        RCASE(1, "pshufd %Vdq Wdq Ib");
        RCASE(2, "pshuflw %Vdq Wdq Ib");
        RCASE(3, "pshufhw %Vdq Wdq Ib");
      }
      break;
    case 0x71:
      switch (ModrmReg(x)) {
        RCASE(2, DisOpNqIbUdqIb(x, p, "psrlw"));
        RCASE(4, DisOpNqIbUdqIb(x, p, "psraw"));
        RCASE(6, DisOpNqIbUdqIb(x, p, "psllw"));
      }
      break;
    case 0x72:
      switch (ModrmReg(x)) {
        RCASE(2, DisOpNqIbUdqIb(x, p, "psrld"));
        RCASE(4, DisOpNqIbUdqIb(x, p, "psrad"));
        RCASE(6, DisOpNqIbUdqIb(x, p, "pslld"));
      }
      break;
    case 0x73:
      switch (ModrmReg(x)) {
        RCASE(2, DisOpNqIbUdqIb(x, p, "psrlq"));
        RCASE(3, DisOpNqIbUdqIb(x, p, "psrldq"));
        RCASE(6, DisOpNqIbUdqIb(x, p, "psllq"));
        RCASE(7, DisOpNqIbUdqIb(x, p, "pslldq"));
      }
      break;
    case 0xAE:
      switch (ModrmReg(x)) {
        RCASE(0, "fxsave Mstx %st %st(1)");
        RCASE(1, "fxrstor Mstx %st %st(1)");
        RCASE(2, "ldmxcsr Md");
        RCASE(3, "stmxcsr Md");
        RCASE(4, "xsave M %edx %eax");
        RCASE(5, "lfence");
        RCASE(6, "mfence");
        case 7:
          if (0xf8 <= x->op.modrm && x->op.modrm <= 0xff) {
            return "sfence";
          } else {
            return "clflush";
          }
      }
      break;
    case 0xBA:
      switch (ModrmReg(x)) {
        RCASE(4, "btWLQ Evqp Ib");
        RCASE(5, "btsWLQ Evqp Ib");
        RCASE(6, "btrWLQ Evqp Ib");
        RCASE(7, "btcWLQ Evqp Ib");
      }
      break;
    case 0x10:
      if (x->op.ild_f3) {
        return "movss %Vss Wss";
      } else if (x->op.ild_f2) {
        return "movsd %Vsd Wsd";
      } else if (Prefix66(x)) {
        return "movupd %Vpd Wpd";
      } else {
        return "movups %Vps Wps";
      }
      break;
    case 0x11:
      if (x->op.ild_f3) {
        return "movss Wss %Vss";
      } else if (x->op.ild_f2) {
        return "movsd Wsd %Vsd";
      } else if (Prefix66(x)) {
        return "movupd Wpd %Vpd";
      } else {
        return "movups Wps %Vps";
      }
      break;
    case 0xC4:
      if (!Prefix66(x)) {
        if (IsModrmRegister(x)) {
          return "pinsrw %Pq %Rdqp Ib";
        } else {
          return "pinsrw %Pq Mw Ib";
        }
      } else {
        if (IsModrmRegister(x)) {
          return "pinsrw %Vdq %Rdqp Ib";
        } else {
          return "pinsrw %Vdq Mw Ib";
        }
      }
      break;
    case 0xC5:
      if (!Prefix66(x)) {
        return "pextrw %Gdqp %Nq Ib";
      } else {
        return "pextrw %Gdqp %Udq Ib";
      }
      break;
    case 0xC6:
      if (!Prefix66(x)) {
        return "shufps %Vps Wps Ib";
      } else {
        return "shufpd %Vpd Wpd Ib";
      }
      break;
    case 0xC7:
      if (Rexw(x)) {
        return "cmpxchg16b Mdq";
      } else {
        return "cmpxchg8b Mq";
      }
      break;
    case 0xD6:
      if (Prefix66(x)) {
        return "movq Wq %Vq";
      } else if (x->op.ild_f3) {
        return "movq2dq %Vdq %Nq";
      } else if (x->op.ild_f2) {
        return "movq2dq %Pq %Uq";
      }
      break;
    case 0x12:
      switch (x->op.rep | Prefix66(x)) {
        case 0:
          if (IsModrmRegister(x)) {
            return "movhlps %Vq %Uq";
          } else {
            return "movlps %Vq Mq";
          }
          break;
        case 1:
          return "movlpd %Vq Mq";
        case 2:
          return "movddup %Vq Wq";
        case 3:
          return "movsldup %Vq Wq";
        default:
          unreachable;
      }
      break;
    case 0x13:
      if (Prefix66(x)) {
        return "movlpd Mq %Vq";
      } else {
        return "movlps Mq %Vq";
      }
      break;
    case 0x16:
      switch (x->op.rep | Prefix66(x)) {
        case 0:
          if (IsModrmRegister(x)) {
            return "movlhps %Vq %Uq";
          } else {
            return "movhps %Vq Mq";
          }
          break;
        case 1:
          return "movhpd %Vq Mq";
        case 3:
          return "movshdup %Vq Wq";
        default:
          break;
      }
      break;
    case 0x17:
      if (Prefix66(x)) {
        return "movhpd Mq %Vq";
      } else {
        return "movhps Mq %Vq";
      }
      break;
    case 0x2A:
      if (x->op.ild_f3) {
        return "cvtsi2ss %Vss Edqp";
      } else if (x->op.ild_f2) {
        return "cvtsi2sd %Vsd Edqp";
      } else if (Prefix66(x)) {
        return "cvtpi2pd %Vpd Qpi";
      } else {
        return "cvtpi2ps %Vps Qpi";
      }
      break;
    case 0x2C:
      if (x->op.ild_f3) {
        return "cvttss2si %Gdqp Wss";
      } else if (x->op.ild_f2) {
        return "cvttsd2si %Gdqp Wsd";
      } else if (Prefix66(x)) {
        return "cvttpd2pi %Ppi Wpd";
      } else {
        return "cvttps2pi %Ppi Wpsq";
      }
      break;
    case 0x2D:
      if (x->op.ild_f3) {
        return "cvtss2si %Gdqp Wss";
      } else if (x->op.ild_f2) {
        return "cvtsd2si %Gdqp Wsd";
      } else if (Prefix66(x)) {
        return "cvtpd2pi %Ppi Wpd";
      } else {
        return "cvtps2pi %Ppi Wpsq";
      }
      break;
    case 0x5a:
      if (x->op.ild_f3) {
        return "cvtss2sd %Vsd Wss";
      } else if (x->op.ild_f2) {
        return "cvtsd2ss %Vss Wsd";
      } else if (Prefix66(x)) {
        return "cvtpd2ps %Vps Wpd";
      } else {
        return "cvtps2pd %Vpd Wps";
      }
      break;
    case 0x5b:
      if (x->op.ild_f3) {
        return "cvttps2dq %Vdq Wps";
      } else if (Prefix66(x)) {
        return "cvtps2dq %Vdq Wps";
      } else {
        return "cvtdq2ps %Vps Wdq";
      }
      break;
    case 0x51:
      if (x->op.ild_f3) {
        return "sqrtss %Vss Wss";
      } else if (x->op.ild_f2) {
        return "sqrtsd %Vsd Wsd";
      } else if (Prefix66(x)) {
        return "sqrtpd %Vpd Wpd";
      } else {
        return "sqrtps %Vps Wps";
      }
      break;
    case 0x6E:
      if (Prefix66(x)) {
        if (Rexw(x)) {
          return "movq %Vdq Eqp";
        } else {
          return "movd %Vdq Ed";
        }
      } else {
        if (Rexw(x)) {
          return "movq %Pq Eqp";
        } else {
          return "movd %Pq Ed";
        }
      }
      break;
    case 0x6F:
      if (x->op.ild_f3) {
        return "movdqu %Vdq Wdq";
      } else if (Prefix66(x)) {
        return "movdqa %Vdq Wdq";
      } else {
        return "movq %Pq Qq";
      }
      break;
    case 0x7E:
      if (x->op.ild_f3) {
        return "movq %Vq Wq";
      } else if (Prefix66(x)) {
        if (Rexw(x)) {
          return "movq Eqp %Vdq";
        } else {
          return "movd Ed %Vdq";
        }
      } else {
        if (Rexw(x)) {
          return "movq Eqp %Pq";
        } else {
          return "movd Ed %Pq";
        }
      }
      break;
    case 0x7F:
      if (x->op.ild_f3) {
        return "movdqu Wdq %Vdq";
      } else if (Prefix66(x)) {
        return "movdqa Wdq %Vdq";
      } else {
        return "movq Qq %Pq";
      }
      break;
    case 0xE6:
      if (x->op.ild_f2) {
        return "cvtpd2dq %Vdq Wpd";
      } else if (Prefix66(x)) {
        return "cvttpd2dq %Vdq Wpd";
      } else if (x->op.ild_f3) {
        return "cvtdq2pd %Vpd Wdq";
      }
      break;
  }
  return UNKNOWN;
}

const char *DisSpecMap2(struct XedDecodedInst *x, char *p) {
  switch (x->op.opcode & 0xff) {
    RCASE(0x00, DisOpPqQqVdqWdq(x, p, "pshufb"));
    RCASE(0x01, DisOpPqQqVdqWdq(x, p, "phaddw"));
    RCASE(0x02, DisOpPqQqVdqWdq(x, p, "phaddd"));
    RCASE(0x03, DisOpPqQqVdqWdq(x, p, "phaddsw"));
    RCASE(0x04, DisOpPqQqVdqWdq(x, p, "pmaddubsw"));
    RCASE(0x05, DisOpPqQqVdqWdq(x, p, "phsubw"));
    RCASE(0x06, DisOpPqQqVdqWdq(x, p, "phsubd"));
    RCASE(0x07, DisOpPqQqVdqWdq(x, p, "phsubsw"));
    RCASE(0x08, DisOpPqQqVdqWdq(x, p, "psignb"));
    RCASE(0x09, DisOpPqQqVdqWdq(x, p, "psignw"));
    RCASE(0x0A, DisOpPqQqVdqWdq(x, p, "psignd"));
    RCASE(0x0B, DisOpPqQqVdqWdq(x, p, "pmulhrsw"));
    RCASE(0x10, "pblendvb %Vdq Wdq");
    RCASE(0x14, "blendvps Vps Wps");
    RCASE(0x15, "blendvpd Vpd Wpd");
    RCASE(0x17, "ptest %Vdq Wdq");
    RCASE(0x1C, DisOpPqQqVdqWdq(x, p, "pabsb"));
    RCASE(0x1D, DisOpPqQqVdqWdq(x, p, "pabsw"));
    RCASE(0x1E, DisOpPqQqVdqWdq(x, p, "pabsd"));
    RCASE(0x20, "pmovsxbw %Vdq Mq");
    RCASE(0x21, "pmovsxbd %Vdq Md");
    RCASE(0x22, "pmovsxbq %Vdq Mw");
    RCASE(0x23, "pmovsxwd %Vdq Mq");
    RCASE(0x24, "pmovsxwq %Vdq Md");
    RCASE(0x25, "pmovsxdq %Vdq Mq");
    RCASE(0x28, "pmuldq %Vdq Wdq");
    RCASE(0x29, "pcmpeqq %Vdq Wdq");
    RCASE(0x2A, "movntdqa %Vdq Mdq");
    RCASE(0x2B, "packusdw %Vdq Wdq");
    RCASE(0x30, "pmovzxbw %Vdq Mq");
    RCASE(0x31, "pmovzxbd %Vdq Md");
    RCASE(0x32, "pmovzxbq %Vdq Mw");
    RCASE(0x33, "pmovzxwd %Vdq Mq");
    RCASE(0x34, "pmovzxwq %Vdq Md");
    RCASE(0x35, "pmovzxdq %Vdq Mq");
    RCASE(0x37, "pcmpgtq %Vdq Wdq");
    RCASE(0x38, "pminsb %Vdq Wdq");
    RCASE(0x39, "pminsd %Vdq Wdq");
    RCASE(0x3A, "pminuw %Vdq Wdq");
    RCASE(0x3B, "pminud %Vdq Wdq");
    RCASE(0x3C, "pmaxsb %Vdq Wdq");
    RCASE(0x3D, "pmaxsd %Vdq Wdq");
    RCASE(0x3E, "pmaxuw %Vdq Wdq");
    RCASE(0x3F, "pmaxud %Vdq Wdq");
    RCASE(0x40, "pmulld %Vdq Wdq");
    RCASE(0x41, "phminposuw %Vdq Wdq");
    RCASE(0x80, "invept %Gq Mdq");
    RCASE(0x81, "invvpid %Gq Mdq");
    default:
      return UNKNOWN;
  }
}

const char *DisSpecMap3(struct XedDecodedInst *x, char *p) {
  switch (x->op.opcode & 0xff) {
    RCASE(0x0F, DisOpPqQqIbVdqWdqIb(x, p, "palignr"));
    default:
      return UNKNOWN;
  }
}

const char *DisSpec(struct XedDecodedInst *x, char *p) {
  switch (x->op.map & 7) {
    case XED_ILD_MAP0:
      return DisSpecMap0(x, p);
    case XED_ILD_MAP1:
      return DisSpecMap1(x, p);
    case XED_ILD_MAP2:
      return DisSpecMap2(x, p);
    case XED_ILD_MAP3:
      return DisSpecMap3(x, p);
    default:
      return UNKNOWN;
  }
}
