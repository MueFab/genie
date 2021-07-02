/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "apps/genie/transcode-sam/sam/sam_to_mgrec/sam_group.h"

// ---------------------------------------------------------------------------------------------------------------------

#include <iostream>
#include <map>
#include <string>
#include <utility>
#include "apps/genie/transcode-sam/utils.h"
#include "boost/optional/optional.hpp"
#include "genie/core/record/alignment_external/other-rec.h"
#include "genie/core/record/alignment_split/other-rec.h"
#include "genie/core/record/alignment_split/unpaired.h"

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
    SamRecord *base_rec = r1 ? r1 : r2;  // Record to derive name and flags from
    if (r1 && r2 && r1->isUnmapped()) {
        base_rec = r2;
    }
    genie::core::record::Alignment alignment(base_rec->getECigar(), base_rec->isReverse());
    alignment.addMappingScore(base_rec->getMapq());

    genie::core::record::AlignmentBox alignmentContainer(base_rec->getPos(), std::move(alignment));

    // Single end or class HM (only possible if both SAM records available ) -> No split alignment
    if (!paired_end || (r1 && r2 && (r1->isUnmapped() || r2->isUnmapped()))) {
        rec.addAlignment(base_rec->getRID(), std::move(alignmentContainer));
        return;
    }

    if (r1 == nullptr || r2 == nullptr) {
        // Only one SAM record is unavailable
        auto r_avail = r1 ? r1 : r2;  // The available SAM record
        if (r_avail->mate_rid == r_avail->getRID()) {
            // Case 1: Paired SAM record is missing, switch to unpaired.
            // TODO(fabian): Maybe still preserve pairing information?
            auto splitAlign = genie::util::make_unique<genie::core::record::alignment_split::Unpaired>();
            alignmentContainer.addAlignmentSplit(std::move(splitAlign));
        } else {
            // Case 2: Paired SAM record is mapped to other reference sequence, no way to check if data is actually
            // there
            auto splitAlign = genie::util::make_unique<genie::core::record::alignment_split::OtherRec>(
                r_avail->mate_pos, r_avail->mate_rid);
            alignmentContainer.addAlignmentSplit(std::move(splitAlign));
        }

    } else {
        // Both SAM records are available
        if (force_split) {  // TODO(fabian): Split automatically if delta > 32767
            // Case 1: split into two MPEG records
            auto splitAlign = genie::util::make_unique<genie::core::record::alignment_split::OtherRec>(r2->getPos() - 1,
                                                                                                       r2->getRID());
            alignmentContainer.addAlignmentSplit(std::move(splitAlign));

        } else {
            // Case 2: Merge into one MPEG record
            genie::core::record::Alignment alignment2(r2->getECigar(), r2->isReverse());
            alignment2.addMappingScore(r2->getMapq());

            auto delta = r2->getPos() - r1->getPos();
            auto splitAlign =
                genie::util::make_unique<genie::core::record::alignment_split::SameRec>(delta, std::move(alignment2));

            alignmentContainer.addAlignmentSplit(std::move(splitAlign));
        }
    }

    // Append alignment to record
    rec.addAlignment(base_rec->getRID(), std::move(alignmentContainer));
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

    // Put SAM record into the bucket for its mapping case
    data[uint8_t(template_type)][uint8_t(mapping_type)].push_back(std::move(rec));
}

// ---------------------------------------------------------------------------------------------------------------------

bool SamRecordGroup::checkIfPaired() {
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

    return count_paired_end > 0;
}

// ---------------------------------------------------------------------------------------------------------------------

