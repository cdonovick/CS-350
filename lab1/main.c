#include "standard.h"

int main(int argc, char **argv) {
    int opt;
    OPT_STRUCT ops = OPT_DEFAULT();
    FILE *iFile = DEFAULT_I;
    FILE *oFile = DEFAULT_O;
    FILE *cFile = DEFAULT_C;
    timer *t;
    struct timeval elapsed;
    char openFiles;
    int val;
    unsigned int uval;

    opt = getopt(argc, argv, OPT_STRING); 
    while (opt != -1) {
        switch (opt) {
            case 'u':
                usage();
                return EXIT_SUCCESS;
                break;
            case 'g':
                ops.g = true;
                break;
            case 'n':
                uval = strtoul(optarg, NULL, 10);
                if (val || isdigit(optarg[0])) {
                    ops.n = uval;
                } else {
                    fprintf(stderr, "Error: %s cannot be coverted to an integer\n", optarg);
                    return EXIT_FAILURE;
                }
                break;
            case 'm':
                val = strtoul(optarg, NULL, 10);
                if (val || isdigit(optarg[0])) {
                    ops.m = val;
                } else {
                    fprintf(stderr, "Error: %s cannot be coverted to an integer\n", optarg);
                    return EXIT_FAILURE;
                }
                break;
            case 'M':
                val = strtoul(optarg, NULL, 10);
                if (val || isdigit(optarg[0])) {
                    ops.M = val;
                } else {
                    fprintf(stderr, "Error: %s cannot be coverted to an integer\n", optarg);
                    return EXIT_FAILURE;
                }
                break;
            case 's':
                uval = strtoul(optarg, NULL, 10);
                if (val || isdigit(optarg[0])) {
                    ops.s = uval;
                } else {
                    fprintf(stderr, "Error: %s cannot be coverted to an integer\n", optarg);
                    return EXIT_FAILURE;
                }
                break;
            case 'i':
                ops.i = optarg;
                break;
            case 'o':
                ops.o = optarg;
                break;
            case 'c':
                ops.c = optarg;
                break;
            case ':':
                missingArg(optopt);
                return EXIT_FAILURE;
                break;
            case '?':
                unknownArg(optopt);
                return EXIT_FAILURE;
                break;
            default:
                fprintf(stderr, "Something happened\n");
                return EXIT_FAILURE;
                break;
        }

        opt = getopt(argc, argv, OPT_STRING); 
    }

    handleOps(&ops, &iFile, &oFile, &cFile, &openFiles);
    if (ops.g) {
        generateMode(ops.n, ops.m, ops.M, &oFile);
    } else {
    	t = new_timer();
        if (!t) {
            fprintf(stderr, "Error: problem with timer, %s\n", timer_error_str[timer_errno]);
            return EXIT_FAILURE; 
        }

    	if (timer_start(t)) {
            fprintf(stderr, "Error: problem with timer, %s\n", timer_error_str[timer_errno]);
            return EXIT_FAILURE; 
        }
        char *user = getenv("USER");
        if (user == NULL) {
            fprintf(stderr, "Error: could not determine userid\n");
            if (openFiles&1) { fclose(iFile); }
            if (openFiles&2) { fclose(oFile); }
            if (openFiles&4) { fclose(cFile); }
        }

        size_t uniqueChars = 0;
        
        unsigned int *charMap = (unsigned int *) calloc(26, sizeof(unsigned int));
        if (charMap == NULL) { 
            fprintf(stderr, "Error: problem with malloc, %s\n", strerror(errno));
            if (openFiles&1) { fclose(iFile); }
            if (openFiles&2) { fclose(oFile); }
            if (openFiles&4) { fclose(cFile); }
            exit(EXIT_FAILURE);
        }

        for (char *i = user; *i != '\0'; ++i) {
            if (islower(*i) && charMap[*i - 'a'] == 0) {
                /* the current char will be counted at position uniqueCharsN - 1
                 * -1 so that 0 indicates not tracked
                 */
                charMap[*i - 'a'] = ++uniqueChars;
            }
        }

        unsigned int *charCount = (unsigned int *) calloc(uniqueChars, sizeof(unsigned int));
        if (charCount == NULL) { 
            fprintf(stderr, "Error: problem with malloc, %s\n", strerror(errno));
            free(charMap);
            if (openFiles&1) { fclose(iFile); }
            if (openFiles&2) { fclose(oFile); }
            if (openFiles&4) { fclose(cFile); }
            exit(EXIT_FAILURE);
        }

        sortMode(ops.n, ops.m, ops.M, &iFile, &oFile, &cFile, charMap, charCount);
        free(charMap);
        free(charCount);
        if (timer_getElasped(t, &elapsed)) {
            fprintf(stderr, "Error: problem with timer, %s\n", timer_error_str[timer_errno]);
            free_timer(t);
            return EXIT_FAILURE; 
        } else {
            fprintf(stderr, "Elapsed time: %f s\n", (double)elapsed.tv_sec + ((double)elapsed.tv_usec)/1000000);
            free_timer(t);
        }
    }

    if (openFiles&1) { fclose(iFile); }
    if (openFiles&2) { fclose(oFile); }
    if (openFiles&4) { fclose(cFile); }

    return EXIT_SUCCESS;
}

