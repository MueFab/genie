/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "importer.h"
#include <genie/core/record/alignment_split/same-rec.h>
#include <genie/core/record/alignment_split/unpaired.h>
#include <genie/util/ordered-section.h>
#include <list>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace sam {

// ---------------------------------------------------------------------------------------------------------------------

Importer::Importer(size_t _blockSize, std::istream &_file)
    : blockSize(_blockSize), samReader(_file), lock(), ref_counter(0) {}

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
void Importer::convertUnmapped(core::record::Chunk &chunk, SamRecords &sam_recs) {
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
void Importer::convertSingleEnd(core::record::Chunk &chunk, SamRecords &sam_recs, bool unmapped_pair) {
    auto&& sam_rec = std::move(sam_recs.front());
    sam_recs.pop_front();

    auto flag_tuple = convertFlags2Mpeg(sam_rec.getFlags());
    core::record::Record rec(1, core::record::ClassType::CLASS_I, sam_rec.moveQname(),
                             "Genie",std::get<1>(flag_tuple));

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
void Importer::convertPairedEndNoSplit(core::record::Chunk &chunk, SamRecords2D &sam_recs_2d) {
    auto& sam_rec_1 = sam_recs_2d.front().front();
    sam_recs_2d.front().pop_front();

    auto& sam_rec_2 = sam_recs_2d.back().front();
    sam_recs_2d.back().pop_front();

    auto flag_tuple = convertFlags2Mpeg(sam_rec_1.getFlags());
    core::record::Record rec(2, core::record::ClassType::CLASS_I, sam_rec_1.moveQname(),
                             "Genie",std::get<1>(flag_tuple), sam_rec_1.getPos() < sam_rec_2.getPos());

    core::record::Alignment alignment(convertCigar2ECigar(sam_rec_1.getCigar(), sam_rec_1.getSeq()),
                                      sam_rec_1.checkFlag(Record::FlagPos::SEQ_REVERSE));
    alignment.addMappingScore(sam_rec_1.getMapQ());

    core::record::AlignmentBox alignmentContainer(sam_rec_1.getPos(), std::move(alignment));
    rec.addAlignment(refs.at(sam_rec_1.getRname()), std::move(alignmentContainer));

    core::record::Segment segment(sam_rec_1.moveSeq());
    if (sam_rec_1.getQual() != "*") {
        segment.addQualities(sam_rec_1.moveQual());
    }
    rec.addSegment(std::move(segment));
}
// ---------------------------------------------------------------------------------------------------------------------
void Importer::convertPairedEndSplitPair(core::record::Chunk &chunk, SamRecords2D &sam_recs_2d) {

    UTILS_DIE("TODO");
    auto is_read_1_avail = true;
    auto& sam_read_1 = sam_recs_2d.front().front();
    sam_recs_2d.front().pop_front();

    auto flag_tuple = convertFlags2Mpeg(sam_read_1.getFlags());
    core::record::Record rec_read_1(1, core::record::ClassType::CLASS_I, sam_read_1.moveQname(),
                               "Genie",std::get<1>(flag_tuple));

    core::record::Segment segment(sam_read_1.moveSeq());
    if (sam_read_1.getQual() != "*") {
        segment.addQualities(sam_read_1.moveQual());
    }
    rec_read_1.addSegment(std::move(segment));

    if (!sam_read_1.isUnmapped()){
        core::record::Alignment alignment(convertCigar2ECigar(sam_read_1.getCigar(), sam_read_1.getSeq()),
                                          sam_read_1.checkFlag(Record::FlagPos::SEQ_REVERSE));
        alignment.addMappingScore(sam_read_1.getMapQ());

        core::record::AlignmentBox abox_read_1(sam_read_1.getPos(), std::move(alignment));
    } else{

    }
}
// ---------------------------------------------------------------------------------------------------------------------
void Importer::convertPairedEnd(core::record::Chunk &chunk, SamRecords2D &sam_recs_2d){

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
            convertPairedEndSplitPair(chunk, sam_recs_2d);
        } else {
            convertPairedEndNoSplit(chunk, sam_recs_2d);
        }

    // One of reads is not ok
    } else {
        if (read_1_unmapped) {
            convertUnmapped(chunk, read_1);
        } else if (!read_1_primary){
            UTILS_DIE("Primary line of read_1 is not found!");
        } else if (read_1_ok){
            convertSingleEnd(chunk, read_1, true);
        }

        if (read_2_unmapped){
            convertUnmapped(chunk, read_2);
        } else if (!read_2_primary){
            UTILS_DIE("Primary line of read_2 is not found!");
        } else if (read_2_ok) {
            convertSingleEnd(chunk, read_2, true);
        }
    }
    sam_recs_2d.clear();
}
// ---------------------------------------------------------------------------------------------------------------------
void Importer::convert(core::record::Chunk &chunk, ReadTemplate &rt) {
    SamRecords2D sam_recs_2d;
    rt.getRecords(sam_recs_2d);

    for (auto& sam_recs:sam_recs_2d){
        for (auto& sam_rc:sam_recs){
            auto& rname = sam_rc.getRname();

            if (rname != "=" || rname != "*"){
                auto it = refs.find(rname);
                if (it == refs.end()) {
                    refs.insert(std::make_pair(sam_rc.getRname(), ref_counter++));
                }
            }
        }
    }

    if (rt.isValid()){
        if (rt.isUnmapped()){
            convertUnmapped(chunk, sam_recs_2d.front());
        } else if (rt.isSingle()){
            convertSingleEnd(chunk, sam_recs_2d.front());
        } else if (rt.isPair()){
            convertPairedEnd(chunk, sam_recs_2d);
        } else {
            UTILS_DIE("Unhandled case found!");
        }
    } else{
        UTILS_DIE("Invalid Read Template. Neither unmapped, single-end nor paired-end");
    }

}
// ---------------------------------------------------------------------------------------------------------------------

bool Importer::pump(size_t id) {
    auto counter = blockSize;
    std::string line;
    std::list<std::string> lines;
    core::record::Chunk chunk;
    {
        util::OrderedSection section(&lock, id);
        while (stream.good() && counter--){
            std::getline(stream, line);
            lines.push_back(std::move(line));
        }
    }


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

}  // namespace sam
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------