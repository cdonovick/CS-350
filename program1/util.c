#include "standard.h"
#include "util.h"

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
