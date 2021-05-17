#include "sam_transcoder.h"
#include <record/alignment_split/unpaired.h>
//#include <genie/util/make-unique.h>
//#include <genie/util/runtime-exception.h>

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
    auto &sam_rec = data[uint8_t(Index::UNMAPPED)].front();

    auto flag_tuple = convertFlags2Mpeg(sam_rec.getFlag());
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
    bool unmapped_pair,
    bool is_read_1_first
) {

    auto &sam_recs = data[uint8_t(Index::NOT_PAIRED)];
    auto sam_rec = std::move(sam_recs.front());
    sam_recs.pop_front();

    auto flag_tuple = convertFlags2Mpeg(sam_rec.getFlag());
    genie::core::record::Record rec(1,
                                    genie::core::record::ClassType::CLASS_I,
                                    sam_rec.moveQname(),
                                    "Genie",
                                    std::get<1>(flag_tuple),
                                    is_read_1_first);

    {
        genie::core::record::Alignment alignment(
            sam_rec.getECigar(),
            sam_rec.isReverse()
        );
        alignment.addMappingScore(sam_rec.getMapq());

        genie::core::record::AlignmentBox alignmentContainer(sam_rec.getPos(), std::move(alignment));
        rec.addAlignment(sam_rec.getRID(), std::move(alignmentContainer));
    }

    genie::core::record::Segment segment(sam_rec.moveSeq());
    if (sam_rec.getQual() != "*") {
        segment.addQualities(sam_rec.moveQual());
    } else {
        segment.addQualities(std::string("\0", 1));
    }
    rec.addSegment(std::move(segment));

    for (auto sam_rec_it = sam_recs.begin(); sam_rec_it != sam_recs.end(); sam_rec_it++) {
        //        UTILS_DIE_IF(sam_rec_it->isCatUnmapped(),
        //                     "Unaligned sam record found for qname " + sam_rec.getQname());

        genie::core::record::Alignment alignment(
            sam_rec.getECigar(),
            sam_rec_it->isReverse()
        );
        alignment.addMappingScore(sam_rec_it->getMapq());

        // Convert from sam 1-based mapping position to mpeg-g 0-based coordinate
        genie::core::record::AlignmentBox alignmentContainer(sam_rec.getPos(), std::move(alignment));

        if (unmapped_pair) {
            auto splitAlign = genie::util::make_unique<genie::core::record::alignment_split::Unpaired>();
            alignmentContainer.addAlignmentSplit(std::move(splitAlign));
        }

        // TODO
//        rec.addAlignment(refs.at(sam_rec_it->getRname()), std::move(alignmentContainer));
    }

    records.push_back(std::move(rec));
}

void SamRecordGroup::convertPairedEnd(std::list<genie::core::record::Record> &records, bool force_split) {

}

SamRecordGroup::SamRecordGroup() : data(size_t(Index::TOTAL_TYPES)) {}

void SamRecordGroup::addRecord(Record &&rec) {

    // Default is class "UNKNOWN"
    auto idx = Index::UNKNOWN;

    // Handles unmapped read
    if (rec.isUnmapped()){
        idx = Index::UNMAPPED;
    }
    // Single-end read
    else if (!rec.isPaired()){
        idx = Index::NOT_PAIRED;
    }
    // Handles paired-end read / multi segments
    else if (rec.isPaired()) {
        if (rec.isRead1()) {
            idx = Index::PAIR_READ1;
        } else if (rec.isRead2()) {
            idx = Index::PAIR_READ2;
        } else {
            UTILS_DIE("Neiter read1 nor read2:" + rec.getQname());
        }
    // Should never be reached
    } else {
        UTILS_DIE("Unhandeled case found:" + rec.getQname());
    }

    // Push data based on property "isPrimary"
    if (rec.isPrimary()) {
        data[uint8_t(idx)].push_front(std::move(rec));
    } else {
        data[uint8_t(idx)].push_back(std::move(rec));
    }
}

bool SamRecordGroup::getRecords(std::list<std::list<Record>> &sam_recs) {
    sam_recs.clear();
    if (isValid()){
        if (isCatUnmapped()){
            sam_recs.push_back(std::move(data[uint8_t(Index::UNMAPPED)]));
        } else if (isCatNotPaired()) {
            sam_recs.push_back(std::move(data[uint8_t(Index::NOT_PAIRED)]));
        } else if (isCatPaired()){
            sam_recs.push_back(std::move(data[uint8_t(Index::PAIR_READ1)]));
            sam_recs.push_back(std::move(data[uint8_t(Index::PAIR_READ2)]));
        }
    } else {
        return false;
    }
}

bool SamRecordGroup::isCatUnmapped() { return data[uint8_t(Index::UNMAPPED)].size() == 1; }

bool SamRecordGroup::isCatNotPaired() {
    // Single non- and multiplie alignements
    return !data[uint8_t(Index::NOT_PAIRED)].empty() && isPrimary(data[uint8_t(Index::NOT_PAIRED)].front());
}

bool SamRecordGroup::isCatPaired() {
    return (!data[uint8_t(Index::PAIR_READ1)].empty() && isPrimary(data[uint8_t(Index::PAIR_READ1)].front())) ||
           (!data[uint8_t(Index::PAIR_READ2)].empty() && isPrimary(data[uint8_t(Index::PAIR_READ2)].front()));
}

bool SamRecordGroup::isCatUnknown() { return !data[uint8_t(Index::UNKNOWN)].empty(); }

bool SamRecordGroup::isValid() {
    if (isCatUnknown() || data[uint8_t(Index::UNMAPPED)].size() > 1) {
        return false;
    } else {
        auto is_unmapped = isCatUnmapped();
        auto is_single = isCatNotPaired();
        auto is_pair = isCatPaired();

        // Check if the current sam record groups belong to only one group
        return (is_unmapped && !is_single && !is_pair) || (!is_unmapped && is_single && !is_pair) ||
               (!is_unmapped && !is_single && is_pair);
    }
}

void SamRecordGroup::convert(std::list<genie::core::record::Record> &records, bool force_split) {
    if (!isValid()) {
        // TODO: Add error here
    }

    auto is_unmapped = isCatUnmapped();
    auto is_single = isCatNotPaired();
    auto is_pair = isCatPaired();

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
    auto sam_rec = Record(sam_alignment);
    auto curr_qname = sam_rec.getQname();
    buffer.addRecord(std::move(sam_rec));

    while(sam_read1(sam_file, sam_header, sam_alignment) != -1){
        sam_rec = Record(sam_alignment);

        if (sam_rec.getQname() != curr_qname){
            curr_qname = sam_rec.getQname();
            buffer.convert(records);
        }
        buffer.addRecord(std::move(sam_rec));
    }

    // Destroy header
    sam_hdr_destroy(sam_header);
}


}