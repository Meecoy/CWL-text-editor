#include <string.h>
#include <stdlib.h>
#include <termios.h>
#include "input.h"
#include "definitions.h"
#include "output.h"
#include "row.h"

void find_index_callback(char *query, int key) {
  static int last_match = -1;
  static int direction = 1;


  static int saved_hl_line;
  static char *saved_hl = NULL;

  if (saved_hl) {
    memcpy(config.row[saved_hl_line].hl, saved_hl, config.row[saved_hl_line].render_size);
    free(saved_hl);
    saved_hl = NULL;
  }
  
  if (key == '\r' || key == '\x1b') {
    last_match = -1;
    direction = 1;
    return;
  } else if (key == ARROW_RIGHT || key == ARROW_DOWN) {
    direction = 1;
  } else if (key == ARROW_LEFT || key == ARROW_UP) {
    direction = -1;
  }
  else {
    last_match = -1;
    direction = 1;
  }

  if (last_match == -1) direction = 1;
  int current = last_match; // Current row index

  for(int i = 0; i < config.numrows; i++){
    current += direction;
    if (current == -1) current = config.numrows - 1;
    else if ( current == config.numrows) current = 0;
    
    editor_row *row = &config.row[current];
    char *match = strstr(row->render, query);
    if (match){
      last_match = current;
      config.cy = current;
      config.cx = rx_to_cx(row, match - row->render);
      config.row_offset = config.numrows;
      set_status_message("Match found at line %d", config.cy + 1);


      saved_hl_line = current;
      saved_hl = malloc(row->render_size);
      memcpy(saved_hl, row->hl, row->render_size);
      memset(&row->hl[match - row->render], HL_MATCH, strlen(query));
      
      break;
    }
  }
}

void find_index(){
  int s_cx = config.cx;
  int s_cy = config.cy;
  int s_c_off = config.column_offset;
  int s_r_off = config.row_offset;
  
  char *query = prompt("Search: %s (ESC to abort)", find_index_callback);

  if (query) free(query);
  else {
    config.cx = s_cx;
    config.cy = s_cy;
    config.column_offset = s_c_off;
    config.row_offset = s_r_off;
  }
}
