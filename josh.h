#ifndef JOSH_H
#define JOSH_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_INPUT 1024
#define DELIM " "


void allocCheck(void *pointer);
char ** parse(char *input);
int main();

#endif
