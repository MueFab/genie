//
//  reads_compression.c
//  XC_s2fastqIO
//
//  Created by Mikel Hernaez on 11/5/14.
//  Copyright (c) 2014 Mikel Hernaez. All rights reserved.
//


#include "read_compression.h"
#include "id_compression.h"
#include <iostream>
#define DEBUG false
#define VERIFY false

using namespace std;

char *reference = NULL;
uint8_t snpInRef[MAX_BP_CHR];
uint32_t cumsumP = 0;


/************************
 * Compress the read
 **********************/
uint32_t compress_read(Arithmetic_stream as, read_models models, read_line samLine, uint8_t chr_change, void *thread_info){
    char nm[40];
    char number[15];
    struct compressor_info_t *info = ((struct compressor_info_t *)thread_info);

    int tempF, PosDiff, chrPos, k;
    uint32_t mask;
    uint16_t maskedReadVal;
    // compress read length (assume int)
    for (k=0;k<4;k++) {
        mask = 0xFF<<(k*8);
        maskedReadVal = (uint8_t)(models->read_length & mask)>>(k*8);
        strcpy(nm, "rlength");
        sprintf(number, "%d", k);
        strcat(nm, number);
        compress_uint8tt(as, models->rlength[k], maskedReadVal, &info->frlength[k], &info->rlength[k], nm);
    }

    // Compress sam line
    PosDiff = compress_pos(as, models->pos, models->pos_alpha, samLine->pos, chr_change, thread_info);

    tempF = compress_flag(as, models->flag, samLine->invFlag, thread_info);

    chrPos = compress_edits(as, models, samLine->edits, samLine->cigar, samLine->read, samLine->pos, PosDiff, tempF, &(samLine->cigarFlags), thread_info);

    if (VERIFY) assert(samLine->pos  == chrPos);

    return 1;
}


/***********************
 * Compress the Flag
 **********************/
uint32_t compress_flag(Arithmetic_stream a, stream_model *F, uint16_t flag, void *thread_info){


    // In this case we need to compress the whole flag, althugh the binary information of whether the
    // read is in reverse or not is the most important. Thus, we return the binary info.
    //we use F[0] as there is no context for the flag.

    uint16_t x = 0;

    x = flag << 11;
    x >>= 15;

    struct compressor_info_t *info = ((struct compressor_info_t *)thread_info);

    compress_uint16tt(a, flag, info->fflag);

    return x;

}

/***********************************
 * Compress the Alphabet of Position
 ***********************************/
uint32_t compress_pos_alpha(Arithmetic_stream as, stream_model *PA, uint32_t x, void *thread_info){

    uint32_t Byte = 0;

    uint8_t tmp;
    struct compressor_info_t *info = ((struct compressor_info_t *)thread_info);

    // Send B0 to the Arithmetic Stream using the alphabet model
    Byte = x >> 24;
    tmp = ((uint8_t) Byte);

    if(info->rpos_alpha[0] == 0) {
        info->fpos_alpha[0]= fopen("pos_alpha0", "wb");
        info->rpos_alpha[0] = 1;
    }
    fwrite(&tmp, 1, 1, info->fpos_alpha[0]);

    // Send B1 to the Arithmetic Stream using the alphabet model
    Byte = (x & 0x00ff0000) >> 16;
    tmp = ((uint8_t) Byte);

    if(info->rpos_alpha[1] == 0) {
        info->fpos_alpha[1]= fopen("pos_alpha1", "wb");
        info->rpos_alpha[1] = 1;
    }
    fwrite(&tmp, 1, 1, info->fpos_alpha[1]);

    // Send B2 to the Arithmetic Stream using the alphabet model
    Byte = (x & 0x0000ff00) >> 8;
    tmp = ((uint8_t) Byte);

    if(info->rpos_alpha[2] == 0) {
        info->fpos_alpha[2]= fopen("pos_alpha2", "wb");
        info->rpos_alpha[2] = 1;
    }
    fwrite(&tmp, 1, 1, info->fpos_alpha[2]);

    // Send B3 to the Arithmetic Stream using the alphabet model
    Byte = (x & 0x000000ff);
    tmp = ((uint8_t) Byte);

    if(info->rpos_alpha[3] == 0) {
        info->fpos_alpha[3]= fopen("pos_alpha3", "wb");
        info->rpos_alpha[3] = 1;
    }
    fwrite(&tmp, 1, 1, info->fpos_alpha[3]);

    return 1;


}

