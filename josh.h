#ifndef JOSH_H
#define JOSH_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_INPUT 1024
#define DELIM " "

char *josh_prompt = ":J0Sh:$ ";
pid_t child;

struct args {
  char *program;
  char **program_args;
  char **pipe_args;
  int out_redir;
  int in_redir;
  char * out_file;
  char * in_file;
  int background;
};

void freeArgs(struct args *arguments);
void string_array_free(char **strings);
char ** pargs(char **args);
void sigHandler(int sig);
void strRealloc(char *string);
void p2(struct args *arguments);
void allocCheck(void *pointer);
struct args * parse(char *input);
int main();



#endif
