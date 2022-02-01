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
    auto start_pos = reader.getPos();
    auto length = reader.readBypassBE<uint64_t>();
    auto end_pos = start_pos + static_cast<int64_t>(length) - 4;
    header = DatasetGroupHeader(reader);
    while (reader.getPos() < end_pos) {
        reader.getPos();
        std::string tmp_str(4, '\0');
        reader.readBypass(tmp_str);
        if (tmp_str == "rfgn") {
            UTILS_DIE_IF(!reference_metadatas.empty(), "Reference must be before ref metadata");
            UTILS_DIE_IF(labels != boost::none, "Reference must be before labels");
            UTILS_DIE_IF(metadata != boost::none, "Reference must be before metadata");
            UTILS_DIE_IF(protection != boost::none, "Reference must be before protection");
            UTILS_DIE_IF(!dataset.empty(), "Reference must be before dataset");
            references.emplace_back(reader);
        } else if (tmp_str == "rfmd") {
            UTILS_DIE_IF(labels != boost::none, "Ref metadata must be before labels");
            UTILS_DIE_IF(metadata != boost::none, "Ref metadata must be before metadata");
            UTILS_DIE_IF(protection != boost::none, "Ref metadata must be before protection");
            UTILS_DIE_IF(!dataset.empty(), "Ref metadata must be before dataset");
            reference_metadatas.emplace_back(reader);
        } else if (tmp_str == "labl") {
            UTILS_DIE_IF(labels != boost::none, "Labels already present");
            UTILS_DIE_IF(metadata != boost::none, "Labels must be before metadata");
            UTILS_DIE_IF(protection != boost::none, "Labels must be before protection");
            UTILS_DIE_IF(!dataset.empty(), "Labels must be before dataset");
            labels.emplace(reader);
        } else if (tmp_str == "dgmd") {
            UTILS_DIE_IF(metadata != boost::none, "Metadata already present");
            UTILS_DIE_IF(protection != boost::none, "Metadata must be before protection");
            UTILS_DIE_IF(!dataset.empty(), "Metadata must be before dataset");
            metadata = DatasetGroupMetadata(reader);
        } else if (tmp_str == "dtpr") {
            UTILS_DIE_IF(protection != boost::none, "Protection already present");
            UTILS_DIE_IF(!dataset.empty(), "Protection must be before dataset");
            protection = DatasetGroupProtection(reader);
        } else if (tmp_str == "dtcn") {
            dataset.emplace_back(reader, version);
        } else {
            UTILS_DIE("Unknown box");
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

DatasetGroup::DatasetGroup(uint8_t _ID, uint8_t _version, core::MPEGMinorVersion _mpeg_version)
    : header(_ID, _version), version(_mpeg_version) {}

// ---------------------------------------------------------------------------------------------------------------------

void DatasetGroup::addDataset(Dataset ds) {
    dataset.emplace_back(std::move(ds));
    header.addDatasetID(dataset.back().getHeader().getDatasetID());
}

// ---------------------------------------------------------------------------------------------------------------------

const DatasetGroupHeader& DatasetGroup::getHeader() const { return header; }

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

uint64_t DatasetGroup::getSize() const {
    std::stringstream stream;
    genie::util::BitWriter writer(&stream);
    write(writer);
    return stream.str().length();
}

// ---------------------------------------------------------------------------------------------------------------------

const std::string& DatasetGroup::getKey() const {
    static const std::string key = "dgcn";
    return key;
}

// ---------------------------------------------------------------------------------------------------------------------

void DatasetGroup::write(util::BitWriter& wr) const {
    GenInfo::write(wr);
    header.write(wr);
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
