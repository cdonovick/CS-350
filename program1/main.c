#include "standard.h"

int main(int argc, char **argv) {
    opt_struct opts = opt_builder(argc, argv);
    printOpts(&opts);
    return 0;
}
