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

// ---------------------------------------------------------------------------------------------------------------------

void *checkedmalloc(size_t n);

// ---------------------------------------------------------------------------------------------------------------------

void data_out(unsigned char *start, size_t len, void *file);

// ---------------------------------------------------------------------------------------------------------------------

void jbg85_set_enc_options(
    struct jbg85_enc_state *s,
    bool deterministic_pred,
    bool typical_pred,
    bool diff_layer_typical_pred,
//    bool var_img_height,
    bool two_line_template
);

// ---------------------------------------------------------------------------------------------------------------------

void jbg85_set_def_enc_options(struct jbg85_enc_state *s);

// ---------------------------------------------------------------------------------------------------------------------

#endif  // JBIG_CODEC_H
