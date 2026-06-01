#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <stdlib.h>
#include <ctype.h>
#include "terminal.h"
#include "definitions.h"
#include "operations.h"
#include "fileio.h"
#include "output.h"
#include "find.h"

char *prompt(char *prompt, void (*callback)(char *, int)){
  size_t bufsize = 128;
  char *buf = malloc(bufsize);

  size_t buflen = 0;
  buf[0] = '\0';

  while(1){
    set_status_message(prompt,buf);
    refresh_screen();
    
    int c = read_key();
    if(c == DELETE_KEY || c == CTRL_KEY('h') || c == BACKSPACE){
      if (buflen != 0) buf[--buflen] = '\0';
    } else if(c == '\x1b'){
      set_status_message("");
      if (callback) callback(buf,c);
      free(buf);
      return NULL;
    } else if (c == '\r'){
      if(buflen != 0){
	set_status_message("");
	if (callback) callback(buf,c);
	return buf;
      }
    } else if (!iscntrl(c) && c < 128) {
      if (buflen == bufsize - 1) {
	bufsize *= 2;
	buf = realloc(buf, bufsize);
      }
      buf[buflen++] = c;
      buf[buflen] = '\0';
    }

    if (callback) callback(buf,c);
  }
}

void move_cursor(int key) {
  editor_row* row = (config.cy >= config.numrows) ? NULL : &config.row[config.cy];
  switch(key){
  case ARROW_LEFT:
    if(config.cx != 0){
      config.cx--;
    } else if (config.cy > 0) {
      config.cy--;
      config.cx = config.row[config.cy].size;
    }
    break;
  case ARROW_RIGHT:
    if (row && config.cx < row->size){
      config.cx++;
    } else if (row && config.cx == row->size){
      config.cy++;
      config.cx = 0;
    }
    break;
  case ARROW_UP:
    if(config.cy != 0) config.cy--;
    break;
  case ARROW_DOWN:
    if(config.cy < config.numrows) config.cy++;
    break;
  }

  row = (config.cy >= config.numrows) ? NULL : &config.row[config.cy];
  int rowlen = row ? row->size : 0;
  if (config.cx > rowlen) config.cx = rowlen;
}

void process_keypress() {
  static int quit = QUIT_INPUTS;
  int c = read_key();
  editor_row* row = (config.cy >= config.numrows) ? NULL : &config.row[config.cy];
  switch (c) {
  case '\r':
    insert_new_line();
    break;
  case CTRL_KEY('q'):
    if (config.dirty && quit > 0){
      set_status_message("File unsaved! Exit %d more times to quit without saving.", quit);
      quit--;
      return;
    }
    write(STDOUT_FILENO,"\x1b[2J",4);
    write(STDOUT_FILENO,"\x1b[H",3);
    exit(0);
    break;

  case CTRL_KEY('s'):
    save_file();
    break;

  case CTRL_KEY('f'):
    find_index();
    break;
    
  case HOME_KEY:
    config.cx = 0;
    break;
  case END_KEY:
    if(config.cy < config.numrows)
      config.cx = config.row[config.cy].size;
    break;

  case DELETE_KEY:
  case BACKSPACE:
    if (c == DELETE_KEY) move_cursor(ARROW_RIGHT);
    delete_char();
    break;
  case CTRL_KEY('h'):
    break;
  case PAGE_UP:
  case PAGE_DOWN:
    {
      if (c == PAGE_UP) {
        config.cy = config.row_offset;
      } else if (c == PAGE_DOWN) {
        config.cy = config.row_offset + config.screen_rows - 1;
        if (config.cy > config.numrows) config.cy = config.numrows;
      }
      int times = config.screen_rows;
      while (times--)
        move_cursor(c == PAGE_UP ? ARROW_UP : ARROW_DOWN);
    }
    break;
  case ARROW_UP:
  case ARROW_DOWN:
  case ARROW_LEFT:
  case ARROW_RIGHT:
    move_cursor(c);
    break;

  case CTRL_KEY('l'):
  case '\x1b':
    break;

  default:
    insert_char(c);
    break;
  }

  quit = QUIT_INPUTS;
}


