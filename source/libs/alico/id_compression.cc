//
//  id_compression.c
//  XC_s2fastqIO
//
//  Created by Mikel Hernaez on 12/10/14.
//  Copyright (c) 2014 Mikel Hernaez. All rights reserved.
//

// Compression of the IDs -- work based on the compression of ID in Samcomp by Mahonney and Bonfiled (2012)

#include <stdio.h>
#include "sam_block.h"

uint8_t decompress_uint8tt(Arithmetic_stream as, stream_model model, FILE** fin, int* flag, char* name){
    uint8_t c;

    if(*flag == 0) {
        (*fin) = fopen(name, "rb");
        *flag = 1;
    }
    fread(&c, 1, 1, (*fin));

/*
    // Send the value to the Arithmetic Stream
    uint8_t c = read_value_from_as(as, model);

    // Update model
    update_model(model, c);
*/
    return c;

}

uint8_t decompress_uint8t(Arithmetic_stream as, stream_model model) {
    // Send the value to the Arithmetic Stream
    uint8_t c = read_value_from_as(as, model);

    // Update model
    update_model(model, c);

    return c;
}

int compress_uint8tt(Arithmetic_stream as, stream_model model, uint8_t c, FILE** fin, int* flag, char* name){

    if(*flag == 0) {
        (*fin) = fopen(name, "wb");
        *flag = 1;
    }
    fwrite(&c, 1, 1, (*fin));

    /*
    // Send the value to the Arithmetic Stream
    send_value_to_as(as, model, c);

    // Update model
    update_model(model, c);*/

    return 1;

}

int compress_uint8t(Arithmetic_stream as, stream_model model, uint8_t c) {
    // Send the value to the Arithmetic Stream
    send_value_to_as(as, model, c);

    // Update model
    update_model(model, c);

    return 1;
}

int compress_rname(Arithmetic_stream as, rname_models models, char *rname, void *thread_info){
    static char prev_name[1024] = {0};
    static int prevChar = 0;

    char nm[35];
    char number[4];

    uint32_t ctr = 0;
    uint8_t tmp;
    struct compressor_info_t *info = ((struct compressor_info_t *)thread_info);

    if(strcmp(rname, prev_name) == 0){
        tmp = 0;
        fwrite(&tmp, 1, 1, info->fsame_ref);
        /*compress_uint8t(as, models->same_ref[0], 0);*/
        return 0;

    }

    else{
        tmp = 1;
        fwrite(&tmp, 1, 1, info->fsame_ref);
        /*compress_uint8t(as, models->same_ref[0], 1);*/

        while (*rname) {
            strcpy(nm, "rname");
            sprintf(number, "%d", prevChar);
            strcat(nm, number);
            compress_uint8tt(as, models->rname[prevChar], *rname, &info->frname[prevChar], &info->irname[prevChar], nm);
            prev_name[ctr++] = *rname;
            prevChar = *rname++;
        }

        strcpy(nm, "rname");
        sprintf(number, "%d", prevChar);
        strcat(nm, number);
        compress_uint8tt(as, models->rname[prevChar], 0, &info->frname[prevChar], &info->irname[prevChar], nm);

        prev_name[ctr] = 0;
        return 1;
    }

}

int compress_mapq(Arithmetic_stream as, mapq_models models, uint8_t mapq, void *thread_info){
    struct compressor_info_t *info = ((struct compressor_info_t *)thread_info);

    fwrite(&mapq, 1, 1, info->fmapq);
    //compress_uint8t(as, models->mapq[0], mapq);

    return 0;
}

int decompress_mapq(Arithmetic_stream as, mapq_models models, uint8_t *mapq, void *thread_info){
    struct compressor_info_t *info = ((struct compressor_info_t *)thread_info);

    fread(mapq, 1, 1, info->fmapq);

    //*mapq = decompress_uint8t(as, models->mapq[0]);

    return 0;
}

int compress_rnext(Arithmetic_stream as, rnext_models models, char *rnext, void *thread_info){

    static char prev_name[1024] = {0};
    static int prevChar = 0;

    uint32_t ctr = 0;
    uint8_t tmp;
    char nm[40];
    char number[15];
    struct compressor_info_t *info = ((struct compressor_info_t *)thread_info);

    switch (rnext[0]) {
        case '=':
            tmp = 0;
            fwrite(&tmp, 1, 1, info->rnext_same);
            //compress_uint8t(as, models->same_ref[0], 0);
            return 0;
        case '*':
            //printf("reached\n");
            tmp = 1;
            fwrite(&tmp, 1, 1, info->rnext_same);
            //compress_uint8t(as, models->same_ref[0], 1);
            return 1;
        default:
            tmp = 2;
            fwrite(&tmp, 1, 1, info->rnext_same);
            //compress_uint8t(as, models->same_ref[0], 2);
            break;
    }


    while (*rnext) {
        strcpy(nm, "prev");
        sprintf(number, "%d", prevChar);
        strcat(nm, number);
        compress_uint8tt(as, models->rnext[prevChar], *rnext, &info->frnext_prev[prevChar], &info->rnext_prev[prevChar], nm);

        prev_name[ctr++] = *rnext;
        prevChar = *rnext++;
    }
    strcpy(nm, "prev");
    sprintf(number, "%d", prevChar);
    strcat(nm, number);
    compress_uint8tt(as, models->rnext[prevChar], 0, &info->frnext_prev[prevChar], &info->rnext_prev[prevChar], nm);
    prev_name[ctr] = 0;

    return 1;
}

int decompress_rnext(Arithmetic_stream as, rnext_models models, char *rnext, void *thread_info){

    static char prev_name[1024] = {0};
    static int prevChar = 0;

    uint8_t equal_flag = 0, ch;

    uint8_t tmp;
    char nm[40];
    char number[15];
    struct compressor_info_t *info = ((struct compressor_info_t *)thread_info);

    uint32_t ctr = 0;

    fread(&equal_flag, 1, 1, info->rnext_same);
    //equal_flag = decompress_uint8t(as, models->same_ref[0]);
//printf("%d\n", equal_flag);
    switch (equal_flag) {
        case 0:
            *rnext = '=', rnext++;
            *rnext = 0;
            return 0;
        case 1:
            *rnext = '*', rnext++;
            *rnext = 0;
            return 1;
        default:
            break;
    }

    strcpy(nm, "prev");
    sprintf(number, "%d", prevChar);
    strcat(nm, number);
    while ( (ch = decompress_uint8tt(as, models->rnext[prevChar], &info->frnext_prev[prevChar], &info->rnext_prev[prevChar], nm)) ) {

        *rnext = ch, rnext++;
        prev_name[ctr++] = ch;
        prevChar = ch;

        strcpy(nm, "prev");
        sprintf(number, "%d", prevChar);
        strcat(nm, number);
    }
    *rnext = 0;

    return 2;

}

int compress_pnext(Arithmetic_stream as, pnext_models models, uint32_t pos, int32_t tlen, uint32_t pnext, uint8_t rname_rnextDiff, char* cigar, void *thread_info){

    uint32_t p = 0, pn = 0, cig_op_len = 0, readLength = 0;

    if (rname_rnextDiff) {

        compress_uint8t(as, models->raw_pnext[0], (pnext >> 0) & 0xff);
        compress_uint8t(as, models->raw_pnext[1], (pnext >> 8) & 0xff);
        compress_uint8t(as, models->raw_pnext[2], (pnext >> 16) & 0xff);
        compress_uint8t(as, models->raw_pnext[3], (pnext >> 24) & 0xff);

        return 0;
    }

    if (tlen == 0) {
        if (pnext == pos) {
            compress_uint8t(as, models->assumption[0], 0);
        }

        else{
            compress_uint8t(as, models->assumption[0], 1);
            compress_pnext_raw(as, models, pos, pnext, thread_info);
        }

        return 0;
    }
    else if (tlen > 0){

        p = pos;
        pn = pnext;

    }

    else{
        p = pnext;
        pn = pos;
        tlen = -tlen;
    }

    while (*cigar) {
        cig_op_len = atoi(cigar);
        cigar += compute_num_digits(cig_op_len);
        if (*cigar == 'M' || *cigar == 'D' || *cigar == 'N') {
            readLength += cig_op_len;
        }
        ++cigar;
    }

    if (pn == tlen + p - readLength) {
        compress_uint8t(as, models->assumption[0], 0);
    }
    else{
        compress_uint8t(as, models->assumption[0], 1);
        compress_pnext_raw(as, models, p, pn, thread_info);
    }


    return 1;
}


