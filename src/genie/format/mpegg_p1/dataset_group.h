/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MPEGG_P1_DATASET_GROUP_H_
#define SRC_GENIE_FORMAT_MPEGG_P1_DATASET_GROUP_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <memory>
#include <string>
#include <vector>
#include "genie/format/mpegg_p1/dataset/class_description.h"
#include "genie/format/mpegg_p1/file_header.h"
#include "genie/format/mpegg_p1/util.h"
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"
#include "genie/util/exception.h"
#include "genie/util/make-unique.h"
#include "genie/util/runtime-exception.h"

#include "boost/optional/optional.hpp"
#include "dataset.h"
#include "dataset/dataset.h"
#include "dataset_group_metadata.h"
#include "dataset_group_protection.h"
#include "genie/format/mpegg_p1/dataset_group_header.h"
#include "label_list.h"
#include "reference.h"
#include "reference_metadata.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

/**
 * @brief
 */
class DatasetGroup : public GenInfo {
 private:
    DatasetGroupHeader header;
    std::vector<Reference> references;
    std::vector<ReferenceMetadata> reference_metadatas;
    boost::optional<LabelList> labels;
    boost::optional<DatasetGroupMetadata> metadata;
    boost::optional<DatasetGroupProtection> protection;
    std::vector<Dataset> dataset;

 public:

    DatasetGroup(util::BitReader& reader) {
        auto start_pos = reader.getPos();
        auto length = reader.readBypassBE<uint64_t>();
        auto end_pos = start_pos + static_cast<int64_t>(length) - 4;
        header = DatasetGroupHeader(reader);
        while(reader.getPos() < end_pos) {
            auto tmp_pos = reader.getPos();
            std::string tmp_str(4, '\0');
            reader.readBypass(tmp_str);
            if(tmp_str == "rfgn") {
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
                dataset.emplace_back(reader);
            } else {
                UTILS_DIE("Unknown box");
            }
        }
    }

    DatasetGroup(uint8_t _ID, uint8_t _version) : header(_ID, _version) {}

    void addDataset(Dataset ds) {
        dataset.emplace_back(std::move(ds));
        header.addDatasetID(dataset.back().getHeader().getDatasetID());
    }

    const DatasetGroupHeader& getHeader() const { return header; }

    const std::vector<Reference>& getReferences() const { return references; }

    const std::vector<ReferenceMetadata>& getReferenceMetadata() const { return reference_metadatas; }

    bool hasLabelList() const { return labels != boost::none; }

    const LabelList& getLabelList() const { return *labels; }

    bool hasMetadata() const { return metadata != boost::none; }

    const DatasetGroupMetadata& getMetadata() const { return *metadata; }

    bool hasProtection() const { return protection != boost::none; }

    const DatasetGroupProtection& getProtection() const { return *protection; }

    void addMetadata(DatasetGroupMetadata md) { metadata = std::move(md); }

    void addProtection(DatasetGroupProtection pr) { protection = std::move(pr); }

    const std::vector<Dataset>& getDatasets() const { return dataset; }

    uint64_t getSize() const override {
        std::stringstream stream;
        genie::util::BitWriter writer(&stream);
        write(writer);
        return stream.str().length();
    }

    const std::string& getKey() const override {
        static const std::string key = "dgcn";
        return key;
    }

    void write (util::BitWriter& wr) const override {
        GenInfo::write(wr);
        header.write(wr);
        for(const auto& r : references) {
            r.write(wr);
        }
        for(const auto& r : reference_metadatas) {
            r.write(wr);
        }
        if(labels != boost::none) {
            labels->write(wr);
        }
        if(metadata != boost::none) {
            metadata->write(wr);
        }
        if(protection != boost::none) {
            protection->write(wr);
        }
        for(const auto& d : dataset) {
            d.write(wr);
        }
    }
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MPEGG_P1_DATASET_GROUP_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
