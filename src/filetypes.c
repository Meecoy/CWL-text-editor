#include "definitions.h"
#include <stdlib.h>
#include <termios.h>

char *C_HL_extensions[] = {".c",".cpp",".h", NULL};

char *C_HL_keywords[] = {
  "switch", "if", "while", "for", "break", "continue", "return", "else",
  "struct", "union", "typedef", "static", "enum", "class", "case",
  
  "int|", "long|", "double|", "float|", "char|", "unsigned|", "signed|",
  "void|", NULL
};

struct editor_syntax HLDB[] = {
  {
    "c",
    C_HL_extensions,
    C_HL_keywords,
    "//",
    HL_HIGHLIGHT_NUMBERS | HL_HIGHLIGHT_STRINGS
  },
};

int hldb_entries = sizeof(HLDB) / sizeof(HLDB[0]);
