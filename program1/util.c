#include "standard.h"
#include "util.h"

void synch_fprintf(FILE *stream, const char *format, ...) {
    static pthread_mutex_t mx = PTHREAD_MUTEX_INITIALIZER;
    va_list args;
    va_start(args, format); 
    pthread_mutex_lock(&mx);
    vfprintf(stream, format, args);
    pthread_mutex_unlock(&mx);
    va_end(args);
}

#ifndef EC_FUNC
#define EC_FUNC(lib, type, method,  ...) \
    int error = lib##_##type##_##method(__VA_ARGS__); \
    if (error) { \
        fprintf(stderr, "Error: " #lib "_" #type "_" #method " failed\nError %i\n", error); \
        exit(EXIT_FAILURE); \
    }
#endif

void mutex_init(pthread_mutex_t *mx, const pthread_mutexattr_t *a) {
    EC_FUNC(pthread, mutex, init, mx, a);
}

void mutex_destroy(pthread_mutex_t *mx) {
    EC_FUNC(pthread, mutex, destroy, mx);
}

void mutex_lock(pthread_mutex_t *mx) {
    EC_FUNC(pthread, mutex, lock, mx);
}

void mutex_unlock(pthread_mutex_t *mx) {
    EC_FUNC(pthread, mutex, unlock, mx);
}


void cond_init(pthread_cond_t *c, const pthread_condattr_t *a) {
    EC_FUNC(pthread, cond, init, c, a);
}


void cond_destroy(pthread_cond_t *c) {
    EC_FUNC(pthread, cond, destroy, c);
}


void cond_wait(pthread_cond_t *c, pthread_mutex_t *m) {
    EC_FUNC(pthread, cond, wait, c, m);
}

void cond_signal(pthread_cond_t *c) {
    EC_FUNC(pthread, cond, signal, c);
}

void cond_broadcast(pthread_cond_t *c) {
    EC_FUNC(pthread, cond, broadcast, c);
}

opt_struct opt_builder(int argc, char** argv) {
    /* macro magic to build OPT_STRING */
    static const char *OPT_STRING = ":"
    #define X(type, arg, var, min, max, def, fmt) #arg":"
        OPT_FIELDS_RAW;
    #undef X 

    opt_struct opts;
    /* macro magic assign each opt its default val */
    #define X(type, arg, var, min, max, def, fmt) opts.var = def;
        OPT_FIELDS;
    #undef X
    int opt = getopt(argc, argv, OPT_STRING); 
    while (opt != -1) {
        int32_t val;
        switch (opt) {
        /* macro magic case for each field */
        #define X(type, arg, var, min, max, def, fmt) \
            case arg: \
                val = strtol(optarg, NULL, 10); \
                if (val < min || val > max) { \
                    fprintf(stderr, "Error: %s is not in [%" fmt ", %" fmt "]\n", optarg, min, max); \
                    usage(); \
                    exit(EXIT_FAILURE); \
                } \
                opts.var = (type) val; \
                break;
            OPT_FIELDS;
        #undef X
            case ':':
                missingArg(optopt);
                exit(EXIT_FAILURE);
                break;
            case '?':
                unknownArg(optopt);
                exit(EXIT_FAILURE);
                break;
            default:
                fprintf(stderr, "Something happened\n");
                exit(EXIT_FAILURE);
                break;
        }
        opt = getopt(argc, argv, OPT_STRING); 
    }

    /* handle default case */
    if (opts.students_per_assignment == 0) {
        opts.students_per_assignment = opts.num_students;
    }

    /* macro magic enforce ranges */
    #define X(min, max) \
        if (opts.min > opts.max) { \
            fprintf(stderr, "Error: " #min " > " #max "\n"); \
            exit(EXIT_FAILURE); \
        }
        OPT_RANGES;
    #undef X

    return opts;
}

void usage(void) {
    /* macro magic to build USAGE_STRING */
    static const char *USAGE_STRING =   "Usage:\n"
                                        "./program1 "
    #define X(type, arg, var, min, max, def, fmt) " [-"#arg" <"#var">]"
        OPT_FIELDS_RAW;
    #undef X 
    fprintf(stdout, "%s\n", USAGE_STRING);
}

void missingArg(int arg) {
    fprintf(stderr, "Error: missing argument, -%c expects a value\n", arg);
    usage();
}

void unknownArg(int arg) {
    fprintf(stderr, "Error: unknown argument, -%c\n", arg);
    usage();
}

void printOpts(opt_struct *opts) {
    /* macro magic to print opts */
    #define X(type, arg, var, min, max, def, fmt) \
        fprintf(stdout, "%s = %" fmt "\n", #var, opts->var);
        OPT_FIELDS;
    #undef X
}
