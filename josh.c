#include "josh.h"

void allocCheck(void *pointer) {
  if (NULL == pointer) {
    fprintf(stderr, "%s\n", "Could not allocate memory");
    exit(EXIT_FAILURE);
  }
}

void p2(char **args){
  pid_t child = fork();
  if (-1 == child){
    perror("Fork failed");
    exit(EXIT_FAILURE);
  }
  if (child){
    int status;
    wait(&status);
  }
  else {
    execvp(args[0], args);
    perror("Exec failed");
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
  return args;
}

int main() {
  int c, i;
  char **args, *input;
  i = 0;
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
