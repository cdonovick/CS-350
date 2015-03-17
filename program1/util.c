#include "standard.h"
#include "util.h"
/*
 * ./program1 [-a <num_assignings>]
 * [-w <min_prof_wait>] [-W <max_prof_wait>]
 * [-n <min_num_assignments>] [-N <max_num_assignments>]
 * [-h <min_assignment_hours>] [-H <max_assignment_hours>]
 * [-p <num_professors>] [-s <num_students>]
 * [-d <students_per_assignment>] [-q <queue_size>]
 */

/*
 * <num_assignings>             min: 1      max: 100000     default: 10
 * <min_prof_wait>              min: 1      max: 10         default: 1
 * <max_prof_wait>              min: 1      max: 100        default: 5
 * <min_num_assignments>        min: 1      max: 10         default: 1
 * <max_num_assignments>        min: 1      max: 100        default: 10
 * <min_assignment_hours>       min: 1      max: 5          default: 1
 * <max_assignment_hours>       min: 1      max: 10         default: 5
 * <num_professors>             min: 1      max: 10         default: 2
 * <num_students>               min: 1      max: 10         default: 2
 * <students_per_assignment>    min: 1      max: 10         default: <num_students>
 * <queue_size>                 min: 1      max: 256        default: 8 
 */

opt_struct opt_builder(int argc, char** argv) {
    /* macro magic to build OPT_STRING */
    static const char *OPT_STRING = ":"
    #define X(type, arg, name, min, max, def) #arg":"
        OPT_FIELDS_RAW;
    #undef X 

    opt_struct opts;
    /* macro magic assign each opt its default val */
    #define X(type, arg, name, min, max, def) opts.name = def;
        OPT_FIELDS;
    #undef X
    int opt = getopt(argc, argv, OPT_STRING); 
    while (opt != -1) {
        int val;
        switch (opt) {
        /* macro magic case for each field */
        #define X(type, arg, name, min, max, def) \
            case arg: \
                val = strtol(optarg, NULL, 10); \
                if (val < min || val > max) { \
                    fprintf(stderr, "Error: %s is not in [%d, %d]\n", optarg, min, max); \
                    usage(); \
                    exit(EXIT_FAILURE); \
                } \
                opts.name = val; \
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
        if (opts.min >= opts.max) { \
            fprintf(stderr, "Error: " #min " >= " #max "\n"); \
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
    #define X(type, arg, name, min, max, def) " [-"#arg" <"#name">]"
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
    #define X(type, arg, name, min, max, def) \
        fprintf(stdout, "%s = %lu\n", #name, opts->name);
        OPT_FIELDS;
    #undef X
}
