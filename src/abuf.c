#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "abuf.h"

void ab_append(struct abuf* ab, const char* s, int len){
  char* new = realloc(ab->buf, ab->len + len);

  if (new == NULL) return;
  memcpy(&new[ab->len], s, len);
  ab->buf = new;
  ab->len += len;
}

void ab_free(struct abuf* ab){
  free(ab->buf);
}