/***********************
 * Compress the Position
 **********************/
uint32_t compress_pos(Arithmetic_stream as, stream_model *P, stream_model *PA, uint32_t pos, uint8_t chr_change, void *thread_info){

    static uint32_t prevPos = 0;
    enum {SMALL_STEP = 0, BIG_STEP = 1};
    int32_t x = 0;

    uint8_t tmp;
    struct compressor_info_t *info = ((struct compressor_info_t *)thread_info);

    // Check if we are changing chromosomes.
    if (chr_change) {
        prevPos = 0;
    }

    // Compress the position diference (+ 1 to reserve 0 for new symbols)
    x = pos - prevPos + 1;

    if (P[0]->alphaExist[x]){
        compress_int32tt(as, P[0], ((uint32_t) P[0]->alphaMap[x]), info->fpos);
  
    }
    else{
        tmp = 0;
        compress_int32tt(as, P[0], 0, info->fpos);
      
        // Send the new letter to the Arithmetic Stream using the alphabet model
        compress_pos_alpha(as, PA, x, thread_info);

        // Update the statistics of the alphabet for x
        P[0]->alphaExist[x] = 1;

        if(x >= P[0]->alphaMap_size) {P[0]->alphaMap = ((int32_t*) realloc(P[0]->alphaMap, (x+1)*sizeof(int32_t))); P[0]->alphaMap_size=x+1;}

        P[0]->alphaMap[x] = P[0]->alphabetCard; // We reserve the bin 0 for the new symbol flag
        P[0]->alphabet[P[0]->alphabetCard] = x;

        // Update model
        update_model(P[0], P[0]->alphabetCard++);
    }

    prevPos = pos;

    return x;
}

/****************************
 * Compress the match
 *****************************/
uint32_t compress_match(Arithmetic_stream a, stream_model *M, uint8_t match, uint32_t P, void *thread_info){

    uint32_t ctx = 0;
    static uint8_t  prevM = 0;

    char nm[40];
    char number[15];
    struct compressor_info_t *info = ((struct compressor_info_t *)thread_info);

    // Compute Context
    P = (P != 1)? 0:1;
    //prevP = (prevP > READ_LENGTH)? READ_LENGTH:prevP;
    //prevP = (prevP > READ_LENGTH/4)? READ_LENGTH:prevP;

    ctx = (P << 1) | prevM;

    //ctx = 0;
    if(info->rmatch[ctx] == 0) {
      strcpy(nm, "match");
      sprintf(number, "%d", ctx);
      strcat(nm, number);
      info->fmatch[ctx] = fopen(nm, "wb");
      info->rmatch[ctx] = 1;
    }
    fwrite(&match, 1, 1, info->fmatch[ctx]);

    prevM = match;

    return 1;
}

/*************************
 * Compress the snps
 *************************/
uint32_t compress_snps(Arithmetic_stream a, stream_model *S, uint8_t numSnps, void *thread_info){
    struct compressor_info_t *info = ((struct compressor_info_t *)thread_info);

    // No context is used for the numSnps for the moment.
    fwrite(&numSnps, 1, 1, info->fsnps);

    return 1;

}


/********************************
 * Compress the indels
 *******************************/
uint32_t compress_indels(Arithmetic_stream a, stream_model *I, uint8_t numIndels, void *thread_info){
    struct compressor_info_t *info = ((struct compressor_info_t *)thread_info);

    // Nos context is used for the numIndels for the moment.
    fwrite(&numIndels, 1, 1, info->findels);

    return 1;

}

/*******************************
 * Compress the variations
 ********************************/
