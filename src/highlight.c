#include <stdlib.h>
#include <ctype.h>
#include <termios.h>
#include <string.h>
#include "definitions.h"
#include "filetypes.h"

int is_separator(int c) {
  return isspace(c) || c == '\0' || strchr(",.()+-/*=~&<>[];", c) != NULL;
}

void update_syntax(editor_row *row) {
  row->hl = realloc(row->hl, row->render_size);
  memset(row->hl, HL_NORMAL, row->render_size);
 
  if (config.syntax == NULL) return;

  char **keywords = config.syntax->keywords;
  
  char *scs = config.syntax->sl_comments;
  int scs_len = scs ? strlen(scs) : 0;
  
  int prev_separator = 1;
  int in_string = 0;
  
  int i = 0;
  while (i < row->render_size) {
    char c = row->render[i];
    unsigned char prev_hl = (i > 0) ? row->hl[i - 1] : HL_NORMAL;

    if (scs_len && !in_string) {
      if (!strncmp(&row->render[i], scs, scs_len)) {
	memset(&row->hl[i], HL_COMMENT, row->size - i);
	break;
      }
    }
    
    if (config.syntax->flags & HL_HIGHLIGHT_STRINGS) {
      if(in_string){
	row->hl[i] = HL_STRING;
	if (c == '\\' && i + 1 < row->size) {
	  row->hl[i + 1] = HL_STRING;
	  i += 2;
	  continue;
	}
	if (c == in_string) in_string = 0;
	i++;
	prev_separator = 1;
	continue;
      } else {
	if (c == '"' || c == '\'') {
	  in_string = c;
	  row->hl[i] = HL_STRING;
	  i++;
	  continue;
	}
      }
    }
    
    if (config.syntax->flags & HL_HIGHLIGHT_NUMBERS){
      if(isdigit(c) && (prev_separator || prev_hl == HL_NUMBER) || (c == '.' && prev_hl == HL_NUMBER)){
	row->hl[i] = HL_NUMBER;
	i++;
	prev_separator = 0;
	continue;
      }
    }

    if (prev_separator) {
      int j;
      for (j = 0; keywords[j]; j++){
	int klen = strlen(keywords[j]);
	int types = keywords[j][klen - 1] == '|';
	if (types) klen--;

	if (!strncmp(&row->render[i], keywords[j], klen) && is_separator(row->render[i + klen])) {
	  memset(&row->hl[i], types ? HL_TYPE : HL_KEYWORD, klen);
	  i += klen;
	  break;;
	} 
      }

      if (keywords[j] != NULL) {
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
  case HL_KEYWORD: return 184;
  case HL_TYPE: return 115;
  case HL_COMMENT: return 94;
  case HL_STRING: return 28;
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
