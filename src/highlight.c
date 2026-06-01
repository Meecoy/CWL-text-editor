#include <stdlib.h>
#include <ctype.h>
#include <termios.h>
#include <string.h>
#include "definitions.h"

int is_separator(int c) {
  return isspace(c) || c == '\0' || strchr(",.()+-/*=~&<>[];", c) != NULL;
}

void update_syntax(editor_row *row) {
  row->hl = realloc(row->hl, row->render_size);
  memset(row->hl, HL_NORMAL, row->render_size);
 
  if (config.syntax == NULL) return;
  
  int prev_separator = 1;
  
  int i = 0;
  while (i < row->render_size) {
    char c = row->render[i];
    unsigned char prev_hl = (i > 0) ? row->hl[i - 1] : HL_NORMAL;
    if (config.syntax->flags & HL_HIGHLIGHT_NUMBERS){
      if(isdigit(c) && (prev_separator || prev_hl == HL_NUMBER) || (c == '.' && prev_hl == HL_NUMBER)){
	row->hl[i] = HL_NUMBER;
	i++;
	prev_separator = 0;
	continue;
      }
    }
    prev_separator = is_separator(c);
    
    i++;
  }
}

int syntax_to_color(int hl){
  switch(hl){
  case HL_NUMBER: return 47;
  case HL_MATCH: return 37;
  default: return 231;
  }
}

void select_highlight(){
  config.syntax = NULL;
  if (config.filename == NULL) return;

  char *ext = strrchr(config.filename, '.');

  for (unsigned int i = 0; i < hldb_entries; i++){
    struct editor_syntax *s = &HLDB[i];
    unsigned int j = 0;
    while (s->filematch[j]){
      int is_ext = (s->filematch[j][0] == '.');
      if((is_ext && ext && !strcmp(ext, s->filematch[j])) ||
	 (!is_ext && strstr(config.filename, s->filematch[j]))){
	config.syntax = s;

	for(int filerow = 0; filerow < config.numrows; filerow++) {
	  update_syntax(&config.row[filerow]);
	}
	return;
      }
      j++;
    }
  }
}
