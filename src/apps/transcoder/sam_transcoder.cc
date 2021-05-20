#include "sam_transcoder.h"

//#include <iostream>
#include <algorithm>
#include <fstream>
//#include <map>
//#include <cstdio>

//#include <genie/util/make-unique.h>
//#include <genie/util/runtime-exception.h>
#include <record/alignment_split/unpaired.h>

#include "sam_reader.h"
#include "sam_sorter.h"

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
    std::list<genie::core::record::Record> &records, SamRecord& sam_rec
) {

    auto flag_tuple = convertFlags2Mpeg(sam_rec.getFlag());
    genie::core::record::Record rec(1,
                                    genie::core::record::ClassType::CLASS_I,
                                    sam_rec.moveQname(),
                                    "Genie",
                                    std::get<1>(flag_tuple));

    /// No alignmnent
    // core::record::Alignment alignment(convertCigar2ECigar(sam_rec.getCigar(), sam_rec.getSeq()),
    //                                   sam_rec.checkFlag(SamRecord::FlagPos::SEQ_REVERSE));

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

    /// No alignment
    //    ret.addAlignment(ref, std::move(alignmentContainer));
    records.push_back(std::move(rec));
}

void SamRecordGroup::convertSingleEnd(
    std::list<genie::core::record::Record> &records,
    std::list<SamRecord>& sam_recs,
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
    std::list<std::list<SamRecord>>& sam_recs,
    bool force_split
) {

}

SamRecordGroup::SamRecordGroup() : data(size_t(Index::TOTAL_INDICES)) {}

void SamRecordGroup::addRecord(SamRecord&&rec) {

    /// Default is class "UNKNOWN"
    auto idx = Index::UNKNOWN;

    /// Handles unmapped read
    if (rec.isUnmapped()){
        idx = Index::UNMAPPED;
    }
    /// Single-end read
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

    /// Should never be reached
    } else {
        UTILS_DIE("Unhandeled case found:" + rec.getQname());
    }

    data[uint8_t(idx)].push_back(std::move(rec));
}

