#ifndef _EDIT_H_
#define _EDIT_H_

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include "sam_block.h"
#include <assert.h>
#include <ctype.h>
#include <stdint.h>
#include <vector>

typedef enum {INSERT, REPLACE} edit;

struct sequence {
  struct snp *SNPs;  
  uint32_t n_snps;
  struct ins *Insers;
  uint32_t n_ins;
  uint32_t *Dels;
  uint32_t n_dels;
};


void init_sequence(struct sequence *seq, uint32_t *Dels, struct ins *Insers, struct snp *SNPs);
uint32_t edit_dist(char *str1, char *str2, uint32_t s1, uint32_t s2);

void reconstruct_read_from_ops(struct sequence *seq, char *ref, char *target, uint32_t len);
uint32_t edit_sequence(char *str1, char *str2, uint32_t s1, uint32_t s2, struct sequence &seq);
#endif 
