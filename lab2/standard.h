#ifndef STANDARD_H
#define STANDARD_H
#define NDEBUG
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h> 
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>
#include <getopt.h>
#include <ctype.h>
#include <errno.h>
#include <string.h>

/* 
 *  -u  usage
 *  -N <num-levels>
 *  -M <num-children>
 *  -p  pause leaves
 *  -s <sleep-time>
 */

static const char *OPT_STRING = ":uN:M:ps:";

static const char *USAGE_STRING = "Usage:\n"
                                  "lab1 [-u] [-N <num-levels>] [-M <num-children>] [-M <max-int>] [-p] [-s <sleep-time>]\n";

static const int DEFAULT_NUM_CHILDREN = 1;
static const int     MAX_NUM_CHILDREN = 4;

static const int DEFAULT_NUM_LEVELS = 1;
static const int     MAX_NUM_LEVELS = 3;

static const size_t BUFF_SIZE = 1024;

static void usage(void);

static void missingArg(int arg);

static void unknownArg(int arg);

static int parseStr(char *str, int *val);

#endif
