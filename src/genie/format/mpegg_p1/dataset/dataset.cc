/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/format/mpegg_p1/dataset/dataset.h"
#include <string>
#include <utility>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

// ---------------------------------------------------------------------------------------------------------------------

DTMetadata::DTMetadata(): DT_metadata_value() {}

// ---------------------------------------------------------------------------------------------------------------------

DTMetadata::DTMetadata(std::vector<uint8_t>&& _DT_metadata_value) : DT_metadata_value(std::move(_DT_metadata_value)) {}

// ---------------------------------------------------------------------------------------------------------------------

DTMetadata::DTMetadata(util::BitReader& reader, FileHeader& fhd, size_t start_pos, size_t length): DT_metadata_value() {
    UTILS_DIE_IF(!reader.isAligned() || reader.getPos() - start_pos != length,
                 "Invalid dataset metadata length!");
}

// ---------------------------------------------------------------------------------------------------------------------

uint64_t DTMetadata::getLength() const {
    return 0;
//    /// Key c(4) Length u(64)
//    uint64_t len = (4 * sizeof(char) + 8);  // gen_info
//
//    // DT_metadata_value[] std::vector<uint8_t>
//    len += DT_metadata_value.size();
//
//    return len;
}

// ---------------------------------------------------------------------------------------------------------------------

void DTMetadata::write(genie::util::BitWriter& bit_writer) const {
    /// KLV (Key Length Value) format
    // Key of KLV format
    bit_writer.write("dtmd");

    // Length of KLV format
    bit_writer.write(getLength(), 64);

    // DT_metadata_value[] std::vector<uint8_t>
    for (auto val : DT_metadata_value) {
        bit_writer.write(val, 8);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

DTProtection::DTProtection() : DT_protection_value() {}

// ---------------------------------------------------------------------------------------------------------------------

DTProtection::DTProtection(std::vector<uint8_t>&& _dt_protection_value)
    : DT_protection_value(std::move(_dt_protection_value)) {}

// ---------------------------------------------------------------------------------------------------------------------

DTProtection::DTProtection(util::BitReader& reader, FileHeader& fhd, size_t start_pos, size_t length) :
    DT_protection_value() {

    UTILS_DIE_IF(!reader.isAligned() || reader.getPos() - start_pos != length,
                 "Invalid dataset protection length!");
}

// ---------------------------------------------------------------------------------------------------------------------

uint64_t DTProtection::getLength() const {
    /// Key c(4) Length u(64)
    uint64_t len = (4 * sizeof(char) + 8);  // gen_info

    // DT_protection_value[] std::vector<uint8_t>
    len += DT_protection_value.size();

    return len;
}

// ---------------------------------------------------------------------------------------------------------------------

void DTProtection::write(genie::util::BitWriter& bit_writer) const {
    /// // KLV (Key Length Value) format
    // Key of KVL format
    bit_writer.write("dtpr");

    // Length of KVL format
    bit_writer.write(getLength(), 64);

    // DT_protection_value[] std::vector<uint8_t>
    for (auto val : DT_protection_value) {
        bit_writer.write(val, 8);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

Dataset::Dataset():
    header(),
    metadata(),
    protection(),
    parameter_sets()
{};

// ---------------------------------------------------------------------------------------------------------------------

Dataset::Dataset(util::BitReader& reader, FileHeader& fhd, size_t start_pos, size_t length) {
    /// dataset_header
    size_t box_start_pos = reader.getPos();
    std::string box_key = readKey(reader);
    auto box_length = reader.read<uint64_t>();
    UTILS_DIE_IF(box_key != "dthd",
                 "Dataset group header is not found!");
    header = DatasetHeader(reader, fhd, box_start_pos, box_length);

    do {
        /// Read K,L of KLV
        box_start_pos = reader.getPos();
        box_key = readKey(reader);
        box_length = reader.read<uint64_t>();

        /// Dataset Metadata
        if (box_key == "dtmd") {
            metadata = util::make_unique<DTMetadata>(reader, fhd, box_start_pos, box_length);

        /// Dataset Protection
        } else if (box_key == "dtpr") {
            protection = util::make_unique<DTProtection>(reader, fhd, box_start_pos, box_length);

        /// Dataset Parameter Sets
        } else if (box_key == "pars") {
            parameter_sets.emplace_back(reader, fhd, box_start_pos, box_length);
        } else if (box_key == "mitb") {

        } else if (box_key == "aucn") {

        } else if (box_key == "dscn") {

        }
    } while (reader.getPos() - start_pos < length);

    UTILS_DIE_IF(!reader.isAligned() || reader.getPos() - start_pos != length,
                 "Invalid Dataset length!");
}

// ---------------------------------------------------------------------------------------------------------------------

DatasetHeader& Dataset::getHeader() { return header; }

// ---------------------------------------------------------------------------------------------------------------------

std::vector<DatasetParameterSet>& Dataset::getParameterSets() { return parameter_sets; }

// ---------------------------------------------------------------------------------------------------------------------

std::vector<AccessUnit>& Dataset::getAccessUnits() { return access_units; }

// ---------------------------------------------------------------------------------------------------------------------

std::vector<DescriptorStream>& Dataset::getDescriptorStreams() { return descriptor_streams; }

// ---------------------------------------------------------------------------------------------------------------------

uint64_t Dataset::getLength() const {
    /// Key c(4) Length u(64)
    uint64_t len = (4 * sizeof(char) + 8);  // gen_info

    /// dataset_header
    len += header.getLength();

    /// metadata
    if (metadata != nullptr) {
        len += metadata->getLength();
    }

    /// protection
    if (protection != nullptr) {
        len += protection->getLength();
    }

    /// dataset_parameter_set[]
    for (auto const& ps : parameter_sets) {
        len += ps.getLength();
    }

    // TODO(Raouf): Master Index Table
    // write master_index_table depending on MIT_FLAG
    // if (getBlockHeader().getMITFlag()){
    //    if (master_index_table != nullptr) {
    //        master_index_table->getLength();
    //    }
    // }

    /// access_units[]
    for (auto const& ac : access_units) {
        len += ac.getLength();
    }

    /// descriptor_streams[]
    if (!header.getBlockHeaderFlag()) {
        for (auto& ds : descriptor_streams) {
            len += ds.getLength();
        }
    }

    return len;
}

// ---------------------------------------------------------------------------------------------------------------------

void Dataset::write(util::BitWriter& writer) const {
    // KLV (Key Length Value) format

    // Key of KLV format
    writer.write("dtcn");

    // Length of KLV format
    writer.write(getLength(), 64);

    /// dataset_header
    header.write(writer);

    /// metadata
    if (metadata != nullptr) {
        metadata->write(writer);
    }

    /// protection
    if (protection != nullptr) {
        protection->write(writer);
    }

    /// dataset_parameter_set[]
    for (auto const& ps : parameter_sets) {
        ps.write(writer);
    }

    // TODO(Raouf): Master Index Table

    /// access_units[]
    for (auto const& ac : access_units) {
        ac.write(writer);
    }

    /// descriptor_streams[]
    if (!header.getBlockHeaderFlag()) {
        for (auto& ds : descriptor_streams) {
            ds.write(writer);
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
