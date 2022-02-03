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

bool DatasetGroup::operator==(const GenInfo& info) const {
    if (!GenInfo::operator==(info)) {
        return false;
    }
    const auto& other = dynamic_cast<const DatasetGroup&>(info);
    return header == other.header && references == other.references &&
           reference_metadatas == other.reference_metadatas && labels == other.labels && metadata == other.metadata &&
           protection == other.protection && dataset == other.dataset;
}

// ---------------------------------------------------------------------------------------------------------------------

DatasetGroup::DatasetGroup(util::BitReader& reader, core::MPEGMinorVersion _version) : version(_version) {
    auto start_pos = reader.getPos() - 4;
    auto length = reader.readBypassBE<uint64_t>();
    auto end_pos = start_pos + static_cast<int64_t>(length);
    while (reader.getPos() != end_pos) {
        UTILS_DIE_IF(reader.getPos() > end_pos, "Read " + std::to_string(reader.getPos() - end_pos) + " bytes too far");
        read_box(reader, false);
    }

    std::cout << std::to_string(end_pos - reader.getPos()) << std::endl;
    UTILS_DIE_IF(header == boost::none, "Datasetgroup without header");
}

// ---------------------------------------------------------------------------------------------------------------------

DatasetGroup::DatasetGroup(uint8_t _ID, uint8_t _version, core::MPEGMinorVersion _mpeg_version)
    : header(DatasetGroupHeader(_ID, _version)), version(_mpeg_version) {}

// ---------------------------------------------------------------------------------------------------------------------

void DatasetGroup::addDataset(Dataset ds) {
    dataset.emplace_back(std::move(ds));
    header->addDatasetID(dataset.back().getHeader().getDatasetID());
}

// ---------------------------------------------------------------------------------------------------------------------

const DatasetGroupHeader& DatasetGroup::getHeader() const { return *header; }

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<Reference>& DatasetGroup::getReferences() const { return references; }

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<ReferenceMetadata>& DatasetGroup::getReferenceMetadata() const { return reference_metadatas; }

// ---------------------------------------------------------------------------------------------------------------------

bool DatasetGroup::hasLabelList() const { return labels != boost::none; }

// ---------------------------------------------------------------------------------------------------------------------

const LabelList& DatasetGroup::getLabelList() const { return *labels; }

// ---------------------------------------------------------------------------------------------------------------------

bool DatasetGroup::hasMetadata() const { return metadata != boost::none; }

// ---------------------------------------------------------------------------------------------------------------------

const DatasetGroupMetadata& DatasetGroup::getMetadata() const { return *metadata; }

// ---------------------------------------------------------------------------------------------------------------------

bool DatasetGroup::hasProtection() const { return protection != boost::none; }

// ---------------------------------------------------------------------------------------------------------------------

const DatasetGroupProtection& DatasetGroup::getProtection() const { return *protection; }

// ---------------------------------------------------------------------------------------------------------------------

void DatasetGroup::addMetadata(DatasetGroupMetadata md) { metadata = std::move(md); }

// ---------------------------------------------------------------------------------------------------------------------

void DatasetGroup::addProtection(DatasetGroupProtection pr) { protection = std::move(pr); }

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<Dataset>& DatasetGroup::getDatasets() const { return dataset; }

// ---------------------------------------------------------------------------------------------------------------------

const std::string& DatasetGroup::getKey() const {
    static const std::string key = "dgcn";
    return key;
}

// ---------------------------------------------------------------------------------------------------------------------

void DatasetGroup::box_write(util::BitWriter& wr) const {
    header->write(wr);
    for (const auto& r : references) {
        r.write(wr);
    }
    for (const auto& r : reference_metadatas) {
        r.write(wr);
    }
    if (labels != boost::none) {
        labels->write(wr);
    }
    if (metadata != boost::none) {
        metadata->write(wr);
    }
    if (protection != boost::none) {
        protection->write(wr);
    }
    for (const auto& d : dataset) {
        d.write(wr);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
