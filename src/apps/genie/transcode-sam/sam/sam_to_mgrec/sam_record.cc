/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

// #include <htslib/sam.h>
// #include <genie/format/sam/record.h>

// ---------------------------------------------------------------------------------------------------------------------

#include "apps/genie/transcode-sam/sam/sam_to_mgrec/sam_record.h"
#include <algorithm>
#include <string>
#include <utility>
#include "genie/core/record/alignment_external/other-rec.h"
#include "genie/core/record/alignment_split/other-rec.h"
#include "genie/core/record/alignment_split/same-rec.h"
#include "genie/core/record/alignment_split/unpaired.h"
#include "genie/util/runtime-exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genieapp::transcode_sam::sam::sam_to_mgrec {

// ---------------------------------------------------------------------------------------------------------------------

char SamRecord::fourBitBase2Char(uint8_t int_base) {
    switch (int_base) {
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

// ---------------------------------------------------------------------------------------------------------------------

std::string SamRecord::getCigarString(bam1_t* sam_alignment) {
    auto n_cigar = sam_alignment->core.n_cigar;
    auto cigar_ptr = bam_get_cigar(sam_alignment);

    std::string cigar;
    for (uint32_t i = 0; i < n_cigar; i++) {
        char cigar_op = bam_cigar_opchr(bam_cigar_op(cigar_ptr[i]));
        auto cigar_op_len = bam_cigar_oplen(cigar_ptr[i]);
        cigar += std::to_string(cigar_op_len) + cigar_op;
    }
    return cigar;
}

// ---------------------------------------------------------------------------------------------------------------------

std::string SamRecord::getSeqString(bam1_t* sam_alignment) {
    auto seq_len = sam_alignment->core.l_qseq;
    auto seq_ptr = bam_get_seq(sam_alignment);
    std::string tmp_seq(seq_len, ' ');
    for (auto i = 0; i < seq_len; i++) {
        tmp_seq[i] = fourBitBase2Char(bam_seqi(seq_ptr, i));
    }

    return tmp_seq;
}

// ---------------------------------------------------------------------------------------------------------------------

std::string SamRecord::getQualString(bam1_t* sam_alignment) {
    auto seq_len = sam_alignment->core.l_qseq;
    auto qual_ptr = bam_get_qual(sam_alignment);
    std::string tmp_qual(seq_len, ' ');
    for (auto i = 0; i < seq_len; i++) {
        tmp_qual[i] = static_cast<char>(qual_ptr[i] + 33);
    }
    return tmp_qual;
}

// ---------------------------------------------------------------------------------------------------------------------

char SamRecord::convertCigar2ECigarChar(char token) {
    static const auto lut_loc = []() -> std::string {
        std::string lut(128, 0);
        lut['M'] = '=';
        lut['='] = '=';
        lut['X'] = '=';
        lut['I'] = '+';
        lut['D'] = '-';
        lut['N'] = '*';
        lut['S'] = ')';
        lut['H'] = ']';
        lut['P'] = ']';
        return lut;
    }();
    UTILS_DIE_IF(token < 0, "Invalid cigar token " + std::to_string(token));
    char ret = lut_loc[token];
    UTILS_DIE_IF(ret == 0, "Invalid cigar token" + std::to_string(token));
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

int SamRecord::stepSequence(char token) {
    static const auto lut_loc = []() -> std::string {
        std::string lut(128, 0);
        lut['M'] = 1;
        lut['='] = 1;
        lut['X'] = 1;
        lut['I'] = 1;
        lut['D'] = 0;
        lut['N'] = 0;
        lut['S'] = 1;
        lut['H'] = 0;
        lut['P'] = 0;
        return lut;
    }();
    UTILS_DIE_IF(token < 0, "Invalid cigar token " + std::to_string(token));
    return lut_loc[token];
}

// ---------------------------------------------------------------------------------------------------------------------

std::string SamRecord::convertCigar2ECigar(const std::string& cigar, const std::string& seq) {
    std::string ecigar;
    const size_t EXPECTED_ELONGATION = 4;  // Additional braces for softclips + hardclips
    ecigar.reserve(cigar.length() + EXPECTED_ELONGATION);
    size_t seq_pos = 0;
    std::string digits;
    for (const auto& a : cigar) {
        if (std::isdigit(a)) {
            digits += a;
            continue;
        }
        if (a == 'X') {
            size_t end = std::stoi(digits) + seq_pos;
            UTILS_DIE_IF(end >= seq.length(), "CIGAR not valid for seq");
            for (; seq_pos < end; ++seq_pos) {
                ecigar += std::toupper(seq[seq_pos]);
            }
        } else {
            if (a == 'S') {
                ecigar += '(';
            } else if (a == 'H') {
                ecigar += '[';
            }
            char token = convertCigar2ECigarChar(a);
            seq_pos += stepSequence(a) * std::stoi(digits);
            ecigar += digits;
            ecigar += token;
        }
        digits.clear();
    }
    return ecigar;
}

// ---------------------------------------------------------------------------------------------------------------------

SamRecord::SamRecord() : flag(0), rid(0), pos(0), mapq(0), mate_rid(0), mate_pos(0) {}

// ---------------------------------------------------------------------------------------------------------------------

SamRecord::SamRecord(bam1_t* sam_alignment)
    : qname(bam_get_qname(sam_alignment)),
      flag(sam_alignment->core.flag),
      rid(sam_alignment->core.tid),
      pos(static_cast<uint32_t>(sam_alignment->core.pos)),
      mapq(sam_alignment->core.qual),
      cigar(getCigarString(sam_alignment)),
      mate_rid(sam_alignment->core.mtid),
      mate_pos(static_cast<uint32_t>(sam_alignment->core.mpos)),
      //      tlen(sam_alignment->core.isize),
      seq(getSeqString(sam_alignment)),   // Initialized with empty char due to conversion later
      qual(getQualString(sam_alignment))  // Initialized with empty char due to conversion later
{}

// ---------------------------------------------------------------------------------------------------------------------

const std::string& SamRecord::getQname() const { return qname; }

// ---------------------------------------------------------------------------------------------------------------------

std::string&& SamRecord::moveQname() { return std::move(qname); }

// ---------------------------------------------------------------------------------------------------------------------

uint16_t SamRecord::getFlag() const { return flag; }

// ---------------------------------------------------------------------------------------------------------------------

int32_t SamRecord::getRID() const { return rid; }

// ---------------------------------------------------------------------------------------------------------------------

uint32_t SamRecord::getPos() const { return pos; }

// ---------------------------------------------------------------------------------------------------------------------

uint8_t SamRecord::getMapq() const { return mapq; }

// ---------------------------------------------------------------------------------------------------------------------

const std::string& SamRecord::getCigar() const { return cigar; }

// ---------------------------------------------------------------------------------------------------------------------

std::string&& SamRecord::moveCigar() { return std::move(cigar); }

// ---------------------------------------------------------------------------------------------------------------------

std::string SamRecord::getECigar() const { return convertCigar2ECigar(cigar, seq); }

// ---------------------------------------------------------------------------------------------------------------------

int32_t SamRecord::getMRID() const { return mate_rid; }

// ---------------------------------------------------------------------------------------------------------------------

uint32_t SamRecord::getMPos() const { return mate_pos; }

// ---------------------------------------------------------------------------------------------------------------------

const std::string& SamRecord::getSeq() const { return seq; }

// ---------------------------------------------------------------------------------------------------------------------

std::string&& SamRecord::moveSeq() { return std::move(seq); }

// ---------------------------------------------------------------------------------------------------------------------

void SamRecord::setSeq(std::string&& _seq) { seq = _seq; }

// ---------------------------------------------------------------------------------------------------------------------

void SamRecord::setSeq(const std::string& _seq) { seq = _seq; }

// ---------------------------------------------------------------------------------------------------------------------

const std::string& SamRecord::getQual() const { return qual; }

// ---------------------------------------------------------------------------------------------------------------------

std::string&& SamRecord::moveQual() { return std::move(qual); }

// ---------------------------------------------------------------------------------------------------------------------

void SamRecord::setQual(const std::string& _qual) { qual = _qual; }

// ---------------------------------------------------------------------------------------------------------------------

bool SamRecord::checkFlag(uint16_t _flag) const { return (flag & _flag) == _flag; }  // All must be set

// ---------------------------------------------------------------------------------------------------------------------

bool SamRecord::checkNFlag(uint16_t _flag) const { return (flag & _flag) == 0; }  // All must be unset

// ---------------------------------------------------------------------------------------------------------------------

bool SamRecord::isUnmapped() const { return checkFlag(BAM_FUNMAP); }

// ---------------------------------------------------------------------------------------------------------------------

bool SamRecord::isMateUnmapped() const { return checkFlag(BAM_FMUNMAP); }

// ---------------------------------------------------------------------------------------------------------------------

bool SamRecord::isPrimary() const {
    // "... satisfies ‘FLAG& 0x900 == 0’.  This line is called the primary line of the read."
    return checkNFlag(BAM_FSECONDARY | BAM_FSUPPLEMENTARY);
}

// ---------------------------------------------------------------------------------------------------------------------

bool SamRecord::isSecondary() const { return checkFlag(BAM_FSECONDARY); }

// ---------------------------------------------------------------------------------------------------------------------

bool SamRecord::isDuplicates() const { return checkFlag(BAM_FDUP); }

// ---------------------------------------------------------------------------------------------------------------------

bool SamRecord::isSupplementary() const { return checkFlag(BAM_FSUPPLEMENTARY); }

// ---------------------------------------------------------------------------------------------------------------------

bool SamRecord::isPaired() const { return checkFlag(BAM_FPAIRED); }

// ---------------------------------------------------------------------------------------------------------------------

bool SamRecord::isRead1() const { return checkFlag(BAM_FPAIRED | BAM_FREAD1); }

// ---------------------------------------------------------------------------------------------------------------------

bool SamRecord::isRead2() const { return checkFlag(BAM_FPAIRED | BAM_FREAD2); }

// ---------------------------------------------------------------------------------------------------------------------

bool SamRecord::isProperlyPaired() const { return checkFlag(BAM_FPAIRED | BAM_FPROPER_PAIR) && checkNFlag(BAM_FUNMAP); }

// ---------------------------------------------------------------------------------------------------------------------

bool SamRecord::isPairedAndBothMapped() const { return checkFlag(BAM_FPAIRED) && checkNFlag(BAM_FUNMAP | BAM_FMUNMAP); }

// ---------------------------------------------------------------------------------------------------------------------

bool SamRecord::isReverse() const { return checkFlag(BAM_FREVERSE); }

// ---------------------------------------------------------------------------------------------------------------------

bool SamRecord::isPairOf(SamRecord& r) const {
    /// Both must be paired-ended
    if (isPaired() != r.isPaired() || !isPaired()) {
        return false;
    }

    if (isPrimary() == r.isPrimary()) {
        if (isUnmapped() == r.isMateUnmapped() && r.isUnmapped() == isMateUnmapped() && getMPos() == r.getPos() &&
            r.getMPos() == getPos()) {
            /// Is pair because one of the RNEXT cannot be determined
            if (getMPos() == 0 || r.getMPos() == 0) {
                return true;
            } else {
                return getRID() == r.getMRID() && r.getRID() == getMRID() && isRead1() != r.isRead1();
            }
        }
    }

    return false;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genieapp::transcode_sam::sam::sam_to_mgrec

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
