#include "sam_group.h"
#include <record/alignment_split/other-rec.h>

namespace sam_transcoder {

std::tuple<bool, uint8_t> SamRecordGroup::convertFlags2Mpeg(uint16_t flags) {
    uint8_t flags_mpeg = 0;

    flags_mpeg |= (flags & BAM_FDUP) >> 10u;         // PCR / duplicate
    flags_mpeg |= (flags & BAM_FQCFAIL) >> 8u;       // vendor quality check failed
    flags_mpeg |= (flags & BAM_FPROPER_PAIR) << 1u;  // proper pair

    bool rcomp = flags & BAM_FREVERSE;  // rcomp

    return std::make_tuple(rcomp, flags_mpeg);
}

genie::core::record::Record SamRecordGroup::pairedEndedToMpegg(SamRecord& r1,
                                                               SamRecord* r2,
                                                               bool force_split){

    bool is_r1_read1 = r1.isRead1();
    bool is_r1_first;
    if (r2 == nullptr || r2->isUnmapped()) {
        is_r1_first = is_r1_read1;
    } else{
        is_r1_first = is_r1_read1 ? r1.getPos() < r2->getPos() : r1.getPos() > r2->getPos();
    }

    auto flag_tuple = convertFlags2Mpeg(r1.getFlag());

    genie::core::record::Record rec(2,
                                    genie::core::record::ClassType::CLASS_I, r1.moveQname(),
                                    "Genie",
                                    std::get<1>(flag_tuple),
                                    is_r1_first);

    genie::core::record::Segment segment(r1.moveSeq());
    if (r1.getQual() != "*") {
        segment.addQualities(r1.moveQual());
    } else {
        segment.addQualities(std::string("\0", 1));
    }
    rec.addSegment(std::move(segment));

    if (r2 != nullptr && !force_split){
        genie::core::record::Segment segment2(r2->moveSeq());
        if (r2->getQual() != "*") {
            segment2.addQualities(r2->moveQual());
        } else {
            segment2.addQualities(std::string("\0", 1));
        }
        rec.addSegment(std::move(segment2));
    }

    return rec;
}

void SamRecordGroup::addAlignment(genie::core::record::Record& rec,
                                  SamRecord& r,
                                  SamRecord* other_r,
                                  bool force_split){


    genie::core::record::Alignment alignment(r.getECigar(),
                                             r.isReverse());
    alignment.addMappingScore(r.getMapq());

    genie::core::record::AlignmentBox alignmentContainer(r.getPos(), std::move(alignment));

    /// Case4: Unmapped
    if (other_r == nullptr || other_r->isUnmapped()) {
        auto splitAlign = genie::util::make_unique<genie::core::record::alignment_split::Unpaired>();
        alignmentContainer.addAlignmentSplit(std::move(splitAlign));

    } else if (r.getRID() == other_r->getRID()) {
        /// Case 2: OtherRec - Same RID
        if (force_split){
            auto splitAlign = genie::util::make_unique<genie::core::record::alignment_split::OtherRec>(other_r->getPos(),
                                                                                                       other_r->getRID());
            alignmentContainer.addAlignmentSplit(std::move(splitAlign));

        /// Case 1: SameRec
        } else {
            genie::core::record::Alignment alignment2(other_r->getECigar(),
                                                      other_r->isReverse());
            alignment2.addMappingScore(other_r->getMapq());

            auto delta = (int64_t)other_r->getPos() - (int64_t)r.getPos();
            auto splitAlign = genie::util::make_unique<genie::core::record::alignment_split::SameRec>(delta,
                                                                                                      std::move(alignment2));

            alignmentContainer.addAlignmentSplit(std::move(splitAlign));
        }

    /// Case 2 : OtherRec - Different RID
    } else {
        auto splitAlign = genie::util::make_unique<genie::core::record::alignment_split::OtherRec>(other_r->getPos(),
                                                                                                   other_r->getRID());
        alignmentContainer.addAlignmentSplit(std::move(splitAlign));
    }

    rec.addAlignment(r.getRID(), std::move(alignmentContainer));
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
    genie::core::record::Record rec(1,
                                    genie::core::record::ClassType::CLASS_I,
                                    sam_rec.moveQname(), "Genie",
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

void SamRecordGroup::convertPairedEnd(std::map<int32_t, genie::core::record::Record>& recs1_by_rid,
                                      std::map<int32_t, genie::core::record::Record>& recs2_by_rid,
                                      std::list<std::list<SamRecord>> sam_recs_2d){


    auto r1_template = sam_recs_2d.begin();
    auto r2_template = std::next(r1_template);

    int32_t recs1_primary_rid, recs2_primary_rid;

    /// Only one of the primary alignment found
    if (!r1_template->empty()){
        recs1_primary_rid = r1_template->front().getRID();

        genie::core::record::Record rec = pairedEndedToMpegg(r1_template->front(), nullptr);

        if (!r2_template->empty()){
            addAlignment(rec, r1_template->front(), &r2_template->front());
        } else {
            addAlignment(rec, r1_template->front(), nullptr);
        }

        recs1_by_rid.emplace(r1_template->front().getRID(), std::move(rec));
    }
    if (!r2_template->empty()){
        recs2_primary_rid = r2_template->front().getRID();

        genie::core::record::Record rec = pairedEndedToMpegg(r2_template->front(), nullptr);

        if (!r1_template->empty()){
            addAlignment(rec, r2_template->front(), &r1_template->front());
        } else {
            addAlignment(rec, r2_template->front(), nullptr);
        }

        recs2_by_rid.emplace(r2_template->front().getRID(), std::move(rec));
    }

    if (!r1_template->empty()) r1_template->pop_front();
    if (!r2_template->empty()) r2_template->pop_front();

    while (!r1_template->empty()){
        auto r1_rec_iter = r1_template->begin();

        bool pair_found = false;

        /// Find pair of r1_rec_iter and add alignment to MPEG-G record if exist, otherwise create new record first
        for (auto r2_rec_iter = r2_template->begin(); r2_rec_iter != r2_template->end(); r2_rec_iter++){
            if (r1_rec_iter->isPairOf(*r2_rec_iter)){
                {
                    auto mgrec_iter = recs1_by_rid.find(r1_rec_iter->getRID());

                    if (mgrec_iter == recs1_by_rid.end()){

                        if (r1_rec_iter->getSeq().empty()){
                            r1_rec_iter->setSeq(
                                recs1_by_rid.at(recs1_primary_rid).getSegments().front().getSequence()
                            );
                        }

                        if (r1_rec_iter->getQual().empty()){
                            r1_rec_iter->setQual(
                                recs1_by_rid.at(recs1_primary_rid).getSegments().front().getQualities().front()
                            );
                        }

                        genie::core::record::Record rec = pairedEndedToMpegg(*r1_rec_iter,
                                                                             &*r2_rec_iter);

                        addAlignment(rec, *r1_rec_iter, &*r2_rec_iter);

                        recs1_by_rid.emplace(r1_rec_iter->getRID(), std::move(rec));

                    } else {
                        addAlignment(mgrec_iter->second, *r1_rec_iter, &*r2_rec_iter);
                    }
                }

                {
                    auto mgrec_iter = recs2_by_rid.find(r2_rec_iter->getRID());

                    if (mgrec_iter == recs2_by_rid.end()){

                        if (r2_rec_iter->getSeq().empty()){
                            r2_rec_iter->setSeq(
                                recs2_by_rid.at(recs2_primary_rid).getSegments().front().getSequence()
                            );
                        }

                        if (r2_rec_iter->getQual().empty()){
                            r2_rec_iter->setQual(
                                recs2_by_rid.at(recs2_primary_rid).getSegments().front().getQualities().front()
                            );
                        }

                        genie::core::record::Record rec = pairedEndedToMpegg(*r2_rec_iter,
                                                                             &*r1_rec_iter);

                        addAlignment(rec, *r2_rec_iter, &*r1_rec_iter);

                        recs2_by_rid.emplace(r2_rec_iter->getRID(), std::move(rec));

                    } else {
                        addAlignment(mgrec_iter->second, *r2_rec_iter, &*r1_rec_iter);
                    }
                }

                r2_template->erase(r2_rec_iter);
                pair_found = true;
                break;
            }
        }

        /// At this point no pair of r1_rec_iter is found, thus handle case 4
        if (!pair_found){
            {
                auto mgrec_iter = recs1_by_rid.find(r1_rec_iter->getRID());

                if (mgrec_iter == recs1_by_rid.end()){

                    if (r1_rec_iter->getSeq().empty()){
                        r1_rec_iter->setSeq(
                            recs1_by_rid.at(recs1_primary_rid).getSegments().front().getSequence()
                        );
                    }

                    if (r1_rec_iter->getQual().empty()){
                        r1_rec_iter->setQual(
                            recs1_by_rid.at(recs1_primary_rid).getSegments().front().getQualities().front()
                        );
                    }

                    genie::core::record::Record rec = pairedEndedToMpegg(*r1_rec_iter,
                                                                         nullptr);

                    addAlignment(rec,
                                 *r1_rec_iter,
                                 nullptr);

                    recs1_by_rid.emplace(r1_rec_iter->getRID(), std::move(rec));

                } else {
                    addAlignment(mgrec_iter->second, *r1_rec_iter, nullptr);
                }
            }
        }

        r1_template->pop_front();
        //r1_template->erase(r1_rec_iter);
    }

    while (!r2_template->empty()){
        auto r2_rec_iter = r2_template->begin();

        bool pair_found = false;

        /// Find pair of r1_rec_iter and add alignment to MPEG-G record if exist, otherwise create new record first
        for (auto r1_rec_iter = r1_template->begin(); r1_rec_iter != r1_template->end(); r1_rec_iter++){
            if (r2_rec_iter->isPairOf(*r1_rec_iter)){

                /// Handle Read2
                {
                    auto mgrec_iter = recs2_by_rid.find(r2_rec_iter->getRID());

                    if (mgrec_iter == recs2_by_rid.end()){

                        if (r2_rec_iter->getSeq().empty()){
                            r2_rec_iter->setSeq(
                                recs2_by_rid.at(recs2_primary_rid).getSegments().front().getSequence()
                            );
                        }

                        if (r2_rec_iter->getQual().empty()){
                            r2_rec_iter->setQual(
                                recs2_by_rid.at(recs2_primary_rid).getSegments().front().getQualities().front()
                            );
                        }

                        genie::core::record::Record rec = pairedEndedToMpegg(*r2_rec_iter,
                                                                             &*r1_rec_iter);

                        addAlignment(rec, *r2_rec_iter, &*r1_rec_iter);

                        recs1_by_rid.emplace(r2_rec_iter->getRID(), std::move(rec));

                    } else {
                        addAlignment(mgrec_iter->second, *r2_rec_iter, &*r1_rec_iter);
                    }
                }

                /// Handle Read1
                {
                    auto mgrec_iter = recs1_by_rid.find(r1_rec_iter->getRID());

                    if (mgrec_iter == recs1_by_rid.end()){

                        if (r1_rec_iter->getSeq().empty()){
                            r1_rec_iter->setSeq(
                                recs1_by_rid.at(recs1_primary_rid).getSegments().front().getSequence()
                            );
                        }

                        if (r1_rec_iter->getQual().empty()){
                            r1_rec_iter->setQual(
                                recs1_by_rid.at(recs1_primary_rid).getSegments().front().getQualities().front()
                            );
                        }

                        genie::core::record::Record rec = pairedEndedToMpegg(*r2_rec_iter,
                                                                             &*r1_rec_iter);

                        addAlignment(rec, *r2_rec_iter, &*r1_rec_iter);

                        recs2_by_rid.emplace(r2_rec_iter->getRID(), std::move(rec));

                    } else {
                        addAlignment(mgrec_iter->second, *r2_rec_iter, &*r1_rec_iter);
                    }
                }

                r2_template->erase(r2_rec_iter);
                pair_found = true;
                break;
            }
        }

        /// At this point no pair of r1_rec_iter is found, thus handle case 4
        if (!pair_found){
            {
                auto mgrec_iter = recs2_by_rid.find(r2_rec_iter->getRID());

                if (mgrec_iter == recs2_by_rid.end()){

                    if (r2_rec_iter->getSeq().empty()){
                        r2_rec_iter->setSeq(
                            recs2_by_rid.at(recs2_primary_rid).getSegments().front().getSequence()
                        );
                    }

                    if (r2_rec_iter->getQual().empty()){
                        r2_rec_iter->setQual(
                            recs2_by_rid.at(recs2_primary_rid).getSegments().front().getQualities().front()
                        );
                    }

                    genie::core::record::Record rec = pairedEndedToMpegg(*r2_rec_iter,
                                                                         nullptr);

                    addAlignment(rec, *r2_rec_iter, nullptr);

                    recs1_by_rid.emplace(r2_rec_iter->getRID(), std::move(rec));

                } else {
                    addAlignment(mgrec_iter->second, *r2_rec_iter, nullptr);
                }
            }
        }

        r2_template->pop_front();
        //r2_template->erase(r1_rec_iter);
    }
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

void SamRecordGroup::convert(std::list<genie::core::record::Record> &records, bool create_same_rec) {
    std::list<std::list<SamRecord>> sam_recs_2d;

    std::map<int32_t, genie::core::record::Record> recs1_by_rid, recs2_by_rid;

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
            // TODO: fix more_alignments
            convertPairedEnd(recs1_by_rid, recs2_by_rid, sam_recs_2d);
            if (create_same_rec){
                UTILS_DIE("Not Implemented");
            } else {
                for (auto &entry: recs1_by_rid){
                    records.emplace_back(std::move(entry.second));
                }
                recs1_by_rid.clear();

                for (auto &entry: recs2_by_rid){
                    records.emplace_back(std::move(entry.second));
                }
                recs2_by_rid.clear();
            }
            break;
        default:
            UTILS_DIE("Unhandeled conversion");
            break;
    }
}

}