static OPT_STRUCT OPT_DEFAULT(void) {
    OPT_STRUCT ops;

    ops.g = false;
    ops.n = 100;
    ops.m = 1;
    ops.M = 26;
    ops.s = (unsigned int) time(NULL);
    ops.i = NULL;
    ops.o = NULL;
    ops.c = NULL;

    return ops;
}

static void usage(void) {
    fprintf(stderr, "%s", USAGE_STRING);
}


static void missingArg(int arg) {
    fprintf(stderr, "Error: missing argument, -%c expects a value\n", arg);
    usage();
}

static void unknownArg(int arg) {
    fprintf(stderr, "Error: unknown argument, -%c\n", arg);
    usage();
}

static void handleOps(OPT_STRUCT *ops, FILE **i, FILE **o, FILE **c, char *openFiles) {
    #ifndef NDEBUG
    printOps(ops);
    #endif

    *openFiles = 0;

    /* hande numerics */
    if (ops->n > MAX_n) {
        fprintf(stderr, "Error: num-integers > 1,000,000. Using default value of 100\n");
        ops->n = 100;
    }

    if (ops->m > ops->M) {
        fprintf(stderr, "Error: min-int > max-int\n");
        exit(EXIT_FAILURE);
    }
    
    if (ops->g) {
        srand(ops->s);
    }

    /* handle files */
    if (ops->i && !ops->g) {
        *i = fopen(ops->i, "r");
        if (!*i) {
            fprintf(stderr, "Error: problem with file %s, %s\n", ops->i, strerror(errno));
            exit(EXIT_FAILURE);
        }

        *openFiles |= 1;
    } else if (ops->i) {
            fprintf(stderr, "Warning: input file not supported in generate mode, flag will be ignored\n");
    }

    if (ops->o) {
        if (ops->i && !strcmp(ops->o, ops->i)) {
            fprintf(stderr, "Error: cannot use %s for both input and output\n", ops->i);
            if (*openFiles&1) { fclose(*i); }
            exit(EXIT_FAILURE);
        }
        *o = fopen(ops->o, "w");

        if (!*o) { 
            fprintf(stderr, "Error: problem with file %s, %s\n", ops->o, strerror(errno));
            if (*openFiles&1) { fclose(*i); }
            exit(EXIT_FAILURE);
        }

        *openFiles |= 2;
    }
    
    if (ops->c && !ops->g) {
        if (ops->i && !strcmp(ops->c, ops->i)) {
            fprintf(stderr, "Error: cannot use %s for both input and output\n", ops->i);
            if (*openFiles&1) { fclose(*i); }
            if (*openFiles&2) { fclose(*o); }
            exit(EXIT_FAILURE);
        }
    
        if (ops->o && !strcmp(ops->c, ops->o)) {
            *c = *o;
        } else {    
            *c = fopen(ops->c, "w");
        }

        if (!*c) { 
            fprintf(stderr, "Error: problem with file %s, %s\n", ops->c, strerror(errno));
            if (*openFiles&1) { fclose(*i); }
            if (*openFiles&2) { fclose(*o); }
            exit(EXIT_FAILURE);
        }

        *openFiles |= 4;
    } else if (ops->c) {
            fprintf(stderr, "Warning: count file not supported in generate mode, flag will be ignored\n");
    }
}


void generateMode(unsigned int n, int m, int M, FILE **outFile) {
    assert(m < M);

    fprintf(*outFile, "%u\n", n);
    for (unsigned int i = 0; i < n; ++i) {
        fprintf(*outFile, "%u\n", (rand()%(M - m + 1)) + m);
    }
}


void sortMode(unsigned int n, int m, int M, FILE **iFile, FILE **oFile, FILE **cFile, unsigned int *charMap, unsigned int *charCount) {
    assert(m < M);
    unsigned int i = 0;
    int *buf = (int *) malloc(n*sizeof(int));
    if (buf == NULL) {
        fprintf(stderr, "Error: problem with malloc, %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    while (i < n && fscanf(*iFile, "%i", buf + i) != EOF) {
        if (buf[i] >= m && buf[i] <= M) {
            if (buf[i] >= 1 && buf[i] <= 26 && charMap[buf[i]-1]) {
                charCount[charMap[buf[i]-1]-1]++;
            }
            i++;
        }
    }

    qsort(buf, i, sizeof(int), compare);

    for (unsigned int j = 0; j < i; ++j) {
        fprintf(*oFile, "%i\n", buf[j]);  
    }

    for (unsigned int j = 0; j < 26; ++j) {
        if (charMap[j]) {
            fprintf(*cFile, "%i(%c): %u\n", j+1, j+'a', charCount[charMap[j]-1]);
        }
    }

    free(buf);
}

int compare (const void *a, const void *b) {
    return (*(int *)a - *(int *)b);
}

#ifndef NDEBUG
static void printOps(OPT_STRUCT *ops) {
    fprintf(stderr, "g: %x\n", ops->g);
    fprintf(stderr, "n: %u\n", ops->n);
    fprintf(stderr, "m: %i\n", ops->m);
    fprintf(stderr, "M: %i\n", ops->M);
    fprintf(stderr, "s: %u\n", ops->s);
    fprintf(stderr, "i: %s\n", ops->i);
    fprintf(stderr, "o: %s\n", ops->o);
    fprintf(stderr, "c: %s\n", ops->c);
}
#endif
