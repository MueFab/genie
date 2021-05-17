#include "sam_transcoder.h"

#include <fstream>
#include <map>
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

void SamRecordGroup::convertUnmapped(
    std::list<genie::core::record::Record> &records,
    Record& sam_rec
) {

    auto flag_tuple = convertFlags2Mpeg(sam_rec.getFlag());
    genie::core::record::Record rec(1,
                                    genie::core::record::ClassType::CLASS_I,
                                    sam_rec.moveQname(),
                                    "Genie",
                                    std::get<1>(flag_tuple));

    // No alignmnent
    // core::record::Alignment alignment(convertCigar2ECigar(sam_rec.getCigar(), sam_rec.getSeq()),
    //                                   sam_rec.checkFlag(Record::FlagPos::SEQ_REVERSE));

    // alignment.addMappingScore(sam_rec.getMapQ());
    // core::record::AlignmentBox alignmentContainer(sam_rec.getPos(), std::move(alignment));

    genie::core::record::Segment segment(sam_rec.moveSeq());
    auto qual = sam_rec.getQual();
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
}

void SamRecordGroup::convertSingleEnd(
    std::list<genie::core::record::Record> &records,
    std::list<Record>& sam_recs,
    bool unmapped_pair,
    bool is_read_1_first
) {

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
        genie::core::record::Alignment alignment(sam_rec.getECigar(),
                                          sam_rec.isReverse());

        alignment.addMappingScore(sam_rec.getMapq());

        genie::core::record::AlignmentBox alignmentContainer(sam_rec.getPos(), std::move(alignment));
        rec.addAlignment(sam_rec.getRID(), std::move(alignmentContainer));
    }

    genie::core::record::Segment segment(sam_rec.moveSeq());
    if (sam_rec.getQual() != "*") {
        segment.addQualities(sam_rec.moveQual());
    } else {
        segment.addQualities("");
    }
    rec.addSegment(std::move(segment));

    for (auto sam_rec_it = ++sam_recs.begin(); sam_rec_it != sam_recs.end(); sam_rec_it++) {

        genie::core::record::Alignment alignment(sam_rec_it->getECigar(),
                                                 sam_rec_it->isReverse());
        alignment.addMappingScore(sam_rec_it->getMapq());

        genie::core::record::AlignmentBox alignmentContainer(sam_rec_it->getPos(), std::move(alignment));

        if (unmapped_pair) {
            auto splitAlign = genie::util::make_unique<genie::core::record::alignment_split::Unpaired>();
            alignmentContainer.addAlignmentSplit(std::move(splitAlign));
        }

        rec.addAlignment(sam_rec_it->getRID(), std::move(alignmentContainer));
    }

    records.push_back(std::move(rec));
}

void SamRecordGroup::convertPairedEnd(
    std::list<genie::core::record::Record> &records,
    std::list<std::list<Record>>& sam_recs,
    bool force_split
) {

}

SamRecordGroup::SamRecordGroup() : data(size_t(Index::TOTAL_INDICES)) {}

void SamRecordGroup::addRecord(Record &&rec) {

    // Default is class "UNKNOWN"
    auto idx = Index::UNKNOWN;

    // Handles unmapped read
    if (rec.isUnmapped()){
        idx = Index::UNMAPPED;
    }
    // Single-end read
    else if (!rec.isPaired()){
        idx = Index::SINGLE;
    }
    // Handles paired-end read / multi segments
    else if (rec.isPaired()) {
        if (rec.isRead1()) {
            if (rec.isPrimary()){
                idx = Index::PAIR_READ1_PRIMARY;
            }
            else {
                idx = Index::PAIR_READ1_NONPRIMARY;
            }
        } else if (rec.isRead2()) {
            if (rec.isPrimary()){
                idx = Index::PAIR_READ2_PRIMARY;
            }
            else {
                idx = Index::PAIR_READ2_NONPRIMARY;
            }
        } else {
            UTILS_DIE("Neiter read1 nor read2:" + rec.getQname());
        }

    // Should never be reached
    } else {
        UTILS_DIE("Unhandeled case found:" + rec.getQname());
    }

    data[uint8_t(idx)].push_back(std::move(rec));
}

SamRecordGroup::Class SamRecordGroup::getRecords(std::list<std::list<Record>> &sam_recs) {
    sam_recs.clear();

    auto cls = computeClass();

    switch (cls){
        case Class::UNMAPPED:
            sam_recs.push_back(std::move(data[uint8_t(Index::UNMAPPED)]));
            break;
        case Class::SINGLE:
            sam_recs.push_back(std::move(data[uint8_t(Index::SINGLE)]));
            break;
        case Class::PAIRED:
            sam_recs.push_back(data[uint8_t(Index::PAIR_READ1_PRIMARY)]);
            sam_recs.push_back(data[uint8_t(Index::PAIR_READ2_PRIMARY)]);

            sam_recs.front().splice(sam_recs.front().end(), data[uint8_t(Index::PAIR_READ1_NONPRIMARY)]);
            sam_recs.back().splice(sam_recs.back().end(), data[uint8_t(Index::PAIR_READ2_NONPRIMARY)]);
            break;
        default:
            break;
    }

    return cls;
}

bool SamRecordGroup::isUnmapped() { return data[uint8_t(Index::UNMAPPED)].size() == 1; }

bool SamRecordGroup::isSingle() const { return !data[(uint8_t) Index::SINGLE].empty(); } // Single non- and multiplie alignements

bool SamRecordGroup::isPaired() const {
    return !data[(uint8_t) Index::PAIR_READ1_PRIMARY].empty() &&
           !data[(uint8_t) Index::PAIR_READ2_PRIMARY].empty();
}

