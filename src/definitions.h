#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#define CTRL_KEY(k) ((k) & 0x1f)
#define CWL_VERSION "0.2.0"
#define TAB_STOP 4
#define QUIT_INPUTS 2
enum keys {
  BACKSPACE = 127,
  ARROW_LEFT = 1000,
  ARROW_RIGHT,
  ARROW_UP,
  ARROW_DOWN,
  DELETE_KEY,
  HOME_KEY,
  END_KEY,
  PAGE_UP,
  PAGE_DOWN
};

typedef struct editor_row{
  int size;
  int render_size;
  char* chars;
  char* render;
} editor_row;

struct editor_config {
  int cx, cy;
  int rx;
  int row_offset;
  int column_offset;
  int screen_rows;
  int screen_columns;
  int numrows;
  int dirty;
  editor_row *row;
  char *filename;
  char statusmsg[80];
  time_t statusmsg_time;
  struct termios origin_t;
};

extern struct editor_config config;
#endif
