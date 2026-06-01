#ifndef HIGHLIGHT_H
#define HIGHLIGHT_H

#include "definitions.h"

void update_syntax(editor_row *row);

int syntax_to_color(int hl);

void select_highlight();

#endif