bool SamRecordGroup::isUnknown() { return !data[uint8_t(Index::UNKNOWN)].empty(); }

bool SamRecordGroup::isValid() {
    if (isUnknown() || data[uint8_t(Index::UNMAPPED)].size() > 1) {
        return false;
    } else {
        auto is_unmapped = isUnmapped();
        auto is_single = isSingle();
        auto is_pair = isPaired();

        // Check if the current sam record groups belong to only one group
        return (is_unmapped && !is_single && !is_pair) || (!is_unmapped && is_single && !is_pair) ||
               (!is_unmapped && !is_single && is_pair);
    }
}

SamRecordGroup::Class SamRecordGroup::computeClass() {
    if (isUnknown() || data[uint8_t(Index::UNMAPPED)].size() > 1) {
        return Class::INVALID;
    } else {
        auto is_unmapped = isUnmapped();
        auto is_single = isSingle();
        auto is_pair = isPaired();

        // Check if the current sam record groups belong to only one class
        if ((is_unmapped && !is_single && !is_pair)){
            return Class::UNMAPPED;
        } else if (!is_unmapped && is_single && !is_pair){
            return Class::SINGLE;
        } else if (!is_unmapped && !is_single && is_pair) {
            return Class::PAIRED;
        } else {
            return Class::INVALID;
        }
    }
}

void SamRecordGroup::convert(std::list<genie::core::record::Record> &records, bool force_split) {

    std::list<std::list<Record>> sam_recs_2d;

    auto cls = getRecords(sam_recs_2d);
    if (cls == Class::INVALID) {
        UTILS_DIE("SAM records belong to multiple category (unmapped, single, paired)");
    }

    switch (cls){
        case Class::UNMAPPED:
            convertUnmapped(records, sam_recs_2d.front().front());
            break;
        case Class::SINGLE:
            convertSingleEnd(records, sam_recs_2d.front());
            break;
        case Class::PAIRED:
            convertPairedEnd(records, sam_recs_2d, force_split);
            break;
        default:
            UTILS_DIE("Unhandeled conversion");
            break;
    }
}

bool save_mgrecs_by_rid(
    std::list<genie::core::record::Record>&recs,
    std::map<int32_t, genie::util::BitWriter>& writers
){

    for (auto & rec : recs) {
        auto ref_id = rec.getAlignmentSharedData().getSeqID();

        try {
            auto writer = writers.at(ref_id);

            // TODO: Handle case where harddrive is full
            rec.write(writer);
            writer.flush();
        } catch (std::out_of_range){
            return false;
        }
    }

    recs.clear();

    return true;
}

uint8_t sam_to_mgrec_phase1(
    transcoder::ProgramOptions& options,
    int& nref
){
    // Phase 1
    std::list<std::ofstream> p1_writers;
    std::map<int32_t, genie::util::BitWriter> p1_bitwriters;
    int res;
    uint8_t return_code = 0;

    samFile *sam_file = hts_open(options.sam_file_path.c_str(), "r"); //open bam file
    bam_hdr_t *sam_header = NULL; //read header
    bam1_t *sam_alignment = bam_init1(); //initialize an alignment
    SamRecordGroup buffer;

    if (!sam_file) {
        return_code = 1;
        bam_destroy1(sam_alignment);
        bam_hdr_destroy(sam_header);
        if (sam_file) sam_close(sam_file);

        return return_code;
    }

    sam_header = sam_hdr_read(sam_file); //read header
    if (!sam_header) {
        return_code = 2;
        bam_destroy1(sam_alignment);
        bam_hdr_destroy(sam_header);
        if (sam_file) sam_close(sam_file);

        return return_code;
    }

    nref = sam_hdr_nref(sam_header);
    for (int32_t i=0; i<nref; i++){
        std::string fpath = options.tmp_dir_path + "/" + std::to_string(i) + ".mgrec";

        p1_writers.emplace_back(fpath, std::ios::trunc | std::ios::binary);
        p1_bitwriters.emplace(i, &(p1_writers.back()));
    }

    std::list<genie::core::record::Record> records;
    if ((res = sam_read1(sam_file, sam_header, sam_alignment)) < 0){
        return_code = 3;
        bam_destroy1(sam_alignment);
        bam_hdr_destroy(sam_header);
        if (sam_file) sam_close(sam_file);

        return return_code;
    }
    auto sam_rec = Record(sam_alignment);
    auto curr_qname = sam_rec.getQname();
    buffer.addRecord(std::move(sam_rec));

    while((res = sam_read1(sam_file, sam_header, sam_alignment)) >= 0){
        sam_rec = Record(sam_alignment);

        if (sam_rec.getQname() != curr_qname){
            curr_qname = sam_rec.getQname();
            buffer.convert(records);

            if(!save_mgrecs_by_rid(records, p1_bitwriters)){
                UTILS_DIE("Cannot save MPEG-G Records");
            }
        }

        buffer.addRecord(std::move(sam_rec));
    }
    if (res < -1) {
        return_code = 3;
        bam_destroy1(sam_alignment);
        bam_hdr_destroy(sam_header);
        if (sam_file) sam_close(sam_file);

        return return_code;
    }

    buffer.convert(records);
    if(!save_mgrecs_by_rid(records, p1_bitwriters)){
        UTILS_DIE("Cannot save MPEG-G Records");
    }

    // Close all bitreader
    // TODO

    bam_destroy1(sam_alignment);
    bam_hdr_destroy(sam_header);
    if (sam_file) sam_close(sam_file);

    return return_code;
}

uint8_t sam_to_mgrec(transcoder::ProgramOptions& options){

    int nref;

    auto status =sam_to_mgrec_phase1(options, nref) != 0;
    if (status != 0){
        return status;
    }

    return status;

}


}