#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <stdlib.h>
#include <ctype.h>
#include "definitions.h"
#include <sys/ioctl.h>

void error(const char *err){
  write(STDOUT_FILENO,"\x1b[2J",4);
  write(STDOUT_FILENO,"\x1b[H",3);
  
  perror(err);
  exit(1);
}

void disable_raw_mode(){
  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &config.origin_t) == -1) error("Couldn't set terminal attributes when disabling raw mode.");
}

void enable_raw_mode(){
  if (tcgetattr(STDIN_FILENO, &config.origin_t) == -1) error("Couldn't get terminal attributes.");
  atexit(disable_raw_mode);
  
  struct termios raw = config.origin_t;

  // Flipping bits on terminal flags
  
  raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
  raw.c_oflag &= ~(OPOST);
  raw.c_cflag |= (CS8);
  raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);

  raw.c_cc[VMIN] = 0;
  raw.c_cc[VTIME] = 1;
  
  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1 ) error("Couldn't set terminal attributes when enabling raw mode.");

}

int read_key(){
  int nread;
  char input;

  while ((nread = read(STDIN_FILENO, &input, 1)) != 1) {
    if (nread == -1) error ("Couldn't read input key.");
  }

  if (input == '\x1b') {
    char seq[3];
    // return ESC if it doesn't have anything else
    if (read(STDIN_FILENO, &seq[0], 1) != 1) return '\x1b';
    if (read(STDIN_FILENO, &seq[1], 1) != 1) return '\x1b';
    
    if (seq[0] == '[') {
      if (seq[1] >= '0' && seq[1] <= '9') {
	if (read(STDIN_FILENO, &seq[2], 1) != 1) return '\x1b';
	if (seq[2] == '~') {
	  switch(seq[1]){
	  case '1': return HOME_KEY;
	  case '3': return DELETE_KEY;
	  case '4': return END_KEY;
	  case '5': return PAGE_UP;
	  case '6': return PAGE_DOWN;
	  case '7': return HOME_KEY;
	  case '8': return END_KEY;
	  }
	}
      } else{
      switch (seq[1]) {
      case 'A': return ARROW_UP;
      case 'B': return ARROW_DOWN;
      case 'C': return ARROW_RIGHT;
      case 'D': return ARROW_LEFT;
      case 'H': return HOME_KEY;
      case 'F': return END_KEY;
      }
      }
    } else if (seq[0] == 'O') {
      switch (seq[1]) {
      case 'H': return HOME_KEY;
      case 'F': return END_KEY;
      }
    }
    return '\x1b';
  } else {
    return input;
  }
}

// In case ioctl doesn't work

int get_cursor_position(int *rows, int *cols) {
  char buf[32];
  unsigned int i = 0;

  if(write(STDOUT_FILENO, "\x1b[6n", 4) != 4) return -1;

  while (i < sizeof(buf) - 1){
    if(read(STDIN_FILENO, &buf[i], 1) != 1) break;
    if(buf[i] == 'R') break;
    i++;
  }
  buf[i] = '\0';

  if (buf[0] != '\x1b' || buf[1] != '[') return -1;
  if(sscanf(&buf[2], "%d;%d", rows, cols) != 2) return -1;

  return 0;
  
}



int get_window_size(int* rows, int* cols){
  struct winsize ws;

  if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0){
    if (write(STDOUT_FILENO, "\x1b[999C\x1b[999B", 12) != 12) return -1;
    return get_cursor_position(rows, cols);
  }
  else {
    *rows = ws.ws_row;
    *cols = ws.ws_col;
    return 0;
  }
}
