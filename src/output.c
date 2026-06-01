#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdarg.h>
#include <time.h>
#include "definitions.h"
#include "abuf.h"
#include <string.h>
#include "row.h"

void scroll() {
  config.rx = 0;
  if (config.cy < config.numrows) {
    config.rx = cx_to_rx(&config.row[config.cy], config.cx);
  }
  
  if (config.cy < config.row_offset){
    config.row_offset = config.cy;
  }
  else if (config.cy >= config.row_offset + config.screen_rows) {
    config.row_offset = config.cy - config.screen_rows + 1;
  }
  else if (config.rx < config.column_offset) {
    config.column_offset = config.cx;
  }
  else if ( config.rx >= config.column_offset + config.screen_columns) {
    config.column_offset = config.cx - config.screen_columns + 1;
  }
}

void draw_rows(struct abuf *ab){
  int y;
  for (y = 0; y < config.screen_rows; y++) {

    ab_append(ab,"\x1b[48;5;235m",11);
    
    int filerow = y + config.row_offset;
    if (filerow >= config.numrows) {
      if (config.numrows == 0 && y == config.screen_rows / 3) {;
	char welcome[100];
	int welcomelen = snprintf(welcome, sizeof(welcome), "𝘾𝙒𝙇 𝙏𝙀𝙓𝙏 𝙀𝘿𝙄𝙏𝙊𝙍 - 𝙑𝙀𝙍𝙎𝙄𝙊𝙉 %s", CWL_VERSION);
	if (welcomelen > config.screen_columns) welcomelen = config.screen_columns;
	int padding = (config.screen_columns - welcomelen) / 2 + 10;
	if (padding) {
	  ab_append(ab, "~",1);
	  padding--;
	}
	while (padding--) ab_append(ab," ",1);
	ab_append(ab,"\x1b[38;5;38m\x1b[48;5;235m",21);
	ab_append(ab,welcome,welcomelen);
	ab_append(ab,"\x1b[39m",5);
      }
      else{
	ab_append(ab,"~",1);
      }
    } else {
      int len = config.row[filerow].render_size - config.column_offset;
      if (len < 0) len = 0;
      if (len > config.screen_columns) len = config.screen_columns;
      char *c = &config.row[filerow].render[config.column_offset];
      ab_append(ab, "\x1b[48;5;235m", 11);

      for (int i = 0; i < len; i++) {
	if (isdigit((unsigned char)c[i])) {
	  ab_append(ab, "\x1b[38;5;47m", 10);
	  ab_append(ab, &c[i], 1);
	  ab_append(ab, "\x1b[39m", 5);       
	} else {
	  ab_append(ab, &c[i], 1);
	}
      }
    }
    ab_append(ab,"\x1b[K",3);
    
    ab_append(ab,"\r\n",2);
    
  }
  
}

void status_bar(struct abuf *ab){
  ab_append(ab, "\x1b[48;5;38m\x1b[38;5;15m", 20);
  
  char status[80], rstatus[80];
  int len = snprintf(status, sizeof(status), " %.20s - %d lines %s",
		     config.filename ? config.filename : "[No File]",
		     config.numrows, config.dirty ? "| unsaved" : "");
  int rlen = snprintf(rstatus, sizeof(rstatus), "%d/%d", config.cy + 1, config.numrows);
  
  if (len > config.screen_columns) len = config.screen_columns;
  ab_append(ab, status, len);
  
  while (len < config.screen_columns){
    if(config.screen_columns - len == rlen) {
      ab_append(ab,rstatus,rlen);
      break;
    }
    else{
      ab_append(ab," ",1);
      len++;
    }
  }
  
  ab_append(ab,"\x1b[0m",4);
  ab_append(ab,"\r\n",2);
}

void message_bar(struct abuf *ab){
  ab_append(ab, "\x1b[K", 3);
  int len = config.screen_columns;
  int msglen = strlen(config.statusmsg);
  if (msglen > config.screen_columns) msglen = config.screen_columns;
  if (msglen && time(NULL) - config.statusmsg_time < 5)
    ab_append(ab, config.statusmsg, msglen);
}

void refresh_screen(){
  scroll();
  
  struct abuf ab = ABUF_INIT;

  ab_append(&ab, "\x1b[?25l", 6);

  ab_append(&ab, "\x1b[H",3);
  if (config.numrows > 0)
    ab_append(&ab, "\x1b[48;5;235m",11);

  draw_rows(&ab);
  status_bar(&ab);
  message_bar(&ab);

  char buf[32];
  snprintf(buf,sizeof(buf), "\x1b[%d;%dH", (config.cy - config.row_offset) + 1, (config.rx - config.column_offset) + 1);
  ab_append(&ab, buf, strlen(buf));

  ab_append(&ab, "\x1b[?25h",6);
  write(STDOUT_FILENO, ab.buf, ab.len);
  ab_free(&ab);
}

void set_status_message(const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vsnprintf(config.statusmsg, sizeof(config.statusmsg), fmt, ap);
  va_end(ap);
  config.statusmsg_time = time(NULL);
}
