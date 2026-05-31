#ifndef TERMINAL_H
#define TERMINAL_H

void error(const char *err);

void disable_raw_mode();

void enable_raw_mode();

int read_key();

int get_window_size(int* rows, int* cols);
#endif
