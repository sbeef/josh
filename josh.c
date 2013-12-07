#include "josh.h"

void allocCheck(void *pointer) {
  if (NULL == pointer) {
    fprintf(stderr, "%s\n", "Could not allocate memory");
    exit(EXIT_FAILURE);
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

char ** parse(char *input) {
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
  printf("MAXINPUT:%d\n", MAX_INPUT);
  int c, i;
  char **args, *input;
  i = 0;
  input = malloc(sizeof(char) * MAX_INPUT);
  allocCheck(input);
  while (EOF != (c = getchar())) {
    if (i == MAX_INPUT) {
      // handle overflow
    } else if ('\n' == c) {
      input[i] = '\0';
      args = parse(input);
      p2(args);
      allocCheck(input);
      i = 0;
    } else {
      input[i] = c;
      i++;
    }
  }
}
