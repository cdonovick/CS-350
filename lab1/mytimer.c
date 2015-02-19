#include "mytimer.h"

timer_error_t timer_errno;
static const timer_error_t NO_ERROR = 0;
static const timer_error_t PERM = 1;
static const timer_error_t GET_NO_START = 2;
static const timer_error_t STOP_NO_START = 3;
static const timer_error_t DOUBLE_STOP = 4;
static const timer_error_t ALLOC = 5;
static const timer_error_t ALLOC_START = 6;
static const timer_error_t ALLOC_STOP = 7;
static const timer_error_t NPE = 8;

const char *timer_error_str[] = {
    NULL,
    "process has insuffeciant privileges",
    "timer_get called before timer_start",
    "timer_stop called before timer_start",
    "timer_stop called while timer is stopped",
    "could not allocate memory for timer",
    "could not allocate memory for start",
    "could not allocate memory for stop",
    "null pointer execption"
};

static const timer_status_t INIT = -1;
static const timer_status_t RUNNING = 0;
static const timer_status_t STOPPED = 1;

timer * new_timer(void) {
    timer* t = (timer *) malloc(sizeof(timer));
    if (!t) {
        timer_errno = ALLOC;
    } else {
        t->start =  (struct timeval *) malloc(sizeof(struct timeval));
        if (!t->start) {
            timer_errno = ALLOC_START;
            free(t);
            return NULL;
        }

        t->end =  (struct timeval *) malloc(sizeof(struct timeval));
        if (!t->end) {
            timer_errno = ALLOC_STOP;
            free(t->start);
            free(t);
            return NULL;
        }
        
        t->status = INIT;
        timer_errno = NO_ERROR;
    }

    return t;
}

void free_timer(timer *t) {
    free(t->start);
    free(t->end);
    free(t);
}

int timer_start(timer *t) {
    if (gettimeofday(t->start, NULL)) {
        assert(errno == EPERM);
        timer_errno = PERM;
        return -1;
    } else {
        t->status = RUNNING; 
        timer_errno = NO_ERROR;
        return 0;
    }
}

int timer_stop(timer *t) {
    if (t->status == INIT) {
        timer_errno = STOP_NO_START;
        return -1;
    } else if (t->status == STOPPED) {
        timer_errno = DOUBLE_STOP;
        return -1;
    } else if (gettimeofday(t->end, NULL)) {
        assert(errno == EPERM);
        timer_errno = PERM;
        return -1;
    } else {
        t->status = STOPPED; 
        timer_errno = NO_ERROR;
        return 0;
    }
}

int timer_getElasped(timer *t, struct timeval *elasped) {
    if (elasped == NULL) {
        timer_errno = NPE;
        return -1;
    } else if (t->status == INIT) {
        timer_errno = GET_NO_START;
        return -1;
    } else if (t->status == STOPPED) {
        elasped->tv_sec = t->end->tv_sec - t->start->tv_sec;
        elasped->tv_usec = t->end->tv_usec - t->start->tv_usec;
        return 0;
    } else {
        struct timeval now;
        if (gettimeofday(&now, NULL)) {
            assert(errno == EPERM);
            timer_errno = PERM;
            return -1;
        }
        elasped->tv_sec = now.tv_sec - t->start->tv_sec;
        elasped->tv_usec = now.tv_usec - t->start->tv_usec;
        return 0;
    }
}
