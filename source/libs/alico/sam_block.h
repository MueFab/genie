//
//  sam_line.h
//  XC_s2fastqIO
//
//  Created by Mikel Hernaez on 11/5/14.
//  Copyright (c) 2014 Mikel Hernaez. All rights reserved.
//

#ifndef XC_s2fastqIO_sam_line_h
#define XC_s2fastqIO_sam_line_h

#include <stdio.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>

#include <sys/stat.h>
#include <fcntl.h>

#include <pthread.h>

#include "stream_model.h"
#include "pmf.h"
#include "qv_codebook.h"
#include "util.h"
#include "well.h"
#include "quantizer.h"
#include "aux_data.h"
#include <vector>

// Calq File

#include "calq/CQFile.h"
#include "calq/SAMRecord.h"
#define MAX_READ_LENGTH 1024
#define MAX_NUMBER_TOKENS_ID 128
#define VAR_MODEL_SIZE 0xffff

// This limits us to chunks that aren't too big to fit into a modest amount of memory at a time
#define MAX_LINES_PER_BLOCK			1
#define MAX_READS_PER_LINE			1022
#define READ_LINEBUF_LENGTH			(MAX_READS_PER_LINE+2)

// Error codes for reading a line block
#define LF_ERROR_NONE				0
#define LF_ERROR_NOT_FOUND			1
#define LF_ERROR_NO_MEMORY			2
#define LF_ERROR_TOO_LONG			4

#define get_qv_model_index(a,b) ((a & 0xff) << 8 | (b & 0xff))

#define MAX_ALPHA 5000000
#define MAX_CARDINALITY 50000000

struct sam_line_t{
    char ID[1024];
    uint32_t flag;
    char rname[1024];
    uint32_t pos;
    uint8_t mapq;
    char cigar[MAX_CIGAR_LENGTH];
    char rnext[1024];
    uint32_t pnext;
    int32_t tlen;
    char read[1024];
    char quals[1024];
    char aux[MAX_AUX_FIELDS*(MAX_AUX_LENGTH+1)];

    uint32_t readLength;
};


struct compressor_info_t{
    FILE *fsam;
    FILE *fref;
    FILE *fcomp;

    FILE *fcodebook[1024];
    int icodebook[1024];

    FILE *fmost_common_list;
    FILE *fsame_ref;

    FILE *frname[256];
    int irname[256];

    FILE *ftoken_type[128];
    int itoken_type[128];

    FILE *falpha_len[128];
    int ialpha_len[128];

    FILE *falpha_value[128];
    int ialpha_value[128];

    FILE *fzero_run[128];
    int izero_run[128];

    FILE *fdelta[128];
    int idelta[128];

    FILE *finteger[512];
    int iinteger[512];

    FILE *fchars[128];
    int ichars[128];

    FILE *fmapq;
    FILE *rnext_same;

    FILE *frnext_prev[256];
    int rnext_prev[256];

    FILE *frlength[4];
    int rlength[4];

    FILE *fpos;

    FILE *fpos_alpha[4];
    int rpos_alpha[4];

    FILE *fflag;

    FILE *fmatch[4];
    int rmatch[4];

    FILE *fsnps;
    FILE *findels;

    FILE *fvar;

    FILE *read_chars[6];
    int iread_chars[6];

    FILE *fcigarFlags;
    FILE *cigar;

    FILE *ftlen_zero;
    FILE *ftlen_sign;

    FILE *ftlen_tlen[4];
    int itlen_tlen[4];

    FILE *fpnext_zero;
    FILE *fpnext_sign;

    FILE *fdiff_pnext[4];
    int idiff_pnext[4];

    FILE *fmost_common_flag;

    FILE *fmost_common_values_wContext[102];
    int imost_common_values_wContext[102];

    FILE *faux_TagType[102];
    int iaux_TagType[102];

    FILE *ftag[2];
    int itag[2];

    FILE *ftypeLUTflag;
    FILE *ftypeRAW;
    FILE *ftypeLUT;

    FILE *fintegers_wContext[408];
    int iintegers_wContext[408];

    FILE *fdescBytes_wContext[204];
    int idescBytes_wContext[204];

    FILE *fiidBytes_wContext[102];
    int iiidBytes_wContext[102];

    FILE *fqv;

    FILE *funmapped;

    FILE *frefcom; //hongyi
    FILE *fsinchr; //hongyi

    FILE *fqual;
    char cq_name[1024];
    uint8_t mode;
    struct qv_options_t *qv_opts;
    uint8_t lossiness;
    uint8_t calqmode;
    uint8_t compress_ref;
    uint8_t decompress_ref;
};

