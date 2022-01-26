/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/format/mpegg_p1/dataset_group_header.h"
#include <sstream>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

// ---------------------------------------------------------------------------------------------------------------------

DatasetGroupHeader::DatasetGroupHeader() : ID(0), version(0), dataset_IDs() {}

// ---------------------------------------------------------------------------------------------------------------------

DatasetGroupHeader::DatasetGroupHeader(genie::util::BitReader& reader, FileHeader&, size_t start_pos, size_t length)
    : ID(reader.read<uint8_t>()), version(reader.read<uint8_t>()), dataset_IDs() {
    auto num_datasets = (length - 14) / 2;
    for (size_t i = 0; i < num_datasets; i++) {
        dataset_IDs.push_back(reader.read<uint16_t>());
    }

    uint64_t tlen = reader.getPos() - start_pos;

#if ROUNDTRIP_CONSTRUCTOR
    std::stringstream ss;
    util::BitWriter tmp_writer(&ss);
    write(tmp_writer);
    tmp_writer.flush();
    uint64_t wlen = tmp_writer.getBitsWritten() / 8;
    uint64_t elen = getLength();
    UTILS_DIE_IF(wlen != length, "Invalid DatasetGroupHeader write()");
    UTILS_DIE_IF(elen != length, "Invalid DatasetGroupHeader getLength()");
#endif

    UTILS_DIE_IF(!reader.isAligned() || tlen != length, "Invalid DatasetGroupHeader length!");
}

// ---------------------------------------------------------------------------------------------------------------------

uint8_t DatasetGroupHeader::getID() const { return ID; }

// ---------------------------------------------------------------------------------------------------------------------

void DatasetGroupHeader::setID(uint8_t _ID) { ID = _ID; }

// ---------------------------------------------------------------------------------------------------------------------

uint8_t DatasetGroupHeader::getVersion() const { return version; }

// ---------------------------------------------------------------------------------------------------------------------

std::vector<uint16_t>& DatasetGroupHeader::getDatasetIDs() { return dataset_IDs; }

// ---------------------------------------------------------------------------------------------------------------------

uint64_t DatasetGroupHeader::getLength() const {
    std::stringstream ss;
    util::BitWriter tmp_writer(&ss);
    write(tmp_writer, true);
    tmp_writer.flush();
    uint64_t len = tmp_writer.getBitsWritten() / 8;

    return len;
}

// ---------------------------------------------------------------------------------------------------------------------

void DatasetGroupHeader::write(genie::util::BitWriter& writer, bool empty_length) const {
    /// key c(4),
    writer.write("rfgn");

    /// Length u(64)
    if (empty_length) {
        writer.write(0, 64);
    } else {
        writer.write(getLength(), 64);
    }

    /// dataset_group_ID u(8)
    writer.write(ID, 8);

    /// version u(8)
    writer.write(version, 8);

    /// dataset_IDs[] u(16)
    for (auto& dataset_ID : dataset_IDs) {
        writer.write(dataset_ID, 16);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
