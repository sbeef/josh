#include "josh.h"

char ** parse(char *input) {
  int i;
  char *temp, **args;
  args = malloc(sizeof(char *) * 1024);
  temp = strtok(input, DELIM);
  i = 0;
  while (NULL != temp) {
    args[i] = strtok(NULL, DELIM);
    i++;
  }
  args[i] = NULL;
  return args;
}

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
}
