#ifndef FUNCTIONS_H
#define FUNCTIONS_H
#include "standard.h"
#include "util.h"
#ifndef PTR_CAST
#define PTR_CAST(type, ptr) ((type *) (ptr))
#endif

/* foward declare all structs */
typedef struct assignment_t assignment_t;
typedef struct professor_t professor_t;
typedef struct student_t student_t;
typedef struct queue_t queue_t;
typedef struct node_t node_t;

struct assignment_t {
    uint32_t ID;
    uint16_t student_mask;
    uint8_t num_hours;
    uint8_t students_working;
    uint8_t num_students_completed;
    professor_t *professor;
};

#ifndef PROFESSOR_FIELDS
#define PROFESSOR_FIELDS \
    X(uint8_t, min_prof_wait, PRIu8) \
    X(uint8_t, max_prof_wait, PRIu8) \
    X(uint8_t, min_num_assignments, PRIu8) \
    X(uint8_t, max_num_assignments, PRIu8) \
    X(uint8_t, min_assignment_hours, PRIu8) \
    X(uint8_t, max_assignment_hours, PRIu8) \
    X(uint32_t, num_assignings, PRIu32)
#endif


struct professor_t {
#define X(type, var, fmt) \
    type var;
    PROFESSOR_FIELDS
#undef X
    uint32_t next_assignment;
    uint8_t ID;
};

struct student_t {
    uint8_t ID;
};

struct queue_t {
    /* const members */
    uint8_t students_per_assignment; 
    uint16_t size;
    node_t *arr; /* should not be accessed */
    
    /* locked members */
    uint8_t num_professors;
    node_t *free_head;
    node_t *used_head;
    node_t *used_tail;

    /* member locks */
    pthread_mutex_t px;
    pthread_mutex_t fx;
    pthread_mutex_t ux;

    /* signals */
    pthread_cond_t not_full;
    pthread_cond_t new_item;
};

struct node_t {
    node_t *prev;
    node_t *next;
    assignment_t *assignment;
    pthread_mutex_t mx;
};


typedef struct work_args {
    void *self;
    queue_t *q;
    pthread_barrier_t *barrier;
} work_args;

/* function declarations */
void assignment_init(assignment_t *assignment, uint8_t num_hours, professor_t *professor, uint32_t id);

void professor_init(professor_t *professor, opt_struct *opts);
void* professor_work(void *pargs);
void printProf(professor_t *professor);

void student_init(student_t *student);
void* student_work(void *pargs);

void queue_init(queue_t *queue, opt_struct *opts);
void queue_destroy(queue_t *queue);
void queue_produce(queue_t *queue, assignment_t *t);
bool queue_consume(queue_t *queue, student_t *student);

#endif

