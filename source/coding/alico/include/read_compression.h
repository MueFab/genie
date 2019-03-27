//
//  reads_compression.h
//  XC_s2fastqIO
//
//  Created by Mikel Hernaez on 11/5/14.
//  Copyright (c) 2014 Mikel Hernaez. All rights reserved.
//

#ifndef XC_s2fastqIO_reads_compression_h
#define XC_s2fastqIO_reads_compression_h

#include "Arithmetic_stream.h"
#include "sam_block.h"
#include <inttypes.h>
#include "edit.h"
#include "id_compression.h"
#include <cstdint>
#include <limits>

#define UINT32_MAX ((uint32_t) - 1)
#define MAX_BP_CHR 300000000
#define CHR_CHANGE_FLAG 0xffffffff
#define END_GENOME_FLAG 0xfffffff0
#define INV_FLAG 0xff000000

//#define MAX_PATH_LENGTH 256
//#define MAX_HEADER 512

#define BITS_DELTA 7

extern char *reference;

extern uint8_t snpInRef[MAX_BP_CHR];
extern uint32_t cumsumP;


// Protorypes for the compression functions
uint32_t compress_flag(Arithmetic_stream a, stream_model *F, uint16_t flag, void *thread_info);
uint32_t compress_pos_alpha(Arithmetic_stream as, stream_model *PA, uint32_t x, void *thread_info);
uint32_t compress_pos(Arithmetic_stream as, stream_model *P, stream_model *PA, uint32_t pos, uint8_t chr_change, void *thread_info);
uint32_t compress_match(Arithmetic_stream a, stream_model *M, uint8_t match, uint32_t P, void *thread_info);
uint32_t compress_snps(Arithmetic_stream a, stream_model *S, uint8_t numSnps, void *thread_info);
uint32_t compress_indels(Arithmetic_stream a, stream_model *I, uint8_t numIndels, void *thread_info);
uint32_t compress_chars(Arithmetic_stream a, stream_model *c, enum BASEPAIR ref, enum BASEPAIR target, void *thread_info);
uint32_t compress_var(Arithmetic_stream a, stream_model *v, uint32_t pos, uint32_t prevPos, uint32_t flag, void *thread_info);


uint32_t compress_qv(Arithmetic_stream a, stream_model *model, uint32_t idx, uint8_t qv, void *thread_info);
double QVs_compress(Arithmetic_stream as, qv_block info, symbol_t *qArray, void* thread_info);
double QVs_compress_lossless(Arithmetic_stream as, stream_model* models, qv_line line, void *thread_info);
double QVs_decompress(Arithmetic_stream as, qv_block info, uint8_t inv, char* qualsi, void* thread_info);
double QVs_decompress_lossless(Arithmetic_stream as, qv_block info, uint8_t inv, char* quals, int num_cols, void *thread_info);
uint32_t decompress_qv(Arithmetic_stream a, stream_model *model, uint32_t idx, void *thread_info);

// Prototypes for the functions to extract the information from the reads
uint32_t compress_edits(Arithmetic_stream as, read_models rs, char *edits, char *cigar, char *read, uint32_t P, uint32_t deltaP, uint8_t flag, uint8_t* cigarFlags, void *thread_info);
int add_snps_to_array(char* edits, snp* SNPs, unsigned int *numSnps, unsigned int insertionPos, char *read);
uint32_t compute_delta_to_first_snp(uint32_t prevPos, uint32_t readLen);

void absolute_to_relative(uint32_t *Dels, uint32_t numDels, ins *Insers, uint32_t numIns);


// Prototypes for tghe decompression functions
uint32_t decompress_flag(Arithmetic_stream a, stream_model *F, uint32_t *flag, void *thread_info);
uint32_t decompress_pos_alpha(Arithmetic_stream as, stream_model *PA, void *thread_info);
uint32_t decompress_pos(Arithmetic_stream as, stream_model *P, stream_model *PA, uint8_t chr_change, uint32_t *p, void *thread_info);
uint32_t decompress_match(Arithmetic_stream a, stream_model *M, uint32_t P, void *thread_info);
uint32_t decompress_snps(Arithmetic_stream a, stream_model *S, void *thread_info);
uint32_t decompress_indels(Arithmetic_stream a, stream_model *I, void *thread_info);
uint32_t decompress_var(Arithmetic_stream a, stream_model *v,  uint32_t prevPos, uint32_t flag, void *thread_info);
uint8_t decompress_chars(Arithmetic_stream a, stream_model *c, enum BASEPAIR ref, void *thread_info);

uint32_t compress_read(Arithmetic_stream as, read_models models, read_line samLine, uint8_t chr_change, void *thread_info);

uint32_t reconstruct_read(Arithmetic_stream as, read_models models, uint32_t pos, uint8_t invFlag, char *read, uint32_t readLen, uint8_t chr_change, char *recCigar, void *thread_info);
uint32_t decompress_read(Arithmetic_stream as, sam_block sb, uint8_t chr_change, struct sam_line_t *sline, void *thread_info);
uint32_t decompress_cigar(Arithmetic_stream as, sam_block sb, struct sam_line_t *sline, void *thread_info);
int context_index(char prefix);
int store_reference_in_memory(FILE* refFile);
int store_reference_in_memory_com(FILE* refFile, Arithmetic_stream as1, uint64_t context[25][6], char* prefix, FILE* fsinchr);


void foo();

#endif
