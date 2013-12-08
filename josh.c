#include "josh.h"


void allocCheck(void *pointer) {
  if (NULL == pointer) {
    fprintf(stderr, "%s\n", "Could not allocate memory");
    exit(EXIT_FAILURE);
  }
}

void sigHandler(int sig) {
  switch(sig) {
    case SIGINT:
      kill(child, SIGINT);
  }
}

void strRealloc(char *string) {
  int len;
  len = strlen(string);
  string = realloc(string, len+1);
}

void argFree(char **args) {
  int i = 0;
  while (NULL != args[i])
    free(args[i++]);
  free(args);
}

// gets just the args for the program
char ** pargs(char **args) {
  int i, j, len;
  i = j = 0;
  char c, **nargs;
  while (NULL != args[i]) {
    c = args[i][0];
    if ('|' == c || '<' == c || '>' == c || '&' == c)
      break;
    i++;
  }
  nargs = malloc(sizeof(char *) * (i+1));
  fflush(stdout);
  for (j = 0; j < i; j++) {
    len = strlen(args[j]) + 1;
    nargs[j] = malloc(sizeof(char) * len);
    allocCheck(args[j]);
    strncpy(nargs[j], args[j], len);
  }
  nargs[j] = NULL;
  return nargs;
}

void p2(struct args *arguments){
  /*while (NULL != args[len])
    len++;*/
  child = fork();
  if (-1 == child){
    perror("Fork failed");
    exit(EXIT_FAILURE);
  }
  if (child){
    int status;
    if ('&' == arguments->shell_args[0][0]) {
      printf("anded");
      waitpid(-1, &status, WNOHANG);
    } else {
      //printf("fg\n");
      signal(SIGINT, sigHandler);
      wait(&status);
    }
  }
  else {
    execvp(arguments->program, arguments->program_args);
    perror("Exec failed");
    //argFree(args);
    exit(EXIT_FAILURE);
  }

}

char ** parseold(char *input) {
    int i;
    char **args;
    args = malloc(sizeof(char *) * 1024);
    allocCheck(args);
    i = 1;
    for (args[0] = strtok(input, DELIM); NULL != args[i-1]; i++) 
      args[i] = strtok(NULL, DELIM);
    args[i] = NULL;
    return args;
}

struct args * parse(char *input) {
  struct args *arguments = malloc(sizeof(struct args));
  allocCheck(arguments);
  int i = 1;
  char **args, temp, tokTemp;
  args = malloc(sizeof(char *) * MAX_INPUT);
  allocCheck(args);
  for (args[0] = strtok(input, DELIM); NULL != args[i-1]; i++)
    args[i] = strtok(NULL, DELIM);
  args[i] = NULL;
  args = realloc(args, sizeof(char *) * (i+1));
  allocCheck(args);
  arguments->program = malloc(sizeof(char) * MAX_INPUT);
  allocCheck(args);
  strncpy(arguments->program, args[0], strlen(args[0]) + 1);
  int j, len, size;
  i = j = 0;
  char c;
  while (NULL != args[i]) {
    c = args[i][0];
    if ('|' == c || '<' == c || '>' == c || '&' == c)
      break;
    i++;
  }
  arguments->program_args = malloc(sizeof(char *) * (i + 1));
  for (j = 0; j < i; j++) {
    len = strlen(args[j]) + 1;
    arguments->program_args[j] = malloc(sizeof(char) * len);
    allocCheck(arguments->program_args[j]);
    strncpy(arguments->program_args[j], args[j], len);
  }
  arguments->program_args[j] = NULL;
  i = i-1;
  size = 0;
  while (NULL != args[i+size]) {
    size ++;
  }
  arguments->shell_args = malloc(sizeof(char *) * (size));
  for (j = 0; j < (size); j++) {
    len = strlen(args[j + i]) + 1;
    arguments->shell_args[j] = malloc(sizeof(char) * len);
    allocCheck(arguments->shell_args[j]);
    strncpy(arguments->shell_args[j], args[j+i], len);
  }
  return arguments;
}

char ** parse_old(char *input) {
  int i;
  char **args, *temp, *tokTemp;
  args = malloc(sizeof(char *) * MAX_INPUT);
  allocCheck(args);
  i = 1;
  temp = malloc(sizeof(char) * MAX_INPUT);
  allocCheck(temp);
  tokTemp = strtok(input, DELIM);
  temp = strncpy(temp, tokTemp, MAX_INPUT);
  //strRealloc(temp);
  allocCheck(temp);
  for (args[0] = temp; NULL != args[i-1]; i++) {
    temp = malloc(sizeof(char) * MAX_INPUT);
    allocCheck(temp);
    tokTemp = strtok(NULL, DELIM);
    if (NULL == tokTemp)
      break;
    temp = strncpy(temp, tokTemp, MAX_INPUT);
    //temp = strncpy(temp, strtok(NULL, DELIM), MAX_INPUT);
    //strRealloc(temp);
    allocCheck(temp);
    args[i] = temp;
  }
  args[i] = NULL;
  return args;
}

int main() {
  int c, i;
  struct args *arguments;
  char **args, *input;
  i = 0;
  input = malloc(sizeof(char) * MAX_INPUT);
  allocCheck(input);
  printf("$");
  while (EOF != (c = getchar())) {
    if (i == MAX_INPUT) {
      // handle overflow
    } else if ('\n' == c) {
      input[i] = '\0';
      arguments = parse(input);
      p2(arguments);
      allocCheck(input);
      i = 0;
      printf("$");
    } else {
      input[i] = c;
      i++;
    }
  }
}
