/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include <algorithm>
#include <list>

#include <genie/core/record/alignment_split/other-rec.h>
#include <genie/core/record/alignment_split/same-rec.h>
#include <genie/core/record/alignment_split/unpaired.h>
#include "importer.h"
//#include <genie/core/record/alignment-external.h>
#include <genie/core/record/alignment_external/other-rec.h>

#include <genie/util/ordered-section.h>
//#include <record/alignment_split/other-rec.h>
//#include <record/alignment_external/other-rec.h>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace sam {

// ---------------------------------------------------------------------------------------------------------------------

Importer::Importer(size_t _blockSize, std::istream &_file) : blockSize(_blockSize), samReader(_file), lock() {

    // Allocate number of references groups
    // 1 for "unmapped"
    records_by_ref.resize(samReader.getRefs().size() + 1);
}

// ---------------------------------------------------------------------------------------------------------------------

std::tuple<bool, uint8_t> Importer::convertFlags2Mpeg(uint16_t flags) {
    uint8_t flags_mpeg = 0;
    bool rcomp = false;
    flags_mpeg |= (flags & 1024u) >> 10u;  // PCR / duplicate
    flags_mpeg |= (flags & 512u) >> 8u;    // vendor quality check failed
    flags_mpeg |= (flags & 2u) << 1u;      // proper pair

    rcomp = flags & 16u;  // rcomp

    return std::make_tuple(rcomp, flags_mpeg);
}

// ---------------------------------------------------------------------------------------------------------------------

