#ifndef OUTPUT_H
#define OUTPUT_h

#include "definitions.h"

void refresh_screen();

void set_status_message(const char *fmt, ...);

int rx_to_cx(editor_row * row, int rx);

#endif
