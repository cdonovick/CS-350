#include "standard.h"
#include "util.h"
#include "functions.c"

int main(int argc, char **argv) {
    opt_struct opts = opt_builder(argc, argv);
    printOpts(&opts);
    NUM_ASSIGNINGS = opts.num_assignings;
    professor_t p;
    professor_t p2;
    professor_init(&p, &opts);
    professor_init(&p2, &opts);

    printProf(&p);
    printProf(&p2);

    queue_init(&QUEUE, &opts); 
    return 0;
}
