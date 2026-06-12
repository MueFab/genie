/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef JBIG_INTERFACE_H
#define JBIG_INTERFACE_H

#include <stdbool.h>
#include "libjbig/jbig_ar.h"
#include "libjbig/jbig85.h"
#include "libjbig/jbig.h"

#ifdef __cplusplus
extern "C" {
#endif

// ---------------------------------------------------------------------------------------------------------------------

void *checkedmalloc(size_t n);

// ---------------------------------------------------------------------------------------------------------------------

void data_out(unsigned char *start, size_t len, void *file);

// ---------------------------------------------------------------------------------------------------------------------

int line_out(
    const struct jbg85_dec_state *s,
    unsigned char *start, size_t len,
    unsigned long y,
    void *file
);

// ---------------------------------------------------------------------------------------------------------------------

void jbg85_set_enc_options(
    struct jbg85_enc_state *s,
    bool deterministic_pred,
    bool typical_pred,
    bool diff_layer_typical_pred,
    bool two_line_template
);

// ---------------------------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------------------------

void jbg85_set_def_enc_options(struct jbg85_enc_state *s);

// ---------------------------------------------------------------------------------------------------------------------

int mpegg_jbig_compress_default(
    unsigned char      **dest,
    size_t              *dest_len,
    const unsigned char  *src,
    size_t                scr_len,
    unsigned long nrows,
    unsigned long ncols
);

int mpegg_jbig_compress(
    unsigned char      **dest,
    size_t              *dest_len,
    const unsigned char *src,
    size_t               scr_len,
    unsigned long nrows,
    unsigned long ncols,
    unsigned long num_lines_per_stripe,
    bool deterministic_pred,
    bool typical_pred,
    bool diff_layer_typical_pred,
    bool two_line_template
);

int mpegg_jbig_decompress_default(
    unsigned char      **dest,
    size_t              *dest_len,
    const unsigned char *src,
    size_t               src_len,
    unsigned long       *nrows,
    unsigned long       *ncols
);

int mpegg_jbig_decompress(
    unsigned char      **dest,
    size_t              *dest_len,
    const unsigned char *src,
    size_t               src_len,
    unsigned long       *nrows,
    unsigned long       *ncols,
    size_t               buf_len,
    size_t               xmax
);

#ifdef __cplusplus
}
#endif

#endif  // JBIG_INTERFACE_H

