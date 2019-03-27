#ifndef ID_COMPRESSION_H
#define ID_COMPRESSION_H

#include "Arithmetic_stream.h"
#include "sam_block.h"
#include "IO/SAM/SAMRecord.h"

uint8_t decompress_uint8tt(Arithmetic_stream as, stream_model model, FILE** fin, int* flag, char* name);
uint8_t decompress_uint8t(Arithmetic_stream as, stream_model model);
int compress_uint8tt(Arithmetic_stream as, stream_model model, uint8_t c, FILE** fin, int* flag, char* name);
int compress_uint8t(Arithmetic_stream as, stream_model model, uint8_t c);
int compress_rname(Arithmetic_stream as, rname_models models, char *rname, void *thread_info);
int decompress_rname(Arithmetic_stream as, rname_models models, char *rname, void *thread_info);
int compress_int32tt(Arithmetic_stream as, stream_model model, uint32_t c, FILE* fin);
int decompress_int32tt(Arithmetic_stream as, stream_model model, FILE* fin);
int compress_uint16tt(Arithmetic_stream as, uint16_t c, FILE* fin);
uint16_t decompress_uint16tt(FILE* fin);

#endif
