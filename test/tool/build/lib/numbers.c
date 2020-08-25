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
#include "test/tool/build/lib/numbers.h"

const uint64_t kNumbers[102] = {
    0x0000000000000000, 0x0000000080000000, 0x0000000000008000,
    0x0000000000000001, 0x0000000000000002, 0x0000000000000003,
    0x0000000000000004, 0x0000000000000005, 0x0000000000000006,
    0x0000000000000007, 0x0000000000000008, 0x0000000000000009,
    0x000000000000000a, 0x000000000000000b, 0x000000000000000c,
    0x000000000000000d, 0x000000000000000e, 0x000000000000000f,
    0x0000000000000010, 0x0000000000000011, 0x0000000000000012,
    0x0000000000000013, 0x0000000000000014, 0x0000000000000015,
    0x0000000000000016, 0x0000000000000017, 0x0000000000000018,
    0x0000000000000019, 0x000000000000001a, 0x000000000000001b,
    0x000000000000001c, 0x000000000000001d, 0x000000000000001e,
    0x000000000000001f, 0x0000000000000020, 0x0000000000000040,
    0x8000000000000000, 0x0000000000000080, 0x0000000000000002,
    0x0000000000000001, 0x0000000000000004, 0x00000000C0000000,
    0xC000000000000000, 0x000000000000C000, 0x00000000000000C0,
    0x0000000000000003, 0x000000000000E000, 0x00000000E0000000,
    0xE000000000000000, 0x00000000000000E0, 0x000000000000001F,
    0x00000000000000FC, 0x000000000000003F, 0x000000000000007F,
    0x00000000000000FB, 0x00000000000000FD, 0x00000000000000FE,
    0x00000000000000FF, 0x000000000000FF1F, 0x0000000000001FFF,
    0x000000000000FFFC, 0x0000000000003FFF, 0x000000000000FF3F,
    0x000000000000FFFD, 0x000000000000FFFE, 0x000000000000FFFB,
    0x000000000000FF7F, 0x0000000000007FFF, 0x000000000000FFFF,
    0x00000000FFFF1FFF, 0x00000000FFFFFF1F, 0x000000001FFFFFFF,
    0x00000000FFFFFF3F, 0x00000000FFFF3FFF, 0x00000000FFFFFFFC,
    0x000000003FFFFFFF, 0x00000000FFFFFF7F, 0x00000000FFFFFFFD,
    0x00000000FFFFFFFE, 0x00000000FFFFFFFB, 0x000000007FFFFFFF,
    0x00000000FFFF7FFF, 0x00000000FFFFFFFF, 0xFFFFFFFF1FFFFFFF,
    0x1FFFFFFFFFFFFFFF, 0xFFFFFFFFFFFF1FFF, 0xFFFFFFFFFFFFFF1F,
    0xFFFFFFFFFFFFFFFC, 0xFFFFFFFFFFFF3FFF, 0xFFFFFFFF3FFFFFFF,
    0xFFFFFFFFFFFFFF3F, 0x3FFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFD,
    0xFFFFFFFFFFFFFFFE, 0xFFFFFFFFFFFFFFFB, 0xFFFFFFFF7FFFFFFF,
    0x7FFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFF7F, 0xFFFFFFFFFFFF7FFF,
    0xFFFFFFFFFFFFFFFF, 0x00DeadBeefCafe00, 0x0000031337000000,
};
