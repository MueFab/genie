/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/format/mgb/raw_reference.h"
#include <sstream>
#include <utility>
#include "genie/util/bitwriter.h"

#include "genie/util/runtime-exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::mgb {

// ---------------------------------------------------------------------------------------------------------------------

RawReference::RawReference(bool headerLess) : DataUnit(DataUnitType::RAW_REFERENCE), seqs(), headerless(headerLess) {}

// ---------------------------------------------------------------------------------------------------------------------

RawReference::RawReference(util::BitReader &reader, bool headerOnly, bool headerLess)
    : DataUnit(DataUnitType::RAW_REFERENCE), seqs(), headerless(headerLess) {
    if (!headerLess) {
        reader.read<uint64_t>();
    }
    auto count = reader.read<uint16_t>();
    for (size_t i = 0; i < count; ++i) {
        seqs.emplace_back(reader, headerOnly);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void RawReference::addSequence(RawReferenceSequence &&ref) {
    for (const auto &a : seqs) {
        if (!a.isIdUnique(ref)) {
            UTILS_THROW_RUNTIME_EXCEPTION("Reference ID is not unique");
        }
    }
    seqs.push_back(std::move(ref));
}

// ---------------------------------------------------------------------------------------------------------------------

void RawReference::write(util::BitWriter &writer) const {
    if (!headerless) {
        DataUnit::write(writer);
        uint64_t size = 0;
        for (auto &i : seqs) {
            size += i.getTotalSize();
        }
        size += (8 + 64 + 16) / 8;  // data_unit_type, data_unit_size, seq_count
        writer.writeBits(size, 64);
    }
    writer.writeBits(seqs.size(), 16);

    for (auto &i : seqs) {
        i.write(writer);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

bool RawReference::isEmpty() const { return seqs.empty(); }

// ---------------------------------------------------------------------------------------------------------------------

RawReferenceSequence &RawReference::getSequence(size_t index) { return seqs[index]; }

// ---------------------------------------------------------------------------------------------------------------------

std::vector<RawReferenceSequence>::iterator RawReference::begin() { return seqs.begin(); }

// ---------------------------------------------------------------------------------------------------------------------

std::vector<RawReferenceSequence>::iterator RawReference::end() { return seqs.end(); }

// ---------------------------------------------------------------------------------------------------------------------

void RawReference::setHeaderLess(bool state) { headerless = state; }

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::mgb

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