int compress_pnext_raw(Arithmetic_stream as, pnext_models models, uint32_t pos, uint32_t pnext, void *thread_info){

    uint32_t delta = 0;
    uint8_t tmp;
    struct compressor_info_t *info = ((struct compressor_info_t *)thread_info);
    char nm[40];
    char number[15];


    if (pnext == 0) {
        tmp = 0;
        fwrite(&tmp, 1, 1, info->fpnext_zero);
        //compress_uint8t(as, models->zero[0], 0);
        return 0;
    }
    else{
        tmp = 1;
        fwrite(&tmp, 1, 1, info->fpnext_zero);
        //compress_uint8t(as, models->zero[0], 1);

        if (pnext > pos){

            delta = pnext - pos;
            tmp = 0;
            fwrite(&tmp, 1, 1, info->fpnext_sign);
            //compress_uint8t(as, models->sign[0], 0);
        }
        else {
            delta = pos - pnext;
            tmp = 1;
            fwrite(&tmp, 1, 1, info->fpnext_sign);
            //compress_uint8t(as, models->sign[0], 1);
        }

        strcpy(nm, "diff");
        sprintf(number, "%d", 0);
        strcat(nm, number);
        compress_uint8tt(as, models->diff_pnext[0], (delta >> 0) & 0xff, &info->fdiff_pnext[0], &info->idiff_pnext[0], nm);

        strcpy(nm, "diff");
        sprintf(number, "%d", 1);
        strcat(nm, number);
        compress_uint8tt(as, models->diff_pnext[1], (delta >> 8) & 0xff, &info->fdiff_pnext[1], &info->idiff_pnext[1], nm);

        strcpy(nm, "diff");
        sprintf(number, "%d", 2);
        strcat(nm, number);
        compress_uint8tt(as, models->diff_pnext[2], (delta >> 16) & 0xff, &info->fdiff_pnext[2], &info->idiff_pnext[2], nm);

        strcpy(nm, "diff");
        sprintf(number, "%d", 3);
        strcat(nm, number);
        compress_uint8tt(as, models->diff_pnext[3], (delta >> 24) & 0xff, &info->fdiff_pnext[3], &info->idiff_pnext[3], nm);
    }

    return 1;
}


int decompress_pnext(Arithmetic_stream as, pnext_models models, uint32_t pos, int32_t tlen, uint32_t readLength, uint32_t *pnext, uint8_t rname_rnextDiff, char* cigar, void *thread_info){

    uint32_t delta = 0, comp_flag = 0;

    uint8_t tmp;
    struct compressor_info_t *info = ((struct compressor_info_t *)thread_info);
    char nm[40];
    char number[15];

    fread(&tmp, 1, 1, info->fpnext_zero);
    comp_flag = ((uint32_t) tmp);
    //comp_flag = decompress_uint8t(as, models->zero[0]);

    if ( comp_flag == 0) {
        *pnext = 0;
        return 0;
    }
    else{
        fread(&tmp, 1, 1, info->fpnext_sign);
        comp_flag = ((uint32_t) tmp);
        //comp_flag = decompress_uint8t(as, models->sign[0]);

        strcpy(nm, "diff");
        sprintf(number, "%d", 0);
        strcat(nm, number);
        delta |= (decompress_uint8tt(as, models->diff_pnext[0], &info->fdiff_pnext[0], &info->idiff_pnext[0], nm) & 0xff) << 0;

        strcpy(nm, "diff");
        sprintf(number, "%d", 1);
        strcat(nm, number);
        delta |= (decompress_uint8tt(as, models->diff_pnext[1], &info->fdiff_pnext[1], &info->idiff_pnext[1], nm) & 0xff) << 8;

        strcpy(nm, "diff");
        sprintf(number, "%d", 2);
        strcat(nm, number);
        delta |= (decompress_uint8tt(as, models->diff_pnext[2], &info->fdiff_pnext[2], &info->idiff_pnext[2], nm) & 0xff) << 16;

        strcpy(nm, "diff");
        sprintf(number, "%d", 3);
        strcat(nm, number);
        delta |= (decompress_uint8tt(as, models->diff_pnext[3], &info->fdiff_pnext[3], &info->idiff_pnext[3], nm) & 0xff) << 24;

        if (comp_flag == 0){

            *pnext = delta + pos;
        }
        else {

            *pnext = pos - delta;
        }
    }

    return 1;
}

int compress_tlen(Arithmetic_stream as, tlen_models models, int32_t tlen, void *thread_info){

    int32_t delta = 0;

    struct compressor_info_t *info = ((struct compressor_info_t *)thread_info);
    uint8_t tmp;
    char nm[40];
    char number[15];

    strcpy(nm, "tlen");

    if (tlen == 0) {
        tmp = 0;
        fwrite(&tmp, 1, 1, info->ftlen_zero);
        //compress_uint8t(as, models->zero[0], 0);
        return 0;
    }
    else{
        tmp = 1;
        fwrite(&tmp, 1, 1, info->ftlen_zero);
        //compress_uint8t(as, models->zero[0], 1);

        if (tlen > 0){
            delta = tlen;
            tmp = 0;
            fwrite(&tmp, 1, 1, info->ftlen_sign);
            //compress_uint8t(as, models->sign[0], 0);
        }
        else {
            delta = -tlen;
            tmp = 1;
            fwrite(&tmp, 1, 1, info->ftlen_sign);
            //compress_uint8t(as, models->sign[0], 1);

        }

        strcpy(nm, "tlen");
        sprintf(number, "%d", 0);
        strcat(nm, number);
        if(info->itlen_tlen[0] == 0) {
            info->ftlen_tlen[0] = fopen(nm, "wb");
            info->itlen_tlen[0] = 1;
        }
        tmp = (delta >> 0) & 0xff;
        fwrite(&tmp, 1, 1, info->ftlen_tlen[0]);

        //compress_uint8t(as, models->tlen[0], (delta >> 0) & 0xff);

        strcpy(nm, "tlen");
        sprintf(number, "%d", 1);
        strcat(nm, number);
        if(info->itlen_tlen[1] == 0) {
            info->ftlen_tlen[1] = fopen(nm, "wb");
            info->itlen_tlen[1] = 1;
        }
        tmp = (delta >> 8) & 0xff;
        fwrite(&tmp, 1, 1, info->ftlen_tlen[1]);

        //compress_uint8t(as, models->tlen[1], (delta >> 8) & 0xff);

        strcpy(nm, "tlen");
        sprintf(number, "%d", 2);
        strcat(nm, number);
        if(info->itlen_tlen[2] == 0) {
            info->ftlen_tlen[2] = fopen(nm, "wb");
            info->itlen_tlen[2] = 1;
        }
        tmp = (delta >> 16) & 0xff;
        fwrite(&tmp, 1, 1, info->ftlen_tlen[2]);

        //compress_uint8t(as, models->tlen[2], (delta >> 16) & 0xff);

        strcpy(nm, "tlen");
        sprintf(number, "%d", 3);
        strcat(nm, number);
        if(info->itlen_tlen[3] == 0) {
            info->ftlen_tlen[3] = fopen(nm, "wb");
            info->itlen_tlen[3] = 1;
        }
        tmp = (delta >> 24) & 0xff;
        fwrite(&tmp, 1, 1, info->ftlen_tlen[3]);

        //compress_uint8t(as, models->tlen[3], (delta >> 24) & 0xff);
    }

    return 1;
}

