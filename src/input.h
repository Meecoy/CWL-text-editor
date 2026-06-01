#ifndef INPUT_H
#define INPUT_H

void process_keypress();

char *prompt(char *prompt, void (*callback)(char *, int));
#endif
