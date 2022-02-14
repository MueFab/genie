/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/format/mgg/label_region.h"
#include <utility>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mgg {

// ---------------------------------------------------------------------------------------------------------------------

mgg::LabelRegion::LabelRegion(uint16_t _seq_ID, uint64_t _start_pos, uint64_t _end_pos)
    : seq_ID((_seq_ID)), class_IDs(), start_pos(_start_pos), end_pos(_end_pos) {}

// ---------------------------------------------------------------------------------------------------------------------

LabelRegion::LabelRegion(util::BitReader& reader) {
    // seq_ID u(16)
    seq_ID = reader.read<uint16_t>();

    // num_classes u(8)
    auto num_classes = reader.read<uint8_t>(4);
    class_IDs.resize(num_classes);
    // for class_IDs[] u(4)
    for (auto& class_ID : class_IDs) {
        class_ID = reader.read<genie::core::record::ClassType>(4);
    }

    // start_pos u(40)
    start_pos = reader.read<uint64_t>(40);

    // end_pos u(40)
    end_pos = reader.read<uint64_t>(40);
}

// ---------------------------------------------------------------------------------------------------------------------

void LabelRegion::addClassID(genie::core::record::ClassType _class_ID) { class_IDs.push_back(_class_ID); }

// ---------------------------------------------------------------------------------------------------------------------

uint16_t LabelRegion::getSeqID() const { return seq_ID; }

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<genie::core::record::ClassType>& LabelRegion::getClassIDs() const { return class_IDs; }

// ---------------------------------------------------------------------------------------------------------------------

uint64_t LabelRegion::getStartPos() const { return start_pos; }

// ---------------------------------------------------------------------------------------------------------------------

uint64_t LabelRegion::getEndPos() const { return end_pos; }

// ---------------------------------------------------------------------------------------------------------------------

void LabelRegion::write(util::BitWriter& bit_writer) const {
    // seq_ID u(16)
    bit_writer.write(seq_ID, 16);

    // num_classes u(8)
    bit_writer.write(class_IDs.size(), 4);

    // for class_IDs[] u(4)
    for (auto& class_ID : class_IDs) {
        bit_writer.write(static_cast<uint8_t>(class_ID), 4);
    }

    // start_pos u(40)
    bit_writer.write(start_pos, 40);

    // end_pos u(40)
    bit_writer.write(end_pos, 40);
}

// ---------------------------------------------------------------------------------------------------------------------

bool LabelRegion::operator==(const LabelRegion& other) const {
    return seq_ID == other.seq_ID && class_IDs == other.class_IDs && start_pos == other.start_pos &&
           end_pos == other.end_pos;
}

// ---------------------------------------------------------------------------------------------------------------------

uint64_t LabelRegion::sizeInBits() const { return sizeof(uint16_t) * 8 + 4 + 4 * class_IDs.size() + 40 + 40; }

// ---------------------------------------------------------------------------------------------------------------------

genie::core::meta::Region LabelRegion::decapsulate() {
    genie::core::meta::Region ret(seq_ID, start_pos, end_pos, std::move(class_IDs));
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mgg
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
