/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/format/mpegg_p1/dataset_group.h"
#include <sstream>
#include <utility>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

// ---------------------------------------------------------------------------------------------------------------------

DGMetadata::DGMetadata() : DG_metadata_value() {
    DG_metadata_value = {0x37, 0xfd, 0x58, 0x7a, 0x00, 0x5a, 0x04, 0x00, 0xd6, 0xe6, 0x46,
                         0xb4, 0x00, 0x00, 0x00, 0x00, 0xdf, 0x1c, 0x21, 0x44, 0xb6, 0x1f,
                         0x7d, 0xf3, 0x00, 0x01, 0x00, 0x00, 0x04, 0x00, 0x5a, 0x59};
}
// ---------------------------------------------------------------------------------------------------------------------

DGMetadata::DGMetadata(util::BitReader& reader, size_t length) {
    std::string key = readKey(reader);
    UTILS_DIE_IF(key != "dgmd", "DGMetadata is not Found");

    size_t start_pos = reader.getPos();

    // DG_metadata_value[uint8_t]
    for (auto& val : DG_metadata_value) {
        val = reader.read<uint8_t>();
    }

    UTILS_DIE_IF(reader.getPos() - start_pos != length, "Invalid DGMetadata length!");
}

// ---------------------------------------------------------------------------------------------------------------------

uint64_t DGMetadata::getLength() const { return DG_metadata_value.size(); }

// ---------------------------------------------------------------------------------------------------------------------