int decompress_tlen(Arithmetic_stream as, tlen_models models, int32_t* tlen, void *thread_info){

    int32_t delta = 0;
    uint32_t decomp_flag = 0;

    struct compressor_info_t *info = ((struct compressor_info_t *)thread_info);
    uint8_t tmp;
    char nm[40];
    char number[15];

    fread(&tmp, 1, 1, info->ftlen_zero);
    decomp_flag = ((uint32_t) tmp);
    //decomp_flag = decompress_uint8t(as, models->zero[0]);

    if ( decomp_flag == 0) {
        *tlen = 0;
        return 0;
    }
    else{
        fread(&tmp, 1, 1, info->ftlen_sign);
        decomp_flag = ((uint32_t) tmp);
        //decomp_flag = decompress_uint8t(as, models->sign[0]);

        strcpy(nm, "tlen");
        sprintf(number, "%d", 0);
        strcat(nm, number);
        if(info->itlen_tlen[0] == 0) {
            info->ftlen_tlen[0] = fopen(nm, "rb");
            info->itlen_tlen[0] = 1;
        }
        fread(&tmp, 1, 1, info->ftlen_tlen[0]);
        delta |= (tmp & 0xff) << 0;

        strcpy(nm, "tlen");
        sprintf(number, "%d", 1);
        strcat(nm, number);
        if(info->itlen_tlen[1] == 0) {
            info->ftlen_tlen[1] = fopen(nm, "rb");
            info->itlen_tlen[1] = 1;
        }
        fread(&tmp, 1, 1, info->ftlen_tlen[1]);
        delta |= (tmp & 0xff) << 8;

        strcpy(nm, "tlen");
        sprintf(number, "%d", 2);
        strcat(nm, number);
        if(info->itlen_tlen[2] == 0) {
            info->ftlen_tlen[2] = fopen(nm, "rb");
            info->itlen_tlen[2] = 1;
        }
        fread(&tmp, 1, 1, info->ftlen_tlen[2]);
        delta |= (tmp & 0xff) << 16;

        strcpy(nm, "tlen");
        sprintf(number, "%d", 3);
        strcat(nm, number);
        if(info->itlen_tlen[3] == 0) {
            info->ftlen_tlen[3] = fopen(nm, "rb");
            info->itlen_tlen[3] = 1;
        }
        fread(&tmp, 1, 1, info->ftlen_tlen[3]);
        delta |= (tmp & 0xff) << 24;

        if (decomp_flag == 0)
            *tlen = delta;
        else
            *tlen = -delta;
    }

    return 1;
}



int compress_id(Arithmetic_stream as, id_models models, char *id, void *thread_info){

    static char prev_ID[1024] = {0};
    static uint32_t prev_tokens_ptr[1024] = {0};
    uint8_t token_len = 0, match_len = 0;
    uint32_t i = 0, k = 0, tmp = 0, token_ctr = 0, digit_value = 0, digit_model = 0, prev_digit = 0;
    int delta = 0;

    char *id_ptr = id, *id_ptr_tok = NULL;
    struct compressor_info_t *info = ((struct compressor_info_t *)thread_info);
    char nm[40];
    char number[15];

    while (*id_ptr != 0) {
      match_len += (*id_ptr == prev_ID[prev_tokens_ptr[token_ctr] + token_len]),
      token_len++;
id_ptr_tok = id_ptr + 1;

        // Check if the token is a alphabetic word
        if (isalpha(*id_ptr)) {

            while ( isalpha( *id_ptr_tok) ){

                // compare with the same token from previous ID
                match_len += (*id_ptr_tok == prev_ID[prev_tokens_ptr[token_ctr] + token_len]), token_len++, id_ptr_tok++;

            }
            if (match_len == token_len && !isalpha(prev_ID[prev_tokens_ptr[token_ctr] + token_len])) {
                // The token is the same as last ID
                // Encode a token_type ID_MATCH
                strcpy(nm, "token_type");
                sprintf(number, "%d", token_ctr);
                strcat(nm, number);
                compress_uint8tt(as, models->token_type[token_ctr], ID_MATCH, &info->ftoken_type[token_ctr], &info->itoken_type[token_ctr], nm);

            }
            else {
                // Encode a token type ID_ALPHA, the length of the string and the string
                strcpy(nm, "token_type");
                sprintf(number, "%d", token_ctr);
                strcat(nm, number);
                compress_uint8tt(as, models->token_type[token_ctr], ID_ALPHA, &info->ftoken_type[token_ctr], &info->itoken_type[token_ctr], nm);

                strcpy(nm, "alpha_len");
                sprintf(number, "%d", token_ctr);
                strcat(nm, number);
                compress_uint8tt(as, models->alpha_len[token_ctr], token_len, &info->falpha_len[token_ctr], &info->ialpha_len[token_ctr], nm);
                for (k = 0; k < token_len; k++) {
                    strcpy(nm, "alpha_value");
                    sprintf(number, "%d", token_ctr);
                    strcat(nm, number);
                    compress_uint8tt(as, models->alpha_value[token_ctr], *(id_ptr+k), &info->falpha_value[token_ctr], &info->ialpha_value[token_ctr], nm);
                }
            }

        }
        // check if the token is a run of zeros
        else if (*id_ptr == '0') {

            while ( *id_ptr_tok == '0' ){

                // compare with the same token from previous ID
                match_len += ('0' == prev_ID[prev_tokens_ptr[token_ctr] + token_len]), token_len++, id_ptr_tok++;

            }
            if (match_len == token_len && prev_ID[prev_tokens_ptr[token_ctr] + token_len] != '0') {
                // The token is the same as last ID
                // Encode a token_type ID_MATCH
                strcpy(nm, "token_type");
                sprintf(number, "%d", token_ctr);
                strcat(nm, number);
                compress_uint8tt(as, models->token_type[token_ctr], ID_MATCH, &info->ftoken_type[token_ctr], &info->itoken_type[token_ctr], nm);

            }
            else {
                // Encode a token type ID_ZEROS and the length of the zeros
                strcpy(nm, "token_type");
                sprintf(number, "%d", token_ctr);
                strcat(nm, number);
                compress_uint8tt(as, models->token_type[token_ctr], ID_ZEROS, &info->ftoken_type[token_ctr], &info->itoken_type[token_ctr], nm);

                strcpy(nm, "zero_run");
                sprintf(number, "%d", token_ctr);
                strcat(nm, number);
                compress_uint8tt(as, models->zero_run[token_ctr], token_len, &info->fzero_run[token_ctr], &info->izero_run[token_ctr], nm);
            }

        }
        // Check if the token is a number smaller than (1<<30)
        else if (isdigit(*id_ptr)) {

            digit_value = (*id_ptr - '0');
            bool prev_token_digit_flag =
          true;  // true if corresponding token in previous read is a digit
      if (*prev_ID != 0) {
        if (isdigit(prev_ID[prev_tokens_ptr[token_ctr] + token_len - 1]) &&
            prev_ID[prev_tokens_ptr[token_ctr] + token_len - 1] != '0') {
          prev_digit =
              prev_ID[prev_tokens_ptr[token_ctr] + token_len - 1] - '0';}
        else {
          prev_token_digit_flag = false;}
}

if (prev_token_digit_flag && *prev_ID != 0) {
tmp = 1;
while (isdigit(prev_ID[prev_tokens_ptr[token_ctr] + tmp]) &&
   prev_digit < (1 << 28)) {
prev_digit = prev_digit * 10 +
           (prev_ID[prev_tokens_ptr[token_ctr] + tmp] - '0');
tmp++;
}
}

while (isdigit(*id_ptr_tok) && digit_value < (1 << 28)) {
digit_value = digit_value * 10 + (*id_ptr_tok - '0');
// if (*prev_ID != 0){
//    prev_digit = prev_digit * 10 + (prev_ID[prev_tokens_ptr[token_ctr]
//    + token_len] - '0');
//}
// compare with the same token from previous ID
match_len +=
(*id_ptr_tok == prev_ID[prev_tokens_ptr[token_ctr] + token_len]),
token_len++, id_ptr_tok++;
}
            if (prev_token_digit_flag && match_len == token_len && !isdigit(prev_ID[prev_tokens_ptr[token_ctr] + token_len]) ) {
                // The token is the same as last ID
                // Encode a token_type ID_MATCH
                strcpy(nm, "token_type");
                sprintf(number, "%d", token_ctr);
                strcat(nm, number);
                compress_uint8tt(as, models->token_type[token_ctr], ID_MATCH, &info->ftoken_type[token_ctr], &info->itoken_type[token_ctr], nm);
            }
            else if (prev_token_digit_flag && (delta = (digit_value - prev_digit)) < 256 && delta > 0){
                strcpy(nm, "token_type");
                sprintf(number, "%d", token_ctr);
                strcat(nm, number);
                compress_uint8tt(as, models->token_type[token_ctr], ID_DELTA, &info->ftoken_type[token_ctr], &info->itoken_type[token_ctr], nm);

                strcpy(nm, "delta");
                sprintf(number, "%d", token_ctr);
                strcat(nm, number);
                compress_uint8tt(as, models->delta[token_ctr], delta, &info->fdelta[token_ctr], &info->idelta[token_ctr], nm);

            }
            else {
                // Encode a token type ID_DIGIT and the value (byte-based)
                strcpy(nm, "token_type");
                sprintf(number, "%d", token_ctr);
                strcat(nm, number);
                compress_uint8tt(as, models->token_type[token_ctr], ID_DIGIT, &info->ftoken_type[token_ctr], &info->itoken_type[token_ctr], nm);

                digit_model = (token_ctr << 2);

                strcpy(nm, "integer");
                sprintf(number, "%d", (digit_model | 0));
                strcat(nm, number);
                compress_uint8tt(as, models->integer[digit_model | 0], (digit_value >> 0) & 0xff, &info->finteger[digit_model | 0], &info->iinteger[digit_model | 0], nm);

                strcpy(nm, "integer");
                sprintf(number, "%d", (digit_model | 1));
                strcat(nm, number);
                compress_uint8tt(as, models->integer[digit_model | 1], (digit_value >> 8) & 0xff, &info->finteger[digit_model | 1], &info->iinteger[digit_model | 1], nm);

                strcpy(nm, "integer");
                sprintf(number, "%d", (digit_model | 2));
                strcat(nm, number);
                compress_uint8tt(as, models->integer[digit_model | 2], (digit_value >> 16) & 0xff, &info->finteger[digit_model | 2], &info->iinteger[digit_model | 2], nm);

                strcpy(nm, "integer");
                sprintf(number, "%d", (digit_model | 3));
                strcat(nm, number);
                compress_uint8tt(as, models->integer[digit_model | 3], (digit_value >> 24) & 0xff, &info->finteger[digit_model | 3], &info->iinteger[digit_model | 3], nm);
            }
        }
        else {

            // compare with the same token from previous ID
            //match_len += (*id_ptr == prev_ID[prev_tokens_ptr[token_ctr]]);

            if (match_len == token_len) {
                // The token is the same as last ID
                // Encode a token_type ID_MATCH
                strcpy(nm, "token_type");
                sprintf(number, "%d", token_ctr);
                strcat(nm, number);
                compress_uint8tt(as, models->token_type[token_ctr], ID_MATCH, &info->ftoken_type[token_ctr], &info->itoken_type[token_ctr], nm);

            }
            else {
                // Encode a token type ID_CHAR and the char
                strcpy(nm, "token_type");
                sprintf(number, "%d", token_ctr);
                strcat(nm, number);
                compress_uint8tt(as, models->token_type[token_ctr], ID_CHAR, &info->ftoken_type[token_ctr], &info->itoken_type[token_ctr], nm);

                strcpy(nm, "chars");
                sprintf(number, "%d", token_ctr);
                strcat(nm, number);
                compress_uint8tt(as, models->chars[token_ctr], *id_ptr, &info->fchars[token_ctr], &info->ichars[token_ctr], nm);
            }

        }

        prev_tokens_ptr[token_ctr] = i;
    i += token_len;
    id_ptr = id_ptr_tok;
    match_len = 0;
    token_len = 0;
token_ctr++;

    }
    strcpy(prev_ID, id);
    strcpy(nm, "token_type");
    sprintf(number, "%d", token_ctr);
    strcat(nm, number);
    compress_uint8tt(as, models->token_type[token_ctr], ID_END, &info->ftoken_type[token_ctr], &info->itoken_type[token_ctr], nm);

    return 1;
}

