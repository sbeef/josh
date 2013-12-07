#include "josh.h"

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
  temp = strtok(input, DELIM);
  i = 0;
  while (NULL != temp) {
    args[i] = strtok(NULL, DELIM);
    i++;
  }
  args[i] = NULL;
  return args;
}

/*int main() {
  int c, i;
  char *prompt = "$";
  char input[MAX_INPUT];
  i = 0;
  printf("%s ", prompt);
  while (EOF != (c = getchar())) {
    if (i == MAX_INPUT) {
      // handle overflow
    } 
    else if ('\n' == c) {
      input[i] = c;
      char **args = parse(input);
      i = 0;
      printf("%s ", prompt);
      if (args[1] == NULL){
        p2(args[0]);
      }
    }
    else 
      input[i] = c;
  }
}*/

int main(){
  char **args;
  args = malloc(sizeof(char *) * 10);
  args[0] = "echo";
  args[1] = "hi";
  args[2] = NULL;
  p2(args);
}
