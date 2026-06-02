#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#define CTRL_KEY(k) ((k) & 0x1f)
#define CWL_VERSION "0.2.1"
#define TAB_STOP 4
#define QUIT_INPUTS 2
#define HL_HIGHLIGHT_NUMBERS (1<<0)
#define HL_HIGHLIGHT_STRINGS (1<<1)

#include <termios.h>
#include <time.h>

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

enum highlight {
  HL_NORMAL = 0,
  HL_NUMBER,
  HL_MATCH,
  HL_STRING,
  HL_COMMENT,
  HL_KEYWORD,
  HL_TYPE
};

typedef struct editor_row{
  int size;
  int render_size;
  char *chars;
  char *render;
  unsigned char *hl;
} editor_row;

struct editor_syntax {
  char *filetype;
  char **filematch;
  char **keywords;
  char *sl_comments;
  int flags;
};

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
  struct editor_syntax *syntax;
  struct termios origin_t;
};

extern struct editor_config config;
#endif
