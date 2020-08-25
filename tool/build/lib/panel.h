#ifndef COSMOPOLITAN_TOOL_BUILD_LIB_PANEL_H_
#define COSMOPOLITAN_TOOL_BUILD_LIB_PANEL_H_
#include "tool/build/lib/buffer.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct Panel {
  int i;
  int top, bottom;
  int left, right;
  struct Buffer *lines;
};

ssize_t PrintPanels(int, long, struct Panel *, long, long);
void PrintMessageBox(int, const char *, long, long);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_TOOL_BUILD_LIB_PANEL_H_ */
