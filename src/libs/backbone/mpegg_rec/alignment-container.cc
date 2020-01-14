/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "alignment-container.h"
#include <util/bitreader.h>
#include <util/bitwriter.h>
#include <util/make_unique.h>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace mpegg_rec {

// ---------------------------------------------------------------------------------------------------------------------

void AlignmentContainer::write(util::BitWriter& writer) const {
    writer.write(mapping_pos, 40);
    alignment.write(writer);
    for (const auto& a : splitAlignmentInfo) {
        a->write(writer);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

AlignmentContainer::AlignmentContainer(ClassType type, uint8_t as_depth, uint8_t number_of_template_segments,
                                       util::BitReader& reader) {
    mapping_pos = reader.read(40);
    alignment = Alignment(as_depth, reader);

    if (type == ClassType::CLASS_HM) {
        return;
    }
    for (size_t tSeg = 1; tSeg < number_of_template_segments; tSeg++) {
        splitAlignmentInfo.push_back(SplitAlignment::factory(as_depth, reader));
    }
}

//------------------------------------------------------------------------------------------------------------------

AlignmentContainer::AlignmentContainer() : mapping_pos(0), alignment(), splitAlignmentInfo(0) {}

// ---------------------------------------------------------------------------------------------------------------------

void AlignmentContainer::addSplitAlignment(std::unique_ptr<SplitAlignment> _alignment) {
    if (_alignment->getType() == SplitAlignment::SplitAlignmentType::SAME_REC) {
        if (dynamic_cast<SplitAlignmentSameRec&>(*_alignment).getAlignment().getMappingScores().size() !=
            alignment.getMappingScores().size()) {
            UTILS_DIE("AS depth incompatible");
        }
    }
    splitAlignmentInfo.push_back(std::move(_alignment));
}

// ---------------------------------------------------------------------------------------------------------------------

uint8_t AlignmentContainer::getNumberOfTemplateSegments() const { return uint8_t(splitAlignmentInfo.size() + 1); }

// ---------------------------------------------------------------------------------------------------------------------

uint64_t AlignmentContainer::getPosition() const { return mapping_pos; }

// ---------------------------------------------------------------------------------------------------------------------

const Alignment& AlignmentContainer::getAlignment() const { return alignment; }

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<std::unique_ptr<SplitAlignment>>& AlignmentContainer::getSplitAlignments() const {
    return splitAlignmentInfo;
}

// ---------------------------------------------------------------------------------------------------------------------

AlignmentContainer::AlignmentContainer(uint64_t _mapping_pos, Alignment&& _alignment)
    : mapping_pos(_mapping_pos), alignment(std::move(_alignment)) {}

// ---------------------------------------------------------------------------------------------------------------------

AlignmentContainer::AlignmentContainer(const AlignmentContainer& container) { *this = container; }

// ---------------------------------------------------------------------------------------------------------------------

AlignmentContainer::AlignmentContainer(AlignmentContainer&& container) noexcept { *this = container; }

// ---------------------------------------------------------------------------------------------------------------------

AlignmentContainer& AlignmentContainer::operator=(const AlignmentContainer& container) {
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

AlignmentContainer& AlignmentContainer::operator=(AlignmentContainer&& container) noexcept {
    this->mapping_pos = container.mapping_pos;
    this->alignment = container.alignment;
    this->splitAlignmentInfo = std::move(container.splitAlignmentInfo);
    return *this;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_rec
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------