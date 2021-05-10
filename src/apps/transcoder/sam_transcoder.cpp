#include "sam_transcoder.h"

namespace sam_transcoder {

std::tuple<bool, uint8_t> SamRecordGroup::convertFlags2Mpeg(uint16_t flags) {
    uint8_t flags_mpeg = 0;

    flags_mpeg |= (flags & BAM_FDUP) >> 10u;         // PCR / duplicate
    flags_mpeg |= (flags & BAM_FQCFAIL) >> 8u;       // vendor quality check failed
    flags_mpeg |= (flags & BAM_FPROPER_PAIR) << 1u;  // proper pair

    bool rcomp = flags & BAM_FREVERSE;  // rcomp

    return std::make_tuple(rcomp, flags_mpeg);
}

void SamRecordGroup::convertUnmapped(std::list<genie::core::record::Record> &records) {
    auto &sam_rec = data[uint8_t(Index::SINGLE_UNMAPPED)].front();

    auto flag_tuple = convertFlags2Mpeg(sam_rec.flag);
    genie::core::record::Record rec(1, genie::core::record::ClassType::CLASS_I, std::move(sam_rec.qname), "Genie",
                                    std::get<1>(flag_tuple));

    // No alignmnent
    // core::record::Alignment alignment(convertCigar2ECigar(sam_rec.getCigar(), sam_rec.getSeq()),
    //                                   sam_rec.checkFlag(Record::FlagPos::SEQ_REVERSE));

    // alignment.addMappingScore(sam_rec.getMapQ());
    // core::record::AlignmentBox alignmentContainer(sam_rec.getPos(), std::move(alignment));

    genie::core::record::Segment segment(std::move(sam_rec.seq));
//    static const std::string NO_QUAL("\0", 1);
//    if (sam_rec.mapq != 255) {
//        //        UTILS_DIE("Unmapped read has quality value!");
//        segment.addQualities(std::move(sam_rec.qual));
//    } else {
//        segment.addQualities();
//    }
    segment.addQualities(std::string("\0", 1));
    rec.addSegment(std::move(segment));

    // No alignment
    //    ret.addAlignment(ref, std::move(alignmentContainer));
    records.push_back(std::move(rec));
}

void SamRecordGroup::convertSingleEnd(
    std::list<genie::core::record::Record> &records,
    bool is_read_1_first
) {

    auto &sam_recs = data[uint8_t(Index::SINGLE_MAPPED)];
    auto sam_rec = std::move(sam_recs.front());
    sam_recs.pop_front();

    auto flag_tuple = convertFlags2Mpeg(sam_rec.flag);
    genie::core::record::Record rec(1, genie::core::record::ClassType::CLASS_I, std::move(sam_rec.qname), "Genie", std::get<1>(flag_tuple),
                             is_read_1_first);

    {
        genie::core::record::Alignment alignment(
            convertCigar2ECigar(sam_rec.cigar, sam_rec.seq),
            checkFlag(sam_rec, BAM_FREVERSE)
        );
        alignment.addMappingScore(sam_rec.mapq);

        genie::core::record::AlignmentBox alignmentContainer(sam_rec.pos, std::move(alignment));
        rec.addAlignment(sam_rec.rid, std::move(alignmentContainer));
    }

    genie::core::record::Segment segment(std::move(sam_rec.seq));
    if (sam_rec.qual != "*") {
        segment.addQualities(std::move(sam_rec.qual));
    } else {
        segment.addQualities(std::string("\0", 1));
    }
    rec.addSegment(std::move(segment));

    for (auto sam_rec_it = sam_recs.begin(); sam_rec_it != sam_recs.end(); sam_rec_it++) {
        //        UTILS_DIE_IF(sam_rec_it->isUnmapped(),
        //                     "Unaligned sam record found for qname " + sam_rec.getQname());

        genie::core::record::Alignment alignment(
            convertCigar2ECigar(sam_rec_it->cigar, sam_rec_it->seq),
            checkFlag(*sam_rec_it, BAM_FREVERSE)
        );
        alignment.addMappingScore(sam_rec_it->mapq);

        // Convert from sam 1-based mapping position to mpeg-g 0-based coordinate
        genie::core::record::AlignmentBox alignmentContainer(sam_rec.pos, std::move(alignment));

        if (unmapped_pair) {
            auto splitAlign = util::make_unique<core::record::alignment_split::Unpaired>();
            alignmentContainer.addAlignmentSplit(std::move(splitAlign));
        }

        rec.addAlignment(refs.at(sam_rec_it->getRname()), std::move(alignmentContainer));
    }

    records.push_back(std::move(rec));
}

void SamRecordGroup::convertPairedEnd(std::list<genie::core::record::Record> &records, bool force_split) {

}

SamRecordGroup::SamRecordGroup() : data(size_t(Index::TOTAL_TYPES)) {}

void SamRecordGroup::addRecord(SamRecord &&rec) {
    auto idx = Index::UNKNOWN;

    // Handles paired-end read / multi segments
    if (checkFlag(rec, 1)) {
        if (checkFlag(rec, BAM_FREAD1)) {
            idx = Index::PAIR_FIRST;
        } else if (checkFlag(rec, BAM_FREAD2)) {
            idx = Index::PAIR_LAST;
        } else {
            //            UTILS_DIE("Neither first nor last segment for Template " + rec.qname + " !");
        }

        // Handles single-end read / single segment
        // TODO (Yeremia): for linear template FIRST_SEGMENT and LAST_SEGMENT are set,
    } else if (!checkFlag(rec, BAM_FREAD1) && !checkFlag(rec, BAM_FREAD2)) {
        idx = checkFlag(rec, BAM_FUNMAP) ? Index::SINGLE_UNMAPPED : Index::SINGLE_MAPPED;

        // Should never be reached
    } else {
        //        UTILS_DIE("Not handled case found for QNAME " + rec.qname + " !");
    }

    if (isPrimary(rec)) {
        data[uint8_t(idx)].push_front(std::move(rec));
    } else {
        data[uint8_t(idx)].push_back(std::move(rec));
    }
}

bool SamRecordGroup::isUnmapped() { return data[uint8_t(Index::SINGLE_UNMAPPED)].size() == 1; }

bool SamRecordGroup::isSingle() {
    // Single non- and multiplie alignements
    return !data[uint8_t(Index::SINGLE_MAPPED)].empty() && isPrimary(data[uint8_t(Index::SINGLE_MAPPED)].front());
}

bool SamRecordGroup::isPair() {
    return (!data[uint8_t(Index::PAIR_FIRST)].empty() && isPrimary(data[uint8_t(Index::PAIR_FIRST)].front())) ||
           (!data[uint8_t(Index::PAIR_LAST)].empty() && isPrimary(data[uint8_t(Index::PAIR_LAST)].front()));
}

bool SamRecordGroup::isUnknown() { return !data[uint8_t(Index::UNKNOWN)].empty(); }

bool SamRecordGroup::isValid() {
    if (isUnknown() || data[uint8_t(Index::SINGLE_UNMAPPED)].size() > 1) {
        return false;
    } else {
        auto is_unmapped = isUnmapped();
        auto is_single = isSingle();
        auto is_pair = isPair();

        // Check if the current sam record groups belong to only one group
        return (is_unmapped && !is_single && !is_pair) || (!is_unmapped && is_single && !is_pair) ||
               (!is_unmapped && !is_single && is_pair);
    }
}

void SamRecordGroup::convert(std::list<genie::core::record::Record> &records, bool force_split) {
    if (!isValid()) {
        // TODO: Add error here
    }

    auto is_unmapped = isUnmapped();
    auto is_single = isSingle();
    auto is_pair = isPair();

    if (is_unmapped) {
        convertUnmapped(records);
    } else if (is_single) {
        convertSingleEnd(records);
    } else if (is_pair) {
        convertPairedEnd(records, force_split);
    } else {
        // UTILS_DIE("Unhandled read type found. Should never be reached!");
        // TODO: Add error here
    }
}

uint8_t sam_to_mgrec(transcoder::ProgramOptions& options){

    samFile *sam_file = hts_open(options.sam_file_path.c_str(), "r"); //open bam file
    bam_hdr_t *sam_header = sam_hdr_read(sam_file); //read header
    bam1_t *sam_alignment = bam_init1(); //initialize an alignment
    SamRecordGroup buffer;

    std::list<genie::core::record::Record> records;

    auto res = sam_read1(sam_file, sam_header, sam_alignment);
    if (res == -1){
        return 1; // return ERROR
    }
    auto sam_rec = alignment2Record(sam_alignment);
    std::string curr_qname = sam_rec.qname;
    buffer.addRecord(std::move(sam_rec));

    while(sam_read1(sam_file, sam_header, sam_alignment) != -1){
        sam_rec = sam_transcoder::alignment2Record(sam_alignment);

        if (sam_rec.qname != curr_qname){
            curr_qname = sam_rec.qname;
            buffer.convert(records);
        }
        buffer.addRecord(std::move(sam_rec));
    }

    // Destroy header
    sam_hdr_destroy(sam_header);
}

std::string SamRecordGroup::convertCigar2ECigar(const std::string &cigar, const std::string &seq) {
    std::string ecigar;
    const size_t EXPECTED_ELONGATION = 4;  // Additional braces for softclips + hardclips
    ecigar.reserve(cigar.length() + EXPECTED_ELONGATION);
    size_t seq_pos = 0;
    std::string digits;
    for (const auto &a : cigar) {
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

int SamRecordGroup::stepSequence(char token) {
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

char SamRecordGroup::convertCigar2ECigarChar(char token) {
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

}