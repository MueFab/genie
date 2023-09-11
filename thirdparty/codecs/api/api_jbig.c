/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "mpegg-codecs.h"
#include "/Data/MPEGG/supportLibraries/fmemopen_windows/libfmemopen.h"
#include "../supportLibraries/fmemopen_windows/libfmemopen.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "jbig/jbig_interface.h"
#include "libjbig/jbig_ar.h"
#include "libjbig/jbig85.h"
#include "libjbig/jbig.h"

#if defined(_WIN32) || defined(_WIN64)

#include <windows.h>
#include <share.h>
#include <io.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdio.h>
/* https://github.com/Arryboom/fmemopen_windows  */

FILE *fmemopen_windows(void *buf, size_t len, const char *type)
{
    int fd;
    FILE *fp;
    char tp[MAX_PATH - 13];
    char fn[MAX_PATH + 1];
    int * pfd = &fd;
    int retner = -1;
    char tfname[] = "MemTF_";
    if (!GetTempPathA(sizeof(tp), tp))
        return NULL;
    if (!GetTempFileNameA(tp, tfname, 0, fn))
        return NULL;
    retner = _sopen_s(pfd, fn, _O_CREAT | _O_SHORT_LIVED | _O_TEMPORARY | _O_RDWR | _O_BINARY | _O_NOINHERIT, _SH_DENYRW, _S_IREAD | _S_IWRITE);
    if (retner != 0)
        return NULL;
    if (fd == -1)
        return NULL;
    fp = _fdopen(fd, "wb+");
    if (!fp) {
        _close(fd);
        return NULL;
    }
    /*File descriptors passed into _fdopen are owned by the returned FILE * stream.If _fdopen is successful, do not call _close on the file descriptor.Calling fclose on the returned FILE * also closes the file descriptor.*/
    fwrite(buf, len, 1, fp);
    rewind(fp);
    return fp;
}
#endif

// ---------------------------------------------------------------------------------------------------------------------

unsigned long y_0;
fpos_t ypos; // TODO @Yeremia: somehow they use this as global variable
int ypos_error = 1;
//unsigned long ymax = 0;
#define JBIG_YMAX 0
#define JBIG_DEF_XMAX 8192
#define JBIG_DEF_BUFLEN 8192
#define JBIG_MAX_BUF_SIZE = 1<<25;

// ---------------------------------------------------------------------------------------------------------------------

int mpegg_jbig_compress_default(
    unsigned char      **dest,
    size_t              *dest_len,
    const unsigned char  *src,
    size_t                scr_len,
    unsigned long nrows,
    unsigned long ncols
){
    return mpegg_jbig_compress(
        dest,
        dest_len,
        src,
        scr_len,
        nrows,
        ncols,
        -1,     // num_lines_per_stripe
        true,   // deterministic_pred
        false,  // typical_pred
        false,  // diff_layer_typical_pred
        false   // two_line_template
    );
};

// ---------------------------------------------------------------------------------------------------------------------

int mpegg_jbig_compress(
    unsigned char      **dest,
    size_t              *dest_len,
    const unsigned char *src,
    size_t               scr_len,
    unsigned long nrows,
    unsigned long ncols,
    unsigned long num_lines_per_stripe, // Valid range: 1 to 2^32-1
    bool deterministic_pred,
    bool typical_pred,
    bool diff_layer_typical_pred,
    bool two_line_template
){

    int i;
    FILE *fout = stdout;
    struct jbg85_enc_state se;
    unsigned char* src_ptr = (unsigned char*)src;

    size_t bpl = (ncols >> 3) + ((ncols & 7) != 0);     /* bytes per line */

    *dest_len = scr_len * 3; // Expect worst case 3x source size
    *dest = (unsigned char *) malloc (*dest_len * sizeof(unsigned  char));
#if defined(_WIN32) || defined(_WIN64)
    fout = fmemopen_windows(*dest, *dest_len * sizeof(unsigned  char), "wb");
#else
    fout = fmemopen(*dest, *dest_len * sizeof(unsigned  char), "wb");
#endif
    if (!fout){
        fprintf(stderr, "Can't open input file");
        exit(1);
    }

    jbg85_enc_init(&se, ncols, nrows, data_out, fout);      /* initialize encoder */
    jbg85_set_enc_options(
        &se,
        deterministic_pred ,
        typical_pred,
        diff_layer_typical_pred,
        two_line_template
    );
    se.l0 = num_lines_per_stripe;

    for (i = 0; i < nrows; i++) {
        // encode line
        jbg85_enc_lineout(&se, src_ptr+i*bpl, src_ptr+(i-1)*bpl, src_ptr+(i-2)*bpl);
    }

    // Resize output
    *dest_len = ftell(fout);
    *dest = (unsigned char*)realloc(*dest, *dest_len * sizeof(unsigned char));
    if (dest == NULL){
            fprintf(stderr, "Problem with resizing");
            perror("'");
            exit(1);
    }

    if (ferror(fout) || fclose(fout)) {
        fprintf(stderr, "Problem while writing output file");
        perror("'");
        exit(1);
    }

    return 0;
}

// ---------------------------------------------------------------------------------------------------------------------

