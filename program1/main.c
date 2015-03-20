#include "standard.h"
#include "util.h"
#include "functions.h"

int main(int argc, char **argv) {
    opt_struct opts = opt_builder(argc, argv);
    int error;

    queue_t queue;
    professor_t p[opts.num_professors];
    student_t s[opts.num_students];

    work_args args[opts.num_professors + opts.num_students];
    pthread_t t[opts.num_professors + opts.num_students];
    
    pthread_barrier_t b;

    if ((error = pthread_barrier_init(&b, NULL, opts.num_professors + opts.num_students + 1))) {
        fprintf(stderr, "Error: pthread_barrier_init failed\nError %i\n", error);
        exit(EXIT_FAILURE);
    }

    queue_init(&queue, &opts); 

    for (uint8_t i = 0; i < opts.num_professors; ++i) {
       professor_init(p + i, &opts);
       args[i].self = PTR_CAST(void, p + i);
       args[i].q = &queue;
       args[i].barrier = &b;
    }

    for (uint8_t i = 0; i < opts.num_students; ++i) {
       student_init(s + i);
       args[i + opts.num_professors].self = PTR_CAST(void, s + i);
       args[i + opts.num_professors].q = &queue;
       args[i + opts.num_professors].barrier = &b;
    }
    
    for (uint8_t i = 0; i < opts.num_professors + opts.num_students; ++i) {
       if (pthread_create(t+i, NULL, (i < opts.num_professors ? professor_work : student_work) , PTR_CAST(void, args+i))) {
           perror("pthread create");
           exit(EXIT_FAILURE);
       }
    }
    int rc = pthread_barrier_wait(&b);
    if (rc && rc != PTHREAD_BARRIER_SERIAL_THREAD) {
        fprintf(stderr, "Error: pthread_barrier_wait failed\nError %i\n", rc);
        exit(EXIT_FAILURE);

    }

    if ((error = pthread_barrier_destroy(&b))) {
        fprintf(stderr, "Error: pthread_barrier_destroy failed\nError %i\n", error);
        exit(EXIT_FAILURE);
    }

    queue_destroy(&queue);

    for (uint8_t i = 0; i < opts.num_professors + opts.num_students; ++i) {
       if (pthread_join(t[i], NULL)) {
           perror("join");
           exit(EXIT_FAILURE);
       }
    }
    return 0;
}
