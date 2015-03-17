#ifndef STANDARD_H
#define STANDARD_H
#define NDEBUG
#include <stdio.h> 
#include <stdbool.h>
#include <stdlib.h>
#include <getopt.h>
#include <assert.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>

#ifndef OPT_FIELDS
#define OPT_FIELDS \
    X(size_t, 'a', num_assignings, 1, 100000, 10) \
    X(size_t, 'w', min_prof_wait, 1, 10, 1) \
    X(size_t, 'W', max_prof_wait, 1, 100, 5) \
    X(size_t, 'n', min_num_assignments, 1, 10, 1) \
    X(size_t, 'N', max_num_assignments, 1, 100, 10) \
    X(size_t, 'h', min_assignment_hours, 1, 5, 1) \
    X(size_t, 'H', max_assignment_hours, 1, 10, 5) \
    X(size_t, 'p', num_professors, 1, 10, 2) \
    X(size_t, 's', num_students, 1, 10, 2) \
    X(size_t, 'd', students_per_assignment, 1, 10, 0) \
    X(size_t, 'q', queue_size, 1, 256, 8)
#endif

/* macro magic */
typedef struct opt_struct {
#define X(type, arg, name, min, max, def) type name;
    OPT_FIELDS
#undef X
} opt_struct;

/* functions defined in util.c */
opt_struct opt_builder(int argc, char** argv);
void usage(void);
void missingArg(int arg);
void unknownArg(int arg);
void printOpts(opt_struct *opts);

typedef struct assignment assignment;
typedef struct professor professor;
typedef struct student student;

struct assignment {
    size_t n;
    size_t prof;
    size_t students_needed;
    size_t students_completed;
    size_t hours;
}

struct professor {
    size_t n;
}

struct student {
    size_t n;
    size_t worked;
    assignment *current;
}

typedef struct assignment_queue {
    assignment **assignments;
    //locks
    //semaphores
    //etc..
} assignment_queue;

#endif

