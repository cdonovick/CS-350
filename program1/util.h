#ifndef UTIL_H
#define UTIL_H
#include "standard.h"
#include <getopt.h>
#include <stdarg.h>

/* wrapper functions */
void synch_fprintf(FILE *stream, const char *format, ...); 

void mutex_init(pthread_mutex_t *mx, const pthread_mutexattr_t *a);
void mutex_destroy(pthread_mutex_t *mx);
void mutex_lock(pthread_mutex_t *mx);
void mutex_unlock(pthread_mutex_t *mx);

void cond_init(pthread_cond_t *c, const pthread_condattr_t *a);
void cond_destroy(pthread_cond_t *c);
void cond_wait(pthread_cond_t *c, pthread_mutex_t *m);
void cond_signal(pthread_cond_t *c);
void cond_broadcast(pthread_cond_t *c);

#ifndef RANDRANGE
#define RANDRANGE(min, max) \
    ((rand() % ((max) - (min) + 1)) + (min))
#endif
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

#ifndef OPT_FIELDS
#define OPT_FIELDS \
    X(uint32_t, 'a', num_assignings, 1, 100000, 10, PRIu32) \
    X(uint8_t, 'w', min_prof_wait, 1, 10, 1, PRIu8) \
    X(uint8_t, 'W', max_prof_wait, 1, 100, 5, PRIu8) \
    X(uint8_t, 'n', min_num_assignments, 1, 10, 1, PRIu8) \
    X(uint8_t, 'N', max_num_assignments, 1, 100, 10, PRIu8) \
    X(uint8_t, 'h', min_assignment_hours, 1, 5, 1, PRIu8) \
    X(uint8_t, 'H', max_assignment_hours, 1, 10, 5, PRIu8) \
    X(uint8_t, 'p', num_professors, 1, 10, 2, PRIu8) \
    X(uint8_t, 's', num_students, 1, 10, 2, PRIu8) \
    X(uint8_t, 'd', students_per_assignment, 1, 10, 0, PRIu8) \
    X(uint16_t, 'q', queue_size, 1, 256, 8, PRIu16)
#endif

#ifndef OPT_FIELDS_RAW
#define OPT_FIELDS_RAW \
    X(uint32_t, a, num_assignings, 1, 100000, 10, PRIu32) \
    X(uint8_t, w, min_prof_wait, 1, 10, 1, PRIu8) \
    X(uint8_t, W, max_prof_wait, 1, 100, 5, PRIu8) \
    X(uint8_t, n, min_num_assignments, 1, 10, 1, PRIu8) \
    X(uint8_t, N, max_num_assignments, 1, 100, 10, PRIu8) \
    X(uint8_t, h, min_assignment_hours, 1, 5, 1, PRIu8) \
    X(uint8_t, H, max_assignment_hours, 1, 10, 5, PRIu8) \
    X(uint8_t, p, num_professors, 1, 10, 2, PRIu8) \
    X(uint8_t, s, num_students, 1, 10, 2, PRIu8) \
    X(uint8_t, d, students_per_assignment, 1, 10, 0, PRIu8) \
    X(uint16_t, q, queue_size, 1, 256, 8, PRIu16)
#endif

#ifndef OPT_RANGES
#define OPT_RANGES \
    X(min_prof_wait, max_prof_wait) \
    X(min_num_assignments, max_num_assignments) \
    X(min_assignment_hours, max_assignment_hours) \
    X(students_per_assignment, num_students)
#endif

/* macro magic */
typedef struct opt_struct {
#define X(type, arg, var, min, max, def, fmt) \
    type var;
    OPT_FIELDS
#undef X
} opt_struct;

/* opts functions */
opt_struct opt_builder(int argc, char** argv);
void usage(void);
void missingArg(int arg);
void unknownArg(int arg);
void printOpts(opt_struct *opts);
#endif

