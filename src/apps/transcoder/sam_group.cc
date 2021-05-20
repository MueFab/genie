#include "sam_group.h"

namespace sam_transcoder {

std::tuple<bool, uint8_t> SamRecordGroup::convertFlags2Mpeg(uint16_t flags) {
    uint8_t flags_mpeg = 0;

    flags_mpeg |= (flags & BAM_FDUP) >> 10u;         // PCR / duplicate
    flags_mpeg |= (flags & BAM_FQCFAIL) >> 8u;       // vendor quality check failed
    flags_mpeg |= (flags & BAM_FPROPER_PAIR) << 1u;  // proper pair

    bool rcomp = flags & BAM_FREVERSE;  // rcomp

    return std::make_tuple(rcomp, flags_mpeg);
}

void SamRecordGroup::convertUnmapped(std::list<genie::core::record::Record> &records, SamRecord &sam_rec) {
    auto flag_tuple = convertFlags2Mpeg(sam_rec.getFlag());
    genie::core::record::Record rec(1, genie::core::record::ClassType::CLASS_I, sam_rec.moveQname(), "Genie",
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

void SamRecordGroup::convertSingleEnd(std::list<genie::core::record::Record> &records, std::list<SamRecord> &sam_recs,
                                      bool unmapped_pair, bool is_read_1_first) {
    auto sam_rec = std::move(sam_recs.front());
    sam_recs.pop_front();

    auto flag_tuple = convertFlags2Mpeg(sam_rec.getFlag());
    genie::core::record::Record rec(1, genie::core::record::ClassType::CLASS_I, sam_rec.moveQname(), "Genie",
                                    std::get<1>(flag_tuple), is_read_1_first);

    {
        genie::core::record::Alignment alignment(sam_rec.getECigar(), sam_rec.isReverse());

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
        genie::core::record::Alignment alignment(sam_rec_it->getECigar(), sam_rec_it->isReverse());
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

void SamRecordGroup::convertPairedEnd(std::list<genie::core::record::Record> &records,
                                      std::list<std::list<SamRecord>> &sam_recs_2d,
                                      bool force_split) {

    auto &read_1 = sam_recs_2d.front();
    auto read_1_empty = read_1.empty();
    auto read_1_unmapped = false;
    auto read_1_primary = false;
    if (!read_1_empty) {
        read_1_primary = read_1.front().isPrimary();
        read_1_unmapped = read_1.front().isUnmapped();
    }
    auto read_1_ok = !read_1_empty && read_1_primary && !read_1_unmapped;

    auto &read_2 = sam_recs_2d.back();
    auto read_2_empty = read_2.empty();
    auto read_2_unmapped = false;
    auto read_2_primary = false;
    if (!read_2_empty) {
        read_2_primary = read_2.front().isPrimary();
        read_2_unmapped = read_2.front().isUnmapped();
    }
    auto read_2_ok = !read_2_empty && read_2_primary && !read_2_unmapped;

//    if (read_1_ok && read_2_ok) {
//        UTILS_DIE_IF(!read_1.front().isPairOf(read_2.front()), "read_1 is not pair of read_2");
//
//        auto create_split_records = false;
//
//        // Check whether read_1 has the same number of alignments as read_2 and the references are the same
//        if (read_1.size() == read_2.size()) {
//            auto read_1_it = read_1.begin();
//            auto read_2_it = read_2.begin();
//            auto any_different_ref = false;
//            while (read_1_it != read_1.end() && read_2_it != read_2.end()) {
//                if (read_1_it->getRID() != read_2_it->getRID()) {
//                    any_different_ref = true;
//                    break;
//                }
//
//                read_1_it++;
//                read_2_it++;
//            }
//
//            create_split_records = any_different_ref;
//        }
//
//        if (create_split_records || force_split) {
////            // Swap template so that the first segment has lower mapping position
////            if (sam_recs_2d.front().front().getPos() > sam_recs_2d.front().back().getPos()){
////                std::swap(sam_recs_2d.front().front(), sam_recs_2d.front().back());
////            }
//
//            convertPairedEndSplitPair(records, sam_recs_2d, refs);
//        } else {
////            // Swap template so that the first segment has lower mapping position
////            if (sam_recs_2d.front().front().getPos() > sam_recs_2d.front().back().getPos()){
////                std::swap(sam_recs_2d.front().front(), sam_recs_2d.front().back());
////            }
//
//            convertPairedEndNoSplit(records, sam_recs_2d, refs);
//        }
//
//        // Handle alignments where one of the reads is not complete (unpaired etc)
//    } else {
//        if (!read_1_empty) {
//            if (read_1_unmapped) {
//                convertUnmapped(records, read_1);
//            } else if (!read_1_primary) {
//                UTILS_DIE("Cannot find the primary line of read_1!");
//            } else if (read_1_ok) {
//                convertSingleEnd(records, read_1, refs, true, true);
//            }
//        }
//
//        if (!read_2_empty) {
//            if (read_2_unmapped) {
//                convertUnmapped(records, read_2);
//            } else if (!read_2_primary) {
//                UTILS_DIE("Cannot find the primary line of read_2!");
//            } else if (read_2_ok) {
//                convertSingleEnd(records, read_2, refs, true, false);
//            }
//        }
//    }
//    sam_recs_2d.clear();
}

SamRecordGroup::SamRecordGroup() : data(size_t(Index::TOTAL_INDICES)) {}

void SamRecordGroup::addRecord(SamRecord &&rec) {
    /// Default is class "UNKNOWN"
    auto idx = Index::UNKNOWN;

    /// Single-end read
    if (!rec.isPaired()) {
        if (rec.isUnmapped()){
            idx = Index::UNMAPPED;
        } else {
            idx = Index::SINGLE;
        }
    }
    // Handles paired-end read / multi segments
    else {
        if (rec.isRead1()) {
            if (rec.isPrimary()) {
                idx = Index::PAIR_READ1_PRIMARY;
            } else {
                idx = Index::PAIR_READ1_NONPRIMARY;
            }
        } else if (rec.isRead2()) {
            if (rec.isPrimary()) {
                idx = Index::PAIR_READ2_PRIMARY;
            } else {
                idx = Index::PAIR_READ2_NONPRIMARY;
            }
        } else {
            UTILS_DIE("Neiter read1 nor read2:" + rec.getQname());
        }
    }

    data[uint8_t(idx)].push_back(std::move(rec));
}

SamRecordGroup::Category SamRecordGroup::getRecords(std::list<std::list<SamRecord>> &sam_recs) {
    sam_recs.clear();

    auto cls = computeClass();

    switch (cls) {
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

bool SamRecordGroup::isSingle() const {
    return !data[(uint8_t)Index::SINGLE].empty();
}  // Single non- and multiplie alignements

bool SamRecordGroup::isPaired() const {
    return !data[(uint8_t)Index::PAIR_READ1_PRIMARY].empty() && !data[(uint8_t)Index::PAIR_READ2_PRIMARY].empty();
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
        if ((is_unmapped && !is_single && !is_pair)) {
            return Category::UNMAPPED;
        } else if (!is_unmapped && is_single && !is_pair) {
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

    switch (cls) {
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

}