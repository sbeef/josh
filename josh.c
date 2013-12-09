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



void p2(struct args *arguments){
  FILE *input, *output;
  /*while (NULL != args[len])
    len++;*/
  child = fork();
  if (-1 == child){
    perror("Fork failed");
    exit(EXIT_FAILURE);
  }
  if (child){
    int status;
    if (NULL != arguments->shell_args[0] && '&' == arguments->shell_args[0][0]) {
      printf("anded");
      waitpid(-1, &status, WNOHANG);
    } else {
      //printf("fg\n");
      signal(SIGINT, sigHandler);
      wait(&status);
    }
  }
  else {
    if (NULL != arguments->shell_args[0] && '<' == arguments->shell_args[0][0]) {
      input = fopen(arguments->shell_args[1], "r");
      dup2(fileno(input), STDIN_FILENO);
      fclose(input);
    }
    if (NULL != arguments->shell_args[0] && '>' == arguments->shell_args[0][0]) {
      output = fopen(arguments->shell_args[1], "w");
      dup2(fileno(output), STDOUT_FILENO);
      fclose(output);
    }
    execvp(arguments->program, arguments->program_args);
    perror("Exec failed");
    //argFree(args);
    exit(EXIT_FAILURE);
  }

}


/* figures out what's what*/
struct args * parse(char *input) {
  // the arguements will go here
  struct args *arguments = malloc(sizeof(struct args));
  allocCheck(arguments); //did malloc work?
  int i = 1; //this will be important in the later day
  char **args;// the tokenized input
  args = malloc(sizeof(char *) * MAX_INPUT);
  allocCheck(args);
  /*TOKENIZE THAT!  YEA*/
  for (args[0] = strtok(input, DELIM); NULL != args[i-1]; i++)
    args[i] = strtok(NULL, DELIM);
  args[i] = NULL;
  //shrink it down to size
  args = realloc(args, sizeof(char *) * (i+1));
  allocCheck(args);
  /* alrighty, what's the program we should run */
  arguments->program = malloc(sizeof(char) * MAX_INPUT);
  allocCheck(args);
  strncpy(arguments->program, args[0], strlen(args[0]) + 1);
  int j, len, size;
  i = j = 0;
  char c;
  /* how many arguments apply to this specific program? */
  while (NULL != args[i]) {
    c = args[i][0];
    if ('|' == c || '<' == c || '>' == c || '&' == c)
      break;
    i++;
  }
  arguments->program_args = malloc(sizeof(char *) * (i + 1));
  /* now fill program_args with the arguments for that program */
  for (j = 0; j < i; j++) {
    len = strlen(args[j]) + 1;
    arguments->program_args[j] = malloc(sizeof(char) * len);
    allocCheck(arguments->program_args[j]);
    strncpy(arguments->program_args[j], args[j], len);
  }
  arguments->program_args[j] = NULL;
  /* now figure out how many arguments are left */
  //i = i-1;
  size = 0;
  while (NULL != args[i+size]) {
    size ++;
  }
  /* stick the remaining arguments in the shell_args */
  arguments->shell_args = malloc(sizeof(char *) * (size + 1));
  for (j = 0; j < (size); j++) {
    len = strlen(args[j + i]) + 1;
    arguments->shell_args[j] = malloc(sizeof(char) * len);
    allocCheck(arguments->shell_args[j]);
    strncpy(arguments->shell_args[j], args[j+i], len);
  }
  arguments->shell_args[j] = NULL;
  // you're done!
  i = 0;
  /*while(NULL != arguments->shell_args[i]) {
    printf("arg[%d]: %s\n", i, arguments->shell_args[i]);
    i++;
  }*/
  return arguments;
}


int main() {
  int c, i;
  struct args *arguments;
  char *input;
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