int decompress_rname(Arithmetic_stream as, rname_models models, char *rname, void *thread_info){

    static char prev_name[1024] = {0};
    static int prevChar = 0;
    struct compressor_info_t *info = (struct compressor_info_t *)thread_info;

    uint8_t chr_change = 0;
    char ch;
    char nm[35];
    char number[4];

    uint32_t ctr = 0;

    fread(&chr_change, 1, 1, info->fsame_ref);

    //chr_change = decompress_uint8t(as, models->same_ref[0]);

    if (chr_change) {
        strcpy(nm, "rname");
        sprintf(number, "%d", prevChar);
        strcat(nm, number);
        while ( (ch = decompress_uint8tt(as, models->rname[prevChar], &info->frname[prevChar], &info->irname[prevChar], nm)) ) {
            if (ch == '\n') {
                return -1;
            }
            prev_name[ctr++] = ch;
            prevChar = ch;
            *rname = ch, rname++;

            strcpy(nm, "rname");
            sprintf(number, "%d", prevChar);
            strcat(nm, number);
          }

        *rname = '\0';
    }

    return chr_change;

}




int decompress_id(Arithmetic_stream as, id_models model, char *id, void *thread_info){

    static char prev_ID[1024] = {0};
    static uint32_t prev_tokens_ptr[1024] = {0};
    static uint32_t prev_tokens_len[1024] = {0};
    //char id[1024] = {0};
    uint8_t token_len = 0;
    uint32_t i = 0, k = 0, token_ctr = 0, digit_value = 0;
    uint32_t delta = 0;

    enum token_type tok;

    struct compressor_info_t *info = ((struct compressor_info_t *)thread_info);
    char nm[40];
    char number[15];

    id[0]='\0';
    strcpy(nm, "token_type");
    sprintf(number, "%d", token_ctr);
    strcat(nm, number);
    while ( (tok = uint8t2token(decompress_uint8tt(as, model->token_type[token_ctr], &info->ftoken_type[token_ctr], &info->itoken_type[token_ctr], nm))) != ID_END ) {

        switch (tok) {
            case ID_MATCH:
                memcpy(id+i, &(prev_ID[prev_tokens_ptr[token_ctr]]), prev_tokens_len[token_ctr]);
                token_len = prev_tokens_len[token_ctr];
                break;
            case ID_ALPHA:
                strcpy(nm, "alpha_len");
                sprintf(number, "%d", token_ctr);
                strcat(nm, number);
                token_len = decompress_uint8tt(as, model->alpha_len[token_ctr], &info->falpha_len[token_ctr], &info->ialpha_len[token_ctr], nm);

                for (k = 0; k < token_len; k++) {
                    strcpy(nm, "alpha_value");
                    sprintf(number, "%d", token_ctr);
                    strcat(nm, number);
                    id[i+k] = decompress_uint8tt(as, model->alpha_value[token_ctr], &info->falpha_value[token_ctr], &info->ialpha_value[token_ctr], nm);
                }
                break;
            case ID_DIGIT:
                digit_value = 0;
                strcpy(nm, "integer");
                sprintf(number, "%d", ((token_ctr << 2) | 0));
                strcat(nm, number);
                digit_value |= (( decompress_uint8tt(as, model->integer[(token_ctr << 2) | 0], &info->finteger[(token_ctr << 2) | 0], &info->iinteger[(token_ctr << 2) | 0], nm) & 0xff ) << 0);

                strcpy(nm, "integer");
                sprintf(number, "%d", ((token_ctr << 2) | 1));
                strcat(nm, number);
                digit_value |= (( decompress_uint8tt(as, model->integer[(token_ctr << 2) | 1], &info->finteger[(token_ctr << 2) | 1], &info->iinteger[(token_ctr << 2) | 1], nm) & 0xff ) << 8);

                strcpy(nm, "integer");
                sprintf(number, "%d", ((token_ctr << 2) | 2));
                strcat(nm, number);
                digit_value |= (( decompress_uint8tt(as, model->integer[(token_ctr << 2) | 2], &info->finteger[(token_ctr << 2) | 2], &info->iinteger[(token_ctr << 2) | 2], nm) & 0xff ) << 16);

                strcpy(nm, "integer");
                sprintf(number, "%d", ((token_ctr << 2) | 3));
                strcat(nm, number);
                digit_value |= (( decompress_uint8tt(as, model->integer[(token_ctr << 2) | 3], &info->finteger[(token_ctr << 2) | 3], &info->iinteger[(token_ctr << 2) | 3], nm) & 0xff ) << 24);

                sprintf(id+i, "%u", digit_value);
                token_len = compute_num_digits(digit_value);
                break;
            case ID_DELTA:
                digit_value = 0;
                strcpy(nm, "delta");
                sprintf(number, "%d", token_ctr);
                strcat(nm, number);
                delta = decompress_uint8tt(as, model->delta[token_ctr], &info->fdelta[token_ctr], &info->idelta[token_ctr], nm);

                memcpy(id+i, &(prev_ID[prev_tokens_ptr[token_ctr]]), prev_tokens_len[token_ctr]);
                id[i+prev_tokens_len[token_ctr]] = '\0';
                digit_value = atoi(id+i) + delta;
                sprintf(id+i, "%u", digit_value);
                token_len = compute_num_digits(digit_value);
                break;
            case ID_ZEROS:
                strcpy(nm, "zero_run");
                sprintf(number, "%d", token_ctr);
                strcat(nm, number);
                token_len = decompress_uint8tt(as, model->zero_run[token_ctr], &info->fzero_run[token_ctr], &info->izero_run[token_ctr], nm);

                memset(id+i, '0', token_len);
                break;
            case ID_CHAR:
                strcpy(nm, "chars");
                sprintf(number, "%d", token_ctr);
                strcat(nm, number);
                id[i] = decompress_uint8tt(as, model->chars[token_ctr], &info->fchars[token_ctr], &info->ichars[token_ctr], nm);

                token_len = 1;
                break;
            default:
                break;
        }

        prev_tokens_ptr[token_ctr] = i;
        prev_tokens_len[token_ctr] = token_len;
        i += token_len;
        id[i]='\0';
        token_len = 0;
        token_ctr++;

        strcpy(nm, "token_type");
        sprintf(number, "%d", token_ctr);
        strcat(nm, number);
    }
    id[i]='\0';
    strcpy(prev_ID, id);

    return 1;
}

