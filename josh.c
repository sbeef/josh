#include "josh.h"

int main() {
  int c, i;
  char input[MAX_INPUT];
  i = 0;
  while (EOF != (c = getchar())) {
    if (i == MAX_INPUT) {
      // handle overflow
    } else if ('\n' == c) {
      input[i] = c;
      parse(input);
      i = 0;
    } else 
      input[i] = c;
  }