void DGMetadata::write(util::BitWriter& bit_writer) const {
    for (auto val : DG_metadata_value) {
        bit_writer.write(val, 8);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

DGProtection::DGProtection() : DG_protection_value() {
    DG_protection_value = {0x37, 0xfd, 0x58, 0x7a, 0x00, 0x5a, 0x04, 0x00, 0xd6, 0xe6, 0x46,
                           0xb4, 0x00, 0x00, 0x00, 0x00, 0xdf, 0x1c, 0x21, 0x44, 0xb6, 0x1f,
                           0x7d, 0xf3, 0x00, 0x01, 0x00, 0x00, 0x04, 0x00, 0x5a, 0x59};
}

// ---------------------------------------------------------------------------------------------------------------------

DGProtection::DGProtection(util::BitReader& reader, size_t length) {
    std::string key = readKey(reader);
    UTILS_DIE_IF(key != "dgpr", "DGProtection is not Found");

    size_t start_pos = reader.getPos();

    // DG_protection_value[uint8_t]
    for (auto& val : DG_protection_value) {
        val = reader.read<uint8_t>();
    }

    UTILS_DIE_IF(reader.getPos() - start_pos != length, "Invalid DGProtection length!");
}

// ---------------------------------------------------------------------------------------------------------------------

uint64_t DGProtection::getLength() const { return DG_protection_value.size(); }

// ---------------------------------------------------------------------------------------------------------------------

void DGProtection::write(util::BitWriter& bit_writer) const {
    for (auto val : DG_protection_value) {
        bit_writer.write(val, 8);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

// DatasetGroup::DatasetGroup(std::vector<Dataset>&& _datasets):
//     ID(0),
//     version(0),  // FIXME: Fix version number
//     dataset_IDs(),
//     datasets(std::move(_datasets)) {
//
//     std::vector<uint16_t> dataset_IDs(getDatasetIDs(true));
//
//     auto it = std::unique(dataset_IDs.begin(), dataset_IDs.end());
//
//     UTILS_DIE_IF(!(it == dataset_IDs.end()), "ID is not unique!");
// }

// ---------------------------------------------------------------------------------------------------------------------

DatasetGroup::DatasetGroup(util::BitReader& reader, FileHeader& fhd, size_t start_pos, size_t length)
    : references(), reference_metadata(), label_list(), DG_metadata(nullptr), DG_protection(nullptr), datasets() {
    /// dataset_group_header
    size_t box_start_pos = reader.getPos();
    std::string box_key = readKey(reader);
    auto box_length = reader.read<uint64_t>();
    UTILS_DIE_IF(box_key != "dghd", "Dataset group header is not found!");
    header = DatasetGroupHeader(reader, fhd, box_start_pos, box_length);

    auto curr_len = reader.getPos() - start_pos;
    do {
        /// Read K,L of KLV
        box_start_pos = reader.getPos();
        box_key = readKey(reader);
        box_length = reader.read<uint64_t>();

        /// reference[]
        if (box_key == "rfgn") {
            references.emplace_back(reader, fhd, box_start_pos, box_length);

            /// reference_metadata[]
        } else if (box_key == "rfmd") {
            reference_metadata.emplace_back(reader, fhd, box_start_pos, box_length);

            /// label_list
        } else if (box_key == "labl") {
            label_list = util::make_unique<LabelList>(reader, box_length);

            /// DG_metadata
        } else if (box_key == "dgmd") {
            DG_metadata = util::make_unique<DGMetadata>(reader, box_length);

            /// DG_protection
        } else if (box_key == "dgpr") {
            DG_protection = util::make_unique<DGProtection>(reader, box_length);

            /// Dataset
        } else if (box_key == "dtcn") {
            datasets.emplace_back(reader, fhd, box_start_pos, box_length);
        }
        curr_len = reader.getPos() - start_pos;
    } while (curr_len < length);

#if ROUNDTRIP_CONSTRUCTOR
    std::stringstream ss;
    util::BitWriter tmp_writer(&ss);
    write(tmp_writer);
    tmp_writer.flush();
    uint64_t wlen = tmp_writer.getBitsWritten() / 8;
    uint64_t elen = getLength();
    UTILS_DIE_IF(wlen != length, "Invalid DatasetGroup write()");
    UTILS_DIE_IF(elen != length, "Invalid DatasetGroup getLength()");
#endif

    UTILS_DIE_IF(!reader.isAligned() || (reader.getPos() - start_pos != length), "Invalid DatasetGroup length!");
}

// ---------------------------------------------------------------------------------------------------------------------

void DatasetGroup::addReferences(std::vector<Reference>&& _references) { references = std::move(_references); }

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<Reference>& DatasetGroup::getReferences() const { return references; }

// ---------------------------------------------------------------------------------------------------------------------

void DatasetGroup::addReferenceMetadata(std::vector<ReferenceMetadata>&& _ref_metadata) {
    reference_metadata = std::move(_ref_metadata);
}

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<ReferenceMetadata>& DatasetGroup::getReferenceMetadata() const { return reference_metadata; }

// ---------------------------------------------------------------------------------------------------------------------

void DatasetGroup::addLabels(std::vector<Label>&& _labels) {
    UTILS_DIE_IF(label_list != nullptr, "Label list is already added");

    label_list = util::make_unique<LabelList>(header.getID(), std::move(_labels));
}

// ---------------------------------------------------------------------------------------------------------------------

void DatasetGroup::addLabelList(std::unique_ptr<LabelList> _label_list) {
    UTILS_DIE_IF(label_list != nullptr, "Label list is already added");

    _label_list = std::move(label_list);
}

// ---------------------------------------------------------------------------------------------------------------------

const LabelList& DatasetGroup::getLabelList() const { return *label_list; }

// ---------------------------------------------------------------------------------------------------------------------

void DatasetGroup::addDGMetadata(std::unique_ptr<DGMetadata> _dg_metadata) { DG_metadata = std::move(_dg_metadata); }

// ---------------------------------------------------------------------------------------------------------------------

const DGMetadata& DatasetGroup::getDgMetadata() const { return *DG_metadata; }

// ---------------------------------------------------------------------------------------------------------------------

void DatasetGroup::addDGProtection(std::unique_ptr<DGProtection> _dg_protection) {
    DG_protection = std::move(_dg_protection);
}

// ---------------------------------------------------------------------------------------------------------------------

const DGProtection& DatasetGroup::getDgProtection() const { return *DG_protection; }

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<Dataset>& DatasetGroup::getDatasets() const { return datasets; }

// ---------------------------------------------------------------------------------------------------------------------

void DatasetGroup::setID(uint8_t _ID) {
    header.setID(_ID);

    for (auto& ref : references) {
        ref.setDatasetGroupId(header.getID());
    }

    for (auto& ref_meta : reference_metadata) {
        ref_meta.setDatasetGroupId(header.getID());
    }

    if (label_list != nullptr) {
        label_list->setDatasetGroupId(header.getID());
    }

    /// DG_metadata has no ID
    /// DG_protection has no ID

    for (auto& ds : datasets) {
        ds.setGroupID(header.getID());
    }
}

// ---------------------------------------------------------------------------------------------------------------------

uint64_t DatasetGroup::getLength() const {
    /// Key c(4) Length u(64)
    uint64_t len = (4 * sizeof(char) + 8);  // gen_info

    /// DatasetGroupHeader
    len += header.getLength();

    /// DatasetGroupHeader
    for (auto& ref : references) {
        len += ref.getLength();
    }

    for (auto& ref_meta : reference_metadata) {
        len += ref_meta.getLength();
    }

    if (label_list != nullptr) {
        len += label_list->getLength();
    }

    if (DG_metadata != nullptr) {
        len += DG_metadata->getLength();
    }

    if (DG_metadata != nullptr) {
        len += DG_metadata->getLength();
    }

    for (auto& ds : datasets) {
        len += ds.getLength();
    }

    return len;
}

// ---------------------------------------------------------------------------------------------------------------------

void DatasetGroup::write(util::BitWriter& writer) const {
    /// KLV (Key Length Value) format
    writer.write("dgcn");

    /// Length of KLV format
    writer.write(getLength(), 64);

    /// dataset_group_header
    header.write(writer);

    /// reference (optional)
    for (auto& reference : references) {
        reference.write(writer);
    }

    /// reference_metadata (optional)
    for (auto& ref_metadata : reference_metadata) {
        ref_metadata.write(writer);
    }

    /// label_list (optional)
    if (label_list != nullptr) {
        label_list->writeToFile(writer);
    }

    /// DG_metadata (optional)
    if (DG_metadata != nullptr) {
        DG_metadata->write(writer);
    }

    /// DG_protection (optional)
    if (DG_protection != nullptr) {
        DG_protection->write(writer);
    }

    /// dataset[]
    for (auto& ds : datasets) {
        ds.write(writer);
    }

    writer.flush();
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
