#ifndef SAM_RECORD_H
#define SAM_RECORD_H

#include <htslib/sam.h>
#include <vector>
//#include <list>
//#include <genie/core/record/record.h>

#include "program-options.h"

namespace sam_transcoder {

typedef struct SamRecord{
    std::string qname; // Query template name
    uint16_t flag; // Flag
    int32_t rid; // Reference sequence ID
    uint32_t pos; // Position
    uint8_t mapq; // Mapping Quality
    std::string cigar; // CIGAR
    int32_t mate_rid; // Mate reference sequence ID
    uint32_t mate_pos; // Mate position
    int64_t tlen; // Observed template length
    std::string seq; // Read sequence
    std::string qual;
} SamRecord;

bool checkFlag(SamRecord& rec, uint16_t flag);

bool isPrimary(SamRecord& rec);

char fourBitBase2Char(uint8_t int_base);

std::string getCigarString(bam1_t *sam_alignment);

SamRecord alignment2Record(bam1_t *sam_alignment);

}

#endif  // SAM_RECORD_H
