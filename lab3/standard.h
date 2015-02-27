#ifndef STANDARD_H
#define STANDARD_H

#define CAST_C(ptr) ((args_c *) (ptr))
#define CAST_L(ptr) ((args_l *) (ptr))
#define CAST_VOID(ptr) ((void *) (ptr))

#include <pthread.h>
#include <stdio.h> 
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <string.h>


typedef struct args_c {
    int level;
    int threadNumber;
} args_c;

typedef struct args_l {
    int threadNumber;
} args_l;

static const char *USAGE_STRING = "Usage:\n./lab3 num-levels\n";
static const int MAX_TREE_LEVELS = 5;
static const int MIN_TREE_LEVELS = 0;

static void usage(void);
static int parseStr(char *str, int *val);

void* create_thread(void *);
void* leaf_func(void *);


#endif
