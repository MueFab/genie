/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#if WIN32
#include<stdio.h>
#endif
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
//    fwrite(start, len, 1, (FILE *) file);
}

// ---------------------------------------------------------------------------------------------------------------------

int line_out(
    const struct jbg85_dec_state *s,
    unsigned char *start, size_t len,
    unsigned long y,
    void *file
){
//    if (y == 0) {
//        /* prefix first line with PBM header */
//        fprintf((FILE *) file, "P4\n");
//        fprintf((FILE *) file, "%10lu\n", jbg85_dec_getwidth(s));
//        /* store file position of height, so we can update it after NEWLEN */
//        y_0 = jbg85_dec_getheight(s);
//        ypos_error = fgetpos((FILE *) file, &ypos);
//        fprintf((FILE *) file, "%10lu\n", y_0); /* pad number to 10 bytes */
//    }
//    fwrite(start, len, 1, (FILE *) file);
    fwrite(start, 1, len, (FILE *) file);
    return y == -1;
}

// ---------------------------------------------------------------------------------------------------------------------

void jbg85_set_enc_options(
    struct jbg85_enc_state *s,
    bool deterministic_pred,
    bool typical_pred,
    bool diff_layer_typical_pred,
    bool two_line_template
){
    s->options = 0;
    if (deterministic_pred)
        s->options |= JBG_DPON;
    if (typical_pred)
        s->options |= JBG_TPBON;
    if (diff_layer_typical_pred)
        s->options |= JBG_TPDON;
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
        false // two_line_template: template should be three lines
    );
}

// ---------------------------------------------------------------------------------------------------------------------