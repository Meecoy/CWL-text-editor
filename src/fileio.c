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
#include "row.h"
#include "highlight.h"

void open_file(char *filename){
  free(config.filename);
  config.filename = strdup(filename);

  select_highlight();

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

void save_file() {
  if (config.filename == NULL) {
    config.filename = prompt("Save as: %s (ESC to cancel)", NULL);
    if (config.filename == NULL){
      set_status_message("Aborted");
      return;
    }
    select_highlight();
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