uint32_t compress_var(Arithmetic_stream a, stream_model *v, uint32_t pos, uint32_t prevPos, uint32_t flag, void *thread_info){

    uint32_t ctx = 0;

    struct compressor_info_t *info = ((struct compressor_info_t *)thread_info);

    //flag = 0;
    ctx = prevPos << 1 | flag;

    compress_int32tt(a, *v, pos, info->fvar);
//printf("reached\n");
    return 1;

}

/*****************************************
 * Compress the chars
 ******************************************/
uint32_t compress_chars(Arithmetic_stream a, stream_model *c, enum BASEPAIR ref, enum BASEPAIR target, void *thread_info){
    char nm[40];
    char number[15];
    struct compressor_info_t *info = ((struct compressor_info_t *)thread_info);

    if(info->ichars[ref] == 0) {
        strcpy(nm, "char");
        sprintf(number, "%d", ref);
        strcat(nm, number);
        info->fchars[ref] = fopen(nm, "wb");
        info->ichars[ref] = 1;
    }
    fwrite(&target, 1, 1, info->fchars[ref]);

    return 1;

}

/*****************************************
 * Compress the edits
 ******************************************/
uint32_t compress_edits(Arithmetic_stream as, read_models rs, char *edits, char *cigar, char *read, uint32_t P, uint32_t deltaP, uint8_t flag, uint8_t* cigarFlags, void *thread_info){

    uint32_t i = 0;
    uint32_t Dels[MAX_READ_LENGTH];
    ins Insers[MAX_READ_LENGTH];
    snp SNPs[MAX_READ_LENGTH];

    char recCigar[MAX_CIGAR_LENGTH];

    char *tmpcigar, *tmpEdits;
    char *origCigar = cigar;

    // pos in the reference
    cumsumP = cumsumP + deltaP - 1;// DeltaP is 1-based

    uint32_t prev_pos = 0;
    uint32_t delta = 0;

    // Check if read matches reference
    bool matches = true;
    for (i = 0; i < rs->read_length; i++) {
      if (read[i] != reference[P-1 + i]) {
        matches = false;
        break;
      }
    }
    if (matches) {
      compress_match(as, rs->match, 1, deltaP, thread_info);

      reconstructCigar(Dels, Insers, 0, 0, rs->read_length, recCigar);
      *cigarFlags = 0;
      if (strcmp(recCigar, origCigar) == 0) {
        *cigarFlags = 1;
      }
      return cumsumP;
    }

    struct sequence seq;
    init_sequence(&seq, Dels, Insers, SNPs);

    uint32_t dist = edit_sequence(read, &(reference[P-1]), rs->read_length, rs->read_length, seq);

    // The edit distance from the sam file may be wrong.
    if (dist == 0) {
      compress_match(as, rs->match, 1, deltaP, thread_info);
      return cumsumP;
    }

    uint32_t numIns = seq.n_ins;
    uint32_t numSnps = seq.n_snps;
    uint32_t numDels = seq.n_dels;


    if (DEBUG) {
      printf("snps %d, dels %d, ins %d\n", numSnps, numDels, numIns);
      assert(numSnps + numDels + numIns > 0);
    }

    compress_match(as, rs->match, 0, deltaP, thread_info);

    // Compress the edits
    if ((numDels | numIns) == 0) {
        compress_snps(as, rs->snps, numSnps, thread_info);
    }
    else{
        compress_snps(as, rs->snps, 0, thread_info);
        compress_indels(as, rs->indels, numSnps, thread_info);
        compress_indels(as, rs->indels, numDels, thread_info);
        compress_indels(as, rs->indels, numIns, thread_info);
    }

    // Store the positions and Chars in the corresponding vector
    prev_pos = 0;

    for (i = 0; i < numDels; i++){
        if (VERIFY) assert(prev_pos < rs->read_length);
        Dels[i] = Dels[i] - prev_pos;
        compress_var(as, rs->var, Dels[i], prev_pos, flag, thread_info);
        if (DEBUG) printf("Delete at offset %d, prev %d \n", Dels[i], prev_pos);
        prev_pos += Dels[i];
    }

    prev_pos = 0;
    for (i = 0; i < numIns; i++){
        Insers[i].pos = Insers[i].pos - prev_pos;
        compress_var(as, rs->var, Insers[i].pos, prev_pos, flag, thread_info);
        compress_chars(as, rs->chars, O, Insers[i].targetChar, thread_info);
        if (DEBUG) printf("Insert %c at offset %d, prev_pos %d\n", basepair2char(Insers[i].targetChar), Insers[i].pos, prev_pos);
        prev_pos += Insers[i].pos;
    }


    prev_pos = 0;
    for (i = 0; i < numSnps; i++){
        SNPs[i].pos = SNPs[i].pos - prev_pos;
        delta = compute_delta_to_first_snp(prev_pos + 1, rs->read_length);

        delta = (delta << BITS_DELTA);
        compress_var(as, rs->var, SNPs[i].pos, delta + prev_pos, flag, thread_info);
        snpInRef[cumsumP - 1 + prev_pos + SNPs[i].pos] = 1;

        compress_chars(as, rs->chars, SNPs[i].refChar, SNPs[i].targetChar, thread_info);
        if (DEBUG) printf("Replace %c with %c offset %d, prev_pos = %d\n", basepair2char(SNPs[i].refChar), basepair2char(SNPs[i].targetChar), SNPs[i].pos, prev_pos);
        prev_pos += SNPs[i].pos;
    }

    reconstructCigar(Dels, Insers, numDels, numIns, rs->read_length, recCigar);
    //printf("%s\n", recCigar);
    *cigarFlags = 0;

    //printf("%d\n", strlen(recCigar));
    //printf("%d\n", strlen(origCigar));
    if (strcmp(recCigar, origCigar) == 0) {
        *cigarFlags = 1;
    }
    return cumsumP;

}