int compress_aux(Arithmetic_stream as, aux_models models, char **aux_str, uint8_t aux_cnt, aux_block aux)
{
    //We first compress the no. of aux fields
    compress_uint8t(as, models->qAux[0], aux_cnt);

    char *ptr, *ptr_data;
    uint8_t mappedChar1, mappedChar2, mappedType, rawType;

    uint32_t desc_length;

    int32_t i,j,k; char *auxPtr;

    uint8_t it;
    uint16_t numericTagType;
    uint8_t most_common_token; //en el resto definido como 32... hay que cambiar.
    for(it = 0; it<aux_cnt; it++) {
        ptr = aux_str[it];

        most_common_token = get_most_common_token(aux->most_common, aux->most_common_size, ptr);
        if (most_common_token != aux->most_common_size)
        {
            //aux field found in most common list, flag = 1.
            compress_uint8t(as, models->most_common_flag[0], 1);

            //send token
            compress_uint8t(as, models->most_common_values[0], most_common_token);

            //globalCnt++;
            //if(globalCnt>400000)printf("%d\n",globalCnt);
            continue;
        }
        //continue;

        //Aux field not found in most common list, flag = 0.
        compress_uint8t(as, models->most_common_flag[0], 0);

        numericTagType = preprocessTagType(ptr, &rawType);
        if (numericTagType & NOTFOUNDLUTFLAG) {
            //TagType not in LUT, send LUT flag = 1, then values.
            compress_uint8t(as, models->tagtypeLUTflag[0], 1);
            mappedChar1 = (numericTagType & MASKTAGCHAR1) >> 9;
            mappedChar2 = (numericTagType & MASKTAGCHAR2) >> 3;
            mappedType = numericTagType & MASKTYPE;

            compress_uint8t(as,models->tag[0],mappedChar1);
            compress_uint8t(as,models->tag[1],mappedChar2);

            if(mappedType == TYPELUTLENGTH) {
                //Unknown type (not in typeLUT, very unlikely, is it even possible?) send flag 1, then ascii value (rawType).
                compress_uint8t(as, models->typeLUTflag[0], 1);
                compress_uint8t(as,models->typeRAW[0],rawType);
            } else {
                //Type in typeLUT, send flag 0, then mapped type value.
                compress_uint8t(as, models->typeLUTflag[0], 0);
                compress_uint8t(as,models->typeLUT[0],mappedType);
            }
        } else {
            //TagType in tagtypeLUT, send LUTflag = 0, then value.
            compress_uint8t(as, models->tagtypeLUTflag[0], 0);
            compress_uint8t(as, models->tagtypeLUT[0], numericTagType & 0xFF);
        }

        //We compress the actual data (1st approach: byte per byte)
        ptr_data = ptr + 5;

        //Crear enum o algo cuando esta tabla este lista:
        //TIPOS:
        //0: int,
        //1: resto.

        //cambiar a switch
        if(ptr[3]=='i') {
            globalCnt++;
            i = strlen(ptr);
            auxPtr = ptr+5;
            //for(j=5;j<i;j++) printf("%c",ptr[j]);
            k=atoi(auxPtr);

            uint8_t first_byte = (k >> 24) & UINT8_MAX;
            uint8_t second_byte = (k >> 16) & UINT8_MAX;
            uint8_t third_byte = (k >> 8) & UINT8_MAX;
            uint8_t fourth_byte = (k) & UINT8_MAX;
            compress_uint8t(as, models->integers[0], first_byte);
            compress_uint8t(as, models->integers[1], second_byte);
            compress_uint8t(as, models->integers[2], third_byte);
            compress_uint8t(as, models->integers[3], fourth_byte);
        } else {
            desc_length = strlen(ptr_data);
            if(desc_length>=UINT16_MAX) {
              desc_length=UINT16_MAX;
            }

            uint8_t first_byte = (desc_length >> 8) & UINT8_MAX;
            uint8_t second_byte = (desc_length) & UINT8_MAX;

            compress_uint8t(as, models->descBytes[0], first_byte);
            compress_uint8t(as, models->descBytes[1], second_byte);

            uint8_t buff_cnt = 0;
            while (buff_cnt < UINT16_MAX && *ptr_data!=0) {
                compress_uint8t(as, models->iidBytes[0], *ptr_data);
                ptr_data++;
                buff_cnt++;
            }
        }

    }

    return 1;
}

int decompress_aux(Arithmetic_stream as, aux_block aux, char* finalLine)
{
    //
    uint8_t aux_fields;

    uint8_t it, most_common_flag, most_common_token;
    uint8_t tagtypeLUTflag, typeLUTflag, tagtypeLUTindex, mappedChar1, mappedChar2;
    char tagChar1, tagChar2, typeChar;
    uint16_t desc_length = 0;
    char auxFieldString[MAX_AUX_LENGTH] = {0};

    char* finalLine_ptr = finalLine;

    aux_models models = aux->models;

    aux_fields = decompress_uint8t(as, models->qAux[0]);

    for(it = 0; it<aux_fields; it++) {

        most_common_flag = decompress_uint8t(as, models->most_common_flag[0]);

        if(most_common_flag == 1) {
            //The aux field is in the most common list.
            most_common_token = decompress_uint8t(as, models->most_common_values[0]);
            strcpy(finalLine_ptr,aux->most_common[most_common_token]);
            finalLine_ptr += strlen(aux->most_common[most_common_token]);
            *finalLine_ptr = '\t';
            finalLine_ptr++;
            continue;
        }
        //tag & type dec.
        tagtypeLUTflag = decompress_uint8t(as, models->tagtypeLUTflag[0]);

        if(tagtypeLUTflag==1) {
            mappedChar1 = decompress_uint8t(as,models->tag[0]);
            mappedChar2 = decompress_uint8t(as,models->tag[1]);
            tagChar1 = inverseCharMap(mappedChar1);
            tagChar2 = inverseCharMap(mappedChar2);

            typeLUTflag = decompress_uint8t(as, models->typeLUTflag[0]);
            if(typeLUTflag==1) {
                typeChar = decompress_uint8t(as,models->typeRAW[0]);
            } else {
                typeChar = typeLUT[decompress_uint8t(as,models->typeLUT[0])];
            }
        } else {
            tagtypeLUTindex = decompress_uint8t(as, models->tagtypeLUT[0]);
            tagChar1 = tagTypeLUT[tagtypeLUTindex][0];
            tagChar2 = tagTypeLUT[tagtypeLUTindex][1];
            typeChar = tagTypeLUT[tagtypeLUTindex][2];
        }


        //cambiar a switch
        uint8_t sign; int value;

        char buffer[MAX_AUX_LENGTH] = {0};
        uint16_t buff_cnt;

        if(typeChar == 'i') {
            value = decompress_uint8t(as, models->integers[0]) << 24; //alerta (ver analogo en comp)
            value |= decompress_uint8t(as, models->integers[1]) << 16;
            value |= decompress_uint8t(as, models->integers[2]) << 8;
            value |= decompress_uint8t(as, models->integers[3]);
            sprintf(buffer,"%d",value);
            desc_length = strlen(buffer);
            buffer[desc_length] = '\t';
        } else {
            //value dec.
            desc_length = decompress_uint8t(as, models->descBytes[0]) << 8;
            desc_length |= decompress_uint8t(as, models->descBytes[1]);
            for (buff_cnt=0;buff_cnt<desc_length;buff_cnt++) {
                buffer[buff_cnt] = decompress_uint8t(as, models->iidBytes[0]);
            }
            buffer[buff_cnt] = '\t';
        }

        // recomp.
        auxFieldString[0] = tagChar1;
        auxFieldString[1] = tagChar2;
        auxFieldString[2] = ':';
        auxFieldString[3] = typeChar;
        auxFieldString[4] = ':';

        char* str_pnt;
        str_pnt = auxFieldString+5;
        strcpy(str_pnt,buffer);

        strcpy(finalLine_ptr,auxFieldString);

        finalLine_ptr += 5+desc_length+1; // tag:type:desc\t

    }

    *(finalLine_ptr-1) = 0; //We dont want the last tab

    return 1;

}




