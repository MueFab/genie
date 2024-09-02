/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MGG_DATASET_GROUP_H_
#define SRC_GENIE_FORMAT_MGG_DATASET_GROUP_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <optional>
#include <string>
#include <vector>
#include "genie/core/constants.h"
#include "genie/format/mgg/dataset.h"
#include "genie/format/mgg/dataset_group_header.h"
#include "genie/format/mgg/dataset_group_metadata.h"
#include "genie/format/mgg/dataset_group_protection.h"
#include "genie/format/mgg/gen_info.h"
#include "genie/format/mgg/label_list.h"
#include "genie/format/mgg/reference.h"
#include "genie/format/mgg/reference_metadata.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::mgg {

/**
 * @brief
 */
class DatasetGroup : public GenInfo {
 private:
    std::optional<DatasetGroupHeader> header;            //!< @brief
    std::vector<Reference> references;                   //!< @brief
    std::vector<ReferenceMetadata> reference_metadatas;  //!< @brief
    std::optional<LabelList> labels;                     //!< @brief
    std::optional<DatasetGroupMetadata> metadata;        //!< @brief
    std::optional<DatasetGroupProtection> protection;    //!< @brief
    std::vector<Dataset> dataset;                        //!< @brief

    core::MPEGMinorVersion version;  //!< @brief

 public:
    /**
     * @brief
     * @param meta
     */
    void setMetadata(DatasetGroupMetadata meta);

    /**
     * @brief
     * @param prot
     */
    void setProtection(DatasetGroupProtection prot);

    /**
     * @brief
     * @param r
     */
    void addReference(Reference r);

    /**
     * @brief
     * @param r
     */
    void addReferenceMeta(ReferenceMetadata r);

    /**
     * @brief
     * @param l
     */
    void setLabels(LabelList l);

    /**
     * @brief
     * @param groupID
     */
    void patchID(uint8_t groupID);

    /**
     * @brief
     * @param _old
     * @param _new
     */
    void patchRefID(uint8_t _old, uint8_t _new);

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
    [[nodiscard]] const DatasetGroupHeader& getHeader() const;

    /**
     * @brief
     * @return
     */
    [[nodiscard]] const std::vector<Reference>& getReferences() const;

    /**
     * @brief
     * @return
     */
    std::vector<Reference>& getReferences();

    /**
     * @brief
     * @return
     */
    [[nodiscard]] const std::vector<ReferenceMetadata>& getReferenceMetadata() const;

    /**
     * @brief
     * @return
     */
    std::vector<ReferenceMetadata>& getReferenceMetadata();

    /**
     * @brief
     * @return
     */
    [[nodiscard]] bool hasLabelList() const;

    /**
     * @brief
     * @return
     */
    [[nodiscard]] const LabelList& getLabelList() const;

    /**
     * @brief
     * @return
     */
    LabelList& getLabelList();

    /**
     * @brief
     * @return
     */
    [[nodiscard]] bool hasMetadata() const;

    /**
     * @brief
     * @return
     */
    [[nodiscard]] const DatasetGroupMetadata& getMetadata() const;

    /**
     * @brief
     * @return
     */
    DatasetGroupMetadata& getMetadata();

    /**
     * @brief
     * @return
     */
    [[nodiscard]] bool hasProtection() const;

    /**
     * @brief
     * @return
     */
    [[nodiscard]] const DatasetGroupProtection& getProtection() const;

    /**
     * @brief
     * @return
     */
    DatasetGroupProtection& getProtection();

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
    [[nodiscard]] const std::vector<Dataset>& getDatasets() const;

    /**
     * @brief
     * @return
     */
    std::vector<Dataset>& getDatasets();

    /**
     * @brief
     * @return
     */
    [[nodiscard]] const std::string& getKey() const override;

    /**
     * @brief
     * @param wr
     */
    void box_write(util::BitWriter& wr) const override;

    /**
     * @brief
     * @param output
     * @param depth
     * @param max_depth
     */
    void print_debug(std::ostream& output, uint8_t depth, uint8_t max_depth) const override;

    /**
     * @brief
     * @param reader
     * @param in_offset
     */
    void read_box(util::BitReader& reader, bool in_offset);
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::mgg

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGG_DATASET_GROUP_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
