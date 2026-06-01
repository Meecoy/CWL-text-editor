#define _DEFAULT_SOURCE
#define _BSD_SOURCE
#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include "input.h"
#include "terminal.h"
#include "definitions.h"
#include "output.h"

// MOVE IT LATER TO ROW.C

void row_update(editor_row* row){
  int tabs = 0;
  for(int i = 0; i < row->size; i++) if(row->chars[i] == '\t') tabs++;
  
  free(row->render);
  row->render = malloc(row->size + tabs*(TAB_STOP - 1) + 1);
  int idx = 0;
  for (int i = 0; i < row->size; i++){
    if (row->chars[i] == '\t') {
      row->render[idx++] = ' ';
      while (idx % TAB_STOP != 0) row->render[idx++] = ' ';
    }else{
      row->render[idx++] = row->chars[i];
    }
  }
  row->render[idx] = '\0';
  row->render_size = idx;
}

void row_insert(int at, char *s, size_t len){
  if (at < 0 || at > config.numrows) return;

  config.row = realloc(config.row, sizeof(editor_row) * (config.numrows + 1));
  memmove(&config.row[at + 1], &config.row[at], sizeof(editor_row) * (config.numrows - at));
  config.row[at].size = len;
  config.row[at].chars = malloc(len + 1);
  memcpy(config.row[at].chars, s, len);
  config.row[at].chars[len] = '\0';

  config.row[at].render_size = 0;
  config.row[at].render = NULL;
  row_update(&config.row[at]);
  
  config.numrows++;
  config.dirty++;
}

void row_insert_char(editor_row *row, int at, int c){
  if( at < 0 || at > row->size) at = row->size;
  row->chars = realloc(row->chars, row->size+2);
  memmove(&row->chars[at + 1], &row->chars[at], row->size - at + 1);
  row->size++;
  row->chars[at] = c;
  row_update(row);
  config.dirty++;
}

char *rows_to_string(int *buflen) {
  int totlen = 0;
  for (int i = 0; i < config.numrows; i++)
    totlen += config.row[i].size + 1;

  *buflen = totlen;

  char *buf = malloc(totlen);
  char *p = buf;
  for (int i = 0; i < config.numrows; i++) {
    memcpy(p, config.row[i].chars, config.row[i].size);
    p += config.row[i].size;
    *p = '\n';
    p++;
  }

  return buf;
}
// ---------------------
void open_file(char *filename){
  free(config.filename);
  config.filename = strdup(filename);
  FILE *fp = fopen(filename, "r");
  if (!fp) error("Can't open the file.");

  char *line = NULL;
  size_t linecap = 0;
  ssize_t linelen;
  
    while ((linelen = getline(&line, &linecap, fp)) != -1){
      while(linelen > 0 && (line[linelen - 1] == '\n' || line[linelen - 1] == '\r'))
	linelen--;
      row_insert(config.numrows, line, linelen);
    }
 
  free(line);
  fclose(fp);
  config.dirty = 0;
}

// MOVE IT LATER TO ROW.C

void row_delete_char(editor_row *row, int at){
  if (at < 0 || at >= row->size) return;
  memmove(&row->chars[at], &row->chars[at + 1], row->size - at);
  row->size--;
  row_update(row);
  config.dirty++;
}

void row_free(editor_row *row){
  free(row->render);
  free(row->chars);
}

void row_delete(int at) {
  if (at < 0 || at >= config.numrows) return;
  row_free(&config.row[at]);
  memmove(&config.row[at], &config.row[at + 1], sizeof(editor_row) * (config.numrows - at - 1));
  config.numrows--;
  config.dirty++;
}

void row_append_string(editor_row *row, char *s, size_t len){
  row->chars = realloc(row->chars, row->size + len + 1);
  memcpy(&row->chars[row->size], s, len);
  row->size += len;
  row->chars[row->size] = '\0';
  row_update(row);
  config.dirty++;
}

// ------------------------

void save_file() {
  if (config.filename == NULL) {
    config.filename = prompt("Save as: %s (ESC to cancel)", NULL);
    if (config.filename == NULL){
      set_status_message("Aborted");
      return;
    }
  };

  int len;
  char *buf = rows_to_string(&len);

  int fd = open(config.filename, O_RDWR | O_CREAT, 0644);
  if(fd != -1) {
    if(ftruncate(fd, len) != -1){
      if(write(fd,buf,len) == len) {
	close(fd);
	free(buf);
	set_status_message("%d bytes saved to %s", len, config.filename);
	config.dirty = 0;
	return;
      }
    }
    close(fd);
  }
  free(buf);
  set_status_message("Unable to save %s", strerror(errno));
}
