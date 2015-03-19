#include "standard.h"
#include "functions.h"

void assignment_init(assignment_t *assignment, uint8_t num_hours, professor_t *professor, uint32_t id) {
    assert(id <= professor->num_assignings);
    assignment->ID = id;
    assignment->student_mask = 0;
    assignment->num_hours = num_hours;
    assignment->students_working = 0;
    assignment->num_students_completed = 0;
    assignment->professor = professor;
}

void professor_init(professor_t *professor, opt_struct *opts) {
    static uint8_t id = 0;
    ++id;
    professor->ID = id;
    professor->next_assignment = 1;
#define X(type, var, fmt) \
    professor->var = opts->var;
    PROFESSOR_FIELDS
#undef X
}

void printProf(professor_t *professor) {
    fprintf(stdout, "professor (%" PRIu8 "):\n", professor->ID);
#define X(type, var, fmt) \
    fprintf(stdout, "%s = %" fmt "\n", #var, professor->var);
    PROFESSOR_FIELDS
#undef X
}

void student_init(student_t *student) {
    static uint8_t id = 0;
    ++id;
    student->ID = id;
}

void queue_init(queue_t *queue, opt_struct *opts) {
    queue->size = opts->queue_size;
    queue->students_per_assignment = opts->students_per_assignment;
    queue->arr = (node_t *) malloc(queue->size * sizeof(node_t));


    if (queue->arr == NULL) {
        fprintf(stderr, "Error: problem with malloc, %s\n", strerror(errno));
        exit(EXIT_FAILURE);      
    }
    
    for (int i = 0; i < queue->size; ++i) {
        queue->arr[i].prev = (i != 0               ? &queue->arr[i-1] : NULL);
        queue->arr[i].next = (i != queue->size - 1 ? &queue->arr[i+1] : NULL);
        queue->arr[i].assignment = NULL;
        mutex_init(&queue->arr[i].mx, NULL);
    }

    queue->free_head = queue->arr;
    queue->used_head = NULL;
    queue->used_tail = NULL;

    mutex_init(&queue->fx, NULL);
    mutex_init(&queue->ux, NULL);

    cond_init(&queue->not_full, NULL);
    cond_init(&queue->new_item, NULL);
}

void queue_destroy(queue_t *queue) {
    for (int i = 0; i < queue->size; ++i) {
        assert(queue->arr[i].assignment == NULL);
    }

    mutex_destroy(&queue->fx);
    mutex_destroy(&queue->ux);

    cond_destroy(&queue->not_full);
    cond_destroy(&queue->new_item);
    free(queue->arr);

    queue->arr = NULL;
}

void queue_produce(queue_t *queue, assignment_t *t) {
    mutex_lock(&queue->fx);
    while (queue->free_head == NULL) {
        cond_wait(&queue->not_full, &queue->fx);
    }
    node_t *n = queue->free_head;
    queue->free_head = queue->free_head->next;
    queue->free_head->prev = NULL;
    mutex_unlock(&queue->fx);

    n->assignment = t;
    n->next = NULL;
    n->prev = NULL;
    
    mutex_lock(&queue->ux);
    if (queue->used_head == NULL) {
        queue->used_head = n;
        queue->used_tail = n;
    } else {
        queue->used_tail->next = n;
        n->prev = queue->used_tail;
        queue->used_tail = n;

    }
    mutex_unlock(&queue->ux);
    cond_broadcast(&queue->new_item);
}


void queue_consume(queue_t *queue, student_t *student) {
    assert(student->ID < 16);
    bool release;
    node_t * work = NULL;
    mutex_lock(&queue->ux);
    while (work == NULL) {
        if (queue->used_head == NULL) {
            cond_wait(&queue->new_item, &queue->ux);
        }

        for (node_t *n = queue->used_head; n != NULL; n=n->next) {
            if ((n->assignment->student_mask & (1<<student->ID)) == 0) {
                if (n->prev == NULL) {
                    queue->used_head = n->next;
                } else {
                    n->prev->next = n->next;
                }
                if (n->next == NULL) {
                    queue->used_tail = n->prev;
                } else {
                    n->next->prev = n->prev;
                }
                work = n;
                break;
            }
        }

        if (work == NULL) {
            cond_wait(&queue->new_item, &queue->ux);
        }
    }

    mutex_unlock(&queue->ux);

    mutex_lock(&work->mx);
    work->assignment->student_mask |= (1<<student->ID);
    assert(work->assignment->students_working < queue->students_per_assignment);
    work->assignment->students_working++;
    
    if (work->assignment->students_working == queue->students_per_assignment) {
        mutex_unlock(&work->mx);
        mutex_lock(&queue->ux);
        queue->used_tail->next = work;
        work->next = NULL;
        work->prev = queue->used_tail;
        queue->used_tail = work;
        mutex_unlock(&queue->ux);
        release = false;
    } else {
        mutex_unlock(&work->mx);
        release = true;
    }
}


void* professor_work(void *pargs) {
    work_args *args = PTR_CAST(work_args, pargs);
    assert(args->self != NULL);
    professor_t *self = PTR_CAST(professor_t, args->self);
    queue_t *queue = PTR_CAST(queue_t, args->q);
    uint8_t prof_wait;
    uint8_t num_assignments;
    uint8_t hours;
    assignment_t *temp;

    for(uint32_t i = 0; i < self->num_assignings; ++i) {
        prof_wait = (uint8_t) RANDRANGE(self->min_prof_wait, self->max_prof_wait);
        sleep(prof_wait);
        num_assignments = (uint8_t) RANDRANGE(self->min_num_assignments, self->max_num_assignments);
        for (uint8_t j = 0; j < num_assignments; ++j) {
            temp = (assignment_t *) malloc(sizeof(assignment_t));
            if (temp == NULL) {
                fprintf(stderr, "Error: problem with malloc, %s\n", strerror(errno));
                exit(EXIT_FAILURE);      
            }
            hours = (uint8_t) RANDRANGE(self->min_assignment_hours, self->max_assignment_hours);
            assignment_init(temp, hours, self, self->next_assignment++);
            queue_produce(queue, temp);
        }
    }

    return 0;
}



#if 0
    void* student_work(void *student);







#endif
