/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "jbig_interface.h"
#include <malloc.h>
#include <stdlib.h>

// ---------------------------------------------------------------------------------------------------------------------

/*
 * malloc() with exception handler
 */
void *checkedmalloc(
    size_t n
){
    void *p;

    if ((p = malloc(n)) == NULL) {
        fprintf(stderr, "Sorry, not enough memory available!\n");
        exit(1);
    }

    return p;
}

// ---------------------------------------------------------------------------------------------------------------------

void data_out(
    unsigned char *start,
    size_t len,
    void *file
){
    fwrite(start, 1, len, (FILE *) file);
}

// ---------------------------------------------------------------------------------------------------------------------

void jbg85_set_enc_options(
    struct jbg85_enc_state *s,
    bool deterministic_pred,
    bool typical_pred,
    bool diff_layer_typical_pred,
//    bool var_img_height,
    bool two_line_template
){
    s->options = 0;
    if (deterministic_pred)
        s->options |= JBG_DPON;
    if (typical_pred)
        s->options |= JBG_TPBON;
    if (diff_layer_typical_pred)
        s->options |= JBG_TPDON;
//    if (var_img_height)
//        s->options |= JBG_VLENGTH;
    if (two_line_template)
        s->options |= JBG_LRLTWO;
}

// ---------------------------------------------------------------------------------------------------------------------

void jbg85_set_def_enc_options(
    struct jbg85_enc_state *s
){
    jbg85_set_enc_options(
        s,
        true, // deterministic_pred
        false, // typical_pred: rarely similar line after each other
        false, // diff_layer_typical_pred: multiple layer is turned off
//        false, // var_img_height: img_heigh is known
        false // two_line_template: template should be three lines
    );
}

// ---------------------------------------------------------------------------------------------------------------------