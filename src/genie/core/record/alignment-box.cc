/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/core/record/alignment-box.h"
#include <utility>
#include "genie/core/record/alignment_split/same-rec.h"
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"


// ---------------------------------------------------------------------------------------------------------------------

namespace genie::core::record {

// ---------------------------------------------------------------------------------------------------------------------

void AlignmentBox::write(util::BitWriter& writer) const {
    writer.writeBypassBE<uint64_t, 5>(mapping_pos);
    alignment.write(writer);
    for (const auto& a : splitAlignmentInfo) {
        a->write(writer);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

AlignmentBox::AlignmentBox(ClassType type, uint8_t as_depth, uint8_t number_of_template_segments,
                           util::BitReader& reader)
    : splitAlignmentInfo(type == ClassType::CLASS_HM ? 0 : number_of_template_segments - 1) {
    mapping_pos = reader.readBypassBE<uint64_t, 5>();
    alignment = Alignment(as_depth, reader);

    if (type == ClassType::CLASS_HM) {
        return;
    }
    for (size_t tSeg = 1; tSeg < number_of_template_segments; tSeg++) {
        splitAlignmentInfo[tSeg - 1] = AlignmentSplit::factory(as_depth, reader);
    }
}

//------------------------------------------------------------------------------------------------------------------

AlignmentBox::AlignmentBox() : mapping_pos(0), alignment(), splitAlignmentInfo(0) {}

// ---------------------------------------------------------------------------------------------------------------------

void AlignmentBox::addAlignmentSplit(std::unique_ptr<AlignmentSplit> _alignment) {
    if (_alignment->getType() == AlignmentSplit::Type::SAME_REC) {
        UTILS_DIE_IF(dynamic_cast<alignment_split::SameRec&>(*_alignment).getAlignment().getMappingScores().size() !=
                         alignment.getMappingScores().size(),
                     "AS depth incompatible");
    }
    splitAlignmentInfo.push_back(std::move(_alignment));
}

// ---------------------------------------------------------------------------------------------------------------------

uint8_t AlignmentBox::getNumberOfTemplateSegments() const { return uint8_t(splitAlignmentInfo.size() + 1); }

// ---------------------------------------------------------------------------------------------------------------------

uint64_t AlignmentBox::getPosition() const { return mapping_pos; }

// ---------------------------------------------------------------------------------------------------------------------

const Alignment& AlignmentBox::getAlignment() const { return alignment; }

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<std::unique_ptr<AlignmentSplit>>& AlignmentBox::getAlignmentSplits() const {
    return splitAlignmentInfo;
}

// ---------------------------------------------------------------------------------------------------------------------

AlignmentBox::AlignmentBox(uint64_t _mapping_pos, Alignment&& _alignment)
    : mapping_pos(_mapping_pos), alignment(std::move(_alignment)) {}

// ---------------------------------------------------------------------------------------------------------------------

AlignmentBox::AlignmentBox(const AlignmentBox& container) { *this = container; }

// ---------------------------------------------------------------------------------------------------------------------

AlignmentBox::AlignmentBox(AlignmentBox&& container) noexcept { *this = container; }

// ---------------------------------------------------------------------------------------------------------------------

AlignmentBox& AlignmentBox::operator=(const AlignmentBox& container) {
    if (this == &container) {
        return *this;
    }
    this->mapping_pos = container.mapping_pos;
    this->alignment = container.alignment;
    this->splitAlignmentInfo.clear();
    for (const auto& a : container.splitAlignmentInfo) {
        this->splitAlignmentInfo.emplace_back(a->clone());
    }
    return *this;
}

// ---------------------------------------------------------------------------------------------------------------------

AlignmentBox& AlignmentBox::operator=(AlignmentBox&& container) noexcept {
    this->mapping_pos = container.mapping_pos;
    this->alignment = container.alignment;
    this->splitAlignmentInfo = std::move(container.splitAlignmentInfo);
    return *this;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::core::record

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
