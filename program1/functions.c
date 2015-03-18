#include "standard.h"
#include "functions.h"

void professor_init(professor_t *professor, opt_struct *opts) {
    static uint8_t id = 0;
    ++id;
    professor->ID = id;
#define X(type, var, fmt) \
    professor->min_ ## var = opts->min_ ## var; \
    professor->max_ ## var = opts->max_ ## var;
    PROFESSOR_RANGES
#undef X
}

void printProf(professor_t *professor) {
    fprintf(stdout, "professor (%" PRIu8 "):\n", professor->ID);
#define X(type, var, fmt) \
    fprintf(stdout, "min_%s = %" fmt "\n", #var, professor->min_ ## var); \
    fprintf(stdout, "max_%s = %" fmt "\n",  #var, professor->max_ ## var);
    PROFESSOR_RANGES
#undef X

}

void student_init(student_t *student) {
    static uint8_t id = 0;
    ++id;
    student->ID = id;
}

void queue_init(queue_t *queue, opt_struct *opts) {
    int error;
    queue->queue_size = opts->queue_size;
    queue->students_per_assignment = opts->students_per_assignment;
    queue->arr = malloc(queue->queue_size * sizeof(assignment_t *));

    if (queue->arr == NULL) {
        fprintf(stderr, "Error: problem with malloc, %s\n", strerror(errno));
        exit(EXIT_FAILURE);      
    }
    
    for (int i = 0; i < queue->queue_size; ++i) {
        queue->arr[i] = NULL;
    }

    if((error = pthread_mutex_init(&(queue->hmx), NULL))) {
        fprintf(stderr, "Error: pthread_mutex_init failed\nError %i\n", error);
        exit(EXIT_FAILURE);
    }

    if((error = pthread_mutex_init(&(queue->tmx), NULL))) {
        fprintf(stderr, "Error: pthread_mutex_init failed\nError %i\n", error);
        exit(EXIT_FAILURE);
    }

    if((error = pthread_cond_init(&(queue->not_empty), NULL))) {
        fprintf(stderr, "Error: pthread_cond_init failed\nError %i\n", error);
        exit(EXIT_FAILURE);
    }

    if((error = pthread_cond_init(&(queue->new), NULL))) {
        fprintf(stderr, "Error: pthread_cond_init failed\nError %i\n", error);
        exit(EXIT_FAILURE);
    }

    if((error = pthread_cond_init(&(queue->not_full), NULL))) {
        fprintf(stderr, "Error: pthread_cond_init failed\nError %i\n", error);
        exit(EXIT_FAILURE);
    }
}

void queue_destroy(queue_t *queue) {
    int error;

    if((error = pthread_mutex_destroy(&(queue->hmx)))) {
        fprintf(stderr, "Error: pthread_mutex_destroy failed\nError %i\n", error);
        exit(EXIT_FAILURE);
    }

    if((error = pthread_mutex_destroy(&(queue->tmx)))) {
        fprintf(stderr, "Error: pthread_mutex_destroy failed\nError %i\n", error);
        exit(EXIT_FAILURE);
    }

    if((error = pthread_cond_destroy(&(queue->not_empty)))) {
        fprintf(stderr, "Error: pthread_cond_destroy failed\nError %i\n", error);
        exit(EXIT_FAILURE);
    }

    if((error = pthread_cond_destroy(&(queue->new)))) {
        fprintf(stderr, "Error: pthread_cond_destroy failed\nError %i\n", error);
        exit(EXIT_FAILURE);
    }

    if((error = pthread_cond_destroy(&(queue->not_full)))) {
        fprintf(stderr, "Error: pthread_cond_destroy failed\nError %i\n", error);
        exit(EXIT_FAILURE);
    }
    
    for (int i = 0; i < queue->queue_size; ++i) {
        assert(queue->arr[i] == NULL);
    }

    free(queue->arr);
    queue->arr = NULL;
}

void assignment_init(assignment_t *assignment, uint8_t num_hours, 
                    professor_t *professor, uint32_t id) {
    int error;
    assignment->ID = id;
    assignment->student_mask = 0;
    assignment->num_hours = num_hours;
    assignment->num_students_completed = 0;
    assignment->professor = professor;
    if((error = pthread_mutex_init(&(assignment->mx), NULL))) {
        fprintf(stderr, "Error: pthread_mutex_init failed\nError %i\n", error);
        exit(EXIT_FAILURE);
    }
}

#if 0
    void* professor_work(void *professor);
    void* student_work(void *student);

    void queue_produce(queue_t *queue, professor_t *p);
    void queue_consume(queue_t *queue, student_t *student);
#endif
