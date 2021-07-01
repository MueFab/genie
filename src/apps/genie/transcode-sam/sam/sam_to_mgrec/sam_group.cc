/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "sam_group.h"

// ---------------------------------------------------------------------------------------------------------------------

#include <boost/optional/optional.hpp>
#include <iostream>
#include <map>
#include <string>
#include <utility>

// ---------------------------------------------------------------------------------------------------------------------

#include "genie/core/record/alignment_external/other-rec.h"
#include "genie/core/record/alignment_split/other-rec.h"
#include "genie/core/record/alignment_split/unpaired.h"
#include "apps/genie/transcode-sam/utils.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genieapp {
namespace transcode_sam {
namespace sam {
namespace sam_to_mgrec {

// ---------------------------------------------------------------------------------------------------------------------

std::tuple<bool, uint8_t> SamRecordGroup::convertFlags2Mpeg(uint16_t flags) {
    uint8_t flags_mpeg = 0;

    flags_mpeg |= (flags & BAM_FDUP) >> 10u;         // PCR / duplicate
    flags_mpeg |= (flags & BAM_FQCFAIL) >> 8u;       // vendor quality check failed
    flags_mpeg |= (flags & BAM_FPROPER_PAIR) << 1u;  // proper pair

    bool rcomp = flags & BAM_FREVERSE;  // rcomp

    return std::make_tuple(rcomp, flags_mpeg);
}

// ---------------------------------------------------------------------------------------------------------------------

void SamRecordGroup::addAlignment(genie::core::record::Record &rec, SamRecord *r1, SamRecord *r2, bool paired_end,
                                  bool force_split) {
    SamRecord *base_rec = r1 ? r1 : r2;
    if(r1 && r2 && r1->isUnmapped()) {
        base_rec = r2;
    }
    genie::core::record::Alignment alignment(base_rec->getECigar(), base_rec->isReverse());
    alignment.addMappingScore(base_rec->getMapq());

    genie::core::record::AlignmentBox alignmentContainer(base_rec->getPos(), std::move(alignment));

    // Single end or class HM (only possible if both records available )
    if (!paired_end || (r1 && r2 && (r1->isUnmapped() || r2->isUnmapped()))) {
        rec.addAlignment(base_rec->getRID(), std::move(alignmentContainer));
        return;
    }

    /// Case4: Unmapped
    if (r1 == nullptr || r2 == nullptr) {
        auto splitAlign = genie::util::make_unique<genie::core::record::alignment_split::Unpaired>();
        alignmentContainer.addAlignmentSplit(std::move(splitAlign));

    } else if (r1->getRID() == r2->getRID()) {
        /// Case 2: OtherRec - Same RID
        if (force_split) {
            auto splitAlign =
                genie::util::make_unique<genie::core::record::alignment_split::OtherRec>(r2->getPos() - 1, r2->getRID());
            alignmentContainer.addAlignmentSplit(std::move(splitAlign));

            /// Case 1: SameRec
        } else {
            genie::core::record::Alignment alignment2(r2->getECigar(), r2->isReverse());
            alignment2.addMappingScore(r2->getMapq());

            auto delta = r2->getPos() - r1->getPos();
            auto splitAlign =
                genie::util::make_unique<genie::core::record::alignment_split::SameRec>(delta, std::move(alignment2));

            alignmentContainer.addAlignmentSplit(std::move(splitAlign));
        }

        /// Case 2 : OtherRec - Different RID
    } else {
        auto splitAlign =
            genie::util::make_unique<genie::core::record::alignment_split::OtherRec>(r2->getPos(), r2->getRID());
        alignmentContainer.addAlignmentSplit(std::move(splitAlign));
    }

    rec.addAlignment(base_rec->getRID(), std::move(alignmentContainer));
}

// ---------------------------------------------------------------------------------------------------------------------

void SamRecordGroup::createMgrecAndAddAlignment(std::map<int32_t, genie::core::record::Record> &,
                                                std::vector<int32_t> &, SamRecord &, SamRecord *) {
    /*  auto mgrec_iter = mgrecs_by_rid.find(rec.getRID());

      if (mgrec_iter == mgrecs_by_rid.end()) {
          auto primary_rid = recs_rid_order.front();
          if (rec.getSeq().empty()) {
              rec.setSeq(mgrecs_by_rid.at(primary_rid).getSegments().front().getSequence());
          }

          if (rec.getQual().empty()) {
              rec.setQual(mgrecs_by_rid.at(primary_rid).getSegments().front().getQualities().front());
          }

          auto rid = rec.getRID();

          /// Create new MPEG-G record
          genie::core::record::Record mgrec = pairedEndedToMpegg(rec, other_rec);
          addAlignment(mgrec, rec, other_rec);
          mgrecs_by_rid.emplace(rid, std::move(mgrec));
          recs_rid_order.emplace_back(rid);

      } else {
          addAlignment(mgrec_iter->second, rec, other_rec);
      } */
}

// ---------------------------------------------------------------------------------------------------------------------

void SamRecordGroup::convertPairedEnd(std::map<int32_t, genie::core::record::Record> &, std::vector<int32_t> &,
                                      std::map<int32_t, genie::core::record::Record> &, std::vector<int32_t> &,
                                      std::list<std::list<SamRecord>>) {
    /*   auto r1_template = sam_recs_2d.begin();
       auto r2_template = std::next(r1_template);

       /// Handles the primary alignment of read1
       if (!r1_template->empty()) {
           recs1_rid_order.emplace_back(r1_template->front().getRID());

           genie::core::record::Record rec = pairedEndedToMpegg(r1_template->front(), nullptr);

           if (!r2_template->empty()) {
               addAlignment(rec, r1_template->front(), &r2_template->front());
           } else {
               addAlignment(rec, r1_template->front(), nullptr);
           }

           recs1_by_rid.emplace(r1_template->front().getRID(), std::move(rec));
       }
       /// Handles the primary alignment of read2
       if (!r2_template->empty()) {
           recs2_rid_order.emplace_back(r2_template->front().getRID());

           genie::core::record::Record rec = pairedEndedToMpegg(r2_template->front(), nullptr);

           if (!r1_template->empty()) {
               addAlignment(rec, r2_template->front(), &r1_template->front());
           } else {
               addAlignment(rec, r2_template->front(), nullptr);
           }

           recs2_by_rid.emplace(r2_template->front().getRID(), std::move(rec));
       }

       /// Remove transcoded SAM record
       if (!r1_template->empty()) r1_template->pop_front();
       if (!r2_template->empty()) r2_template->pop_front();

       while (!r1_template->empty()) {
           auto r1_rec_iter = r1_template->begin();

           bool pair_found = false;
           /// Find pair of r1_rec_iter and add alignment to MPEG-G record if exist, otherwise create new record first
           for (auto r2_rec_iter = r2_template->begin(); r2_rec_iter != r2_template->end(); r2_rec_iter++) {
               if (r1_rec_iter->isPairOf(*r2_rec_iter)) {
                   /// Process read1
                   createMgrecAndAddAlignment(recs1_by_rid, recs1_rid_order, *r1_rec_iter, &*r2_rec_iter);

                   /// Process read2
                   createMgrecAndAddAlignment(recs2_by_rid, recs2_rid_order, *r2_rec_iter, &*r1_rec_iter);

                   r2_template->erase(r2_rec_iter);
                   pair_found = true;
                   break;
               }
           }

           /// At this point no pair of r1_rec_iter is found, thus handle case 4 (No pair)
           if (!pair_found) {
               createMgrecAndAddAlignment(recs1_by_rid, recs1_rid_order, *r1_rec_iter, nullptr);
           }

           r1_template->pop_front();
       }

       while (!r2_template->empty()) {
           auto r2_rec_iter = r2_template->begin();

           bool pair_found = false;
           /// Find pair of r1_rec_iter and add alignment to MPEG-G record if exist, otherwise create new record first
           for (auto r1_rec_iter = r1_template->begin(); r1_rec_iter != r1_template->end(); r1_rec_iter++) {
               if (r2_rec_iter->isPairOf(*r1_rec_iter)) {
                   /// Process read2
                   createMgrecAndAddAlignment(recs2_by_rid, recs2_rid_order, *r2_rec_iter, &*r1_rec_iter);

                   /// Process read1
                   createMgrecAndAddAlignment(recs1_by_rid, recs1_rid_order, *r1_rec_iter, &*r2_rec_iter);

                   r2_template->erase(r2_rec_iter);
                   pair_found = true;
                   break;
               }
           }

           /// At this point no pair of r2_rec_iter is found, thus handle case 4 (No pair)
           if (!pair_found) {
               createMgrecAndAddAlignment(recs2_by_rid, recs2_rid_order, *r2_rec_iter, nullptr);
           }

           r2_template->pop_front();
       } */
}

// ---------------------------------------------------------------------------------------------------------------------

void SamRecordGroup::handlesMoreAlignments(std::map<int32_t, genie::core::record::Record> &recs_by_rid,
                                           std::vector<int32_t> &recs_rid_order) {
    UTILS_DIE_IF(recs_by_rid.size() != recs_rid_order.size(), "recs_by_rid and recs_rid_order have different size");

    if (recs_rid_order.size() < 2) {
        return;
    }

    for (auto rid : recs_rid_order) {
        UTILS_DIE_IF(recs_by_rid.find(rid) == recs_by_rid.end(),
                     "Could not find MPEG-G record with with RID " + std::to_string(rid));
    }

    for (auto tail_rid_iter = std::next(recs_rid_order.begin()); tail_rid_iter != recs_rid_order.end();
         tail_rid_iter++) {
        auto head_rid_iter = std::prev(tail_rid_iter);
        auto head_mgrec_iter = recs_by_rid.find(*head_rid_iter);
        auto tail_mgrec_iter = recs_by_rid.find(*tail_rid_iter);

        auto next_seq_ID = *tail_rid_iter;
        auto next_pos = getMinPos(tail_mgrec_iter->second);

        auto more_alignments =
            genie::util::make_unique<genie::core::record::alignment_external::OtherRec>(next_pos, next_seq_ID);
        head_mgrec_iter->second.setMoreAlignmentInfo(std::move(more_alignments));
    }
}

// ---------------------------------------------------------------------------------------------------------------------

SamRecordGroup::SamRecordGroup()
    : data(size_t(TemplateType::TOTAL_INDICES),
           std::vector<std::vector<SamRecord>>(size_t(MappingType::TOTAL_INDICES))) {}

// ---------------------------------------------------------------------------------------------------------------------

void SamRecordGroup::addRecord(SamRecord &&rec) {
    TemplateType template_type = TemplateType::TOTAL_INDICES;
    if (!rec.isPaired()) {
        template_type = TemplateType::SINGLE;
    } else {
        if (rec.isRead1()) {
            template_type = TemplateType::PAIRED_1;
        } else if (rec.isRead2()) {
            template_type = TemplateType::PAIRED_2;
        } else {
            template_type = TemplateType::PAIRED_UNKNOWN;
        }
    }

    MappingType mapping_type = MappingType::TOTAL_INDICES;
    if (rec.isUnmapped()) {
        mapping_type = MappingType::UNMAPPED;
    } else if (rec.isPrimary()) {
        mapping_type = MappingType::PRIMARY;
    } else {
        mapping_type = MappingType::NONPRIMARY;
    }

    data[uint8_t(template_type)][uint8_t(mapping_type)].push_back(std::move(rec));
}

// ---------------------------------------------------------------------------------------------------------------------

std::pair<bool, genie::core::record::ClassType> SamRecordGroup::validate() {
    size_t count_single_end = 0;
    size_t count_paired_end = 0;

    for (auto &a : data[uint8_t(TemplateType::SINGLE)]) {
        count_single_end += a.size();
    }
    for (auto &a : data[uint8_t(TemplateType::PAIRED_1)]) {
        count_paired_end += a.size();
    }
    for (auto &a : data[uint8_t(TemplateType::PAIRED_2)]) {
        count_paired_end += a.size();
    }
    for (auto &a : data[uint8_t(TemplateType::PAIRED_UNKNOWN)]) {
        count_paired_end += a.size();
    }

    UTILS_DIE_IF(count_single_end > 0 && count_paired_end > 0, "Paired and unpaired records in same SAM template");

    bool paired = count_paired_end > 0;
    if (!paired) {
        size_t primary_count = data[uint8_t(TemplateType::SINGLE)][uint8_t(MappingType::PRIMARY)].size() +
                               data[uint8_t(TemplateType::SINGLE)][uint8_t(MappingType::UNMAPPED)].size();
        UTILS_DIE_IF(primary_count > 1, "Multiple primary alignments in single ended SAM record");
        if (primary_count == 0) {
            std::cerr << "Warning: Single ended SAM record without primary reads, discarding whole record" << std::endl;
            data.clear();
            return std::make_pair(false, genie::core::record::ClassType::NONE);
        } else if (data[uint8_t(TemplateType::SINGLE)][uint8_t(MappingType::PRIMARY)].empty() &&
                   !data[uint8_t(TemplateType::SINGLE)][uint8_t(MappingType::NONPRIMARY)].empty()) {
            std::cerr << "Warning: Single ended SAM record without primary alignment, but with secondary alignments, "
                         "discarding "
                         "alignments"
                      << std::endl;
            data[uint8_t(TemplateType::SINGLE)][uint8_t(MappingType::NONPRIMARY)].clear();
        }
        return data[uint8_t(TemplateType::SINGLE)][uint8_t(MappingType::PRIMARY)].empty()
                   ? std::make_pair(false, genie::core::record::ClassType::CLASS_U)
                   : std::make_pair(false, genie::core::record::ClassType::CLASS_I);
    } else {
        while (data[uint8_t(TemplateType::PAIRED_1)][uint8_t(MappingType::PRIMARY)].size() > 1) {
            if (data[uint8_t(TemplateType::PAIRED_1)][uint8_t(MappingType::PRIMARY)].back() ==
                data[uint8_t(TemplateType::PAIRED_1)][uint8_t(MappingType::PRIMARY)].front()) {
                data[uint8_t(TemplateType::PAIRED_1)][uint8_t(MappingType::PRIMARY)].pop_back();
            } else {
                UTILS_DIE("Too many Pair_1 reads");
            }
        }
        while (data[uint8_t(TemplateType::PAIRED_1)][uint8_t(MappingType::UNMAPPED)].size() > 1) {
            if (data[uint8_t(TemplateType::PAIRED_1)][uint8_t(MappingType::UNMAPPED)].back() ==
                data[uint8_t(TemplateType::PAIRED_1)][uint8_t(MappingType::UNMAPPED)].front()) {
                data[uint8_t(TemplateType::PAIRED_1)][uint8_t(MappingType::UNMAPPED)].pop_back();
            } else {
                UTILS_DIE("Too many Pair_1 reads");
            }
        }

        while (data[uint8_t(TemplateType::PAIRED_2)][uint8_t(MappingType::PRIMARY)].size() > 1) {
            if (data[uint8_t(TemplateType::PAIRED_2)][uint8_t(MappingType::PRIMARY)].back() ==
                data[uint8_t(TemplateType::PAIRED_2)][uint8_t(MappingType::PRIMARY)].front()) {
                data[uint8_t(TemplateType::PAIRED_2)][uint8_t(MappingType::PRIMARY)].pop_back();
            } else {
                UTILS_DIE("Too many Pair_2 reads");
            }
        }
        while (data[uint8_t(TemplateType::PAIRED_2)][uint8_t(MappingType::UNMAPPED)].size() > 1) {
            if (data[uint8_t(TemplateType::PAIRED_2)][uint8_t(MappingType::UNMAPPED)].back() ==
                data[uint8_t(TemplateType::PAIRED_2)][uint8_t(MappingType::UNMAPPED)].front()) {
                data[uint8_t(TemplateType::PAIRED_2)][uint8_t(MappingType::UNMAPPED)].pop_back();
            } else {
                UTILS_DIE("Too many Pair_2 reads");
            }
        }
        size_t read_1_count = data[uint8_t(TemplateType::PAIRED_1)][uint8_t(MappingType::PRIMARY)].size() +
                              data[uint8_t(TemplateType::PAIRED_1)][uint8_t(MappingType::UNMAPPED)].size();
        size_t read_2_count = data[uint8_t(TemplateType::PAIRED_2)][uint8_t(MappingType::PRIMARY)].size() +
                              data[uint8_t(TemplateType::PAIRED_2)][uint8_t(MappingType::UNMAPPED)].size();
        size_t unknown_count = data[uint8_t(TemplateType::PAIRED_UNKNOWN)][uint8_t(MappingType::PRIMARY)].size() +
                               data[uint8_t(TemplateType::PAIRED_UNKNOWN)][uint8_t(MappingType::UNMAPPED)].size();
        if (read_1_count + read_2_count + unknown_count > 2) {
            UTILS_DIE_IF(read_1_count + read_2_count + unknown_count > 2,
                         "More than 2 primary reads in paired sam record");
        }

        if (read_1_count + read_2_count + unknown_count == 0) {
            std::cerr << "Warning: Paired-end SAM record without primary reads, discarding whole record" << std::endl;
            data.clear();
            return std::make_pair(true, genie::core::record::ClassType::NONE);
        }

        // Get rid of unknown pair index
        if (data[uint8_t(TemplateType::PAIRED_UNKNOWN)][uint8_t(MappingType::PRIMARY)].size() > 1 &&
            !data[uint8_t(TemplateType::PAIRED_UNKNOWN)][uint8_t(MappingType::NONPRIMARY)].empty()) {
            std::cerr << "Warning: Secondary alignments without read ordering are discarded" << std::endl;
            data[uint8_t(TemplateType::PAIRED_UNKNOWN)][uint8_t(MappingType::NONPRIMARY)].clear();
        }
        while (unknown_count > 0) {
            auto index = data[uint8_t(TemplateType::PAIRED_UNKNOWN)][uint8_t(MappingType::PRIMARY)].empty()
                             ? uint8_t(MappingType::UNMAPPED)
                             : uint8_t(MappingType::PRIMARY);
            if (read_1_count == 0) {
                data[uint8_t(TemplateType::PAIRED_1)][index].push_back(
                    data[uint8_t(TemplateType::PAIRED_UNKNOWN)][index].back());
                data[uint8_t(TemplateType::PAIRED_UNKNOWN)][index].pop_back();
                if (index == uint8_t(MappingType::PRIMARY)) {
                    auto &vec = data[uint8_t(TemplateType::PAIRED_1)][uint8_t(MappingType::NONPRIMARY)];
                    auto &vec2 = data[uint8_t(TemplateType::PAIRED_UNKNOWN)][uint8_t(MappingType::NONPRIMARY)];
                    vec.insert(vec.end(), vec2.begin(), vec2.end());
                    vec2.clear();
                }
                read_1_count++;
                unknown_count--;
            } else if (read_2_count == 0) {
                data[uint8_t(TemplateType::PAIRED_2)][index].push_back(
                    data[uint8_t(TemplateType::PAIRED_UNKNOWN)][index].back());
                data[uint8_t(TemplateType::PAIRED_UNKNOWN)][index].pop_back();
                if (index == uint8_t(MappingType::PRIMARY)) {
                    auto &vec = data[uint8_t(TemplateType::PAIRED_2)][uint8_t(MappingType::NONPRIMARY)];
                    auto &vec2 = data[uint8_t(TemplateType::PAIRED_UNKNOWN)][uint8_t(MappingType::NONPRIMARY)];
                    vec.insert(vec.end(), vec2.begin(), vec2.end());
                    vec2.clear();
                }
                read_2_count++;
                unknown_count--;
            }
        }

        auto s1 = std::move(data[uint8_t(TemplateType::PAIRED_1)][uint8_t(MappingType::NONPRIMARY)]);
        auto s2 = std::move(data[uint8_t(TemplateType::PAIRED_2)][uint8_t(MappingType::NONPRIMARY)]);
        for (auto it1 = s1.begin(); it1 != s1.end(); ++it1) {
            for (auto it2 = s2.begin(); it2 != s2.end(); ++it2) {
                if (it1->isPairOf(*it2)) {
                    data[uint8_t(TemplateType::PAIRED_1)][uint8_t(MappingType::NONPRIMARY)].push_back(*it1);
                    data[uint8_t(TemplateType::PAIRED_2)][uint8_t(MappingType::NONPRIMARY)].push_back(*it2);
                    it1 = s1.erase(it1);
                    it2 = s2.erase(it2);
                    break;
                }
            }
        }
        for (auto &it1 : s1) {
            data[uint8_t(TemplateType::PAIRED_1)][uint8_t(MappingType::NONPRIMARY)].push_back(it1);
        }
        for (auto &it2 : s2) {
            data[uint8_t(TemplateType::PAIRED_2)][uint8_t(MappingType::NONPRIMARY)].push_back(it2);
        }

        size_t unmapped_count = data[uint8_t(TemplateType::PAIRED_1)][uint8_t(MappingType::UNMAPPED)].size() +
                                data[uint8_t(TemplateType::PAIRED_2)][uint8_t(MappingType::UNMAPPED)].size();

        if (unmapped_count == 0) {
            return std::make_pair(true, genie::core::record::ClassType::CLASS_I);
        } else if (unmapped_count == read_1_count + read_2_count) {
            return std::make_pair(true, genie::core::record::ClassType::CLASS_U);
        } else {
            return std::make_pair(true, genie::core::record::ClassType::CLASS_HM);
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void SamRecordGroup::convert(std::list<genie::core::record::Record> &records, bool) {
    std::list<std::list<SamRecord>> sam_recs_2d;

    std::map<int32_t, genie::core::record::Record> recs1_by_rid, recs2_by_rid;
    std::vector<int32_t> recs1_rid_order, recs2_rid_order;

    auto cls = validate();
    if (cls.second == genie::core::record::ClassType::NONE) {
        return;
    }

    SamRecord *r1 = nullptr;
    SamRecord *r2 = nullptr;

    if (!data[uint8_t(TemplateType::SINGLE)][uint8_t(MappingType::PRIMARY)].empty()) {
        r1 = &data[uint8_t(TemplateType::SINGLE)][uint8_t(MappingType::PRIMARY)].front();
    } else if (!data[uint8_t(TemplateType::SINGLE)][uint8_t(MappingType::UNMAPPED)].empty()) {
        r1 = &data[uint8_t(TemplateType::SINGLE)][uint8_t(MappingType::UNMAPPED)].front();
    } else if (!data[uint8_t(TemplateType::PAIRED_1)][uint8_t(MappingType::PRIMARY)].empty()) {
        r1 = &data[uint8_t(TemplateType::PAIRED_1)][uint8_t(MappingType::PRIMARY)].front();
    } else if (!data[uint8_t(TemplateType::PAIRED_1)][uint8_t(MappingType::UNMAPPED)].empty()) {
        r1 = &data[uint8_t(TemplateType::PAIRED_1)][uint8_t(MappingType::UNMAPPED)].front();
    }
    if (!data[uint8_t(TemplateType::PAIRED_2)][uint8_t(MappingType::PRIMARY)].empty()) {
        r2 = &data[uint8_t(TemplateType::PAIRED_2)][uint8_t(MappingType::PRIMARY)].front();
    } else if (!data[uint8_t(TemplateType::PAIRED_2)][uint8_t(MappingType::UNMAPPED)].empty()) {
        r2 = &data[uint8_t(TemplateType::PAIRED_2)][uint8_t(MappingType::UNMAPPED)].front();
    }

    auto flag_tuple = convertFlags2Mpeg(r1 ? r1->getFlag() : r2->getFlag());
    bool is_r1_first = false;
    if (cls.second == genie::core::record::ClassType::CLASS_U || !cls.first ||
        data[uint8_t(TemplateType::PAIRED_2)][uint8_t(MappingType::PRIMARY)].empty()) {
        is_r1_first = true;
    } else if (data[uint8_t(TemplateType::PAIRED_1)][uint8_t(MappingType::PRIMARY)].empty()) {
        is_r1_first = false;
    } else {
        is_r1_first = data[uint8_t(TemplateType::PAIRED_1)][uint8_t(MappingType::PRIMARY)].front().getPos() <
                      data[uint8_t(TemplateType::PAIRED_2)][uint8_t(MappingType::PRIMARY)].front().getPos();
    }

    if( r1 ? r1->getQname() == "HSQ1004:134:C0D8DACXX:1:1101:10050:171246" : r2->getQname() == "HSQ1004:134:C0D8DACXX:1:1101:10050:171246") {
        std::cout << "Gotcha2" << std::endl;
    }

    genie::core::record::Record rec(cls.first ? 2 : 1, cls.second, r1 ? r1->moveQname() : r2->moveQname(), "Genie",
                                    std::get<1>(flag_tuple), is_r1_first);

    if (r1) {
        genie::core::record::Segment seg(r1->moveSeq());
        if (r1->getQual() != "*") {
            seg.addQualities(r1->moveQual());
        } else {
            seg.addQualities("");
        }
        rec.addSegment(std::move(seg));
    }
    if (r2) {
        genie::core::record::Segment seg(r2->moveSeq());
        if (r2->getQual() != "*") {
            seg.addQualities(r2->moveQual());
        } else {
            seg.addQualities("");
        }
        rec.addSegment(std::move(seg));
    }

    if (cls.second == genie::core::record::ClassType::CLASS_U) {
        records.push_back(std::move(rec));
        return;
    }

    if(is_r1_first) {
        addAlignment(rec, r1, r2, cls.first);
    } else {
        addAlignment(rec, r2, r1, cls.first);
    }

    records.push_back(std::move(rec));
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace sam_to_mgrec
}  // namespace sam
}  // namespace transcode_sam
}  // namespace genieapp

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
