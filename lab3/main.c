#include "standard.h"


/*
 * Compile using the -lpthread flag
 */
int main(int argc, char **argv) {
    pthread_t top_level_thread;
    args_c c;
    
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

    c.threadNumber = 0;

    /* note use of "&" when passing arguments */
    if(pthread_create(&top_level_thread, NULL, create_thread, CAST_VOID(&c)) != 0) {
        perror("pthread_create");
        return EXIT_FAILURE;
    }

    if(pthread_join(top_level_thread, NULL) != 0) {
        perror("pthread_join");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

void* (create_thread)(void *args) {
    args_c *c =  CAST_C(args);
    fprintf(stdout, "Level %d, tid:  %ld\n", c->level, pthread_self());
    pthread_t new_thread1, new_thread2;
    void* (*func)(void *);
    void* arg1;
    void* arg2;

    if(c->level == 1) {
        func = leaf_func;

        if ((arg1 = malloc(sizeof(args_l))) == NULL) {
            //error
        }
        if ((arg2 = malloc(sizeof(args_l))) == NULL) { 
            //error
        }

        CAST_L(arg1)->threadNumber = c->threadNumber*2 + 1;
        CAST_L(arg2)->threadNumber = c->threadNumber*2 + 2;
    } else {
        /* make a new level of the tree */
        func = create_thread;

        arg1 = malloc(sizeof(args_l));
        arg2 = malloc(sizeof(args_l));

        CAST_C(arg1)->threadNumber = c->threadNumber*2 + 1;
        CAST_C(arg2)->threadNumber = c->threadNumber*2 + 2;
        CAST_C(arg1)->level = c->level - 1;
        CAST_C(arg2)->level = c->level - 1;
    }

    if(pthread_create(&new_thread1, NULL, func, arg1) != 0) {
        perror("pthread_create");
        exit(EXIT_FAILURE);
    }
    if(pthread_create(&new_thread2, NULL, func, arg2) != 0) {
        perror("pthread_create");
        exit(EXIT_FAILURE);
    }
    
    for (int i = 0; i < 10; ++i) {
        sched_yield();
        fprintf(stdout, "Thread Number: %i, Thread ID: %ld, Index: %i\n", c->threadNumber, pthread_self(), i);
    }
    
    if(pthread_join(new_thread1, NULL) != 0) {
        perror("pthread_join");
        exit(EXIT_FAILURE);
    }
    if(pthread_join(new_thread2, NULL) != 0) {
        perror("pthread_join");
        exit(EXIT_FAILURE);
    }
    
    
    free(arg1);
    free(arg2);
    
    /* finish current thread (this function always succeeds) */
    pthread_exit(0);
}

void* leaf_func(void *args) {
    args_l *l =  CAST_L(args);

    fprintf(stdout, "Level 0, tid:  %ld\n", pthread_self());

    for (int i = 0; i < 10; ++i) {
        sched_yield();
        fprintf(stdout, "Thread Number: %i, Thread ID: %ld, Index: %i\n", l->threadNumber, pthread_self(), i);
    }


    /* finish current thread (this function always succeeds) */
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
