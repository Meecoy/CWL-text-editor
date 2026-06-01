#ifndef ROW_H
#define ROW_H

#include "definitions.h"

int cx_to_rx(editor_row *row, int cx);

int rx_to_cx(editor_row *row, int rx);

void row_update(editor_row *row);

void row_insert(int at, char *s, size_t len);

void row_insert_char(editor_row *row, int at, int c);

void row_delete_char(editor_row *row, int at);

void row_append_string(editor_row *row, char *s, size_t len);

void row_delete(int at);

char *rows_to_string(int *buflen);

#endif
