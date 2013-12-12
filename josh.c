/*
* josh.c
* josh is a shell that handles executing programs, redirection, piping, and backgrounding.
*
* AUTHOR_NAME, DATE
*/

#include "josh.h"

// allocCheck
// Given any pointer to a space in memory, checks is there has been space
// allocated for that pointer. Exits if no space has been allocated.
void allocCheck(void *pointer) {
  if (NULL == pointer) {
    fprintf(stderr, "%s\n", "Could not allocate memory");
    exit(EXIT_FAILURE);
  }
}

// sigHandler
// for CTRL-C. Forwards kill signal to child, without stopping Josh.
void sigHandler(int sig) {
  switch(sig) {
    case SIGINT:
      kill(child, SIGINT);
  }
}

// strRealloc
// Given a string that has more space than it is using, reallocates
// so there is just as much space as the string needs. Reduces allocated memory space.
void strRealloc(char *string) {
  int len;
  len = strlen(string);
  string = realloc(string, len+1);
}

// argFree
// loops through and frees the dynamically allocated "args"
void argFree(char **args) {
  int i = 0;
  while (NULL != args[i])
    free(args[i++]);
  free(args);
}

// p2
// given a command and argument (in form args), forks and execs the call to shell.
// performs redirects, backgrounding and piping as given.
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
    }
    else {
      //printf("fg\n");
      signal(SIGINT, sigHandler);
      wait(&status);
    }
  }
  else {
    int pipebool = 0;
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
    if (NULL != arguments->shell_args[0] && arguments->shell_args[0][0] == '|') {  
      pipebool = 1;
      pid_t forkChild;
      int fd[2];
      pipe(fd);

      forkChild = fork();
      if(-1 == forkChild){
	perror("Fork failed");
	exit(EXIT_FAILURE);
      }
      else if(forkChild == 0){						// Before pipe
	close(fd[0]);
        dup2(fd[1], STDOUT_FILENO);                                    // Change child's input to tunnel output
	close(fd[1]);
        execvp(arguments->program, arguments->program_args);                                         // Executes child's program
      }
      else{								// After pipe
        close(fd[1]);                                                   // Close unnecessary parent input link to tunnel
        dup2(fd[0], STDIN_FILENO);                                     // Change parent's output to tunnel input
	close(fd[0]);
        char *cprog = arguments->shell_args[1];                       // Name child's program
        char **cpargs;                                                // Initialize child's program arguments
        int i = 0;
        while(NULL != arguments->shell_args[i+1])                     // Get number of child's program arguments (terminating NULL included)
          i++;
        cpargs = malloc(sizeof(char *) * (i + 1));
        allocCheck(cpargs);
        for (int j = 0; j < i; j++) {                                 // Fill child's program args
          int len = strlen(arguments->shell_args[j+1]) + 1;
          cpargs[j] = malloc(sizeof(char) * len);
          allocCheck(cpargs[j]);
          strncpy(cpargs[j], arguments->shell_args[j+1], len);
        }
        cpargs[i] = NULL;
        execvp(cprog,cpargs);                                         // Executes child's program
      }
    }
    if(pipebool == 0)
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
  char *usrname = getlogin();
  i = 0;
  input = malloc(sizeof(char) * MAX_INPUT);
  allocCheck(input);
  printf("%s%s",usrname,josh_prompt);
  while (EOF != (c = getchar())) {
    if (i == MAX_INPUT) {
      // handle overflow
    } else if ('\n' == c) {
      input[i] = '\0';
      if(strcmp("exit",input) == 0){
	free(input);
        break;
      } 
      arguments = parse(input);
      p2(arguments);
      allocCheck(input);
      i = 0;
      printf("%s%s",usrname,josh_prompt);
    } else {
      input[i] = c;
      i++;
    }
  }
}