int mpegg_jbig_decompress_default(
    unsigned char      **dest,
    size_t              *dest_len,
    const unsigned char *src,
    size_t               src_len,
    unsigned long       *nrows,
    unsigned long       *ncols
){
    return mpegg_jbig_decompress(
        dest,
        dest_len,
        src,
        src_len, nrows, ncols,
        JBIG_DEF_BUFLEN,
        JBIG_DEF_XMAX
    );
}

// ---------------------------------------------------------------------------------------------------------------------

int mpegg_jbig_decompress(
    unsigned char      **dest,
    size_t              *dest_len,
    const unsigned char *src,
    size_t               src_len,
    unsigned long       *nrows,
    unsigned long       *ncols,
    size_t               buf_len,
    size_t               xmax
){
    FILE *fin = stdin, *fout = stdout;
    const char *fnin = NULL, *fnout = NULL;
    int result;
    struct jbg85_dec_state s;
    unsigned char *inbuf, *outbuf;
    size_t outbuflen, len, cnt, cnt2;
    size_t bytes_read = 0;

    unsigned char* src_ptr = (unsigned char*)src;

    inbuf = (unsigned char *)malloc(buf_len * sizeof(unsigned char));
    outbuflen = ((xmax >> 3) + ((xmax & 7) != 0)) * 3;
    outbuf = (unsigned char *)malloc(outbuflen * sizeof(unsigned char));
    if (!inbuf || !outbuf)
    {
        printf("Sorry, not enough memory available!\n");
        exit(1);
    }
#if defined(_WIN32) || defined(_WIN64)
    fin = fmemopen_windows(src_ptr, src_len * sizeof(unsigned  char), "rb");
#else
    fin = fmemopen(src_ptr, src_len * sizeof(unsigned  char), "rb");
#endif

    if (!fin)
    {
        fprintf(stderr, "Can't open input file '%s", fnin);
        exit(1);
    }

    *dest_len = src_len * 30; // Expect worst case 3x source size
<<<<<<< HEAD
    *dest = (unsigned char *)malloc(*dest_len);
=======
    *dest = (unsigned char *) malloc (*dest_len);
#if defined(_WIN32) || defined(_WIN64)
    fout = fmemopen_windows(*dest, *dest_len * sizeof(unsigned  char), "wb");
#else
>>>>>>> c1fc0cf4346510de4db1eb202caac328f61fbbe1
    fout = fmemopen(*dest, *dest_len * sizeof(unsigned  char), "wb");
#endif

    if (!fout)
    {
        fprintf(stderr, "Can't open input file '%s", fnout);
        exit(1);
    }

    // send input file to decoder
    jbg85_dec_init(&s, outbuf, outbuflen, line_out, fout);
    result = JBG_EAGAIN;
    while ((len = fread(inbuf, 1, buf_len, fin)))
    {
        result = jbg85_dec_in(&s, inbuf, len, &cnt);
        bytes_read += cnt;
        while (result == JBG_EOK_INTR)
        {
            /* demonstrate decoder interrupt at given line number */
            printf("Decoding interrupted after %lu lines and %lu BIE bytes "
                "... continuing ...\n",
                s.y, (unsigned long)bytes_read);
            /* and now continue decoding */
            result = jbg85_dec_in(&s, inbuf + cnt, len - cnt, &cnt2);
            bytes_read += cnt2;
            cnt += cnt2;
        }
        if (result != JBG_EAGAIN)
            break;
    }
    if (ferror(fin))
    {
        fprintf(stderr, "Problem while reading input file '%s", fnin);
        perror("'");
        if (fout != stdout)
        {
            fclose(fout);
            remove(fnout);
        }
        exit(1);
    }
    if (result == JBG_EAGAIN || result == JBG_EOK_INTR)
    {
        /* signal end-of-BIE explicitely */
        result = jbg85_dec_end(&s);
        while (result == JBG_EOK_INTR)
        {
            /* demonstrate decoder interrupt at given line number */
            printf("Decoding interrupted after %lu lines and %lu BIE bytes "
                "... continuing ...\n",
                s.y, (unsigned long)bytes_read);
            result = jbg85_dec_end(&s);
        }
    }
    if (result != JBG_EOK)
    {
        fprintf(stderr, "Problem with input file '%s': %s\n"
                "(error code 0x%02x, %lu = 0x%04lx BIE bytes "
                "and %lu pixel rows processed)\n",
                fnin, jbg85_strerror(result), result,
                (unsigned long)bytes_read, (unsigned long)bytes_read, s.y);
        if (fout != stdout)
        {
            fclose(fout);
            /* remove(fnout); */
        }
        exit(1);
    }

    /* do we have to update the image nrows in the PBM header? */
    if (!ypos_error && y_0 != jbg85_dec_getheight(&s))
    {
        if (fsetpos(fout, &ypos) == 0)
        {
            fprintf(fout, "%10lu", jbg85_dec_getheight(&s)); /* pad to 10 bytes */
        }
        else
        {
            fprintf(stderr, "Problem while updating nrows in output file '%s",
                    fnout);
            perror("'");
            exit(1);
        }
    }

    *dest_len = ftell(fout);

    fclose(fin);

    /* check for file errors and close fout */
    if (ferror(fout) || fclose(fout))
    {
        fprintf(stderr, "Problem while writing output file '%s", fnout);
        perror("'");
        exit(1);
    }

    *ncols = jbg85_dec_getwidth(&s);
    *nrows = jbg85_dec_getheight(&s);

    return 0;
}

// ---------------------------------------------------------------------------------------------------------------------
