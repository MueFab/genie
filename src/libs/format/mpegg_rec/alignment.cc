/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "alignment.h"
#include <util/bitreader.h>
#include <util/bitwriter.h>

// ---------------------------------------------------------------------------------------------------------------------

namespace format {
namespace mpegg_rec {

// ---------------------------------------------------------------------------------------------------------------------

Alignment::Alignment(std::string &&_ecigar_string, uint8_t _reverse_comp)
    : ecigar_string(std::move(_ecigar_string)), reverse_comp(_reverse_comp), mapping_score() {}

// ---------------------------------------------------------------------------------------------------------------------

Alignment::Alignment(uint8_t as_depth, util::BitReader &reader) {
    ecigar_string.resize(reader.read(24));
    reader.read(ecigar_string);

    reverse_comp = reader.read<uint8_t>();
    for (size_t a = 0; a < as_depth; ++a) {
        mapping_score.push_back(reader.read<int32_t>());
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
    writer.write(ecigar_string.length(), 24);
    writer.write(ecigar_string);
    writer.write(reverse_comp, 8);
    for (const auto &q : mapping_score) {
        writer.write(uint32_t(q), 32);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_rec
}  // namespace format

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------