typedef struct id_models_t{
    stream_model *token_type;
    stream_model *integer;
    stream_model *delta;
    stream_model *alpha_len;
    stream_model *alpha_value;
    stream_model *chars;
    stream_model *zero_run;
}*id_models;

typedef struct rname_models_t{
    stream_model *rname;
    stream_model *same_ref;
}*rname_models;

typedef struct rnext_models_t{
    stream_model *rnext;
    stream_model *same_ref;
}*rnext_models;

typedef struct pnext_models_t{
    stream_model *diff_pnext;
    stream_model *zero;
    stream_model *sign;
    stream_model *assumption;
    stream_model *raw_pnext;
}*pnext_models;

typedef struct tlen_models_t{
    stream_model *tlen;
    stream_model *sign;
    stream_model *zero;
}*tlen_models;

typedef struct mapq_models_t{
    stream_model *mapq;
}*mapq_models;

typedef struct read_models_t{
    stream_model *flag;
    stream_model *pos;
    stream_model *pos_alpha;
    stream_model *match;
    stream_model *snps;
    stream_model *indels;
    stream_model *var;
    stream_model *chars;
    stream_model *cigar;
    stream_model *cigarFlags;
    stream_model *rlength;
    uint32_t read_length;
    char _readLength[4];
}*read_models;


typedef struct aux_models_t{

    //idoia
    stream_model *aux_TagType; //To compress TagType given previous TagType
    stream_model *firstAux_TagType; // To compress the TagType of the first Aux field
    stream_model *most_common_values_wContext; //for the values (indices) of the most common list w previous tagType as context
    stream_model *sign_integers_wContext; //type = i
    stream_model *integers_wContext; //type = i
    stream_model *iidBytes_wContext;
    stream_model *descBytes_wContext;
    //idoia

    stream_model *qAux; //no. of aux fields per read
    stream_model *tagtypeLUTflag; //if the tagtype is found on the tagTypeLUT.
    stream_model *typeLUTflag; //if the type is found on the typeLUT.
    stream_model *tagtypeLUT; //value on the tagTypeLUT (0 to TAGTYPELUTLENGTH-1)
    stream_model *tag; //if tagtype not in tagTypeLUT, raw tag value
    stream_model *typeLUT; //if tagtype not in tagTypeLUT, value of type on typeLUT (0 to TYPELUTLENGTH-1)
    stream_model *typeRAW; //if type not in typeLUT, raw type value.
    stream_model *descBytes; //bytes of the desc

    stream_model *most_common_values; //for the values (indices) of the most common list.
    stream_model *most_common_flag; //is it in the common list? flag

    stream_model *most_common_list; //for compressing the list itself so that the decoder knows how to demap the indices.

    stream_model *iidBytes; //everything else

    // PROVISIONALES
    stream_model *sign_integers; //type = i
    stream_model *integers; //type = i
    stream_model *pruebaCharContexto;
}*aux_models;

enum token_type{ ID_ALPHA, ID_DIGIT, ID_CHAR, ID_MATCH, ID_ZEROS, ID_DELTA, ID_END};

// To store the model of the chars both in ref and target
enum BASEPAIR {
    A,
    C,
    G,
    T,
    N,
    O
};

/**
 *
 */
typedef struct read_line_t{
    char *cigar;
    uint8_t cigarFlags; //To check if the cigar can be recovered from indels, etc.
    char *edits;
    char *read;
    uint16_t invFlag;
    int pos;
    uint32_t read_length;
}*read_line;

/**
 *
 */
typedef struct read_block_t{
    struct read_line_t *lines;
    read_models models;
    uint32_t block_length;
} *read_block;

/**
 *
 */
typedef struct rname_block_t{
    char **rnames;
    rname_models models;
    uint32_t block_length;
} *rname_block;

/**
 *
 */
typedef struct mapq_block_t{
    char *mapq;
    mapq_models models;
    uint32_t block_length;
} *mapq_block;

/**
 *
 */
typedef struct rnext_block_t{
    char **rnext;
    rnext_models models;
    uint32_t block_length;
} *rnext_block;

/**
 *
 */
typedef struct pnext_block_t{
    uint32_t *pnext;
    pnext_models models;
    uint32_t block_length;
} *pnext_block;

/**
 *
 */
typedef struct tlen_block_t{
    int32_t *tlen;
    tlen_models models;
    uint32_t block_length;
} *tlen_block;

/**
 *
 */
typedef struct id_block_t{
    char **IDs;
    id_models models;
    uint32_t block_length;
} *id_block;


/**
 *
 */
