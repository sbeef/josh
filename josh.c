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

void freeArgs(struct args *arguments) {
  free(arguments->program);
  string_array_free(arguments->program_args);
  string_array_free(arguments->pipe_args);
  free(arguments->out_file);
  free(arguments->in_file);
  free(arguments);
}

void string_array_free(char ** strings) {
  int i = 0;
  while (NULL != strings[i])
    free(strings[i++]);
  free(strings);
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
    if  (arguments->background) {
      printf("anded");
      waitpid(-1, &status, WNOHANG);
    }
    else {
      //printf("fg\n");
      signal(SIGINT, sigHandler);
      wait(&status);
      freeArgs(arguments);
    }
  }
  else {
    int pipebool = 0;
    if (arguments->in_redir) {
      input = fopen(arguments->in_file, "r");
      dup2(fileno(input), STDIN_FILENO);
      fclose(input);
    }
    if (arguments->out_redir) {
      output = fopen(arguments->out_file, "w");
      dup2(fileno(output), STDOUT_FILENO);
      fclose(output);
    }
    /*if (NULL != arguments->shell_args[0] && arguments->shell_args[0][0] == '|') {  
      pipebool = 1;
      pid_t forkChild;
      int fd[2];
      pipe(fd);

      forkChild = fork();
      if(-1 == forkChild){
  	perror("Fork failed");
  	exit(EXIT_FAILURE);
      }
      else if(forkChild == 0){            // Before pipe
  	close(fd[0]);
        dup2(fd[1], STDOUT_FILENO);                                    // Change child's input to tunnel output
  	close(fd[1]);
        execvp(arguments->program, arguments->program_args);                                         // Executes child's program
      }
 tring_array_free(args);
 else{                // After pipe
        close(fd[1]);                                                   // Close unnecessary parent input link to tunnel
        dup2(fd[0], STDIN_FILENO);                                     // Change parent's output to tunnel input
  	close(fd[0]);
        char *cprog = arguments->shell_args[1];                       // Name child's program
        char **cpargs;                                                // Initialize child's program arguments
        int i = 0;
        while(NULL != arguments->shell_args[i+1])                     // Get number of child's program arguments (terminating NULL included)
          i++;
        cpargs = malloc(sizeof(char *) * (i + 1));
  string_array_free(args);
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
    }*/
    if(pipebool == 0)
      execvp(arguments->program, arguments->program_args);
    perror("Exec failed");
    //argFree(args);
    exit(EXIT_FAILURE);
  }

}

struct args * arginit() {
  struct args *arguments = malloc(sizeof(struct args));
  allocCheck(arguments);
  arguments->program = NULL;
  arguments->program_args = NULL;
  arguments->out_redir = 0;
  arguments->in_redir = 0;
  arguments->in_file = NULL;
  arguments->out_file = NULL;
  arguments->background = 0; 
  arguments->pipe_args = NULL;
  return arguments;
}


/* figures out what's what*/
struct args * parse(char *input) {
  // the arguements will go here
  struct args *arguments = arginit();
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
  //arguments->shell_args = malloc(sizeof(char *) * (size + 1));
  for (j = 0; j < (size); j++) {
    if (args[j+i][0] == '|')
      break;
    else if (args[j][0] == '&')
      arguments->background = 1;
    else if (args[j][0] == '<' && arguments->in_redir == 0) {
      arguments->in_redir = 1;
      j++;
      len = strlen(args[j]) + 1;
      arguments->in_file = malloc(sizeof(char) * len);
      allocCheck(arguments->in_file);
      strncpy(arguments->in_file, args[j], len);
    } else if (args[j][0] == '>' && arguments->out_redir == 0) {
      arguments->out_redir = 1;
      j++;
      len = strlen(args[j]) + 1;
      arguments->out_file = malloc(sizeof(char) * len);
      allocCheck(arguments->out_file);
      strncpy(arguments->out_file, args[j], len);
    }
  }
  arguments->pipe_args = malloc(sizeof(char *) * (size + 1));
  arguments->background = 0; 
  int s = 0;
  for (; j < (size); j++) {
    len = strlen(args[j+i]) + 1;
    arguments->pipe_args[s] = malloc(sizeof(char) * len);
    allocCheck(arguments->pipe_args[s]);
    strncpy(arguments->pipe_args[s], args[j+i], len);
    s++;
  }
  arguments->pipe_args[s] = NULL;
  arguments->pipe_args = realloc(arguments->pipe_args, sizeof(char *) * (j+1));
  allocCheck(arguments->pipe_args);
  // you're done!
  i = 0;
  /*while(NULL != arguments->shell_args[i]) {
    printf("arg[%d]: %s\n", i, arguments->shell_args[i]);
    i++;
  }*/
  //string_array_free(args);
  free(args);
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
      //allocCheck(input);
      i = 0;
      //free(input);
      printf("%s%s",usrname,josh_prompt);
    } else {
      input[i] = c;
      i++;
    }
  }
  if (EOF == c)
    free(input);
}
