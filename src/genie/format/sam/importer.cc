/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "importer.h"
#include <genie/core/record/alignment_split/same-rec.h>
#include <genie/core/record/alignment_split/other-rec.h>
#include <genie/core/record/alignment_split/unpaired.h>
#include <genie/core/record/alignment_external/other-rec.h>

#include <genie/util/ordered-section.h>
//#include <record/alignment_split/other-rec.h>
//#include <record/alignment_external/other-rec.h>
#include <list>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace sam {

// ---------------------------------------------------------------------------------------------------------------------

Importer::Importer(size_t _blockSize, std::istream &_file)
    : blockSize(_blockSize), samReader(_file), lock(){
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

core::record::Record Importer::convert(uint16_t ref, sam::Record &&_r1, sam::Record *_r2) {
//    sam::Record r1 = std::move(_r1);
//    auto flag_tuple = convertFlags2Mpeg(r1.getFlags());
//    core::record::Record ret(_r2 ? 2 : 1, core::record::ClassType::CLASS_I, r1.moveQname(), "Genie",
//                             std::get<1>(flag_tuple));
//
//    core::record::Alignment alignment(convertCigar2ECigar(r1.getCigar(), r1.getSeq()),
//                                      r1.checkFlag(Record::FlagPos::SEQ_REVERSE));
//    alignment.addMappingScore(r1.getMapQ());
//    core::record::AlignmentBox alignmentContainer(r1.getPos(), std::move(alignment));
//
//    core::record::Segment segment(r1.moveSeq());
//    if (r1.getQual() != "*") {
//        segment.addQualities(r1.moveQual());
//    }
//    ret.addSegment(std::move(segment));
//
//    if (_r2) {
//        sam::Record r2 = std::move(*_r2);
//
//        auto ecigar = convertCigar2ECigar(r2.getCigar(), r2.getSeq());
//        core::record::Alignment alignment2(std::move(ecigar), r2.checkFlag(Record::FlagPos::SEQ_REVERSE));
//        alignment2.addMappingScore(r2.getMapQ());
//
//        core::record::Segment segment2(r2.moveSeq());
//        if (r2.getQual() != "*") {
//            segment2.addQualities(r2.moveQual());
//        }
//        ret.addSegment(std::move(segment2));
//
//        auto splitAlign =
//            util::make_unique<core::record::alignment_split::SameRec>(r2.getPos() - r1.getPos(), std::move(alignment2));
//        alignmentContainer.addAlignmentSplit(std::move(splitAlign));
//    } else {
//    }
//
//    ret.addAlignment(ref, std::move(alignmentContainer));  // TODO
//    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

bool Importer::pump(size_t id) {
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

    if (!chunk.empty()) {
        flowOut(std::move(chunk), id);
    }

    // Break if less than blockSize records were read from the SAM file
    return samReader.good();

//    core::record::Chunk chunk;
//    std::vector<sam::Record> s;
//    std::list<sam::Record> samRecords;
//    uint16_t local_ref_num = 0;
//    {
//        util::OrderedSection section(&lock, id);
//        samReader.read(blockSize, s);
//        auto it = refs.find(s.front().getRname());
//        if (it == refs.end()) {
//            local_ref_num = ref_counter;
//            refs.insert(std::make_pair(s.front().getRname(), ref_counter++));
//        } else {
//            local_ref_num = it->second;
//        }
//    }
//    std::copy(s.begin(), s.end(), std::back_inserter(samRecords));
//
//    std::cout << "Read " << samRecords.size() << " SAM record(s) for access unit " << id << std::endl;
//    size_t skipped = 0;
//    while (!samRecords.empty()) {
//        sam::Record samRecord = std::move(samRecords.front());
//        samRecords.erase(samRecords.begin());
//        // Search for mate
//        const std::string &rnameSearchString =
//            samRecord.getRnext() == "=" ? samRecord.getRname() : samRecord.getRnext();
//        auto mate = samRecords.begin();
//        mate = samRecords.end();  // Disable pairs for now TODO: implement
//        if (samRecord.getPnext() == samRecord.getPos() && samRecord.getRname() == rnameSearchString) {
//            mate = samRecords.end();
//        }
//        for (; mate != samRecords.end(); ++mate) {
//            if (mate->getRname() == rnameSearchString && mate->getPos() == samRecord.getPnext()) {
//                // LOG_TRACE << "Found mate";
//                break;
//            }
//        }
//        if (mate == samRecords.end()) {
//            // LOG_TRACE << "Did not find mate";
//            if ((samRecord.getFlags() & (1u << uint16_t(Record::FlagPos::SEGMENT_UNMAPPED))) ||
//                samRecord.getCigar() == "*" || samRecord.getPos() == 0 || samRecord.getRname() == "*") {
//                skipped++;
//            } else {
//                chunk.emplace_back(convert(local_ref_num, std::move(samRecord), nullptr));
//            }
//        } else {
//            // TODO: note the filtering of unaligned reads above. Move this to the encoder
//            chunk.emplace_back(convert(local_ref_num, std::move(samRecord), &*mate));
//            samRecords.erase(mate);
//        }
//    }
//    if (skipped) {
//        std::cerr << "Skipped " << skipped << " unmapped reads! Those are currently not supported." << std::endl;
//    }
//    if (!chunk.empty()) {
//        flowOut(std::move(chunk), id);
//    }
//
//    // Break if less than blockSize records were read from the SAM file
//    return !samReader.isEnd();
}

// ---------------------------------------------------------------------------------------------------------------------

void Importer::dryIn() { dryOut(); }

// ---------------------------------------------------------------------------------------------------------------------
std::unique_ptr<core::record::Record> Importer::convertSam2SameRec(Record &sam_r1, Record &sam_r2,
                                                  std::map<std::string, size_t> &refs) {

    auto flag_tuple = convertFlags2Mpeg(sam_r1.getFlags());

    auto rec = util::make_unique<core::record::Record>(2, core::record::ClassType::CLASS_I, sam_r1.moveQname(),
                             "Genie",std::get<1>(flag_tuple),
                             sam_r1.getPos() < sam_r2.getPos());

    core::record::Segment segment(sam_r1.moveSeq());
    if (sam_r1.getQual() != "*") {
        segment.addQualities(sam_r1.moveQual());
    }
    rec->addSegment(std::move(segment));

    core::record::Segment segment2(sam_r2.moveSeq());
    if (sam_r2.getQual() != "*") {
        segment2.addQualities(sam_r2.moveQual());
    }
    rec->addSegment(std::move(segment2));

    addAlignmentToSameRec(rec, sam_r1, sam_r2, refs);

    return rec;
}
// ---------------------------------------------------------------------------------------------------------------------
void Importer::addAlignmentToSameRec(std::unique_ptr<core::record::Record>& rec, Record& sam_r1, Record& sam_r2,
                                     std::map<std::string, size_t>& refs) {

    UTILS_DIE_IF(sam_r1.getRname() != sam_r2.getRname(), "Different reference sequence");

    core::record::Alignment alignment(convertCigar2ECigar(sam_r1.getCigar(), sam_r1.getSeq()),
                                      sam_r1.checkFlag(Record::FlagPos::SEQ_REVERSE));
    alignment.addMappingScore(sam_r1.getMapQ());

    core::record::AlignmentBox alignmentContainer(sam_r1.getPos(), std::move(alignment));

    core::record::Alignment alignment2(convertCigar2ECigar(sam_r2.getCigar(), sam_r2.getSeq()),
                                       sam_r2.checkFlag(Record::FlagPos::SEQ_REVERSE));
    alignment2.addMappingScore(sam_r2.getMapQ());

    auto splitAlign = util::make_unique<core::record::alignment_split::SameRec>(sam_r2.getPos() - sam_r1.getPos(), std::move(alignment2));
    alignmentContainer.addAlignmentSplit(std::move(splitAlign));

    rec->addAlignment(refs.at(sam_r1.getRname()), std::move(alignmentContainer));
}
// ---------------------------------------------------------------------------------------------------------------------
void Importer::convertPairedEndNoSplit(core::record::Chunk &template_chunk, SamRecords2D &sam_recs_2d, std::map<std::string, size_t>& refs) {

    // Chunk has to contain only records from one template
    template_chunk.clear();

    auto sam_r1_ptr = sam_recs_2d.front().begin();
    auto sam_r2_ptr = sam_recs_2d.back().begin();

    auto rec = convertSam2SameRec(*sam_r1_ptr, *sam_r2_ptr, refs);

    sam_r1_ptr++;
    sam_r2_ptr++;

    while (sam_r1_ptr != sam_recs_2d.front().end() && sam_r2_ptr != sam_recs_2d.back().end()){
        UTILS_DIE_IF(sam_r1_ptr->getRname() != sam_r2_ptr->getRname(), "Read1 and Read2 have different RNAME");
        UTILS_DIE_IF(!sam_r1_ptr->isPairOf(*sam_r2_ptr), "Not read 1 is not pair of read 2");

        if (rec->getAlignmentSharedData().getSeqID() != refs.at(sam_r1_ptr->getRname())){
            // Add more_alignment_info
            auto leftmost_read = sam_r1_ptr->getPos() < sam_r1_ptr->getPos() ? sam_r1_ptr : sam_r2_ptr;
//            auto more_alignment_info = util::make_unique<core::record::alignment_external::OtherRec>(
//                leftmost_read->getPos(), refs.at(leftmost_read->getRname()));
//            rec->setMoreAlignmentInfo(std::move(more_alignment_info));

            template_chunk.push_back(std::move(*rec));

            // Retrieve back Sequence from the primary line due to SEQ and QUAL are set to "*"
            // See "Recommended Practice for SAM Format" in SAM Format documentation for more information
            if (sam_r1_ptr->getSeq() == "*"){
                auto r1_seq = template_chunk.front().getSegments().front().getSequence();
                sam_r1_ptr->setSeq(r1_seq);
            }
            if (sam_r2_ptr->getSeq() == "*"){
                auto r2_seq = template_chunk.front().getSegments().back().getSequence();
                sam_r2_ptr->setSeq(r2_seq);
            }

            rec = convertSam2SameRec(*sam_r1_ptr, *sam_r2_ptr, refs);
        } else {
            addAlignmentToSameRec(rec, *sam_r1_ptr, *sam_r2_ptr, refs);
        }
        sam_r1_ptr++;
        sam_r2_ptr++;
    }

    UTILS_DIE_IF(rec == nullptr, "Empty MPEG-G record");
    template_chunk.push_back(std::move(*rec));

    sam_recs_2d.front().clear();
    sam_recs_2d.back().clear();
}
// ---------------------------------------------------------------------------------------------------------------------
std::unique_ptr<core::record::Record> Importer::convertSam2SplitRec(Record &sam_r1, Record *sam_r2_ptr, std::map<std::string, size_t> &refs) {

    auto flag_tuple = convertFlags2Mpeg(sam_r1.getFlags());

    auto rec = util::make_unique<core::record::Record>(2, core::record::ClassType::CLASS_I, sam_r1.moveQname(),
                           "Genie",std::get<1>(flag_tuple), sam_r1.isFirstRead());

    core::record::Segment segment(sam_r1.moveSeq());
    if (sam_r1.getQual() != "*") {
        segment.addQualities(sam_r1.moveQual());
    }
    rec->addSegment(std::move(segment));

    addAlignmentToSplitRec(rec, sam_r1, sam_r2_ptr, refs);

    return rec;
}
// ---------------------------------------------------------------------------------------------------------------------
void Importer::addAlignmentToSplitRec(std::unique_ptr<core::record::Record> &rec, Record &sam_r1,
                                      Record *sam_r2_ptr, std::map<std::string, size_t> &refs) {

    if (!sam_r1.isUnmapped()){
        core::record::Alignment alignment(convertCigar2ECigar(sam_r1.getCigar(), sam_r1.getSeq()),
                                              sam_r1.checkFlag(Record::FlagPos::SEQ_REVERSE));
        alignment.addMappingScore(sam_r1.getMapQ());

        core::record::AlignmentBox alignmentContainer(sam_r1.getPos(), std::move(alignment));

        if (sam_r2_ptr == nullptr || sam_r2_ptr->isUnmapped()){
            auto splitAlign = util::make_unique<core::record::alignment_split::Unpaired>();
            alignmentContainer.addAlignmentSplit(std::move(splitAlign));
        } else {
            auto splitAlign = util::make_unique<core::record::alignment_split::OtherRec>(
                sam_r2_ptr->getPos(), refs.at(sam_r2_ptr->getRname())
            );
            alignmentContainer.addAlignmentSplit(std::move(splitAlign));
        }

        rec->addAlignment(refs.at(sam_r1.getRname()), std::move(alignmentContainer));
    }
}
// ---------------------------------------------------------------------------------------------------------------------
void Importer::convertPairedEndSplitPair(core::record::Chunk &template_chunk, SamRecords2D &sam_recs_2d, std::map<std::string, size_t>& refs) {

    auto sam_r1_iter = sam_recs_2d.front().begin();
    auto sam_r2_iter = sam_recs_2d.back().begin();

    auto sam_r1_end = sam_recs_2d.front().end();
    auto sam_r2_end = sam_recs_2d.back().end();

    std::unique_ptr<core::record::Record> rec_1, rec_2;

    // Cache SEQ of read 1 & 2 to replace SEQ of other alignment if the value is '*'
    // See "Recommended Practice for SAM Format" in SAM Format documentation for more information
    auto sam_r1_seq = sam_r1_iter != sam_r1_end ? sam_r1_iter->getSeq() : "*";
    auto sam_r2_seq = sam_r2_iter != sam_r2_end ? sam_r2_iter->getSeq() : "*";

    while (sam_r1_iter !=  sam_r1_end || sam_r2_iter != sam_r2_end){

        // Handle Read 1
        if (sam_r1_iter != sam_r1_end){
            auto *sam_r2_ptr = sam_r2_iter != sam_r2_end ? &(*sam_r2_iter) : nullptr;

            if (rec_1 != nullptr){
                if (rec_1->getAlignmentSharedData().getSeqID() != refs.at(sam_r1_iter->getRname())){
                    // Add more_alignment_info
                    auto leftmost_read = sam_r1_iter->getPos() < sam_r2_iter->getPos() ? sam_r1_iter : sam_r2_iter;
    //                auto more_alignment_info = util::make_unique<core::record::alignment_external::OtherRec>(
    //                    leftmost_read->getPos(), refs.at(leftmost_read->getRname()));
    //                rec_1->setMoreAlignmentInfo(std::move(more_alignment_info));

                    template_chunk.push_back(std::move(*rec_1));

                    // Retrieve back Sequence from the primary line due to SEQ and QUAL are set to "*"
                    // See "Recommended Practice for SAM Format" in SAM Format documentation for more information
                    if (sam_r1_iter->getSeq() == "*"){
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
        if (sam_r2_iter != sam_r2_end){
            auto *sam_r1_ptr = sam_r1_iter != sam_r1_end ? &(*sam_r1_iter) : nullptr;

            if (rec_2 != nullptr){
                if (rec_2->getAlignmentSharedData().getSeqID() != refs.at(sam_r2_iter->getRname())){
                    // Add more_alignment_info
                    auto leftmost_read = sam_r1_iter->getPos() < sam_r2_iter->getPos() ? sam_r1_iter : sam_r2_iter;
                    //                auto more_alignment_info = util::make_unique<core::record::alignment_external::OtherRec>(
                    //                    leftmost_read->getPos(), refs.at(leftmost_read->getRname()));
                    //                rec_2->setMoreAlignmentInfo(std::move(more_alignment_info));

                    template_chunk.push_back(std::move(*rec_2));

                    // Retrieve back Sequence from the primary line due to SEQ and QUAL are set to "*"
                    // See "Recommended Practice for SAM Format" in SAM Format documentation for more information
                    if (sam_r2_iter->getSeq() == "*"){
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

    sam_recs_2d.front().clear();
    sam_recs_2d.back().clear();
}
// ---------------------------------------------------------------------------------------------------------------------
void Importer::_convertPairedEnd(core::record::Chunk &chunk, SamRecords2D &sam_recs_2d,
                                 std::map<std::string, size_t> &refs) {
//
//    auto sam_r1_iter = sam_recs_2d.front().begin();
//    auto sam_r2_iter = sam_recs_2d.back().begin();
//
//    std::unique_ptr<core::record::Record> mpeg_r1_ptr = nullptr;
//    std::unique_ptr<core::record::Record> mpeg_r2_ptr = nullptr;
//
//    while (sam_r1_iter != sam_recs_2d.front().end() || sam_r2_iter != sam_recs_2d.back().end()){
//
//        if (sam_r2_iter == sam_recs_2d.back().end()){
//            if (sam_r1_iter != nullptr){
//
//            }
//
//        } else if (sam_r1_iter == sam_recs_2d.front().end() ){
//
//        // Start from here, both sam_r1_iter and sam_r2_iter are valid
//        } else{
//
//            // If mpeg_r1_ptr is SameRec and existing MPEG-G record has the same seqID
//            if (mpeg_r2_ptr != nullptr && mpeg_r2_ptr == nullptr &&
//                sam_r1_iter->getRname() == sam_r2_iter->getRname() &&
//                mpeg_r1_ptr->getAlignmentSharedData().getSeqID() == refs.at(sam_r1_iter->getRname())){
//
//                addAlignmentToSameRec(mpeg_r1_ptr, static_cast<Record &>(*sam_r1_iter), static_cast<Record &>(*sam_r1_iter), refs);
//
//            // Create new MPEG-G record(s) and let the previously created records to point to the new one.
//            } else{
//                // Find the leftmost read
//                auto leftmost_read = sam_r1_iter->getPos() < sam_r1_iter->getPos() ? sam_r1_iter : sam_r2_iter;
//
//                // Points more_alignment_info of mpeg_r1_ptr to the leftmost read
//                if (mpeg_r1_ptr != nullptr){
//                    auto more_alignment_info = util::make_unique<genie::core::record::alignment_external::OtherRec>(
//                        leftmost_read->getPos(), refs.at(leftmost_read->getRname()));
//                    mpeg_r1_ptr->setMoreAlignmentInfo(std::move(more_alignment_info));
//                    chunk.push_back(std::move(*mpeg_r1_ptr));
//                }
//
//                // Points more_alignment_info of mpeg_r2_ptr to the leftmost read
//                if (mpeg_r2_ptr != nullptr) {
//                    auto more_alignment_info = util::make_unique<genie::core::record::alignment_external::OtherRec>(
//                        leftmost_read->getPos(), refs.at(leftmost_read->getRname()));
//                    mpeg_r2_ptr->setMoreAlignmentInfo(std::move(more_alignment_info));
//                    chunk.push_back(std::move(*mpeg_r2_ptr));
//                }
//
//                if (sam_r1_iter->getRname() != sam_r2_iter->getRname()){
//                    mpeg_r1_ptr = convertSam2SplitRec(
//                        static_cast<Record &>(*sam_r1_iter), &(*sam_r2_iter), refs);
//                    mpeg_r2_ptr = convertSam2SplitRec(
//                        static_cast<Record &>(*sam_r2_iter), &(*sam_r1_iter), refs);
//                } else if (sam_r1_iter->getRname() == sam_r2_iter->getRname()) {
//                    mpeg_r1_ptr = convertSam2SameRec(
//                        static_cast<Record &>(*sam_r1_iter), static_cast<Record &>(*sam_r2_iter), refs);
//                }
//            }
//        }
//
//    }

}
// ---------------------------------------------------------------------------------------------------------------------
void Importer::convertUnmapped(core::record::Chunk &chunk, SamRecords &sam_recs, std::map<std::string, size_t>& refs) {
    auto& sam_rec = sam_recs.front();

    auto flag_tuple = convertFlags2Mpeg(sam_rec.getFlags());
    core::record::Record rec(1, core::record::ClassType::CLASS_I, sam_rec.moveQname(),
                             "Genie",std::get<1>(flag_tuple));

    // No alignmnent
    // core::record::Alignment alignment(convertCigar2ECigar(sam_rec.getCigar(), sam_rec.getSeq()),
    //                                   sam_rec.checkFlag(Record::FlagPos::SEQ_REVERSE));

    //alignment.addMappingScore(sam_rec.getMapQ());
    //core::record::AlignmentBox alignmentContainer(sam_rec.getPos(), std::move(alignment));

    core::record::Segment segment(sam_rec.moveSeq());
    if (sam_rec.getQual() != "*") {
        segment.addQualities(sam_rec.moveQual());
    }
    rec.addSegment(std::move(segment));

    // No alignment
    //    ret.addAlignment(ref, std::move(alignmentContainer));

    chunk.push_back(std::move(rec));

    sam_recs.clear();
}
// ---------------------------------------------------------------------------------------------------------------------
void Importer::convertSingleEnd(core::record::Chunk &chunk, SamRecords &sam_recs, std::map<std::string, size_t>& refs, bool unmapped_pair, bool is_read_1_first) {
    auto&& sam_rec = std::move(sam_recs.front());
    sam_recs.pop_front();

    auto flag_tuple = convertFlags2Mpeg(sam_rec.getFlags());
    core::record::Record rec(1, core::record::ClassType::CLASS_I, sam_rec.moveQname(),
                             "Genie",std::get<1>(flag_tuple), is_read_1_first);

    core::record::Alignment alignment(convertCigar2ECigar(sam_rec.getCigar(), sam_rec.getSeq()),
                                      sam_rec.checkFlag(Record::FlagPos::SEQ_REVERSE));
    alignment.addMappingScore(sam_rec.getMapQ());

    core::record::AlignmentBox alignmentContainer(sam_rec.getPos(), std::move(alignment));
    rec.addAlignment(refs.at(sam_rec.getRname()), std::move(alignmentContainer));

    core::record::Segment segment(sam_rec.moveSeq());
    if (sam_rec.getQual() != "*") {
        segment.addQualities(sam_rec.moveQual());
    }
    rec.addSegment(std::move(segment));

    for (auto sam_rec_it = ++sam_recs.begin(); sam_rec_it != sam_recs.end(); sam_rec_it++){

        UTILS_DIE_IF(sam_rec_it->isUnmapped(),
                     "Unaligned sam record found for qname " + sam_rec.getQname());

        core::record::Alignment alignment(convertCigar2ECigar(sam_rec_it->getCigar(), sam_rec_it->getSeq()),
                                          sam_rec.checkFlag(Record::FlagPos::SEQ_REVERSE));
        alignment.addMappingScore(sam_rec.getMapQ());

        core::record::AlignmentBox alignmentContainer(sam_rec.getPos(), std::move(alignment));

        if (unmapped_pair){
            auto splitAlign = util::make_unique<core::record::alignment_split::Unpaired>();
            alignmentContainer.addAlignmentSplit(std::move(splitAlign));
        }

        rec.addAlignment(refs.at(sam_rec.getRname()), std::move(alignmentContainer));
    }

    chunk.push_back(std::move(rec));
}
// ---------------------------------------------------------------------------------------------------------------------
void Importer::convertPairedEnd(core::record::Chunk &chunk, SamRecords2D &sam_recs_2d, std::map<std::string, size_t>& refs){

    auto& read_1 = sam_recs_2d.front();
    auto read_1_empty = read_1.empty();
    auto read_1_unmapped = read_1.front().isUnmapped();
    auto read_1_primary = read_1.front().isPrimaryLine();
    auto read_1_ok = !read_1_empty && read_1_primary && !read_1_unmapped;

    auto& read_2 = sam_recs_2d.back();
    auto read_2_empty = read_2.empty();
    auto read_2_unmapped = read_2.front().isUnmapped();
    auto read_2_primary = read_2.front().isPrimaryLine();
    auto read_2_ok = !read_2_empty && read_2_primary && !read_2_unmapped;

    if (read_1_ok && read_2_ok){
        UTILS_DIE_IF(!read_1.front().isPairOf(read_2.front()),
                     "read_1 is not pair of read_2 or vice versa");

        auto create_split_records = false;

        // Check whether read_1 has the same number of alignments as read_2 and the references are the same
        if (read_1.size() == read_2.size()){
            auto read_1_it = read_1.begin();
            auto read_2_it = read_2.begin();
            auto any_different_ref = false;
            while(read_1_it != read_1.end() && read_2_it != read_2.end()){
                if (read_1_it->getRname() != read_2_it->getRname()){
                    any_different_ref = true;
                    break;
                }

                read_1_it++;
                read_2_it++;
            }

            create_split_records = any_different_ref;
        }

        if (create_split_records){
            convertPairedEndSplitPair(chunk, sam_recs_2d, refs);
        } else {
            convertPairedEndNoSplit(chunk, sam_recs_2d, refs);
        }

        // Handle alignments where one of the reads is not complete (unpaired etc)
    } else {
        if (read_1_unmapped) {
            convertUnmapped(chunk, read_1, refs);
        } else if (!read_1_primary){
            UTILS_DIE("Cannot find the primary line of read_1!");
        } else if (read_1_ok){
            convertSingleEnd(chunk, read_1, refs, true, true);
        }

        if (read_2_unmapped){
            convertUnmapped(chunk, read_2, refs);
        } else if (!read_2_primary){
            UTILS_DIE("Cannot find the primary line of read_2!");
        } else if (read_2_ok) {
            convertSingleEnd(chunk, read_2, refs, true, false);
        }
    }
    sam_recs_2d.clear();
}
// ---------------------------------------------------------------------------------------------------------------------
void Importer::convert(core::record::Chunk &chunk, ReadTemplate &rt, std::map<std::string, size_t>& refs) {
//    // Register Reference Sequence Name RNAME
//    for (auto& sam_recs:sam_recs_2d){
//        for (auto& sam_rc:sam_recs){
//            const auto& rname = sam_rc.getRname();
//
//            if (rname != "=" || rname != "*"){
//                auto it = refs.find(rname);
//                if (it == refs.end()) {
//                    refs.insert(std::make_pair(sam_rc.getRname(), ref_counter++));
//                }
//            }
//        }
//    }

    UTILS_DIE_IF(!rt.isValid(), "Invalid Read Template. Neither unmapped, single-end nor paired-end");

    auto is_unmapped = rt.isUnmapped();
    auto is_single = rt.isSingle();
    auto is_pair = rt.isPair();

    SamRecords2D sam_recs_2d;
    rt.getRecords(sam_recs_2d);

    // Convert SAM records to MPEG-G record(s)
    if (is_unmapped){
        convertUnmapped(chunk, sam_recs_2d.front(), refs);
    } else if (is_single){
        convertSingleEnd(chunk, sam_recs_2d.front(), refs);
    } else if (is_pair){
        convertPairedEnd(chunk, sam_recs_2d, refs);
    } else {
        UTILS_DIE("Unhandled read type found. Should never be reached!");
    }

}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace sam
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------