typedef struct qv_line_t {
    symbol_t *data;
    uint32_t columns;
}*qv_line;

/**
 *
 */
typedef struct aux_block_t{
    char **aux_str;
    char **most_common;
    uint32_t most_common_size;
    aux_models models;
    uint32_t block_length;
    uint8_t aux_cnt;
}*aux_block;



/**
 * Points to a file descriptor that includes important metadata about the qv
 */
typedef struct qv_block_t {
    struct alphabet_t *alphabet;
    uint32_t block_length;
    uint32_t columns;
    struct qv_line_t *qv_lines;
    struct cond_pmf_list_t *training_stats;
    struct cond_quantizer_list_t *qlist;
    struct distortion_t *dist;
    struct qv_options_t *opts;
    struct well_state_t well;
    stream_model *model;
    stream_model *codebook_model;
    FILE *fs;
    symbol_t *qArray;
}*qv_block;

//Deprecated
typedef struct simplified_qv_block_t {
    uint32_t block_length;
    uint32_t columns;
    struct qv_line_t *qv_lines;
    symbol_t *qArray;
    struct well_state_t well;
    stream_model *model;
}*simplified_qv_block;

/**
 *
 */
typedef struct sam_block_t{
    qv_block QVs;
    read_block reads;
    id_block IDs;
    rname_block rnames;
    mapq_block mapq;
    rnext_block rnext;
    pnext_block pnext;
    tlen_block tlen;
    FILE *fs;
    char *path;
    uint32_t read_length;
    uint32_t block_length;
    stream_model *codebook_model;
    FILE *fref;
    uint32_t current_line; // used for decompression

    aux_block aux;
}*sam_block;




// Stuctures to handle the reads
typedef struct ch_t{
    char refChar;
    char targetChar;
}ch_t;

typedef struct snp{
    uint32_t pos;
    enum BASEPAIR refChar;
    enum BASEPAIR targetChar;
    uint32_t ctr;
}snp;

typedef struct ins{
    uint32_t pos;
    enum BASEPAIR targetChar;
}ins;


typedef struct cigarIndels{
    uint32_t pos;
    uint32_t num;
    char letter;
}cigarIndels;


// Function Prototypes

enum BASEPAIR char2basepair(char c);
int basepair2char(enum BASEPAIR c);
char bp_complement(char c);
enum token_type uint8t2token(uint8_t tok);

stream_model *initialize_stream_model_flag(uint32_t rescale);
stream_model* initialize_stream_model_pos(uint32_t rescale);
stream_model* initialize_stream_model_pos_alpha(uint32_t rescale);
stream_model* initialize_stream_model_match(uint32_t rescale);
stream_model* initialize_stream_model_snps(uint32_t readLength, uint32_t rescale);
stream_model* initialize_stream_model_indels(uint32_t readLength, uint32_t rescale);
stream_model* initialize_stream_model_var(uint32_t readLength, uint32_t rescale);
stream_model* initialize_stream_model_chars(uint32_t rescale);
void initialize_stream_model_qv(stream_model *s, struct cond_quantizer_list_t *q_list);
stream_model* initialize_stream_model_codebook(uint32_t rescale);

read_models alloc_read_models_t(uint32_t read_length);
rname_models alloc_rname_models_t();
mapq_models alloc_mapq_models_t();
rnext_models alloc_rnext_models_t();
pnext_models alloc_pnext_models_t();
tlen_models alloc_tlen_models_t();
aux_models alloc_aux_models_t();

//void alloc_stream_model_qv(qv_block qvBlock);

stream_model *alloc_stream_model_qv(uint32_t read_length, uint32_t input_alphabet_size, uint32_t rescale);


sam_block alloc_sam_models(Arithmetic_stream as, FILE * fin, FILE *fref, struct qv_options_t *qv_opts, uint8_t decompression, void *thread_info);
read_block alloc_read_block_t(uint32_t read_length);
qv_block alloc_qv_block_t(struct qv_options_t *opts, uint32_t read_length);
uint32_t get_read_length(FILE *f);
uint32_t load_sam_block(sam_block sb);

// Meat of the implementation
void calculate_statistics(struct qv_block_t *info);
double optimize_for_entropy(struct pmf_t *pmf, struct distortion_t *dist, double target, struct quantizer_t **lo, struct quantizer_t **hi, uint8_t verbose);
void generate_codebooks(struct qv_block_t *info);