int compress_aux_idoia(Arithmetic_stream as, aux_models models, char **aux_str, uint8_t aux_cnt, aux_block aux, void *thread_info)
{

    char *ptr, *ptr_data;
    uint8_t mappedChar1, mappedChar2, mappedType, rawType;

    uint32_t desc_length;

    int i,j,k; char *auxPtr;

    uint8_t it;
    uint16_t numericTagType, prev_numericTagType, numericTagType_notfound;

    uint8_t most_common_token; //en el resto definido como 32... hay que cambiar.

    uint8_t use_mostCommonList;

    struct compressor_info_t *info = ((struct compressor_info_t *)thread_info);
    uint8_t tmp;
    char nm[40];
    char number[15];

    static char tagTypeLUT_update[MAXLUT][4];
    static uint8_t num_auxTypes = TAGTYPELUTLENGTH;
    static uint8_t firstline = 1;

    if (firstline == 1){
        for (i=0;i<TAGTYPELUTLENGTH;i++){
            for (j=0;j<4;j++){
                tagTypeLUT_update[i][j] = tagTypeLUT[i][j];
            }
        }
        firstline = 0;
    }

    use_mostCommonList = 1;

    prev_numericTagType = 0; //first time we use context 0, which is the one for end of line.
    for(it = 0; it<aux_cnt; it++) {
        ptr = aux_str[it];

        if (use_mostCommonList == 1){

            // MOST COMMON LIST
            most_common_token = get_most_common_token(aux->most_common, aux->most_common_size, ptr);

            if (most_common_token != aux->most_common_size)
            {
                tmp = 1;
                fwrite(&tmp, 1, 1, info->fmost_common_flag);
                //aux field found in most common list, flag = 1.
                //compress_uint8t(as, models->most_common_flag[0], 1);

                //send token
                //compress_uint8t(as, models->most_common_values[0], most_common_token);
                strcpy(nm, "com");
                sprintf(number, "%d", prev_numericTagType);
                strcat(nm, number);
                compress_uint8tt(as, models->most_common_values_wContext[prev_numericTagType], most_common_token, &info->fmost_common_values_wContext[prev_numericTagType], &info->imost_common_values_wContext[prev_numericTagType], nm);


                //globalCnt++;
                //if(globalCnt>400000)printf("%d\n",globalCnt);

                // STILL NEED TO CHECK THE numericTagType, for next aux fields
                numericTagType = preprocessTagType_update(ptr, &rawType, tagTypeLUT_update, num_auxTypes);
                // numericTagType = preprocessTagType(ptr, &rawType);

                if (numericTagType & NOTFOUNDLUTFLAG){
                    prev_numericTagType = 1;
                    // update num_auxTypes and tagTypeLUT_update
                    if (num_auxTypes < MAXLUT){
                        tagTypeLUT_update[num_auxTypes][0] = ptr[0];
                        tagTypeLUT_update[num_auxTypes][1] = ptr[1];
                        tagTypeLUT_update[num_auxTypes][2] = ptr[3];
                        tagTypeLUT_update[num_auxTypes][3] = 0;
                        prev_numericTagType = num_auxTypes + 2;
                        num_auxTypes = num_auxTypes + 1;
                    }
                }else{
                    prev_numericTagType = numericTagType + 2;
                }
                continue;
            }
            tmp = 0;
            fwrite(&tmp, 1, 1, info->fmost_common_flag);
            //compress_uint8t(as, models->most_common_flag[0], 0);
        }

        // Check if the TagType is in the LUT
        //numericTagType = preprocessTagType(ptr, &rawType);
        numericTagType = preprocessTagType_update(ptr, &rawType, tagTypeLUT_update, num_auxTypes);

        if (numericTagType & NOTFOUNDLUTFLAG) {
            //TagType not in LUT, encode pos 1
            numericTagType_notfound = numericTagType;
            numericTagType = 1;
            strcpy(nm, "tag");
            sprintf(number, "%d", prev_numericTagType);
            strcat(nm, number);
            compress_uint8tt(as, models->aux_TagType[prev_numericTagType], numericTagType & 0xFF, &info->faux_TagType[prev_numericTagType], &info->iaux_TagType[prev_numericTagType], nm);

            //TagType not in LUT, send LUT flag = 1, then values.
            //compress_uint8t(as, models->tagtypeLUTflag[0], 1);

            // Since it is not in LUT, need to send values of TAG and TYPE explicitly
            mappedChar1 = (numericTagType_notfound & MASKTAGCHAR1) >> 9;
            mappedChar2 = (numericTagType_notfound & MASKTAGCHAR2) >> 3;
            mappedType = numericTagType_notfound & MASKTYPE;

            strcpy(nm, "tg");
            sprintf(number, "%d", 0);
            strcat(nm, number);
            compress_uint8tt(as,models->tag[0],mappedChar1, &info->ftag[0], &info->itag[0], nm);

            strcpy(nm, "tg");
            sprintf(number, "%d", 1);
            strcat(nm, number);
            compress_uint8tt(as,models->tag[1],mappedChar2, &info->ftag[1], &info->itag[1], nm);

            if(mappedType == TYPELUTLENGTH) {
                //Unknown type (not in typeLUT, very unlikely, is it even possible?) send flag 1, then ascii value (rawType).
                tmp = 1;
                fwrite(&tmp, 1, 1, info->ftypeLUTflag);
                //compress_uint8t(as, models->typeLUTflag[0], 1);

                fwrite(&rawType, 1, 1, info->ftypeRAW);
                //compress_uint8t(as,models->typeRAW[0],rawType);
            } else {
                //Type in typeLUT, send flag 0, then mapped type value.
                tmp = 0;
                fwrite(&tmp, 1, 1, info->ftypeLUTflag);
                //compress_uint8t(as, models->typeLUTflag[0], 0);

                fwrite(&mappedType, 1, 1, info->ftypeLUT);
                //compress_uint8t(as,models->typeLUT[0],mappedType);
            }

            // update num_auxTypes and tagTypeLUT_update
            if (num_auxTypes < MAXLUT){
                tagTypeLUT_update[num_auxTypes][0] = ptr[0];
                tagTypeLUT_update[num_auxTypes][1] = ptr[1];
                tagTypeLUT_update[num_auxTypes][2] = ptr[3];
                tagTypeLUT_update[num_auxTypes][3] = 0;
                numericTagType = num_auxTypes + 2;
                num_auxTypes = num_auxTypes + 1;
            }

        } else {
            // TagType in LUT
            numericTagType = numericTagType + 2; // Remember 0 for end of line, and 1 for not found
            strcpy(nm, "tag");
            sprintf(number, "%d", prev_numericTagType);
            strcat(nm, number);
            compress_uint8tt(as, models->aux_TagType[prev_numericTagType], numericTagType & 0xFF, &info->faux_TagType[prev_numericTagType], &info->iaux_TagType[prev_numericTagType], nm);
        }
        prev_numericTagType = numericTagType;

        //We compress the actual data (1st approach: byte per byte)
        ptr_data = ptr + 5;

        //Crear enum o algo cuando esta tabla este lista:
        //TIPOS:
        //0: int,
        //1: resto.

        //cambiar a switch
        if(ptr[3]=='i') {//printf("integer: %s\n", ptr_data);
            globalCnt++;
            i = strlen(ptr);
            auxPtr = ptr+5;
            k=atoi(auxPtr);


            k=atoi(auxPtr);


            // new from geneComp
            uint8_t first_byte = (k >> 24) & UINT8_MAX;
            uint8_t second_byte = (k >> 16) & UINT8_MAX;
            uint8_t third_byte = (k >> 8) & UINT8_MAX;
            uint8_t fourth_byte = (k) & UINT8_MAX;

            strcpy(nm, "wcon");
            sprintf(number, "%d", prev_numericTagType);
            strcat(nm, number);
            compress_uint8tt(as, models->integers_wContext[prev_numericTagType], first_byte, &info->fintegers_wContext[prev_numericTagType], &info->iintegers_wContext[prev_numericTagType], nm);

            strcpy(nm, "wcon");
            sprintf(number, "%d", (MAXLUT+2)+prev_numericTagType);
            strcat(nm, number);
            compress_uint8tt(as, models->integers_wContext[(MAXLUT+2)+prev_numericTagType], second_byte, &info->fintegers_wContext[(MAXLUT+2)+prev_numericTagType], &info->iintegers_wContext[(MAXLUT+2)+prev_numericTagType], nm);

            strcpy(nm, "wcon");
            sprintf(number, "%d", 2*(MAXLUT+2)+prev_numericTagType);
            strcat(nm, number);
            compress_uint8tt(as, models->integers_wContext[2*(MAXLUT+2)+prev_numericTagType], third_byte, &info->fintegers_wContext[2*(MAXLUT+2)+prev_numericTagType], &info->iintegers_wContext[2*(MAXLUT+2)+prev_numericTagType], nm);

            strcpy(nm, "wcon");
            sprintf(number, "%d", 3*(MAXLUT+2)+prev_numericTagType);
            strcat(nm, number);
            compress_uint8tt(as, models->integers_wContext[3*(MAXLUT+2)+prev_numericTagType], fourth_byte, &info->fintegers_wContext[3*(MAXLUT+2)+prev_numericTagType], &info->iintegers_wContext[3*(MAXLUT+2)+prev_numericTagType], nm);
        } else {
   //printf("des: %s\n", ptr_data);
            // NEw GeneComp
            desc_length = strlen(ptr_data);
            if(desc_length>=UINT16_MAX) {
                desc_length=UINT16_MAX;
            }

            uint8_t first_byte = (desc_length >> 8) & UINT8_MAX;
            uint8_t second_byte = (desc_length) & UINT8_MAX;

            strcpy(nm, "des");
            sprintf(number, "%d", prev_numericTagType);
            strcat(nm, number);
            compress_uint8tt(as, models->descBytes_wContext[prev_numericTagType], first_byte, &info->fdescBytes_wContext[prev_numericTagType], &info->idescBytes_wContext[prev_numericTagType], nm);

            strcpy(nm, "des");
            sprintf(number, "%d", (MAXLUT + 2) + prev_numericTagType);
            strcat(nm, number);
            compress_uint8tt(as, models->descBytes_wContext[(MAXLUT + 2) + prev_numericTagType], second_byte, &info->fdescBytes_wContext[(MAXLUT + 2) + prev_numericTagType], &info->idescBytes_wContext[(MAXLUT + 2) + prev_numericTagType], nm);

            uint32_t buff_cnt = 0;
            while (buff_cnt < UINT16_MAX && *ptr_data!=0) {
                strcpy(nm, "iid");
                sprintf(number, "%d", prev_numericTagType);
                strcat(nm, number);
                compress_uint8tt(as, models->iidBytes_wContext[prev_numericTagType], *ptr_data, &info->fiidBytes_wContext[prev_numericTagType], &info->iiidBytes_wContext[prev_numericTagType], nm);
                ptr_data++;
                buff_cnt++;
            }
        }

    }

    // Now we need to indicate end of AUX fields
    numericTagType = 0;
    if (use_mostCommonList == 1){
        tmp = 0;
        fwrite(&tmp, 1, 1, info->fmost_common_flag);
        //compress_uint8t(as, models->most_common_flag[0], 0);
    }
    strcpy(nm, "tag");
    sprintf(number, "%d", prev_numericTagType);
    strcat(nm, number);
    compress_uint8tt(as, models->aux_TagType[prev_numericTagType], numericTagType & 0xFF, &info->faux_TagType[prev_numericTagType], &info->iaux_TagType[prev_numericTagType], nm);

    return 1;
}



