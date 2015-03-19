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
    queue->num_professors = opts->num_professors; 

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
    
    mutex_init(&queue->px, NULL);
    mutex_init(&queue->fx, NULL);
    mutex_init(&queue->ux, NULL);

    cond_init(&queue->not_full, NULL);
    cond_init(&queue->new_item, NULL);
}

void queue_destroy(queue_t *queue) {
    for (int i = 0; i < queue->size; ++i) {
        assert(queue->arr[i].assignment == NULL);
    }

    mutex_destroy(&queue->px);
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
    synch_fprintf(stdout, "ASSIGN Professor  " PRIu8 "adding Assignment " PRIu32 ": " PRIu8 " Hours\n", n->assignment->professor->ID, n->assignment->ID, n->assignment->num_hours);
    mutex_unlock(&queue->ux);
    cond_broadcast(&queue->new_item);
}


bool queue_consume(queue_t *queue, student_t *student) {
    assert(student->ID < 16);
    bool done = false;
    node_t * work = NULL;
    mutex_lock(&queue->ux);
    while (work == NULL) {
        if (queue->used_head == NULL) {
            mutex_lock(&queue->px);
            done = (queue->num_professors == 0);
            mutex_unlock(&queue->px);
            if (!done) {
                cond_wait(&queue->new_item, &queue->ux);
            }
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

        if (work == NULL && done) {
            mutex_unlock(&queue->ux);
            return false;
        }   
    }

    mutex_unlock(&queue->ux);

    mutex_lock(&work->mx);
    synch_fprintf(stdout, "BEGIN Student " PRIu8 " working on Assignment " PRIu32 
            " from Professor " PRIu8 "\n", student->ID, 
            work->assignment->ID, work->assignment->professor->ID);

    work->assignment->student_mask |= (1<<student->ID);
    assert(work->assignment->students_working < queue->students_per_assignment);
    work->assignment->students_working++;

    if (work->assignment->students_working != queue->students_per_assignment) {
        mutex_unlock(&work->mx);
        mutex_lock(&queue->ux);
        queue->used_tail->next = work;
        work->next = NULL;
        work->prev = queue->used_tail;
        queue->used_tail = work;
    }
    
    mutex_unlock(&queue->ux);
        
    for (uint8_t i = 1; i <= work->assignment->num_hours; ++i) {
        sleep(1);
        synch_fprintf(stdout, "WORK Student " PRIu8 " working on Assignment " PRIu32 
                " Hour " PRIu8 " from Professor " PRIu8 "\n", 
                student->ID, work->assignment->ID, i, 
                work->assignment->professor->ID);

    }

    synch_fprintf(stdout, "END Student " PRIu8 " working on Assignment " PRIu32 
            " from Professor " PRIu8 "\n", student->ID, 
            work->assignment->ID, work->assignment->professor->ID);
    
    mutex_lock(&work->mx);
    assert(work->assignment->num_students_completed < queue->students_per_assignment);
    work->assignment->num_students_completed++;
    if (work->assignment->num_students_completed == queue->students_per_assignment) {
        mutex_unlock(&work->mx);
        free(work->assignment);
        work->assignment = NULL;
        mutex_lock(&queue->fx);
        work->next = queue->free_head;
        queue->free_head = work;
        mutex_unlock(&queue->fx);
        cond_broadcast(&queue->not_full);
    } else {
        mutex_unlock(&work->mx);
    }

    return true;
}


void* professor_work(void *pargs) {
    work_args *args = PTR_CAST(work_args, pargs);
    assert(args->self != NULL);
    assert(args->q != NULL);
    professor_t *self = PTR_CAST(professor_t, args->self);
    queue_t *queue = PTR_CAST(queue_t, args->q);
    uint8_t prof_wait;
    uint8_t num_assignments;
    uint8_t hours;
    assignment_t *temp;

    synch_fprintf(stdout, "STARTING Professor  " PRIu8 "\n", self->ID);
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

    mutex_lock(&queue->px);
    queue->num_professors--;
    mutex_unlock(&queue->px);
    synch_fprintf(stdout, "EXIT Professor  " PRIu8 "\n", self->ID);
    pthread_barrier_wait(args->barrier);
    return 0;
}


void* student_work(void *pargs) {
    work_args *args = PTR_CAST(work_args, pargs);
    assert(args->self != NULL);
    assert(args->q != NULL);
    student_t *self = PTR_CAST(student_t, args->self);
    queue_t *queue = PTR_CAST(queue_t, args->q);

    synch_fprintf(stdout, "STARTING Student " PRIu8 "\n", self->ID);
    while (queue_consume(queue, self));
    synch_fprintf(stdout, "EXIT Student " PRIu8 "\n", self->ID);
    pthread_barrier_wait(args->barrier);
    return 0;

}
