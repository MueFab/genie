#include "raw_reference.h"
#include "make_unique.h"
#include "util/bitwriter.h"

#include <sstream>
// -----------------------------------------------------------------------------------------------------------------

namespace format {
// -----------------------------------------------------------------------------------------------------------------

RawReference::RawReference() : DataUnit(DataUnitType::RAW_REFERENCE), seqs(0) {}

// -----------------------------------------------------------------------------------------------------------------

void RawReference::addSequence(std::unique_ptr<RawReferenceSequence> ref) {
    for (const auto &a : seqs) {
        if (!a->isIdUnique(ref.get())) {
            UTILS_THROW_RUNTIME_EXCEPTION("Reference ID is not unique");
        }
    }
    seqs.push_back(std::move(ref));
}

std::unique_ptr<RawReference> RawReference::clone() const {
    auto ret = make_unique<RawReference>();
    for (const auto &a : seqs) {
        ret->seqs.push_back(a->clone());
    }
    return ret;
}

void RawReference::write(util::BitWriter *writer) const {
    DataUnit::write(writer);

    uint64_t size = 0;
    for (auto &i : seqs) {
        size += i->getTotalSize();
    }
    size += (8 + 64 + 16) / 8;  // data_unit_type, data_unit_size, seq_count
    writer->write(size, 64);
    writer->write(seqs.size(), 16);

    for (auto &i : seqs) {
        i->write(writer);
    }
}
}  // namespace format