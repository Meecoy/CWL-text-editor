#ifndef ABUF_H
#define ABUF_H

#define ABUF_INIT {NULL, 0}

struct abuf {
  char* buf;
  int len;
};

void ab_append(struct abuf* ab, const char* s, int len);

void ab_free(struct abuf* ab);
#endif
