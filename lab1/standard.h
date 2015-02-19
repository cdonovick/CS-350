#ifndef STANDARD_H
#define STANDARD_H
#define NDEBUG
#include <stdio.h> 
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <getopt.h>
#include <assert.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include "mytimer.h"
/* 
 *  -u              usage
 *  -g           generate
 *  -n <num-integers>
 *      default       100
 *      max     1,000,000
 *  -m <min-int>
 *      default         1
 *  -M <max-int>
 *      default        26
 *  -s <seed>
 *  -i <input-file-name>
 *      deafult     stdin
 *  -o <output-file-name>
 *      default    stdout
 *  -c <count-file-name>
 *      default    stdout
 */

static const char *OPT_STRING = ":ugn:m:M:s:i:o:c:";

static const char *USAGE_STRING = "Usage:\n"
                                  "lab1 [-u] [-g] [-n <num-integers>] [-m <min-int>] [-M <max-int>] [-s <seed>]\n"
                                  "     [-i <input-file-name>] [-o <output-file-name> ][-c <count-file-name>]\n";

#ifndef DEFAULT_I
#define DEFAULT_I stdin
#endif

#ifndef DEFAULT_O
#define DEFAULT_O stdout
#endif

#ifndef DEFAULT_C
#define DEFAULT_C stdout
#endif

typedef struct OPT_STRUCT {
    bool            g;
    unsigned int    n;
    int             m;
    int             M;
    unsigned int    s;
    char*           i;
    char*           o;
    char*           c;
} OPT_STRUCT;

static const unsigned int MAX_n = 1000000;

static OPT_STRUCT OPT_DEFAULT(void);

static void usage(void);

static void missingArg(int arg);

static void unknownArg(int arg);

static void handleOps(OPT_STRUCT *ops, FILE **i, FILE **o, FILE **c, char *openFiles);

void generateMode(unsigned int n, int m, int M, FILE **outFile);

void sortMode(unsigned int n, int m, int M, FILE **iFile, FILE **oFile, FILE **cFile, unsigned int *charMap, unsigned int *charCount);

int compare (const void *a, const void *b);

#ifndef NDEBUG
static void printOps(OPT_STRUCT* ops);
#endif

#endif
