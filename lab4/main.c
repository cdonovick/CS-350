/******************************************************************************
 * FILE: condvar.c
 * DESCRIPTION:
 *   Example code for using Pthreads condition variables.  The main thread
 *   creates three threads.  Two of those threads increment a "count" variable,
 *   while the third thread watches the value of "count".  When "count" 
 *   reaches a predefined limit, the waiting thread is signaled by one of the
 *   incrementing threads. The waiting thread "awakens" and then modifies
 *   count. The program continues until the incrementing threads reach
 *   TCOUNT. The main program prints the final value of count.
 * SOURCE: Adapted from example code in "Pthreads Programming", B. Nichols
 *   et al. O'Reilly and Associates. 
 * LAST REVISED: 3/26/15  Caleb Donovick
 ******************************************************************************/
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include "mytimer.h"

#define TCOUNT 10
#define COUNT_LIMIT 12

volatile int count = 0;
pthread_mutex_t count_mutex;
pthread_cond_t count_threshold_cv;

typedef struct args_t {
    long id;
    timer *t;
} args_t;

void *inc_count(void *args) 
{
    args_t *a = (args_t *)args;
    int i;
    long my_id = a->id;
    timer *t = a->t;

    for (i=0; i < TCOUNT; i++) {
        pthread_mutex_lock(&count_mutex);
        count++;

        /* 
           Check the value of count and signal waiting thread when condition is
           reached.  Note that this occurs while mutex is locked. 
           */
        if (count == COUNT_LIMIT) {
            timer_start(t);
            printf("inc_count(): thread %ld, count = %d  Threshold reached.\n", my_id, count);
            pthread_cond_signal(&count_threshold_cv);
            printf("inc_count(): thread %ld, just sent signal.\n", my_id);
        }
        printf("inc_count(): thread %ld, count = %d, unlocking mutex\n", 
                my_id, count);
        pthread_mutex_unlock(&count_mutex);

        /* Do some work so threads can alternate on mutex lock */
        sleep(1);
    }
    pthread_exit(NULL);
}

void *watch_count_cond(void *args) 
{
    args_t *a = (args_t *)args;
    struct timeval elapsed;
    long my_id = a->id;
    timer *t = a->t;
    
    printf("Starting watch_count_cond(): thread %ld\n", my_id);

    /*
       Lock mutex and wait for signal.  Note that the pthread_cond_wait routine
       will automatically and atomically unlock mutex while it waits. 
       Also, note that if COUNT_LIMIT is reached before this routine is run by
       the waiting thread, the loop will be skipped to prevent pthread_cond_wait
       from never returning.
       */
    pthread_mutex_lock(&count_mutex);
    while (count < COUNT_LIMIT) {
        printf("watch_count_cond(): thread %ld Count= %d. Going into wait...\n", my_id,count);
        pthread_cond_wait(&count_threshold_cv, &count_mutex);
        timer_getElasped(t, &elapsed);
        printf("watch_count_cond(): thread %ld Condition signal received. Count= %d,\n", my_id,count);
        printf("watch_count_cond(): elapsed time: %f s\n", (double)elapsed.tv_sec + ((double)elapsed.tv_usec)/1000000);
        printf("watch_count_cond(): thread %ld Updating the value of count...\n", my_id);
        count += 125;
        printf("watch_count_cond(): thread %ld count now = %d.\n", my_id, count);
    }
    printf("watch_count_cond(): thread %ld Unlocking mutex.\n", my_id);
    pthread_mutex_unlock(&count_mutex);
    pthread_exit(NULL);
}


void *watch_count_busy(void *args) 
{
    args_t *a = (args_t *)args;
    struct timeval elapsed;
    long my_id = a->id;
    timer *t = a->t;

    printf("Starting watch_count_busy(): thread %ld\n", my_id);

    while (count < COUNT_LIMIT);

    printf("watch_count_busy(): thread %ld Busy wait completed. Count= %d,\n", my_id,count);
    if (timer_getElasped(t, &elapsed)) {
        printf("watch_count_busy(): noticed before timer could be started!\n");
    } else {
        printf("watch_count_busy(): elapsed time: %f s\n", (double)elapsed.tv_sec + ((double)elapsed.tv_usec)/1000000);
    }

    pthread_exit(NULL);
}

void *watch_count_sleep(void *args) 
{
    args_t *a = (args_t *)args;
    struct timeval elapsed;
    long my_id = a->id;
    timer *t = a->t;

    printf("Starting watch_count_sleep(): thread %ld\n", my_id);

    while (count < COUNT_LIMIT) {
        sleep(1);
    }

    printf("watch_count_sleep(): thread %ld Sleep wait completed. Count= %d,\n", my_id,count);
    if (timer_getElasped(t, &elapsed)) {
        printf("watch_count_sleep(): noticed before timer could be started!\n");
    } else {
        printf("watch_count_sleep(): elapsed time: %f s\n", (double)elapsed.tv_sec + ((double)elapsed.tv_usec)/1000000);
    }
    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    int i;
    int numCounters;
    pthread_t *threads;
    args_t *args;
    pthread_attr_t attr;
    timer *t;

    if (argc != 2) {
        fprintf(stderr, "ERROR: Incorrect number of command line arguments\n"
                        "Usage: %s <# COUNTERS>\n", argv[0]);
        exit(EXIT_FAILURE);
    } else if ((numCounters = strtol(argv[1], NULL, 10))) {
        threads = (pthread_t *) malloc((numCounters + 3) * sizeof(pthread_t));
        args = (args_t *) malloc((numCounters + 3) * sizeof(args_t));
        if (threads == NULL || args == NULL) {
            fprintf(stderr, "ERROR: malloc failed\n");
            exit(EXIT_FAILURE);
        }
    } else {
        fprintf(stderr, "ERROR: Bad # COUNTERS\n"
                        "%s is not a positive integer\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    /* Initialize args */
    t = new_timer();
    if (t == NULL) {
        fprintf(stderr, "ERROR: %s\n", timer_error_str[timer_errno]);
        exit(EXIT_FAILURE);
    }

    for (i = 0; i < numCounters + 3; ++i) {
        args[i].id = i;
        args[i].t  = t;
    }

    /* Initialize mutex and condition variable objects */
    pthread_mutex_init(&count_mutex, NULL);
    pthread_cond_init (&count_threshold_cv, NULL);

    /* For portability, explicitly create threads in a joinable state */
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    /* start watchers */
    pthread_create(&threads[0], &attr, watch_count_cond,  (void *)(args + 0));
    pthread_create(&threads[1], &attr, watch_count_busy,  (void *)(args + 1));
    pthread_create(&threads[2], &attr, watch_count_sleep, (void *)(args + 2));

    /* start counters */
    for (i = 3; i < numCounters + 3; ++i) {
        pthread_create(&threads[i], &attr, inc_count,  (void *)(args + i));
    }

    /* Wait for all threads to complete */
    for (i = 0; i < numCounters + 3; i++) {
        pthread_join(threads[i], NULL);
    }

    printf ("Main(): Waited and joined with %d threads. Final value of count = %d. Done.\n", 
            numCounters + 3, count);

    /* Clean up and exit */
    pthread_attr_destroy(&attr);
    pthread_mutex_destroy(&count_mutex);
    pthread_cond_destroy(&count_threshold_cv);
    free(args);
    free(threads);
    free_timer(t);
    pthread_exit (NULL);
}

