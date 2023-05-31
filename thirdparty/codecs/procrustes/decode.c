#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

#include "CSA.h"

static void usage(FILE *out, const char *name) {
    fprintf(out, "usage:\n");
    fprintf(out, "%s [options] <input> <output>\n", name);
    fprintf(out, "options:\n");
    fprintf(out, "\t -s <STRING>\t string to search, optional\n");
    fprintf(out, "\t -h         \t print this message and exit\n");
    fprintf(out, "\n");
}

int main(int argc, char *argv[]) {

    const char *search = NULL;

    int rc = 0, c, i;

    while ((c = getopt(argc, argv, "s:h")) >= 0) {
        switch (c) {
            case 'h': {
                usage(stdout, argv[0]);
                return EXIT_FAILURE;
            }

            case 's': {
                search = optarg;
            }
                break;

            default: {
                fprintf(stderr, "error: option '%c' is unknown\n\n", c);
                usage(stderr, argv[0]);
                return EXIT_FAILURE;
            }
        }
    }


    if (optind + 2 != argc) {
        fprintf(stderr, "error: missing input arguments\n\n");
        usage(stderr, argv[0]);
        return EXIT_FAILURE;
    }

    const char *input = argv[optind + 0];
    const char *output = argv[optind + 1];

    CSA *csa = csa_read(input, 0);
#ifdef PROFILE_BB_RANK_BIT
    bb_rank_bit_stats_init();
#endif
    MB *text = csa_decode(csa, 0, csa->bwt->n, false);
#ifdef PROFILE_BB_RANK_BIT
    bb_rank_bit_stats();
#endif
    mb_write(text, output);
    mb_delete(text);

#ifdef PROFILE_BB_RANK_BIT
    bb_rank_bit_stats_init();
#endif
    
    if (search != NULL) {
        MB *res = csa_search(csa, search, 6);
        const sa_t n = res->len / sizeof(sa_t);
        printf("Found="SA_T_FMT"\n", n);
        for (i = 0; i < n; ++i) {
            MB *str = csa_decode(csa, ((sa_t *) res->buf)[i] - 10, 26, true);
            printf("%s\n", (char *) str->buf);
            mb_delete(str);
        }
        mb_delete(res);
    }

#ifdef PROFILE_BB_RANK_BIT
    bb_rank_bit_stats();
#endif

    csa_delete(csa);
    return 0;
}
