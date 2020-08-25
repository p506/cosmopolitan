#ifndef COSMOPOLITAN_LIBC_DCE_H_
#define COSMOPOLITAN_LIBC_DCE_H_
#include "libc/nexgen32e/kcpuids.h"
/*─────────────────────────────────────────────────────────────────────────────╗
│ cosmopolitan § autotune » dead code elimination                              │
╚─────────────────────────────────────────────────────────────────────────────*/

/**
 * Supported Platforms Tuning Knob (Runtime & Compile-Time)
 *
 * Cosmopolitan's design allows us to support eight platforms. We enable
 * them all by default, since we're able to do eight so efficiently. We
 * can't do more, due to the way the 80x86 instruction set is encoded.
 *
 * The user may optionally tune this bitmask, which effectively asks the
 * compiler to leave out system call polyfills for a platform, which may
 * offer marginal improvements in terms of code size and performance, at
 * the cost of portability.
 */
#define HOSTOS (hostos & SUPPORT_VECTOR)
#ifndef SUPPORT_VECTOR
#define SUPPORT_VECTOR 0b11111111
#endif
#define LINUX   1
#define METAL   2
#define WINDOWS 4
#define XNU     8
#define OPENBSD 16 /* 2019-12-11: new openbsd drm might thwart syscall */
#define FREEBSD 32
/* #define YOUR_CLOUD_PLATFORM_HERE 64  /\* jtunney@gmail.com *\/ */
/* #define YOUR_CLOUD_PLATFORM_HERE 128 /\* jtunney@gmail.com *\/ */

#ifdef NDEBUG
#define NoDebug() 1
#else
#define NoDebug() 0
#endif

#ifdef MODE_DBG
#define IsModeDbg() 1
#else
#define IsModeDbg() 0
#endif

#ifdef __MFENTRY__
#define HaveFentry() 1
#else
#define HaveFentry() 0
#endif

#ifdef TRUSTWORTHY
#define IsTrustworthy() 1
#else
#define IsTrustworthy() 0
#endif

#ifdef SECURITY_BLANKETS
#define UseSecurityBlankets() 1
#else
#define UseSecurityBlankets() 0
#endif

#ifdef __MGENERAL_REGS_ONLY__
#define UseGeneralRegsOnly() 1
#else
#define UseGeneralRegsOnly() 0
#endif

#ifdef TINY
#define IsTiny() 1
#else
#define IsTiny() 0
#endif

#ifdef __OPTIMIZE__
#define IsOptimized() 1
#else
#define IsOptimized() 0
#endif

#if defined(__PIE__) || defined(__PIC__)
#define IsPositionIndependent() 1
#else
#define IsPositionIndependent() 0
#endif

#define SupportsLinux()   ((SUPPORT_VECTOR & LINUX) == LINUX)
#define SupportsMetal()   ((SUPPORT_VECTOR & METAL) == METAL)
#define SupportsWindows() ((SUPPORT_VECTOR & WINDOWS) == WINDOWS)
#define SupportsXnu()     ((SUPPORT_VECTOR & XNU) == XNU)
#define SupportsFreebsd() ((SUPPORT_VECTOR & FREEBSD) == FREEBSD)
#define SupportsOpenbsd() ((SUPPORT_VECTOR & OPENBSD) == OPENBSD)
#define SupportsSystemv() \
  ((SUPPORT_VECTOR & (LINUX | METAL | XNU | OPENBSD | FREEBSD)) != 0)

#ifndef __ASSEMBLER__
#define IsLinux()   ((HOSTOS & LINUX) == LINUX)
#define IsMetal()   ((HOSTOS & METAL) == METAL)
#define IsWindows() ((HOSTOS & WINDOWS) == WINDOWS)
#define IsBsd()     ((HOSTOS & (XNU | FREEBSD | OPENBSD)) != 0)
#define IsXnu()     ((HOSTOS & XNU) == XNU)
#define IsFreebsd() ((HOSTOS & FREEBSD) == FREEBSD)
#define IsOpenbsd() ((HOSTOS & OPENBSD) == OPENBSD)
#else
/* clang-format off */
#define IsLinux() $LINUX,hostos(%rip)
#define IsMetal() $METAL,hostos(%rip)
#define IsWindows() $WINDOWS,hostos(%rip)
#define IsBsd() $XNU|FREEBSD|OPENBSD,hostos(%rip)
#define IsXnu() $XNU,hostos(%rip)
#define IsFreebsd() $FREEBSD,hostos(%rip)
#define IsOpenbsd() $OPENBSD,hostos(%rip)
/* clang-format on */
#endif

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

extern const int hostos;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_DCE_H_ */
