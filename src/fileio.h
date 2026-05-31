#ifndef FILEIO_H
#define FILEIO_H

#include "definitions.h"

void open_file(char *filename);

void row_update(editor_row *row);

void row_insert(int at, char *s, size_t len);

void row_insert_char(editor_row *row, int at, int c);

void row_delete_char(editor_row *row, int at);

void row_append_string(editor_row *row, char *s, size_t len);

void row_delete(int at);

void save_file();
#endif
