#include "standard.h"
#include "util.h"
#include "functions.h"

int main(int argc, char **argv) {
    opt_struct opts = opt_builder(argc, argv);
    professor_t p;
    professor_t p2;
    queue_t queue;
    
    printOpts(&opts);
    
    professor_init(&p, &opts);
    professor_init(&p2, &opts);

    printProf(&p);
    printProf(&p2);

    queue_init(&queue, &opts); 
    return 0;
}
