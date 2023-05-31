#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

#include "CSA.h"

#define  DEFAULT_SA 256
#define  DEFAULT_LF 128
#define  DEFAULT_BS 63
#define  DEFAULT_BB 0

static void usage(FILE *out, const char *name) {
    fprintf(out, "usage:\n");
    fprintf(out, "%s [options] <input> <output>\n", name);
    fprintf(out, "options:\n");
    fprintf(out, "\t -a \t <INT>      \t sampling_rate_sa [%d]\n", DEFAULT_SA);
    fprintf(out, "\t -f \t <INT>      \t sampling_rate_lf [%d]\n", DEFAULT_LF);
    fprintf(out, "\t -b \t <INT>      \t block_size [%d]\n", DEFAULT_BS);
    fprintf(out, "\t -B \t <INT>      \t superblock_size [%d]\n", DEFAULT_BB);
    fprintf(out, "\t -h \t            \t print this message and exit\n");
    fprintf(out, "\n");
}


int main(int argc, char *argv[]) {

    int rc = 0, c, i;

    uint32_t block_size       = DEFAULT_BS;
    uint32_t sampling_rate_sa = DEFAULT_SA;
    uint32_t sampling_rate_lf = DEFAULT_LF;
    uint32_t superblock_size  = DEFAULT_BB;

    while ((c = getopt(argc, argv, "a:f:b:B:h")) >= 0) {
        switch (c) {
            case 'h': {
                usage(stdout, argv[0]);
                return EXIT_FAILURE;
            }

            case 'a': {
                sampling_rate_sa = atoi(optarg);
            } break;

            case 'f': {
                sampling_rate_lf = atoi(optarg);
            } break;

            case 'b': {
                block_size = atoi(optarg);
            } break;

            case 'B': {
                superblock_size = atoi(optarg);
            } break;

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

    const char *input_name    = argv[optind + 0];
    const char *output_prefix = argv[optind + 1];

    MB *input_mb = mb_read(input_name);
    //printf("Len=%ld\n",input_mb->len);
    ch_t *input = (ch_t *) input_mb->buf;
    ch_t input_init(const sa_t n) { return input[n]; }
    CSA *csa = csa_new(input_mb->len / sizeof(ch_t), input_init,
                       sampling_rate_sa, sampling_rate_lf,
                       superblock_size ? superblock_size : ceil_log2(input_mb->len), block_size,
                       true, false);
    csa_write(csa, output_prefix);
    csa_delete(csa);
    mb_delete(input_mb);

    return 0;
}
