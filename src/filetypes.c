#include "definitions.h"
#include <stdlib.h>
#include <termios.h>

char *C_HL_extensions[] = {".c",".cpp",".h", NULL};

char *C_HL_keywords[] = {
  "switch", "if", "while", "for", "break", "continue", "return", "else",
  "struct", "union", "typedef", "static", "enum", "class", "case",
  
  "int|", "long|", "double|", "float|", "char|", "unsigned|", "signed|",
  "void|", "#include|", "#define|", "#pragma|", NULL
};


char *Py_HL_extensions[] = {".py", NULL};

char *Py_HL_keywords[] = {
  "and", "as", "assert", "async", "await", "break", "case", "class",
  "continue", "def", "del", "elif", "else", "except", "False|", "finally",
  "for", "from", "global", "if", "import", "in", "is", "lambda", "match",
  "None|", "nonlocal", "not", "or", "pass", "raise", "return", "True|", "try",
  "while", "with", "yield",

  "int|", "str|", "float|", NULL
};

struct editor_syntax HLDB[] = {
  {
    "c",
    C_HL_extensions,
    C_HL_keywords,
    "//",
    HL_HIGHLIGHT_NUMBERS | HL_HIGHLIGHT_STRINGS
  },
  {
    "python",
    Py_HL_extensions,
    Py_HL_keywords,
    "#",
    HL_HIGHLIGHT_NUMBERS | HL_HIGHLIGHT_STRINGS
  },
};

int hldb_entries = sizeof(HLDB) / sizeof(HLDB[0]);
