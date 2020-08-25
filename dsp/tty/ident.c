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
#include "dsp/tty/tty.h"
#include "libc/bits/safemacros.h"
#include "libc/bits/weaken.h"
#include "libc/calls/calls.h"
#include "libc/calls/termios.h"
#include "libc/fmt/fmt.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/sock/sock.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/poll.h"
#include "libc/sysv/errfuns.h"

static int ttyident_probe(struct TtyIdent *ti, int ttyinfd, int ttyoutfd,
                          const char *msg) {
  ssize_t rc;
  size_t got;
  char buf[64];
  int id, version;
  if ((rc = write(ttyoutfd, msg, strlen(msg))) != -1) {
  TryAgain:
    if ((rc = read(ttyinfd, buf, sizeof(buf))) != -1) {
      buf[min((got = (size_t)rc), sizeof(buf) - 1)] = '\0';
      if (sscanf(buf, "\e[>%d;%d", &id, &version) >= 1) {
        ti->id = id;
        ti->version = version;
        rc = 0;
      } else {
        rc = eio();
      }
    } else if (errno == EINTR) {
      goto TryAgain;
    } else if (errno == EAGAIN) {
      if (poll((struct pollfd[]){{ttyinfd, POLLIN}}, 1, 100) != 0) {
        goto TryAgain;
      } else {
        rc = etimedout();
      }
    }
  }
  return rc;
}

/**
 * Identifies teletypewriter.
 *
 * For example, we can tell if process is running in a GNU Screen
 * session Gnome Terminal.
 *
 * @return object if TTY responds, or NULL w/ errno
 * @see ttyidentclear()
 */
int ttyident(struct TtyIdent *ti, int ttyinfd, int ttyoutfd) {
  int rc;
  struct termios old;
  struct TtyIdent outer;
  rc = -1;
  if (!IsWindows()) {
    if (ttyconfig(ttyinfd, ttysetrawdeadline, 3, &old) != -1) {
      if (ttyident_probe(ti, ttyinfd, ttyoutfd, "\e[>c") != -1) {
        rc = 0;
        memset(&outer, 0, sizeof(outer));
        if (ti->id == 83 /* GNU Screen */ && (ti->next || weaken(malloc)) &&
            ttyident_probe(&outer, ttyinfd, ttyoutfd, "\eP\e[>c\e\\") != -1 &&
            (ti->next = (ti->next ? ti->next
                                  : weaken(malloc)(sizeof(struct TtyIdent))))) {
          memcpy(ti->next, &outer, sizeof(outer));
        } else {
          free_s(&ti->next);
        }
      }
      ttyrestore(ttyinfd, &old);
    }
  }
  return rc;
}
