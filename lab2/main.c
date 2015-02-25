#include "standard.h"

int main(int argc, char **argv) {
    int opt;
    int num_levels = DEFAULT_NUM_LEVELS;
    int num_children = DEFAULT_NUM_CHILDREN;
    int s;
    bool pflag = false;
    bool sflag = false;

    opt = getopt(argc, argv, OPT_STRING); 
    while (opt != -1) {
        switch (opt) {
            case 'u':
                usage();
                return EXIT_SUCCESS;
                break;
            case 'N':
                if (parseStr(optarg, &num_levels)) {
                    fprintf(stderr, "Error: %s cannot be coverted to an integer\n", optarg);
                    return EXIT_FAILURE;
                }

                if (num_levels > MAX_NUM_LEVELS) {
                    fprintf(stderr, "Error: number of levels cannot be <= %i\n", MAX_NUM_LEVELS);
                    return EXIT_FAILURE;
                } else if (num_levels == 0) {
                    num_levels = 1;
                }
                break;
            case 'M':
                if (parseStr(optarg, &num_children)) {
                    fprintf(stderr, "Error: %s cannot be coverted to an integer\n", optarg);
                    return EXIT_FAILURE;
                }

                if (num_children > MAX_NUM_CHILDREN) {
                    fprintf(stderr, "Error: number of children cannot be <= %i\n", MAX_NUM_CHILDREN);
                    return EXIT_FAILURE;
                }
                break;
            case 'p':
                pflag = true;
                break;
            case 's':
                if (parseStr(optarg, &s)) {
                    fprintf(stderr, "Error: %s cannot be coverted to an integer\n", optarg);
                    return EXIT_FAILURE;
                }
                sflag = true;
                break;
            case ':':
                missingArg(optopt);
                return EXIT_FAILURE;
                break;
            case '?':
                unknownArg(optopt);
                return EXIT_FAILURE;
                break;
            default:
                fprintf(stderr, "Something happened\n");
                return EXIT_FAILURE;
                break;
        }

        opt = getopt(argc, argv, OPT_STRING); 
    }

    if (pflag && sflag) {
        fprintf(stderr, "Error: cannot both sleep and pause");
        return EXIT_FAILURE;
    } else if (!pflag && !sflag) {
        sflag = true;
        s = 1;
    }

    pid_t self = getpid();
    pid_t parent = getppid();
    pid_t child;

    fprintf(stdout, "ALIVE: Level %i process with pid=%i, child of ppid=%i.\n", num_levels - 1, self, parent);

    if (num_levels > 1) {
        char sbuff[BUFF_SIZE];
        char nbuff[BUFF_SIZE];
        char mbuff[BUFF_SIZE];

        if (snprintf(nbuff, BUFF_SIZE, "%i", (num_levels - 1)) < 0) {
            fprintf(stderr, "Error: problem with snprintf, %s\n", strerror(errno));
            return EXIT_FAILURE;
        }

        if (snprintf(mbuff, BUFF_SIZE, "%i", num_children) < 0) {
            fprintf(stderr, "Error: problem with snprintf, %s\n", strerror(errno));
            return EXIT_FAILURE;
        }

        if (sflag && snprintf(sbuff, BUFF_SIZE, "%i", s) < 0) {
            fprintf(stderr, "Error: problem with snprintf, %s\n", strerror(errno));
            return EXIT_FAILURE;
        }

        for (int i = 0; i < num_children; ++i) {
            child = fork();
            if (child < 0) {
                fprintf(stderr, "Error: problem with fork, %s\n", strerror(errno));
                return EXIT_FAILURE;
            } else if (child == 0 && sflag) {
                execl(argv[0], argv[0], "-M", mbuff,  "-N", nbuff, "-s", sbuff, NULL);
            } else if (child == 0) {
                execl(argv[0], argv[0], "-M", mbuff,  "-N", nbuff, "-p", NULL);
            }       
        }

        for (int i = 0; i < num_children; ++i) {
            wait(NULL);
        }

    } else if (sflag) {
        sleep(s);
    } else {
        pause();
    }

    fprintf(stdout, "EXITING: Level %i process with pid=%i, child of ppid=%i.\n", num_levels - 1, self, parent);
    return EXIT_SUCCESS;
}

static void usage(void) {
    fprintf(stderr, "%s", USAGE_STRING);
}

static void missingArg(int arg) {
    fprintf(stderr, "Error: missing argument, -%c expects a value\n", arg);
    usage();
}

static void unknownArg(int arg) {
    fprintf(stderr, "Error: unknown argument, -%c\n", arg);
    usage();
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
