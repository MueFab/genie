/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MPEGG_P1_DATASET_GROUP_H_
#define SRC_GENIE_FORMAT_MPEGG_P1_DATASET_GROUP_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <string>
#include <vector>
#include "boost/optional/optional.hpp"
#include "genie/core/constants.h"
#include "genie/format/mpegg_p1/dataset.h"
#include "genie/format/mpegg_p1/dataset_group_header.h"
#include "genie/format/mpegg_p1/dataset_group_metadata.h"
#include "genie/format/mpegg_p1/dataset_group_protection.h"
#include "genie/format/mpegg_p1/gen_info.h"
#include "genie/format/mpegg_p1/label_list.h"
#include "genie/format/mpegg_p1/reference.h"
#include "genie/format/mpegg_p1/reference_metadata.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

/**
 * @brief
 */
class DatasetGroup : public GenInfo {
 private:
    boost::optional<DatasetGroupHeader> header;          //!< @brief
    std::vector<Reference> references;                   //!< @brief
    std::vector<ReferenceMetadata> reference_metadatas;  //!< @brief
    boost::optional<LabelList> labels;                   //!< @brief
    boost::optional<DatasetGroupMetadata> metadata;      //!< @brief
    boost::optional<DatasetGroupProtection> protection;  //!< @brief
    std::vector<Dataset> dataset;                        //!< @brief

    core::MPEGMinorVersion version;  //!< @brief

 public:
    /**
     * @brief
     * @param info
     * @return
     */
    bool operator==(const GenInfo& info) const override;

    /**
     * @brief
     * @param reader
     * @param _version
     */
    explicit DatasetGroup(util::BitReader& reader, core::MPEGMinorVersion _version);

    /**
     * @brief
     * @param _ID
     * @param _version
     * @param _mpeg_version
     */
    DatasetGroup(uint8_t _ID, uint8_t _version, core::MPEGMinorVersion _mpeg_version);

    /**
     * @brief
     * @param ds
     */
    void addDataset(Dataset ds);

    /**
     * @brief
     * @return
     */
    const DatasetGroupHeader& getHeader() const;

    /**
     * @brief
     * @return
     */
    const std::vector<Reference>& getReferences() const;

    /**
     * @brief
     * @return
     */
    const std::vector<ReferenceMetadata>& getReferenceMetadata() const;

    /**
     * @brief
     * @return
     */
    bool hasLabelList() const;

    /**
     * @brief
     * @return
     */
    const LabelList& getLabelList() const;

    /**
     * @brief
     * @return
     */
    bool hasMetadata() const;

    /**
     * @brief
     * @return
     */
    const DatasetGroupMetadata& getMetadata() const;

    /**
     * @brief
     * @return
     */
    bool hasProtection() const;

    /**
     * @brief
     * @return
     */
    const DatasetGroupProtection& getProtection() const;

    /**
     * @brief
     * @param md
     */
    void addMetadata(DatasetGroupMetadata md);

    /**
     * @brief
     * @param pr
     */
    void addProtection(DatasetGroupProtection pr);

    /**
     * @brief
     * @return
     */
    const std::vector<Dataset>& getDatasets() const;


    /**
     * @brief
     * @return
     */
    const std::string& getKey() const override;

    /**
     * @brief
     * @param wr
     */
    void box_write(util::BitWriter& wr) const override;

    void print_debug(std::ostream& output, uint8_t depth, uint8_t max_depth) const override {
        print_offset(output, depth, max_depth, "* Dataset Group");
        header->print_debug(output, depth + 1, max_depth);
        for(const auto& r : references) {
            r.print_debug(output, depth + 1, max_depth);
        }
        for(const auto& r : reference_metadatas) {
            r.print_debug(output, depth + 1, max_depth);
        }
        if (labels) {
            labels->print_debug(output, depth + 1, max_depth);
        }
        if (metadata) {
            metadata->print_debug(output, depth + 1, max_depth);
        }
        if (protection) {
            metadata->print_debug(output, depth + 1, max_depth);
        }
        for(const auto& r : dataset) {
            r.print_debug(output, depth + 1, max_depth);
        }
    }

    void read_box(util::BitReader& reader, bool in_offset) {
        std::string tmp_str(4, '\0');
        reader.readBypass(tmp_str);
        std::cout << tmp_str << std::endl;
        if (tmp_str == "dghd") {
            UTILS_DIE_IF(header != boost::none, "More than one header");
            UTILS_DIE_IF(!reference_metadatas.empty(), "Header must be before ref metadata");
            UTILS_DIE_IF(labels != boost::none, "Header must be before labels");
            UTILS_DIE_IF(metadata != boost::none, "Header must be before metadata");
            UTILS_DIE_IF(protection != boost::none, "Header must be before protection");
            UTILS_DIE_IF(!dataset.empty(), "Header must be before dataset");
            header = DatasetGroupHeader(reader);
        }else if (tmp_str == "rfgn") {
            UTILS_DIE_IF(!reference_metadatas.empty(), "Reference must be before ref metadata");
            UTILS_DIE_IF(labels != boost::none, "Reference must be before labels");
            UTILS_DIE_IF(metadata != boost::none, "Reference must be before metadata");
            UTILS_DIE_IF(protection != boost::none, "Reference must be before protection");
            UTILS_DIE_IF(!dataset.empty(), "Reference must be before dataset");
            references.emplace_back(reader, version);
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
            metadata = DatasetGroupMetadata(reader, version);
        } else if (tmp_str == "dgpr") {
            UTILS_DIE_IF(protection != boost::none, "Protection already present");
            UTILS_DIE_IF(!dataset.empty(), "Protection must be before dataset");
            protection = DatasetGroupProtection(reader, version);
        } else if (tmp_str == "dtcn") {
            dataset.emplace_back(reader, version);
        } else if (tmp_str == "offs") {
            UTILS_DIE_IF(in_offset, "Recursive offset not permitted");
            reader.readBypass(tmp_str);
            uint64_t offset = reader.readBypassBE<uint64_t>();
            if (offset == ~static_cast<uint64_t>(0)) {
                return;
            }
            auto pos_save = reader.getPos();
            reader.setPos(offset);
            read_box(reader, true);
            reader.setPos(pos_save);
        } else {
            UTILS_DIE("Unknown box");
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
