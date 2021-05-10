//#include <htslib/sam.h>
#include <genie/format/sam/record.h>
#include <algorithm>
#include <genie/core/record/alignment_split/other-rec.h>
#include <genie/core/record/alignment_split/same-rec.h>
#include <genie/core/record/alignment_split/unpaired.h>
#include <genie/core/record/alignment_external/other-rec.h>

#include "sam_record.h"

namespace sam_transcoder {

bool checkFlag(SamRecord& rec, uint16_t flag){
    return (rec.flag & flag) != 0;
}

bool isPrimary(SamRecord& rec){
    return !checkFlag(rec, BAM_FSECONDARY) && !checkFlag(rec, BAM_FSUPPLEMENTARY);
}

char fourBitBase2Char(uint8_t int_base){
    switch(int_base){
        case 1:
            return 'A';
        case 2:
            return 'C';
        case 4:
            return 'G';
        case 8:
            return 'T';
        case 15:
            return 'N';
        default:
            // Raise error here
            return -1;
    }
}

std::string getCigarString(bam1_t *sam_alignment){
    auto n_cigar = sam_alignment->core.n_cigar;
    auto cigar_ptr = bam_get_cigar(sam_alignment);

    std::string cigar;
    for (uint32_t i=0; i < n_cigar; i++){
        char cigar_op = bam_cigar_opchr(bam_cigar_op(cigar_ptr[i]));
        auto cigar_op_len = bam_cigar_oplen(cigar_ptr[i]);
        cigar += std::to_string(cigar_op_len) + cigar_op;
    }
    return cigar;
}

SamRecord alignment2Record(bam1_t *sam_alignment){
//    std::string qname(bam_get_qname(sam_alignment));

    auto seq_len = sam_alignment->core.l_qseq;
    auto seq_ptr = bam_get_seq(sam_alignment);
//    std::vector<uint8_t> seq(seq_len);
    std::string seq(seq_len, '\0');
    for (auto i=0; i<seq_len; i++){
        seq[i] = fourBitBase2Char(bam_seqi(seq_ptr, i));
    }

    auto cigar = getCigarString(sam_alignment);

    auto qual_ptr = bam_get_qual(sam_alignment);
    std::string qual(seq_len, ' ');
    for (auto i=0; i<seq_len; i++){
        qual[i] = char(qual_ptr[i] + 33);
    }

    SamRecord sam_rec = {
        bam_get_qname(sam_alignment),
        sam_alignment->core.flag, // flag
        sam_alignment->core.tid, // rid
        (uint32_t) sam_alignment->core.pos, // pos
        sam_alignment->core.qual, // mapq
        std::move(cigar),
        sam_alignment->core.mtid,
        (uint32_t)sam_alignment->core.mpos,
        sam_alignment->core.isize,
        std::move(seq),// Seq,
        std::move(qual) // QUAL
    };

    return sam_rec;
}

}