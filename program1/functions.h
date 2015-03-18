#ifndef FUNCTIONS_H
#define FUNCTIONS_H
#include "standard.h"
#include "util.h"
#ifndef PTR_CAST
#define PTR_CAST(type, ptr) (((type) *) (ptr))
#endif

/* foward declare all structs */
typedef struct assignment_t assignment_t;
typedef struct professor_t professor_t;
typedef struct student_t student_t;
typedef struct queue_t queue_t;


struct assignment_t {
    uint32_t ID;
    uint16_t student_mask;
    uint8_t num_hours;
    uint8_t num_students_completed;
    professor_t *professor;
    pthread_mutex_t mx;
};


#ifndef PROFESSOR_RANGES
#define PROFESSOR_RANGES \
    X(uint8_t, prof_wait, PRIu8) \
    X(uint8_t, num_assignments, PRIu8) \
    X(uint8_t, assignment_hours, PRIu8)
#endif

struct professor_t {
#define X(type, var, fmt) \
    type min_ ## var; \
    type max_ ## var;
    PROFESSOR_RANGES
#undef X
    uint8_t ID;
};

struct student_t {
    uint8_t ID;
};

struct queue_t {
    uint8_t students_per_assignment;
    uint16_t queue_size;
    uint16_t head;
    uint16_t tail;
    assignment_t **arr;
    pthread_mutex_t hmx;
    pthread_mutex_t tmx;
    pthread_cond_t not_empty;
    pthread_cond_t not_full;
    pthread_cond_t new;
};


/* globals */
uint32_t NUM_ASSIGNINGS;
queue_t QUEUE;

/* function declarations */
void professor_init(professor_t *professor, opt_struct *opts);

void* professor_work(void *professor);
void printProf(professor_t *professor);

void student_init(student_t *student);
void* student_work(void *student);

void queue_init(queue_t *queue, opt_struct *opts);
void queue_destroy(queue_t *queue);
void queue_produce(queue_t *queue, professor_t *p);
void queue_consume(queue_t *queue, student_t *student);

void assignment_init(assignment_t *assignment, uint8_t num_hours, professor_t *professor, uint32_t id);
#endif