// Master functions to handle codebooks in the output file
void write_codebooks(Arithmetic_stream as, struct qv_block_t *info, void *thread_info);
void write_codebook(Arithmetic_stream as, struct cond_quantizer_list_t *quantizers, stream_model *model);
void read_codebooks(Arithmetic_stream as, struct qv_block_t *info, void *thread_info);
struct cond_quantizer_list_t *read_codebook(Arithmetic_stream as, struct qv_block_t *info);

void initialize_qv_model(Arithmetic_stream as, qv_block qvBlock, uint8_t decompression, void *thread_info);
void initialize_stream_model_qv_full(stream_model *s, struct cond_quantizer_list_t *q_list);
void reset_QV_block(qv_block qvb, uint8_t direction);

double QVs_compress2(Arithmetic_stream as, qv_block info);
double QVs_compress3(Arithmetic_stream as, stream_model* models, qv_line line);
void quantize_line(qv_block qb, qv_line qline, uint32_t read_length);
id_models alloc_id_models_t();

symbol_t * copy_qlis_to_array(qv_block qb);
void quantize_block(qv_block qb, uint32_t read_length);

stream_model *free_stream_model_qv(struct cond_quantizer_list_t *q_list, stream_model *s);


int compress_most_common_list(Arithmetic_stream as, aux_block aux, void *thread_info);
int decompress_most_common_list(Arithmetic_stream as, aux_block aux, void *thread_info);

int compress_id(Arithmetic_stream as, id_models models, char *id, void *thread_info);
int decompress_id(Arithmetic_stream as, id_models model, char *id, void *thread_info);

int compress_rname(Arithmetic_stream as, rname_models models, char *rname, void *thread_info);
int decompress_rname(Arithmetic_stream as, rname_models models, char *rname, void *thread_info);

int compress_mapq(Arithmetic_stream as, mapq_models models, uint8_t mapq, void *thread_info);
int decompress_mapq(Arithmetic_stream as, mapq_models models, uint8_t *mapq, void *thread_info);

int compress_rnext(Arithmetic_stream as, rnext_models models, char *rnext, void *thread_info);
int decompress_rnext(Arithmetic_stream as, rnext_models models, char *rnext, void *thread_info);

int compress_pnext(Arithmetic_stream as, pnext_models models, uint32_t pos, int32_t tlen, uint32_t pnext, uint8_t rname_rnextDiff, char* cigar, void *thread_info);
int decompress_pnext(Arithmetic_stream as, pnext_models models, uint32_t pos, int32_t tlen, uint32_t readLength, uint32_t* pnext, uint8_t rname_rnextDiff, char* cigar, void *thread_info);
int compress_pnext_raw(Arithmetic_stream as, pnext_models models, uint32_t pos, uint32_t pnext, void *thread_info);
int decompress_pnext_raw(Arithmetic_stream as, pnext_models models, uint32_t pos, uint32_t* pnext, void *thread_info);

int compress_tlen(Arithmetic_stream as, tlen_models models, int32_t tlen, void *thread_info);
int decompress_tlen(Arithmetic_stream as, tlen_models models, int32_t* tlen, void *thread_info);

int compress_aux(Arithmetic_stream as, aux_models models, char **aux_str, uint8_t aux_cnt, aux_block aux);
int decompress_aux(Arithmetic_stream as, aux_block aux, char* finalLine);

//idoia
int compress_aux_idoia(Arithmetic_stream as, aux_models models, char **aux_str, uint8_t aux_cnt, aux_block aux, void *thread_info);
int decompress_aux_idoia(Arithmetic_stream as, aux_block aux, char* finalLine, void *thread_info);
//idoia

int compress_cigar(Arithmetic_stream as, read_models models, char *cigar, uint8_t cigarFlags, void *thread_info);


int compress_block(Arithmetic_stream as, sam_block samBlock);
int decompress_block(Arithmetic_stream as, sam_block samBlock);
int decompress_line(Arithmetic_stream as, sam_block samBlock, uint8_t lossiness, uint8_t calqmode, std::vector<calq::SAMRecord> &samRecords,char prevname[1024], void *thread_info);
void* compress(void *thread_info);
void* decompress(void *thread_info);
char idx_convert(int i);
void reconstruct_ref(void *thread_info);

uint32_t compute_num_digits(uint32_t x);

uint32_t load_qv_line(qv_block QV);
uint32_t load_sam_line(sam_block sb);
uint32_t load_qv_training_block(qv_block QV);

uint8_t create_most_common_list(sam_block sb);
uint8_t get_most_common_token(char** list, uint32_t list_size, char* aux_field);

uint32_t reconstructCigar(uint32_t* Dels, ins* Insers, uint32_t numDels, uint32_t numIns, uint32_t totalReadLength, char* recCigar);



#endif