genie::core::record::ClassType SamRecordGroup::checkClassTypeSingle() {
    size_t primary_count = data[uint8_t(TemplateType::SINGLE)][uint8_t(MappingType::PRIMARY)].size() +
                           data[uint8_t(TemplateType::SINGLE)][uint8_t(MappingType::UNMAPPED)].size();
    UTILS_DIE_IF(primary_count > 1, "Multiple primary alignments in single ended SAM record");
    if (primary_count == 0) {
        std::cerr << "Warning: Single ended SAM record without primary reads, discarding whole record" << std::endl;
        data.clear();
        return genie::core::record::ClassType::NONE;
    } else if (data[uint8_t(TemplateType::SINGLE)][uint8_t(MappingType::PRIMARY)].empty() &&
               !data[uint8_t(TemplateType::SINGLE)][uint8_t(MappingType::NONPRIMARY)].empty()) {
        std::cerr << "Warning: Single ended SAM record without primary alignment, but with secondary alignments, "
                     "discarding "
                     "alignments"
                  << std::endl;
        data[uint8_t(TemplateType::SINGLE)][uint8_t(MappingType::NONPRIMARY)].clear();
    }
    return data[uint8_t(TemplateType::SINGLE)][uint8_t(MappingType::PRIMARY)].empty()
               ? genie::core::record::ClassType::CLASS_U
               : genie::core::record::ClassType::CLASS_I;
}

// ---------------------------------------------------------------------------------------------------------------------

