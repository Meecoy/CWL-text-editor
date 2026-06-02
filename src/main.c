#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include "terminal.h"
#include "input.h"
#include "output.h"
#include "definitions.h"
#include "fileio.h"
#include "filetypes.h"

struct editor_config config;

void init() {
  config.cx = 0;
  config.cy = 0;
  config.rx = 0;
  config.row_offset = 0;
  config.column_offset = 0;
  config.numrows = 0;
  config.dirty = 0;
  config.row = NULL;
  config.filename = NULL;
  config.statusmsg[0] = '\0';
  config.statusmsg_time = 0;
  config.syntax = NULL;
  
  if (get_window_size(&config.screen_rows, &config.screen_columns) == -1) error("Unable to get windows size.");
  config.screen_rows -= 2;
}

int main(int argc, char *argv[]){
  enable_raw_mode();
  init();
  if (argc >= 2) {
    open_file(argv[1]);
  }

  set_status_message("CTRL+Q to quit | CTRL+S to save file");
  
  while (1){
    refresh_screen();
    process_keypress();
  }
  return 0;
}
