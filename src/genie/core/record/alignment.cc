/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/core/record/alignment.h"
#include <string>
#include <utility>
#include <vector>
#include "genie/util/bit-reader.h"
#include "genie/util/bit-writer.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::core::record {

// ---------------------------------------------------------------------------------------------------------------------

Alignment::Alignment(std::string &&_ecigar_string, uint8_t _reverse_comp)
    : ecigar_string(std::move(_ecigar_string)), reverse_comp(_reverse_comp), mapping_score() {}

// ---------------------------------------------------------------------------------------------------------------------

Alignment::Alignment(uint8_t as_depth, util::BitReader &reader) {
    ecigar_string.resize(reader.readAlignedInt<uint32_t, 3>());
    reader.readAlignedBytes(&ecigar_string[0], ecigar_string.size());

    reverse_comp = reader.readAlignedInt<uint8_t>();
    mapping_score.resize(as_depth);
    reader.readAlignedBytes(&mapping_score[0], as_depth * sizeof(int32_t));
    for (auto &s : mapping_score) {
        util::swap_endianness(s);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

Alignment::Alignment() : ecigar_string(), reverse_comp(0), mapping_score(0) {}

// ---------------------------------------------------------------------------------------------------------------------

void Alignment::addMappingScore(int32_t score) { mapping_score.push_back(score); }

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<int32_t> &Alignment::getMappingScores() const { return mapping_score; }

// ---------------------------------------------------------------------------------------------------------------------

const std::string &Alignment::getECigar() const { return ecigar_string; }

// ---------------------------------------------------------------------------------------------------------------------

uint8_t Alignment::getRComp() const { return this->reverse_comp; }

// ---------------------------------------------------------------------------------------------------------------------

void Alignment::write(util::BitWriter &writer) const {
    writer.writeAlignedInt<uint32_t, 3>(static_cast<uint32_t>(ecigar_string.length()));
    writer.writeAlignedBytes(ecigar_string.data(), ecigar_string.length());
    writer.writeAlignedInt(reverse_comp);
    for (auto s : mapping_score) {
        writer.writeAlignedInt(s);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::core::record

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