void SamRecordGroup::removeDuplicatesPaired(TemplateType tempType, MappingType mapType) {
    while (data[uint8_t(tempType)][uint8_t(mapType)].size() > 1) {
        if (data[uint8_t(tempType)][uint8_t(mapType)].back() == data[uint8_t(tempType)][uint8_t(mapType)].front()) {
            data[uint8_t(tempType)][uint8_t(mapType)].pop_back();
        } else {
            UTILS_DIE("Too many Pair_1 reads");
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

size_t SamRecordGroup::primaryTemplateCount(TemplateType tempType) {
    return data[uint8_t(tempType)][uint8_t(MappingType::PRIMARY)].size() +
           data[uint8_t(tempType)][uint8_t(MappingType::UNMAPPED)].size();
}

// ---------------------------------------------------------------------------------------------------------------------

size_t SamRecordGroup::mappingCount(MappingType mapType) {
    return data[uint8_t(TemplateType::PAIRED_1)][uint8_t(mapType)].size() +
           data[uint8_t(TemplateType::PAIRED_2)][uint8_t(mapType)].size() +
           data[uint8_t(TemplateType::PAIRED_UNKNOWN)][uint8_t(mapType)].size();
}

// ---------------------------------------------------------------------------------------------------------------------

void SamRecordGroup::removeAmbiguousSecondaryAlignments() {
    // Get rid of unknown pair index
    if (data[uint8_t(TemplateType::PAIRED_UNKNOWN)][uint8_t(MappingType::PRIMARY)].size() > 1 &&
        !data[uint8_t(TemplateType::PAIRED_UNKNOWN)][uint8_t(MappingType::NONPRIMARY)].empty()) {
        std::cerr << "Warning: Secondary alignments without read ordering are discarded" << std::endl;
        data[uint8_t(TemplateType::PAIRED_UNKNOWN)][uint8_t(MappingType::NONPRIMARY)].clear();
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void SamRecordGroup::moveSecondaryAlignments() {
    // Move secondary alignments which can be associated with a primary read
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

    // Move secondary alignments which can not be associated
    for (auto &it1 : s1) {
        data[uint8_t(TemplateType::PAIRED_1)][uint8_t(MappingType::NONPRIMARY)].push_back(it1);
    }
    for (auto &it2 : s2) {
        data[uint8_t(TemplateType::PAIRED_2)][uint8_t(MappingType::NONPRIMARY)].push_back(it2);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void SamRecordGroup::guessUnknownReadOrder(size_t &unknown_count, size_t &read_1_count, size_t &read_2_count) {
    // Get rid of unknown pair indices
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

    moveSecondaryAlignments();
}

// ---------------------------------------------------------------------------------------------------------------------

genie::core::record::ClassType SamRecordGroup::checkClassTypePaired() {
    removeDuplicatesPaired(TemplateType::PAIRED_1, MappingType::PRIMARY);
    removeDuplicatesPaired(TemplateType::PAIRED_1, MappingType::UNMAPPED);
    removeDuplicatesPaired(TemplateType::PAIRED_2, MappingType::PRIMARY);
    removeDuplicatesPaired(TemplateType::PAIRED_2, MappingType::UNMAPPED);

    size_t read_1_count = primaryTemplateCount(TemplateType::PAIRED_1);
    size_t read_2_count = primaryTemplateCount(TemplateType::PAIRED_2);
    size_t unknown_count = primaryTemplateCount(TemplateType::PAIRED_UNKNOWN);
    size_t primary_count = read_1_count + read_2_count + unknown_count;
    UTILS_DIE_IF(primary_count > 2, "More than 2 primary reads in paired sam record");

    if (primary_count == 0) {
        std::cerr << "Warning: Paired-end SAM record without primary reads, discarding whole record" << std::endl;
        data.clear();
        return genie::core::record::ClassType::NONE;
    }

    removeAmbiguousSecondaryAlignments();
    guessUnknownReadOrder(unknown_count, read_1_count, read_2_count);

    size_t unmapped_count = mappingCount(MappingType::UNMAPPED);

    if (unmapped_count == 0) {
        return genie::core::record::ClassType::CLASS_I;
    } else if (unmapped_count == read_1_count + read_2_count) {
        return genie::core::record::ClassType::CLASS_U;
    } else {
        return genie::core::record::ClassType::CLASS_HM;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

std::pair<bool, genie::core::record::ClassType> SamRecordGroup::validate() {
    bool paired = checkIfPaired();
    if (!paired) {
        return std::make_pair(false, checkClassTypeSingle());
    } else {
        return std::make_pair(true, checkClassTypePaired());
    }
}

// ---------------------------------------------------------------------------------------------------------------------

std::pair<SamRecord *, SamRecord *> SamRecordGroup::getReadTuple() {
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

    return std::make_pair(r1, r2);
}

// ---------------------------------------------------------------------------------------------------------------------

bool SamRecordGroup::isR1First(const std::pair<bool, genie::core::record::ClassType> &cls) {
    if (cls.second == genie::core::record::ClassType::CLASS_U || !cls.first ||
        data[uint8_t(TemplateType::PAIRED_2)][uint8_t(MappingType::PRIMARY)].empty()) {
        return true;
    } else if (data[uint8_t(TemplateType::PAIRED_1)][uint8_t(MappingType::PRIMARY)].empty()) {
        return false;
    } else {
        return data[uint8_t(TemplateType::PAIRED_1)][uint8_t(MappingType::PRIMARY)].front().getPos() <
               data[uint8_t(TemplateType::PAIRED_2)][uint8_t(MappingType::PRIMARY)].front().getPos();
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void SamRecordGroup::addSegment(genie::core::record::Record &rec, SamRecord *r1) {
    if (r1) {
        genie::core::record::Segment seg(r1->moveSeq());
        if (r1->getQual() != "*") {
            seg.addQualities(r1->moveQual());
        } else {
            seg.addQualities("");
        }
        rec.addSegment(std::move(seg));
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void SamRecordGroup::convert(std::list<genie::core::record::Record> &records, bool) {
    auto cls = validate();
    if (cls.second == genie::core::record::ClassType::NONE) {
        return;
    }

    auto tuple = SamRecordGroup::getReadTuple();
    SamRecord *r1 = tuple.first;
    SamRecord *r2 = tuple.second;

    auto flag_tuple = convertFlags2Mpeg(r1 ? r1->getFlag() : r2->getFlag());
    bool is_r1_first = isR1First(cls);

    genie::core::record::Record rec(cls.first ? 2 : 1, cls.second, r1 ? r1->moveQname() : r2->moveQname(), "Genie",
                                    std::get<1>(flag_tuple), is_r1_first);

    addSegment(rec, r1);
    addSegment(rec, r2);

    if (cls.second == genie::core::record::ClassType::CLASS_U) {
        records.push_back(std::move(rec));
        return;
    }

    if (is_r1_first) {
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