int decompress_aux_idoia(Arithmetic_stream as, aux_block aux, char* finalLine, void *thread_info)
{
    uint8_t aux_fields;

    struct compressor_info_t *info = ((struct compressor_info_t *)thread_info);
    uint8_t tmp;
    char nm[40];
    char number[15];

    uint8_t it, most_common_flag, most_common_token;
    uint8_t tagtypeLUTflag, typeLUTflag, tagtypeLUTindex, mappedChar1, mappedChar2, prev_tagtypeLUTindex;
    char tagChar1, tagChar2, typeChar;
    uint32_t desc_length;
    char auxFieldString[MAX_AUX_LENGTH] = {0};

    uint8_t rawType;

    char* finalLine_ptr = finalLine;

    uint16_t tagtypeLUTindex_16;

    aux_models models = aux->models;



    //aux_fields = decompress_uint8t(as, models->qAux[0]);

    uint8_t moreAux = 1;

    uint8_t use_mostCommonList;

    int i,j;
    static char tagTypeLUT_update[MAXLUT][4];
    static uint8_t num_auxTypes = TAGTYPELUTLENGTH;
    static uint8_t firstline = 1;

    if (firstline == 1){
        for (i=0;i<TAGTYPELUTLENGTH;i++){
            for (j=0;j<4;j++){
                tagTypeLUT_update[i][j] = tagTypeLUT[i][j];
            }
        }
        firstline = 0;
    }



    use_mostCommonList  = 1;

    prev_tagtypeLUTindex = 0;

    while(moreAux){

        if (use_mostCommonList == 1){
            //First check if in most common list
            fread(&most_common_flag, 1, 1, info->fmost_common_flag);
            //most_common_flag = decompress_uint8t(as, models->most_common_flag[0]);

            if(most_common_flag == 1) {
                //The aux field is in the most common list.
                strcpy(nm, "com");
                sprintf(number, "%d", prev_tagtypeLUTindex);
                strcat(nm, number);
                most_common_token = decompress_uint8tt(as, models->most_common_values_wContext[prev_tagtypeLUTindex], &info->fmost_common_values_wContext[prev_tagtypeLUTindex], &info->imost_common_values_wContext[prev_tagtypeLUTindex], nm);
                strcpy(finalLine_ptr,aux->most_common[most_common_token]);

                finalLine_ptr += strlen(aux->most_common[most_common_token]);
                *finalLine_ptr = '\t';
                finalLine_ptr++;


                // STILL NEED TO CHECK THE numericTagType, for next aux fields
                tagtypeLUTindex_16 = preprocessTagType_update(aux->most_common[most_common_token], &rawType, tagTypeLUT_update, num_auxTypes);
                if (tagtypeLUTindex_16 & NOTFOUNDLUTFLAG){
                    prev_tagtypeLUTindex = 1;
                    // update num_auxTypes and tagTypeLUT_update
                    if (num_auxTypes < MAXLUT){
                        tagTypeLUT_update[num_auxTypes][0] = aux->most_common[most_common_token][0];
                        tagTypeLUT_update[num_auxTypes][1] = aux->most_common[most_common_token][1];
                        tagTypeLUT_update[num_auxTypes][2] = aux->most_common[most_common_token][3];
                        tagTypeLUT_update[num_auxTypes][3] = 0;
                        prev_tagtypeLUTindex = num_auxTypes + 2;
                        num_auxTypes = num_auxTypes + 1;
                    }
                }else{
                    prev_tagtypeLUTindex = tagtypeLUTindex_16 + 2;
                }

                continue;
            }
        }


        // Decompress LUTindex + 2
        strcpy(nm, "tag");
        sprintf(number, "%d", prev_tagtypeLUTindex);
        strcat(nm, number);
        tagtypeLUTindex = decompress_uint8tt(as, models->aux_TagType[prev_tagtypeLUTindex], &info->faux_TagType[prev_tagtypeLUTindex], &info->iaux_TagType[prev_tagtypeLUTindex], nm);
        prev_tagtypeLUTindex = tagtypeLUTindex;
        // Now 3 cases; 0: no more AUX fields 1: TagType not found in LUT else: TagType found in LUT
        if (tagtypeLUTindex == 0){
            moreAux = 0;
            break;
        }else if(tagtypeLUTindex == 1){
            strcpy(nm, "tg0");
            mappedChar1 = decompress_uint8tt(as,models->tag[0], &info->ftag[0], &info->itag[0], nm);
            strcpy(nm, "tg1");
            mappedChar2 = decompress_uint8tt(as,models->tag[1], &info->ftag[1], &info->itag[1], nm);

            tagChar1 = inverseCharMap(mappedChar1);
            tagChar2 = inverseCharMap(mappedChar2);

            fread(&typeLUTflag, 1, 1, info->ftypeLUTflag);
            //typeLUTflag = decompress_uint8t(as, models->typeLUTflag[0]);

            if(typeLUTflag==1) {
                fread(&typeChar, 1, 1, info->ftypeRAW);
                //printf("raw: %c\n",typeChar);
                //typeChar = decompress_uint8t(as,models->typeRAW[0]);
            } else {
                fread(&typeChar, 1, 1, info->ftypeLUT);
                //printf("lut: %c\n",typeChar);
                typeChar = typeLUT[typeChar];
            }

            // update num_auxTypes and tagTypeLUT_update
            if (num_auxTypes < MAXLUT){
                tagTypeLUT_update[num_auxTypes][0] = tagChar1;
                tagTypeLUT_update[num_auxTypes][1] = tagChar2;
                tagTypeLUT_update[num_auxTypes][2] = typeChar;
                tagTypeLUT_update[num_auxTypes][3] = 0;
                prev_tagtypeLUTindex = num_auxTypes + 2;
                num_auxTypes = num_auxTypes + 1;
            }
        }else{
            tagtypeLUTindex = tagtypeLUTindex - 2;
            tagChar1 = tagTypeLUT_update[tagtypeLUTindex][0];
            tagChar2 = tagTypeLUT_update[tagtypeLUTindex][1];
            typeChar = tagTypeLUT_update[tagtypeLUTindex][2];
        }

        // Decode values if moreAux = 1
        if (moreAux ==1){

            //cambiar a switch
            uint8_t sign; int value;

            char buffer[MAX_AUX_LENGTH] = {0};

            uint16_t buff_cnt;


            if(typeChar == 'i') {

                //NEW
                strcpy(nm, "wcon");
                sprintf(number, "%d", prev_tagtypeLUTindex);
                strcat(nm, number);
                value = decompress_uint8tt(as, models->integers_wContext[prev_tagtypeLUTindex], &info->fintegers_wContext[prev_tagtypeLUTindex], &info->iintegers_wContext[prev_tagtypeLUTindex], nm) << 24; //alerta (ver analogo en comp)

                strcpy(nm, "wcon");
                sprintf(number, "%d", (MAXLUT + 2) + prev_tagtypeLUTindex);
                strcat(nm, number);
                value |= decompress_uint8tt(as, models->integers_wContext[(MAXLUT + 2) + prev_tagtypeLUTindex], &info->fintegers_wContext[(MAXLUT + 2) + prev_tagtypeLUTindex], &info->iintegers_wContext[(MAXLUT + 2) + prev_tagtypeLUTindex], nm) << 16;

                strcpy(nm, "wcon");
                sprintf(number, "%d", 2*(MAXLUT + 2) + prev_tagtypeLUTindex);
                strcat(nm, number);
                value |= decompress_uint8tt(as, models->integers_wContext[2*(MAXLUT + 2) + prev_tagtypeLUTindex], &info->fintegers_wContext[2*(MAXLUT + 2) + prev_tagtypeLUTindex], &info->iintegers_wContext[2*(MAXLUT + 2) + prev_tagtypeLUTindex], nm) << 8;

                strcpy(nm, "wcon");
                sprintf(number, "%d", 3*(MAXLUT + 2) + prev_tagtypeLUTindex);
                strcat(nm, number);
                value |= decompress_uint8tt(as, models->integers_wContext[3*(MAXLUT + 2) + prev_tagtypeLUTindex], &info->fintegers_wContext[3*(MAXLUT + 2) + prev_tagtypeLUTindex], &info->iintegers_wContext[3*(MAXLUT + 2) + prev_tagtypeLUTindex], nm);

                sprintf(buffer,"%d",value);
                desc_length = strlen(buffer);
                buffer[desc_length] = '\t';
                //printf("integer: %s\n", buffer);
            } else {
                //value dec.
                strcpy(nm, "des");
                sprintf(number, "%d", prev_tagtypeLUTindex);
                strcat(nm, number);
                desc_length = decompress_uint8tt(as, models->descBytes_wContext[prev_tagtypeLUTindex], &info->fdescBytes_wContext[prev_tagtypeLUTindex], &info->idescBytes_wContext[prev_tagtypeLUTindex], nm) << 8;

                strcpy(nm, "des");
                sprintf(number, "%d", (MAXLUT + 2) + prev_tagtypeLUTindex);
                strcat(nm, number);
                desc_length |= decompress_uint8tt(as, models->descBytes_wContext[(MAXLUT + 2) + prev_tagtypeLUTindex], &info->fdescBytes_wContext[(MAXLUT + 2) + prev_tagtypeLUTindex], &info->idescBytes_wContext[(MAXLUT + 2) + prev_tagtypeLUTindex], nm);


                for (buff_cnt=0;buff_cnt<desc_length;buff_cnt++) {
                    strcpy(nm, "iid");
                    sprintf(number, "%d", prev_tagtypeLUTindex);
                    strcat(nm, number);
                    buffer[buff_cnt] = decompress_uint8tt(as, models->iidBytes_wContext[prev_tagtypeLUTindex], &info->fiidBytes_wContext[prev_tagtypeLUTindex], &info->iiidBytes_wContext[prev_tagtypeLUTindex], nm);
                }
                buffer[buff_cnt] = '\t';
                //printf("des: %s\n", buffer);
            }

            // recomp.
            auxFieldString[0] = tagChar1;
            auxFieldString[1] = tagChar2;
            auxFieldString[2] = ':';
            auxFieldString[3] = typeChar;
            auxFieldString[4] = ':';

            char* str_pnt;
            str_pnt = auxFieldString+5;
            strcpy(str_pnt,buffer);

            strcpy(finalLine_ptr,auxFieldString);

            finalLine_ptr += 5+desc_length+1; // tag:type:desc\t

        }

    }

    *(finalLine_ptr-1) = 0; //We dont want the last tab

    return 1;

}








