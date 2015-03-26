#ifndef MYTIMER_H
#define MYTIMER_H
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <assert.h>
#include <errno.h>

typedef unsigned int timer_error_t;
typedef signed char timer_status_t;

typedef struct timer {
    timer_status_t status;
    struct timeval *start;
    struct timeval *end;
} timer;


extern timer_error_t timer_errno;
extern const char *timer_error_str[];
/*
 * Returns a timer pointer or null 
 * if not null should be freed with freeTimer
 */
extern timer * new_timer(void);
extern void free_timer(timer *t);
extern int timer_start(timer *t);
extern int timer_stop(timer *t);
extern int timer_getElasped(timer *t, struct timeval *elasped);

#endif

