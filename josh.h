#ifndef JOSH_H
#define JOSH_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_INPUT 1024
#define DELIM " "



char ** parse(char *input);
int main();

#endif