int compress_cigar(Arithmetic_stream as, read_models models, char *cigar, uint8_t cigarFlags, void *thread_info) {

    //We check if the cigarFlags == 1. If so, then with the indels is enough to recover the cigar.

    //Otherwise, we compress the whole cigar.
    uint8_t cigar_length;
    cigar_length = strlen(cigar);

    struct compressor_info_t *info = ((struct compressor_info_t *)thread_info);
    uint8_t tmp;

    if(cigarFlags==1) {
        tmp = 1;
        fwrite(&tmp, 1, 1, info->fcigarFlags);
        //compress_uint8t(as, models->cigarFlags[0], 1);
    } else {
        tmp = 0;
        fwrite(&tmp, 1, 1, info->fcigarFlags);
        //compress_uint8t(as, models->cigarFlags[0], 0);

        fwrite(&cigar_length, 1, 1, info->cigar);
        //compress_uint8t(as, models->cigar[0],cigar_length);
        while(*cigar) {
            fwrite(cigar, 1, 1, info->cigar);
            //compress_uint8t(as, models->cigar[0],*cigar);
            cigar++;
        }
    }

    return 1;
}

int compress_int32tt(Arithmetic_stream as, stream_model model, uint32_t x, FILE* fin) {
    uint32_t Byte = 0;
    uint8_t tmp;

    Byte = x >> 24;
    tmp = ((uint8_t) Byte);
    fwrite(&tmp, 1, 1, fin);

    Byte = (x & 0x00ff0000) >> 16;
    tmp = ((uint8_t) Byte);
    fwrite(&tmp, 1, 1, fin);

    Byte = (x & 0x0000ff00) >> 8;
    tmp = ((uint8_t) Byte);
    fwrite(&tmp, 1, 1, fin);

    Byte = (x & 0x000000ff);
    tmp = ((uint8_t) Byte);
    fwrite(&tmp, 1, 1, fin);

    return 1;
}

int decompress_int32tt(Arithmetic_stream as, stream_model model, FILE* fin) {
    uint32_t Byte = 0, x = 0;
    uint8_t tmp;

    fread(&tmp, 1, 1, fin);
    Byte = ((uint32_t) tmp);
    x = Byte << 24;

    fread(&tmp, 1, 1, fin);
    Byte = ((uint32_t) tmp);
    x |= Byte << 16;

    fread(&tmp, 1, 1, fin);
    Byte = ((uint32_t) tmp);
    x |= Byte << 8;

    fread(&tmp, 1, 1, fin);
    Byte = ((uint32_t) tmp);
    x |= Byte;

    return ((int) x);
}

int compress_uint16tt(Arithmetic_stream as, uint16_t c, FILE* fin) {
    uint32_t Byte = 0;
    uint8_t tmp;

    Byte = c >> 8;
    tmp = ((uint8_t) Byte);

    fwrite(&tmp, 1, 1, fin);

    Byte = (c & 0x00ff);
    tmp = ((uint8_t) Byte);

    fwrite(&tmp, 1, 1, fin);

    return 0;
}

uint16_t decompress_uint16tt(FILE* fin) {
    uint32_t Byte = 0, x = 0;
    uint8_t tmp;

    fread(&tmp, 1, 1, fin);
    Byte = ((uint32_t) tmp);
    x = Byte << 8;

    fread(&tmp, 1, 1, fin);
    Byte = ((uint32_t) tmp);
    x |= Byte;

    return ((uint16_t) x);
}