uint32_t compute_delta_to_first_snp(uint32_t prevPos, uint32_t readLen){

    uint32_t deltaOut;
    uint32_t j = 0;

    deltaOut = readLen + 2;

    for (j=0;j<readLen - prevPos; j++){
        if (snpInRef[cumsumP - 1 + j + prevPos] == 1){
            deltaOut = j;
            break;
        }
    }

    return deltaOut;
}

uint32_t compute_num_digits(uint32_t x){

    //Get the number of digits (We assume readLength < 1000)

    if (x < 10)
        return 1;
    else if (x < 100)
        return 2;
    else if (x < 1000)
        return 3;
    else if (x < 10000)
        return 4;
    else if (x < 100000)
        return 5;
    else if (x < 1000000)
        return 6;
    else if (x < 10000000)
        return 7;
    else if (x < 100000000)
        return 8;
    else
        return 9;

}

void absolute_to_relative(uint32_t *Dels, uint32_t numDels, ins *Insers, uint32_t numIns) {
    // convert to relative
    uint32_t prev_pos = 0;
    uint32_t i=0;
    if (numDels > 0) {
        prev_pos = Dels[i];
    }
    /*
    for (i = 0; i < numDels; i++) {
        printf("Before Dels %d, pos %d\n", i, Dels[i]);
    }
    for (i = 0; i < numIns; i++) {
        printf("Before Ins %d, pos %d\n", i, Insers[i].pos);
    }*/

    for (i = 1; i < numDels; i++) {
        Dels[i] = Dels[i] - prev_pos;
        prev_pos += Dels[i];
    }

    if (numIns > 0) {
        prev_pos = Insers[0].pos;
    }

    for (i = 1; i < numIns; i++) {
        Insers[i].pos = Insers[i].pos - prev_pos;
        prev_pos += Insers[i].pos;
    }

    /*
    for (i = 0; i < numDels; i++) {
        printf("After Dels %d, pos %d\n", i, Dels[i]);
    }
    for (i = 0; i < numIns; i++) {
        printf("After Ins %d, pos %d\n", i, Insers[i].pos);
    }*/
}
