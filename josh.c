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
      if (child) {
        kill(child, SIGINT);
        child = 0;
      }
      break;
    default:
      fprintf(stderr, "Signal %d caught!\n", sig);
  }
  signal(sig, sigHandler);
}

// strRealloc
// Given a string that has more space than it is using, reallocates
// so there is just as much space as the string needs. Reduces allocated memory space.
void strRealloc(char *string) {
  int len;
  len = strlen(string);
  string = realloc(string, len+1);
}

// freeArgs
// frees every argument in args structure
void freeArgs(struct args *arguments) {
  free(arguments->program);
  string_array_free(arguments->program_args);
  string_array_free(arguments->pipe_args);
  free(arguments->out_file);
  free(arguments->in_file);
  free(arguments);
}

// string_array_free
// dynamically frees in an array of strings
void string_array_free(char ** strings) {
  int i = 0;
  while (NULL != strings[i])
    free(strings[i++]);
  free(strings);
}

// pipearg
// creates an three dimensional character array for piper to use
char ***pipearg(struct args *a){
  int i, j, k, g;
  i = j = 0;
  char ***b;
  char *s = a->pipe_args[0];
  while( s != NULL ){				// count number of pipes
    if(strcmp(s,"|") == 0)
      j++;
    i++;
    s = a->pipe_args[i];
  }
  b = malloc( sizeof(char **) * (j + 1));	// Malloc amount of commands
  allocCheck(b);
  
  b[0] = a->program_args;			// Assign first command
  
  s = a->pipe_args[1];
  i = j = k = 1;
  while( s != NULL ){				// Assign other commands
    if(strcmp(s,"|") == 0){
      b[k] = malloc( sizeof(char *) * i);
      allocCheck(b[k]);
      g = 0;
      for(;j<i;j++){
        int len = strlen(a->pipe_args[j]) + 1;
        b[k][g] = malloc(sizeof(char) * len);
        allocCheck(b[k][g]);
        strncpy(b[k][g], a->pipe_args[j], len);
        g++;
      }
      j++;
      b[k][g] = NULL;
      k++;
    }
    i++;
    s = a->pipe_args[i];
  }
  b[k] = malloc( sizeof(char *) * i);		// assign last command
  allocCheck(b[k]);
  g = 0;
  for(;j<i;j++){
    int len = strlen(a->pipe_args[j]) + 1;
    b[k][g] = malloc(sizeof(char) * len);
    allocCheck(b[k][g]);
    strncpy(b[k][g], a->pipe_args[j], len);
    g++;
  }
  b[k][g] = NULL;
  
  return b;
} 

// piper
// executes a multipipe sequence
// Takes an array of argument strings, and the number of pipes in the command.
// The argument string would be split up so "foo a | blah bla bla | bar"
//  would be [ ("foo","a",NULL) , ("blah","bla","bla",NULL) , ("bar",NULL) ]
void piper(char ***args, int numpipes){
  int fd[2*numpipes];
  pid_t cid;
  
  for(int i=0;i<numpipes;i++){
    pipe(fd + 2*i);
  }
  
  int argstep = 0;
  int fdstep = 0;
  while(argstep < numpipes){
    cid = fork();
    if(cid == -1){				// fork failure
      perror("Fork failed");
      exit(EXIT_FAILURE);
    }
    else if(cid == 0){				// Child process
      if(argstep == 0)				// First pipe process
	dup2(fd[fdstep+1],STDOUT_FILENO);	//   just output
      else{					// Any middle pipe
	dup2(fd[fdstep-2],STDIN_FILENO);	//   input redirect
        dup2(fd[fdstep+1],STDOUT_FILENO);	//   output redirect
      }
      for(int i=0;i<2*numpipes;i++){		// Close pipe links
        close(fd[i]);
      }
      execvp(args[argstep][0],args[argstep]);	// Exec child
    }
    else{					// Parent process
     fdstep = fdstep+2;
     argstep++;
    }
  }
  /* At this point, every pipe should be set exept last, only parent process is running*/
  dup2(fd[fdstep-2],STDIN_FILENO);
  for(int i=0;i<2*numpipes;i++){
    close(fd[i]);
  }
  execvp(args[argstep][0],args[argstep]);
  perror("Exec failed");
  exit(EXIT_FAILURE);
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
    if  (arguments->background)
      waitpid(-1, &status, WNOHANG);
    else {
      //printf("fg\n");
      //signal(SIGINT, sigHandler);
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
    if (NULL != arguments->pipe_args[0] && arguments->pipe_args[0][0] == '|') {  
      char ***b = pipearg(arguments);    
      pipebool = 1;
      char *s = arguments->pipe_args[0];
      int i, numpipe;
      i = numpipe = 0;
      while( s != NULL ){                           // count number of pipes
        if(strcmp(s,"|") == 0)
          numpipe++;
        i++;
        s = arguments->pipe_args[i];
      }
      piper(b,numpipe);
      
      /*pid_t forkChild;
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
      else{                // After pipe
        close(fd[1]);                                                   // Close unnecessary parent input link to tunnel
        dup2(fd[0], STDIN_FILENO);                                     // Change parent's output to tunnel input
  	close(fd[0]);
        char *cprog = arguments->pipe_args[1];                       // Name child's program
        char **cpargs;                                                // Initialize child's program arguments
        int i = 0;
        while(NULL != arguments->pipe_args[i+1])                     // Get number of child's program arguments (terminating NULL included)
          i++;
        cpargs = malloc(sizeof(char *) * (i + 1));
        for (int j = 0; j < i; j++) {                                 // Fill child's program args
          int len = strlen(arguments->pipe_args[j+1]) + 1;
          cpargs[j] = malloc(sizeof(char) * len);
          allocCheck(cpargs[j]);
          strncpy(cpargs[j], arguments->pipe_args[j+1], len);
        }
        cpargs[i] = NULL;
        execvp(cprog,cpargs);                                         // Executes child's program
        string_array_free(cpargs);
      }*/
    }
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
  /* stick the remaining arguments in the pipe_args */
  //arguments->pipe_args = malloc(sizeof(char *) * (size + 1));
  for (j = 0; j < (size); j++) {
    if (args[j+i][0] == '|')
      break;
    else if (args[j+i][0] == '&') 
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
  //arguments->background = 0; 
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
  /*while(NULL != arguments->pipe_args[i]) {
    printf("arg[%d]: %s\n", i, arguments->pipe_args[i]);
    i++;
  }*/
  //string_array_free(args);
  free(args);
  return arguments;
}


int main() {
  child = 0;
  int c, i;
  struct args *arguments;
  char *input;
  char *usrname = getenv("LOGNAME");
  i = 0;
  input = malloc(sizeof(char) * MAX_INPUT);
  allocCheck(input);
  printf("%s%s",usrname,josh_prompt);
  signal(SIGINT, sigHandler);
  //c = 0;
  while (1) {
    errno = 0;
    if (EOF == (c = fgetc(stdin))) {
      if (ferror(stdin) && errno == EINTR) {
        continue;
      }
      if (ferror(stdin)) {
        perror("reading input");
        free(input);
        exit(EXIT_FAILURE);
      }
      if (feof(stdin)) {
        fprintf(stderr, "No More input\n");
        free(input);
        exit(EXIT_SUCCESS);
       }
    }
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