char Importer::convertCigar2ECigarChar(char token) {
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

int Importer::stepSequence(char token) {
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

std::string Importer::convertCigar2ECigar(const std::string &cigar, const std::string &seq) {
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

// ---------------------------------------------------------------------------------------------------------------------

bool Importer::good() {
    if (!samReader.good()){
        return anyRemainingMpeggRecord();
    } else{
        return true;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

bool Importer::anyRemainingMpeggRecord() {
    for (auto &iter: records_by_ref){
        if (!iter.empty()){
            return true;
        }
    }
    return false;
}

// ---------------------------------------------------------------------------------------------------------------------

bool Importer::anyGroupReachBlockSize() {
    for (auto &iter: records_by_ref){
        if (iter.size() > blockSize){
            return true;
        }
    }
    return false;
}

// ---------------------------------------------------------------------------------------------------------------------

/*bool Importer::pump(uint64_t& id, std::mutex& lock) {
    std::string line;
    std::list<std::string> lines;
    std::list<ReadTemplate> read_templates;
    core::record::Chunk chunk;
    {
        util::OrderedSection section(&lock, id);
        while (samReader.good() && blockSize > lines.size()){
            samReader.read(lines);
        }
    }

    rtg.addRecords(lines);
    lines.clear();
    rtg.getTemplates(read_templates);

    for (auto & read_template:read_templates){
        convert(chunk, read_template, samReader.getRefs());
    }

    if (!chunk.getData().empty()) {
        flowOut(std::move(chunk), id);
    }

    // Break if less than blockSize records were read from the SAM file
    return samReader.good();
}*/

bool Importer::pumpRetrieve(core::Classifier *_classifier) {
    std::string line;
    std::list<std::string> lines;
    std::list<ReadTemplate> read_templates;
    core::record::Chunk chunk;
    std::list<core::record::Record> recs;

    // Do until the number of MPEG-G records for certain reference is above blockSize
    // or end of file is reached
    while (samReader.good() && anyGroupReachBlockSize()){
        while (samReader.good() && blockSize > lines.size()) {
            samReader.read(lines);
        }

        rtg.addRecords(lines);
        lines.clear();
        rtg.getTemplates(read_templates);

        for (auto &read_template : read_templates) {
            convert(recs, read_template, samReader.getRefs());
            for (auto& rec: recs){

                //TODO: Create if case for unmapped MPEG-G records

                // Store in its respective list
                records_by_ref[rec.getAlignmentSharedData().getSeqID()].push_back(std::move(rec));
            }
        }
        read_templates.clear();
    }

    for (auto& iter: records_by_ref){
        if (iter.size() > blockSize || !samReader.good()){
            std::move(iter.begin(), iter.end(), std::back_inserter(chunk.getData()));
            break;
        }
    }

    auto &records = chunk.getData();
    std::sort(records.begin(), records.end(), compare);

    for (const auto &record : records) {
        if (record.getAlignments().empty()) {
            continue;
        }

        auto minPos = getMinPos(record);

        UTILS_DIE_IF(minPos < last_position, "Records not properly ordered");
        last_position = minPos;
    }

    if (!chunk.getData().empty()) {
        _classifier->add(std::move(chunk));
    }
    return samReader.good();
}

// ---------------------------------------------------------------------------------------------------------------------

void Importer::flushIn(uint64_t &pos) { flushOut(pos); }

// ---------------------------------------------------------------------------------------------------------------------
std::unique_ptr<core::record::Record> Importer::convertSam2SameRec(Record &sam_r1, Record &sam_r2,
                                                                   std::map<std::string, size_t> &refs) {
    auto flag_tuple = convertFlags2Mpeg(sam_r1.getFlags());

    auto rec = util::make_unique<core::record::Record>(2, core::record::ClassType::CLASS_I, sam_r1.moveQname(), "Genie",
                                                       std::get<1>(flag_tuple), sam_r1.getPos() < sam_r2.getPos());

    core::record::Segment segment(sam_r1.moveSeq());
    if (sam_r1.getQual() != "*") {
        segment.addQualities(sam_r1.moveQual());
    } else {
        segment.addQualities(std::string("\0", 1));
    }
    rec->addSegment(std::move(segment));

    core::record::Segment segment2(sam_r2.moveSeq());
    if (sam_r2.getQual() != "*") {
        segment2.addQualities(sam_r2.moveQual());
    } else {
        segment2.addQualities(std::string("\0", 1));
    }
    rec->addSegment(std::move(segment2));

    addAlignmentToSameRec(rec, sam_r1, sam_r2, refs);

    return rec;
}
// ---------------------------------------------------------------------------------------------------------------------
void Importer::addAlignmentToSameRec(std::unique_ptr<core::record::Record> &rec, Record &sam_r1, Record &sam_r2,
                                     std::map<std::string, size_t> &refs) {
    UTILS_DIE_IF(sam_r1.getRname() != sam_r2.getRname(), "Different reference sequence");

    core::record::Alignment alignment(convertCigar2ECigar(sam_r1.getCigar(), sam_r1.getSeq()),
                                      sam_r1.checkFlag(Record::FlagPos::SEQ_REVERSE));
    alignment.addMappingScore(sam_r1.getMapQ());

    // Convert from sam 1-based mapping position to mpeg-g 0-based coordinate
    auto sam_r1_pos = sam_r1.getPos() - 1;
    core::record::AlignmentBox alignmentContainer(sam_r1_pos, std::move(alignment));

    core::record::Alignment alignment2(convertCigar2ECigar(sam_r2.getCigar(), sam_r2.getSeq()),
                                       sam_r2.checkFlag(Record::FlagPos::SEQ_REVERSE));
    alignment2.addMappingScore(sam_r2.getMapQ());

    auto delta = (int64_t)sam_r2.getPos() - (int64_t)sam_r1.getPos();
    auto splitAlign = util::make_unique<core::record::alignment_split::SameRec>(delta, std::move(alignment2));
    alignmentContainer.addAlignmentSplit(std::move(splitAlign));

    rec->addAlignment(refs.at(sam_r1.getRname()), std::move(alignmentContainer));
}
// ---------------------------------------------------------------------------------------------------------------------
void Importer::convertPairedEndNoSplit(std::list<core::record::Record> &records, SamRecords2D &sam_recs_2d,
                                       std::map<std::string, size_t> &refs) {
    /// Swap template so that the primary alignment of the first segment always has lower mapping position
    /// compared to primary alignment of the second segment
    if (sam_recs_2d.front().front().getPos() > sam_recs_2d.back().front().getPos()){
        std::swap(sam_recs_2d.front(), sam_recs_2d.back());
    }

    auto sam_r1_iter = sam_recs_2d.front().begin();
    auto sam_r2_iter = sam_recs_2d.back().begin();

    auto sam_r1_end = sam_recs_2d.front().end();
    auto sam_r2_end = sam_recs_2d.back().end();

    //    if (sam_r1_iter->getPos() > sam_r2_iter->getPos()){
    //        auto tmp_iter = sam_r2_iter;
    //        sam_r2_iter = sam_r1_iter;
    //        sam_r1_iter = tmp_iter;
    //
    //        tmp_iter = sam_r2_end;
    //        sam_r2_end = sam_r1_end;
    //        sam_r1_end = tmp_iter;
    //    }

    // Cache SEQ of read 1 & 2 to replace SEQ of other alignment if the value is '*'
    // See "Recommended Practice for SAM Format" in SAM Format documentation for more information
    auto sam_r1_seq = sam_r1_iter != sam_r1_end ? sam_r1_iter->getSeq() : "*";
    auto sam_r2_seq = sam_r2_iter != sam_r2_end ? sam_r2_iter->getSeq() : "*";

    auto rec = convertSam2SameRec(*sam_r1_iter, *sam_r2_iter, refs);

    sam_r1_iter++;
    sam_r2_iter++;

    while (sam_r1_iter != sam_recs_2d.front().end() && sam_r2_iter != sam_recs_2d.back().end()) {
        UTILS_DIE_IF(sam_r1_iter->getRname() != sam_r2_iter->getRname(), "Read1 and Read2 have different RNAME");
        UTILS_DIE_IF(!sam_r1_iter->isPairOf(*sam_r2_iter), "Not read 1 is not pair of read 2");

        if (rec->getAlignmentSharedData().getSeqID() != refs.at(sam_r1_iter->getRname())) {
            // Add more_alignment_info
            auto leftmost_read = sam_r1_iter->getPos() < sam_r2_iter->getPos() ? sam_r1_iter : sam_r2_iter;
            auto leftmost_read_pos = leftmost_read->getPos() - 1;
            auto more_alignment_info = util::make_unique<core::record::alignment_external::OtherRec>(
                leftmost_read_pos, refs.at(leftmost_read->getRname()));
            rec->setMoreAlignmentInfo(std::move(more_alignment_info));

            records.push_back(std::move(*rec));

            // Copy SEQ from the primary line due to SEQ and QUAL are set to "*"
            // See "Recommended Practice for SAM Format" in SAM Format documentation for more information
            if (sam_r1_iter->getSeq() == "*") {
                sam_r1_iter->setSeq(sam_r1_seq);
            }
            if (sam_r2_iter->getSeq() == "*") {
                sam_r2_iter->setSeq(sam_r2_seq);
            }

            rec = convertSam2SameRec(*sam_r1_iter, *sam_r2_iter, refs);
        } else {
            addAlignmentToSameRec(rec, *sam_r1_iter, *sam_r2_iter, refs);
        }
        sam_r1_iter++;
        sam_r2_iter++;
    }

    UTILS_DIE_IF(rec == nullptr, "Empty MPEG-G record");
    records.push_back(std::move(*rec));

    sam_recs_2d.front().clear();
    sam_recs_2d.back().clear();
}
// ---------------------------------------------------------------------------------------------------------------------
std::unique_ptr<core::record::Record> Importer::convertSam2SplitRec(Record &sam_r1, Record *sam_r2_ptr,
                                                                    std::map<std::string, size_t> &refs) {
    auto flag_tuple = convertFlags2Mpeg(sam_r1.getFlags());

    auto rec = util::make_unique<core::record::Record>(2, core::record::ClassType::CLASS_I, sam_r1.moveQname(), "Genie",
                                                       std::get<1>(flag_tuple), sam_r1.isFirstRead());

    core::record::Segment segment(sam_r1.moveSeq());
    if (sam_r1.getQual() != "*") {
        segment.addQualities(sam_r1.moveQual());
    } else {
        segment.addQualities(std::string("\0", 1));
    }
    rec->addSegment(std::move(segment));

    addAlignmentToSplitRec(rec, sam_r1, sam_r2_ptr, refs);

    return rec;
}
// ---------------------------------------------------------------------------------------------------------------------
void Importer::addAlignmentToSplitRec(std::unique_ptr<core::record::Record> &rec, Record &sam_r1, Record *sam_r2_ptr,
                                      std::map<std::string, size_t> &refs) {
    if (!sam_r1.isUnmapped()) {
        core::record::Alignment alignment(convertCigar2ECigar(sam_r1.getCigar(), sam_r1.getSeq()),
                                          sam_r1.checkFlag(Record::FlagPos::SEQ_REVERSE));
        alignment.addMappingScore(sam_r1.getMapQ());

        // Convert from sam 1-based mapping position to mpeg-g 0-based coordinate
        auto sam_r1_pos = sam_r1.getPos() - 1;
        core::record::AlignmentBox alignmentContainer(sam_r1_pos, std::move(alignment));

        if (sam_r2_ptr == nullptr || sam_r2_ptr->isUnmapped()) {
            auto splitAlign = util::make_unique<core::record::alignment_split::Unpaired>();
            alignmentContainer.addAlignmentSplit(std::move(splitAlign));
        } else {
            auto sam_r2_pos = sam_r2_ptr->getPos() - 1;
            auto splitAlign =
                util::make_unique<core::record::alignment_split::OtherRec>(sam_r2_pos, refs.at(sam_r2_ptr->getRname()));
            alignmentContainer.addAlignmentSplit(std::move(splitAlign));
        }

        rec->addAlignment(refs.at(sam_r1.getRname()), std::move(alignmentContainer));
    }
}
// ---------------------------------------------------------------------------------------------------------------------
void Importer::convertPairedEndSplitPair(std::list<core::record::Record> &records, SamRecords2D &sam_recs_2d,
                                         std::map<std::string, size_t> &refs) {

    auto sam_r1_iter = sam_recs_2d.front().begin();
    auto sam_r2_iter = sam_recs_2d.back().begin();

    auto sam_r1_end = sam_recs_2d.front().end();
    auto sam_r2_end = sam_recs_2d.back().end();

    std::unique_ptr<core::record::Record> rec_1, rec_2;

    // Cache SEQ of read 1 & 2 to replace SEQ of other alignment if the value is '*'
    // See "Recommended Practice for SAM Format" in SAM Format documentation for more information
    auto sam_r1_seq = sam_r1_iter != sam_r1_end ? sam_r1_iter->getSeq() : "*";
    auto sam_r2_seq = sam_r2_iter != sam_r2_end ? sam_r2_iter->getSeq() : "*";

    while (sam_r1_iter != sam_r1_end || sam_r2_iter != sam_r2_end) {

        // Handle Read 1
        if (sam_r1_iter != sam_r1_end) {
            auto *sam_r2_ptr = sam_r2_iter != sam_r2_end ? &(*sam_r2_iter) : nullptr;

            if (rec_1) {
                if (rec_1->getAlignmentSharedData().getSeqID() != refs.at(sam_r1_iter->getRname())) {
                    // Add more_alignment_info
                    //                    auto leftmost_read = sam_r1_iter->getPos() < sam_r2_iter->getPos() ?
                    //                    sam_r1_iter : sam_r2_iter;
                    auto leftmost_read = sam_r1_iter;
                    auto leftmost_read_pos = leftmost_read->getPos() - 1;
                    auto more_alignment_info = util::make_unique<core::record::alignment_external::OtherRec>(
                        leftmost_read_pos, refs.at(leftmost_read->getRname()));
                    rec_1->setMoreAlignmentInfo(std::move(more_alignment_info));

                    //template_chunk.getData().push_back(std::move(*rec_1));
                    records.push_back(std::move(*rec_1));

                    // Retrieve back Sequence from the primary line due to SEQ and QUAL are set to "*"
                    // See "Recommended Practice for SAM Format" in SAM Format documentation for more information
                    if (sam_r1_iter->getSeq() == "*") {
                        sam_r1_iter->setSeq(sam_r1_seq);
                    }

                    rec_1 = convertSam2SplitRec(*sam_r1_iter, sam_r2_ptr, refs);
                } else {
                    addAlignmentToSplitRec(rec_1, *sam_r1_iter, sam_r2_ptr, refs);
                }
            } else {
                rec_1 = convertSam2SplitRec(*sam_r1_iter, sam_r2_ptr, refs);
            }
        }

        // Handle Read 2
        if (sam_r2_iter != sam_r2_end) {
            auto *sam_r1_ptr = sam_r1_iter != sam_r1_end ? &(*sam_r1_iter) : nullptr;

            if (rec_2) {
                if (rec_2->getAlignmentSharedData().getSeqID() != refs.at(sam_r2_iter->getRname())) {
                    // Add more_alignment_info
                    //                    auto leftmost_read = sam_r1_iter->getPos() < sam_r2_iter->getPos() ?
                    //                    sam_r1_iter : sam_r2_iter;
                    auto leftmost_read = sam_r2_iter;
                    auto leftmost_read_pos = leftmost_read->getPos() - 1;
                    auto more_alignment_info = util::make_unique<core::record::alignment_external::OtherRec>(
                        leftmost_read_pos, refs.at(leftmost_read->getRname()));
                    rec_2->setMoreAlignmentInfo(std::move(more_alignment_info));

                    //template_chunk.getData().push_back(std::move(*rec_2));
                    records.push_back(std::move(*rec_2));

                    // Retrieve back Sequence from the primary line due to SEQ and QUAL are set to "*"
                    // See "Recommended Practice for SAM Format" in SAM Format documentation for more information
                    if (sam_r2_iter->getSeq() == "*") {
                        sam_r2_iter->setSeq(sam_r2_seq);
                    }

                    rec_2 = convertSam2SplitRec(*sam_r2_iter, sam_r1_ptr, refs);
                } else {
                    addAlignmentToSplitRec(rec_2, *sam_r2_iter, sam_r1_ptr, refs);
                }
            } else {
                rec_2 = convertSam2SplitRec(*sam_r2_iter, sam_r1_ptr, refs);
            }
        }
        sam_r1_iter++;
        sam_r2_iter++;
    }

    if (rec_1 != nullptr) {
        //template_chunk.getData().push_back(std::move(*rec_1));
        records.push_back(std::move(*rec_1));
    }

    if (rec_2 != nullptr) {
        //template_chunk.getData().push_back(std::move(*rec_2));
        records.push_back(std::move(*rec_2));
    }

    sam_recs_2d.front().clear();
    sam_recs_2d.back().clear();
}

// ---------------------------------------------------------------------------------------------------------------------

void Importer::convertUnmapped(std::list<core::record::Record> &records, SamRecords &sam_recs, std::map<std::string, size_t> &refs) {
    auto &sam_rec = sam_recs.front();

    auto flag_tuple = convertFlags2Mpeg(sam_rec.getFlags());
    core::record::Record rec(1, core::record::ClassType::CLASS_I, sam_rec.moveQname(), "Genie",
                             std::get<1>(flag_tuple));

    // No alignmnent
    // core::record::Alignment alignment(convertCigar2ECigar(sam_rec.getCigar(), sam_rec.getSeq()),
    //                                   sam_rec.checkFlag(Record::FlagPos::SEQ_REVERSE));

    // alignment.addMappingScore(sam_rec.getMapQ());
    // core::record::AlignmentBox alignmentContainer(sam_rec.getPos(), std::move(alignment));

    core::record::Segment segment(sam_rec.moveSeq());
    if (sam_rec.getQual() != "*") {
        //        UTILS_DIE("Unmapped read has quality value!");
        segment.addQualities(sam_rec.moveQual());
    } else {
        segment.addQualities("");
    }
    rec.addSegment(std::move(segment));

    // No alignment
    //    ret.addAlignment(ref, std::move(alignmentContainer));
    records.push_back(std::move(rec));

    sam_recs.clear();
}

// ---------------------------------------------------------------------------------------------------------------------

void Importer::convertSingleEnd(std::list<core::record::Record> &records, SamRecords &sam_recs, std::map<std::string, size_t> &refs,
                                bool unmapped_pair, bool is_read_1_first) {
    auto sam_rec = std::move(sam_recs.front());
    sam_recs.pop_front();

    auto flag_tuple = convertFlags2Mpeg(sam_rec.getFlags());
    core::record::Record rec(1, core::record::ClassType::CLASS_I, sam_rec.moveQname(), "Genie", std::get<1>(flag_tuple),
                             is_read_1_first);

    {
        core::record::Alignment alignment(convertCigar2ECigar(sam_rec.getCigar(), sam_rec.getSeq()),
                                          sam_rec.checkFlag(Record::FlagPos::SEQ_REVERSE));
        alignment.addMappingScore(sam_rec.getMapQ());

        auto pos_zero_base = sam_rec.getPos() - 1;
        core::record::AlignmentBox alignmentContainer(pos_zero_base, std::move(alignment));
        rec.addAlignment(refs.at(sam_rec.getRname()), std::move(alignmentContainer));
    }

    core::record::Segment segment(sam_rec.moveSeq());
    if (sam_rec.getQual() != "*") {
        segment.addQualities(sam_rec.moveQual());
    } else {
        segment.addQualities("");
    }
    rec.addSegment(std::move(segment));

    for (auto sam_rec_it = ++sam_recs.begin(); sam_rec_it != sam_recs.end(); sam_rec_it++) {
        //        UTILS_DIE_IF(sam_rec_it->isUnmapped(),
        //                     "Unaligned sam record found for qname " + sam_rec.getQname());

        core::record::Alignment alignment(convertCigar2ECigar(sam_rec_it->getCigar(), sam_rec_it->getSeq()),
                                          sam_rec_it->checkFlag(Record::FlagPos::SEQ_REVERSE));
        alignment.addMappingScore(sam_rec_it->getMapQ());

        // Convert from sam 1-based mapping position to mpeg-g 0-based coordinate
        auto pos_zero_base = sam_rec_it->getPos() - 1;
        core::record::AlignmentBox alignmentContainer(pos_zero_base, std::move(alignment));

        if (unmapped_pair) {
            auto splitAlign = util::make_unique<core::record::alignment_split::Unpaired>();
            alignmentContainer.addAlignmentSplit(std::move(splitAlign));
        }

        rec.addAlignment(refs.at(sam_rec_it->getRname()), std::move(alignmentContainer));
    }

    records.push_back(std::move(rec));
}
// ---------------------------------------------------------------------------------------------------------------------
void Importer::convertPairedEnd(std::list<core::record::Record> &records, SamRecords2D &sam_recs_2d,
                                std::map<std::string, size_t> &refs, bool force_split) {

    auto &read_1 = sam_recs_2d.front();
    auto read_1_empty = read_1.empty();
    auto read_1_unmapped = false;
    auto read_1_primary = false;
    if (!read_1_empty) {
        read_1_primary = read_1.front().isPrimaryLine();
        read_1_unmapped = read_1.front().isUnmapped();
    }
    auto read_1_ok = !read_1_empty && read_1_primary && !read_1_unmapped;

    auto &read_2 = sam_recs_2d.back();
    auto read_2_empty = read_2.empty();
    auto read_2_unmapped = false;
    auto read_2_primary = false;
    if (!read_2_empty) {
        read_2_unmapped = read_2.front().isUnmapped();
        read_2_primary = read_2.front().isPrimaryLine();
    }
    auto read_2_ok = !read_2_empty && read_2_primary && !read_2_unmapped;

    if (read_1_ok && read_2_ok) {
        UTILS_DIE_IF(!read_1.front().isPairOf(read_2.front()), "read_1 is not pair of read_2");

        auto create_split_records = false;

        // Check whether read_1 has the same number of alignments as read_2 and the references are the same
        if (read_1.size() == read_2.size()) {
            auto read_1_it = read_1.begin();
            auto read_2_it = read_2.begin();
            auto any_different_ref = false;
            while (read_1_it != read_1.end() && read_2_it != read_2.end()) {
                if (read_1_it->getRname() != read_2_it->getRname()) {
                    any_different_ref = true;
                    break;
                }

                read_1_it++;
                read_2_it++;
            }

            create_split_records = any_different_ref;
        }

        if (create_split_records || force_split) {
//            // Swap template so that the first segment has lower mapping position
//            if (sam_recs_2d.front().front().getPos() > sam_recs_2d.front().back().getPos()){
//                std::swap(sam_recs_2d.front().front(), sam_recs_2d.front().back());
//            }

            convertPairedEndSplitPair(records, sam_recs_2d, refs);
        } else {
//            // Swap template so that the first segment has lower mapping position
//            if (sam_recs_2d.front().front().getPos() > sam_recs_2d.front().back().getPos()){
//                std::swap(sam_recs_2d.front().front(), sam_recs_2d.front().back());
//            }

            convertPairedEndNoSplit(records, sam_recs_2d, refs);
        }

        // Handle alignments where one of the reads is not complete (unpaired etc)
    } else {
        if (!read_1_empty) {
            if (read_1_unmapped) {
                convertUnmapped(records, read_1, refs);
            } else if (!read_1_primary) {
                UTILS_DIE("Cannot find the primary line of read_1!");
            } else if (read_1_ok) {
                convertSingleEnd(records, read_1, refs, true, true);
            }
        }

        if (!read_2_empty) {
            if (read_2_unmapped) {
                convertUnmapped(records, read_2, refs);
            } else if (!read_2_primary) {
                UTILS_DIE("Cannot find the primary line of read_2!");
            } else if (read_2_ok) {
                convertSingleEnd(records, read_2, refs, true, false);
            }
        }
    }
    sam_recs_2d.clear();
}
// ---------------------------------------------------------------------------------------------------------------------
void Importer::convert(std::list<core::record::Record> &records, ReadTemplate &rt, std::map<std::string, size_t> &refs,
                       bool force_split) {

    UTILS_DIE_IF(!rt.isValid(), "Invalid Read Template. Neither unmapped, single-end nor paired-end");

    auto is_unmapped = rt.isUnmapped();
    auto is_single = rt.isSingle();
    auto is_pair = rt.isPair();

    SamRecords2D sam_recs_2d;
    rt.getRecords(sam_recs_2d);

    // Convert SAM records to MPEG-G record(s)
    if (is_unmapped) {
        convertUnmapped(records, sam_recs_2d.front(), refs);
    } else if (is_single) {
        convertSingleEnd(records, sam_recs_2d.front(), refs);
    } else if (is_pair) {
        convertPairedEnd(records, sam_recs_2d, refs, force_split);
    } else {
        UTILS_DIE("Unhandled read type found. Should never be reached!");
    }
}

// ---------------------------------------------------------------------------------------------------------------------

bool Importer::compare(const core::record::Record &r1, const core::record::Record &r2) {

    if (r1.getAlignments().empty()) {
        return false;
    }
    if(r2.getAlignments().empty()) {
        return true;
    }
    return getMinPos(r1) < getMinPos(r2);
}

uint64_t Importer::getMinPos(const core::record::Record& r) {
    uint64_t abs_pos = r.getAlignments().front().getPosition();  // pivot
    uint64_t first_pos = abs_pos;                                     // lowest locus

    for (const auto &split : r.getAlignments().front().getAlignmentSplits()) {
        UTILS_DIE_IF(split->getType() != core::record::AlignmentSplit::Type::SAME_REC, "Only same rec split alignments supported");
        first_pos = std::min(
            first_pos, abs_pos + dynamic_cast<const core::record::alignment_split::SameRec &>(*split).getDelta());
    }
    return first_pos;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace sam
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------