SamRecordGroup::Category SamRecordGroup::getRecords(std::list<std::list<SamRecord>> &sam_recs) {
    sam_recs.clear();

    auto cls = computeClass();

    switch (cls){
        case Category::UNMAPPED:
            sam_recs.push_back(std::move(data[uint8_t(Index::UNMAPPED)]));
            break;
        case Category::SINGLE:
            sam_recs.push_back(std::move(data[uint8_t(Index::SINGLE)]));
            break;
        case Category::PAIRED:
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

SamRecordGroup::Category SamRecordGroup::computeClass() {
    if (isUnknown() || data[uint8_t(Index::UNMAPPED)].size() > 1) {
        return Category::INVALID;
    } else {
        auto is_unmapped = isUnmapped();
        auto is_single = isSingle();
        auto is_pair = isPaired();

        // Check if the current sam record groups belong to only one class
        if ((is_unmapped && !is_single && !is_pair)){
            return Category::UNMAPPED;
        } else if (!is_unmapped && is_single && !is_pair){
            return Category::SINGLE;
        } else if (!is_unmapped && !is_single && is_pair) {
            return Category::PAIRED;
        } else {
            return Category::INVALID;
        }
    }
}

void SamRecordGroup::convert(std::list<genie::core::record::Record> &records, bool force_split) {
    std::list<std::list<SamRecord>> sam_recs_2d;

    auto cls = getRecords(sam_recs_2d);
    if (cls == Category::INVALID) {
        UTILS_DIE("SAM records belong to multiple category (unmapped, single, paired)");
    }

    switch (cls){
        case Category::UNMAPPED:
            convertUnmapped(records, sam_recs_2d.front().front());
            break;
        case Category::SINGLE:
            convertSingleEnd(records, sam_recs_2d.front());
            break;
        case Category::PAIRED:
            convertPairedEnd(records, sam_recs_2d, force_split);
            break;
        default:
            UTILS_DIE("Unhandeled conversion");
            break;
    }
}

bool save_mgrecs_by_rid(
    std::list<genie::core::record::Record>&recs,
    std::map<int32_t, genie::util::BitWriter>& bitwriters){

    for (auto & rec : recs) {
        auto ref_id = rec.getAlignmentSharedData().getSeqID();

        try {
            UTILS_DIE_IF(rec.getClassID() != genie::core::record::ClassType::CLASS_I, "Invalid Class");
            auto bitwriter = bitwriters.at(ref_id);

            // TODO: Handle case where harddrive is full
            rec.write(bitwriter);
//            bitwriter.flush();
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

    std::list<std::ofstream> p1_writers;
    std::map<int32_t, genie::util::BitWriter> p1_bitwriters;
    int res;
    uint8_t return_code = 0;
    SamRecordGroup buffer;

    std::list<genie::core::record::Record> records;

    auto sam_reader = SamReader(options.sam_file_path);
    if (!sam_reader.ready()){
        return EXIT_FAILURE;
    }

    nref = sam_reader.getNumRef();
    for (int32_t i=0; i<nref; i++){
        std::string fpath = options.tmp_dir_path + "/" + std::to_string(i) + PHASE1_EXT;

        p1_writers.emplace_back(fpath, std::ios::trunc | std::ios::binary);
        p1_bitwriters.emplace(i, &(p1_writers.back()));
    }

    auto sam_rec = SamRecord();
    if (sam_reader.readSamRecord(sam_rec) < 0){
        return EXIT_FAILURE;
    }

    auto curr_qname = sam_rec.getQname();
    buffer.addRecord(std::move(sam_rec));

    while((res = sam_reader.readSamRecord(sam_rec)) >= 0){

        /// Add to buffers as long as it has the same QNAMEs
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
        return EXIT_FAILURE;
    }

    buffer.convert(records);
    if(!save_mgrecs_by_rid(records, p1_bitwriters)){
        UTILS_DIE("Cannot save MPEG-G Records");
    }

    auto iter_map = p1_bitwriters.begin();
    auto iter_writer = p1_writers.begin();
    while (iter_map != p1_bitwriters.end() && iter_writer == p1_writers.end()){
        iter_map->second.flush();
        iter_writer->close();
    }

    return return_code;
}

std::string gen_p2_tmp_fpath(
    transcoder::ProgramOptions& options,
    int rid,
    int ifile
){
    return options.tmp_dir_path + "/" + std::to_string(rid) +
                '.' + std::to_string(ifile) + PHASE2_TMP_EXT;
}

uint8_t sam_to_mgrec_phase2(
    transcoder::ProgramOptions& options,
    int& nref
){
    for (auto iref = 0; iref < nref; iref++){
        auto n_tmp_files = 0;

        std::string fpath = options.tmp_dir_path + "/" + std::to_string(iref) + PHASE1_EXT;
        SubfileReader mgg_reader(fpath);

        std::string tmp_fpath = options.tmp_dir_path + "/" + std::to_string(iref) + PHASE2_EXT;
        std::ofstream p2_writer(tmp_fpath, std::ios::binary | std::ios::trunc);
        genie::util::BitWriter p2_bitwriter(&p2_writer);

        std::vector<genie::core::record::Record> buffer;

        /// Split mgrec into multiple files and the records are sorted
        while(mgg_reader.good()) {
            /// Read MPEG-G records
            while (mgg_reader.readRecord() && buffer.size() < PHASE2_BUFFER_SIZE) {
                UTILS_DIE_IF(mgg_reader.getRecord().getClassID() != genie::core::record::ClassType::CLASS_I,
                             "Invalid Class found while reading records from phase 1 transcoding");
//
                /// Store unmapped record to output file
                if (mgg_reader.getRecord().getAlignments().empty()) {
                    mgg_reader.writeRecord(p2_bitwriter);
                } else {
                    buffer.emplace_back(mgg_reader.moveRecord());
                }
            }

            /// Sort records in buffer by POS
            std::sort(buffer.begin(), buffer.end(), compare);

            std::ofstream tmp_writer(gen_p2_tmp_fpath(options, iref, n_tmp_files++),
                                     std::ios::trunc | std::ios::binary);
            genie::util::BitWriter tmp_bitwriter(&tmp_writer);

            for (auto& rec : buffer) {
                rec.write(tmp_bitwriter);
                tmp_bitwriter.flush();
            }

            buffer.clear();
            tmp_writer.close();
        }

        /// Open and cache the first record from each temporary file
        std::list<SubfileReader> tmp_file_readers;
        for (auto i_file=0; i_file < n_tmp_files; i_file++){
            tmp_file_readers.emplace_back(gen_p2_tmp_fpath(options, iref, i_file));

            /// Try to cache the first MPEG-G record, otherwise delete
            if (!tmp_file_readers.back().readRecord()){
                tmp_file_readers.pop_back();
            }
        }

        while(!tmp_file_readers.empty()){
            /// Find reader containing MPEG-G record with smallest POS value
            auto reader_with_smallest_pos = tmp_file_readers.begin();
            for (auto iter = std::next(tmp_file_readers.begin()); iter != tmp_file_readers.end(); iter++){
                if (iter->getPos() < reader_with_smallest_pos->getPos()){
                    reader_with_smallest_pos = iter;
                }
            }

            reader_with_smallest_pos->writeRecord(p2_bitwriter);

            /// Close the current tmp file if it contains no more record;
            if (!reader_with_smallest_pos->readRecord()){
                tmp_file_readers.erase(reader_with_smallest_pos);
            }
        }

        /// Close Phase 2 transcoding output file
        p2_bitwriter.flush();
        p2_writer.close();

        /// Remove all temporary file belonging to current RID
        for (auto i_file=0; i_file < n_tmp_files; i_file++){
            std::remove(gen_p2_tmp_fpath(options, iref, i_file).c_str());
        }
    }

    return EXIT_SUCCESS;
}

uint8_t sam_to_mgrec(transcoder::ProgramOptions& options){

    int nref;
    uint8_t status;

    if ((status = sam_to_mgrec_phase1(options, nref)) != EXIT_SUCCESS){
        return EXIT_FAILURE;
    }

    if ((status = sam_to_mgrec_phase2(options, nref)) != EXIT_SUCCESS){
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;

}


}