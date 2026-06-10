#include <termios.h>
#include "definitions.h"
#include "row.h"

void insert_char(int c) {
  if(config.cy == config.numrows){
    row_insert(config.numrows, "", 0);
  }
  row_insert_char(&config.row[config.cy], config.cx, c);
  config.cx++;
}

void insert_new_line() {
  if (config.cx == 0){
    row_insert(config.cy, "", 0);
  } else {
    editor_row *row = &config.row[config.cy];
    row_insert(config.cy + 1, &row->chars[config.cx], row->size - config.cx);
    row = &config.row[config.cy];
    row->size = config.cx;
    row->chars[row->size] = '\0';
    row_update(row);
  }
  config.cy++;
  config.cx = 0;
}

void delete_char(){
  if (config.cy == config.numrows) return;
  if (config.cx == 0 && config.cy == 0) return;
  
  editor_row *row = &config.row[config.cy];
  if (config.cx > 0) {
    row_delete_char(row, config.cx - 1);
    config.cx--;
  } else {
    config.cx = config.row[config.cy - 1].size;
    row_append_string(&config.row[config.cy - 1], row->chars, row->size);
    row_delete(config.cy);
    config.cy--;
  }
}
