#include "josh.h"


void allocCheck(void *pointer) {
  if (NULL == pointer) {
    fprintf(stderr, "%s\n", "Could not allocate memory");
    exit(EXIT_FAILURE);
  }
}

char ** parse(char *input) {
  int i;
  char *temp, **args;
  args = malloc(sizeof(char *) * 1024);
  allocCheck(args);
  i = 1;
  for (args[0] = strtok(input, DELIM); NULL != args[i-1]; i++) 
    args[i] = strtok(NULL, DELIM);
  args[i] = NULL;
  args = realloc(args, sizeof(char *) * (i+1));
  allocCheck(args);
  return args;
}

int main() {
  int c, i;
  char **args, *input;
  i = 0;
  input = malloc(sizeof(char) * MAX_INPUT);
  allocCheck(input);
  while (EOF != (c = getchar())) {
    if (i == MAX_INPUT) {
      // handle overflow
    } else if ('\n' == c) {
      input[i] = c;
      args = parse(input);
      i = 0;
    } else {
      input[i] = c;
      i++;
    }
  }
}
