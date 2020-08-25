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
#include "libc/calls/internal.h"
#include "libc/calls/struct/metastat.h"
#include "libc/dce.h"

#define SWITCHEROO(S1, S2, A, B, C, D, E, F, G, H, I, J, K, L, M) \
  do {                                                            \
    autotype((S2).A) a = (typeof((S2).A))(S1).A;                  \
    autotype((S2).B) b = (typeof((S2).B))(S1).B;                  \
    autotype((S2).C) c = (typeof((S2).C))(S1).C;                  \
    autotype((S2).D) d = (typeof((S2).D))(S1).D;                  \
    autotype((S2).E) e = (typeof((S2).E))(S1).E;                  \
    autotype((S2).F) f = (typeof((S2).F))(S1).F;                  \
    autotype((S2).G) g = (typeof((S2).G))(S1).G;                  \
    autotype((S2).H) h = (typeof((S2).H))(S1).H;                  \
    autotype((S2).I) i = (typeof((S2).I))(S1).I;                  \
    autotype((S2).J) j = (typeof((S2).J))(S1).J;                  \
    autotype((S2).K) k = (typeof((S2).K))(S1).K;                  \
    autotype((S2).L) l = (typeof((S2).L))(S1).L;                  \
    autotype((S2).M) m = (typeof((S2).M))(S1).M;                  \
    (S2).A = a;                                                   \
    (S2).B = b;                                                   \
    (S2).C = c;                                                   \
    (S2).D = d;                                                   \
    (S2).E = e;                                                   \
    (S2).F = f;                                                   \
    (S2).G = g;                                                   \
    (S2).H = h;                                                   \
    (S2).I = i;                                                   \
    (S2).J = j;                                                   \
    (S2).K = k;                                                   \
    (S2).L = l;                                                   \
    (S2).M = m;                                                   \
  } while (0);

forceinline void stat2linux_xnu(union metastat *ms) {
  SWITCHEROO(ms->xnu, ms->linux, st_dev, st_ino, st_nlink, st_mode, st_uid,
             st_gid, st_rdev, st_size, st_blksize, st_blocks, st_atim, st_mtim,
             st_ctim);
}

forceinline void stat2linux_freebsd(union metastat *ms) {
  SWITCHEROO(ms->freebsd, ms->linux, st_dev, st_ino, st_nlink, st_mode, st_uid,
             st_gid, st_rdev, st_size, st_blksize, st_blocks, st_atim, st_mtim,
             st_ctim);
}

forceinline void stat2linux_openbsd(union metastat *ms) {
  SWITCHEROO(ms->openbsd, ms->linux, st_dev, st_ino, st_nlink, st_mode, st_uid,
             st_gid, st_rdev, st_size, st_blksize, st_blocks, st_atim, st_mtim,
             st_ctim);
}

/**
 * Transcodes “The Dismal Data Structure” from BSD→Linux ABI.
 * @asyncsignalsafe
 */
textstartup void stat2linux(void *ms) {
  if (ms) {
    if (SupportsXnu() && IsXnu()) {
      stat2linux_xnu((union metastat *)ms);
    } else if (SupportsFreebsd() && IsFreebsd()) {
      stat2linux_freebsd((union metastat *)ms);
    } else if (SupportsOpenbsd() && IsOpenbsd()) {
      stat2linux_openbsd((union metastat *)ms);
    }
  }
}
