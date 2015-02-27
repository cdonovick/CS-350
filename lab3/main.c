#include "standard.h"

int main(int argc, char **argv) {
    pthread_t top_level_thread;
    args_c c;
    int error;
    
    if (argc != 2) {
        usage();
        return EXIT_FAILURE;
    }

    if (parseStr(argv[1], &(c.level))) {
        fprintf(stderr, "Error: %s cannot be coverted to an integer\n", argv[1]);
        usage();
        return EXIT_FAILURE;
    }

    if (c.level > MAX_TREE_LEVELS || c.level < MIN_TREE_LEVELS) {
        fprintf(stderr, "Error: number of levels must be in [%i, %i]\n", MIN_TREE_LEVELS, MAX_TREE_LEVELS);
        return EXIT_FAILURE;
    }

    if ((error = pthread_barrier_init(&barrier, NULL, (2 << c.level) - 1))) {
        fprintf(stderr, "Error: pthread_barrier_init failed\nError %i\n", error);
        return EXIT_FAILURE;
    }

    c.threadNumber = 0;

    if(pthread_create(&top_level_thread, NULL, create_thread, CAST_VOID(&c))) {
        perror("pthread_create");
        return EXIT_FAILURE;
    }

    if(pthread_join(top_level_thread, NULL)) {
        perror("pthread_join");
        return EXIT_FAILURE;
    }
    if ((error = pthread_barrier_destroy(&barrier))) {
        fprintf(stderr, "Error: pthread_barrier_destroy failed\nError %i\n", error);
        return EXIT_FAILURE;
    }


    return EXIT_SUCCESS;
}

void* create_thread(void *args) {
    args_c *c =  CAST_C(args);
    fprintf(stdout, "Level %d, tid:  %ld\n", c->level, pthread_self());
    pthread_t new_thread1, new_thread2;
    void* (*func)(void *);
    void* arg1;
    void* arg2;

    if(c->level == 1) {
        func = leaf_func;

        if ((arg1 = malloc(sizeof(args_l))) == NULL) {
            fprintf(stderr, "Error: problem with malloc, %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
        if ((arg2 = malloc(sizeof(args_l))) == NULL) { 
            fprintf(stderr, "Error: problem with malloc, %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }

        CAST_L(arg1)->threadNumber = c->threadNumber*2 + 1;
        CAST_L(arg2)->threadNumber = c->threadNumber*2 + 2;
    } else {
        /* make a new level of the tree */
        func = create_thread;

        if ((arg1 = malloc(sizeof(args_c))) == NULL) {
            fprintf(stderr, "Error: problem with malloc, %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
        if ((arg2 = malloc(sizeof(args_c))) == NULL) { 
            fprintf(stderr, "Error: problem with malloc, %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }

        CAST_C(arg1)->threadNumber = c->threadNumber*2 + 1;
        CAST_C(arg2)->threadNumber = c->threadNumber*2 + 2;
        CAST_C(arg1)->level = c->level - 1;
        CAST_C(arg2)->level = c->level - 1;
    }

    if(pthread_create(&new_thread1, NULL, func, arg1)) {
        perror("pthread_create");
        exit(EXIT_FAILURE);
    }
    if(pthread_create(&new_thread2, NULL, func, arg2)) {
        perror("pthread_create");
        exit(EXIT_FAILURE);
    }

    
    busyWork(c->threadNumber);

    if(pthread_join(new_thread1, NULL)) {
        perror("pthread_join");
        exit(EXIT_FAILURE);
    }
    if(pthread_join(new_thread2, NULL)) {
        perror("pthread_join");
        exit(EXIT_FAILURE);
    }
    
    
    free(arg1);
    free(arg2);
    
    fprintf(stdout, "Thread %i is exiting\n", c->threadNumber); 
    pthread_exit(0);
}

void* leaf_func(void *args) {
    args_l *l =  CAST_L(args);

    fprintf(stdout, "Level 0, tid:  %ld\n", pthread_self());
    busyWork(l->threadNumber);
    pthread_exit(0);
}

static void usage(void) {
    fprintf(stderr, "%s", USAGE_STRING);
}

static int parseStr(char *str, int *val) {
    int p = strtoul(str, NULL, 10);
    if (p || isdigit(str[0])) {
        *val = p;
        return 0;
    } else {
        return -1;
    }
}

static void busyWork(int TN) {
    fprintf(stdout, "Thread %i has reached barrier\n", TN); 
    int rc = pthread_barrier_wait(&barrier);
    if (rc && rc != PTHREAD_BARRIER_SERIAL_THREAD) {
        fprintf(stderr, "Error: pthread_barrier_wait failed\nError %i\n", rc);
        exit(EXIT_FAILURE);

    }


    fprintf(stdout, "Thread %i has passed barrier\n", TN); 

    for (int i = 0; i < 10; ++i) {
        fprintf(stdout, "Thread Number: %i, Thread ID: %ld, Index: %i\n", TN, pthread_self(), i);
        sched_yield();
    }
}
