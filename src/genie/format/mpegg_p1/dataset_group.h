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
#include <memory>
#include "genie/util/make-unique.h"
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"
#include "genie/util/exception.h"
#include "genie/util/runtime-exception.h"
#include "genie/format/mpegg_p1/file_header.h"
#include "genie/format/mpegg_p1/util.h"
#include "genie/format/mpegg_p1/dataset/class_description.h"

#include "genie/format/mpegg_p1/dataset_group_header.h"
#include "reference/reference.h"
#include "reference_metadata/reference_metadata.h"
#include "label_list/label_list.h"
#include "dataset/dataset.h"
//#include <algorithm>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

/**
 * @brief
 */
class DGMetadata {
 public:
    /**
     * @brief
     */
    DGMetadata();

    /**
     * @brief
     * @param reader
     * @param length
     */
    DGMetadata(util::BitReader& reader, size_t length);

    /**
     * @brief Get length of DG metadata in bytes
     * @return
     */
    uint64_t getLength() const;

    /**
     * @brief
     * @param bit_writer
     */
    void write(util::BitWriter& bit_writer) const;

 private:
    std::vector<uint8_t> DG_metadata_value;  //!< @brief
};

/**
 * @brief
 */
class DGProtection {
 public:
    /**
     * @brief
     */
    DGProtection();

    /**
     * @brief
     * @param reader
     * @param length
     */
    DGProtection(util::BitReader& reader, size_t length);

    /**
     * @brief Get length of DG protection in bytes
     * @return
     */
    uint64_t getLength() const;

    /**
     * @brief
     * @param bit_writer
     */
    void write(util::BitWriter& bit_writer) const;

 private:
    std::vector<uint8_t> DG_protection_value;  //!< @brief
};

/**
 * @brief
 */
class DatasetGroup {
 private:

    /** ------------------------------------------------------------------------------------------------------------
     * ISO 23092-1 Section 6.5.1 table 8
     * ------------------------------------------------------------------------------------------------------------- */

    DatasetGroupHeader header;

    std::vector<Reference> references;  //!< @brief ISO 23092-1 Section 6.5.1.3, optional

    std::vector<ReferenceMetadata> reference_metadata;  //!< @brief ISO 23092-1 Section 6.5.1.4, optional

    std::unique_ptr<LabelList> label_list;  //!< @brief ISO 23092-1 Section 6.5.1.5, optional

    std::unique_ptr<DGMetadata> DG_metadata;  //!< @brief ISO 23092-1 Section 6.5.1.6 specification 23092-3, optional

    std::unique_ptr<DGProtection>
        DG_protection;  //!< @brief ISO 23092-1 Section 6.5.1.7 specification 23092-3, optional

    std::vector<Dataset> datasets;  //!< @brief ISO 23092-1 Section 6.5.2.1

 public:
    /**
     * @brief
     * @param _datasets
     */
//    explicit DatasetGroup(std::vector<Dataset>&& _datasets);

    /**
     *
     * @param reader
     * @param fhd
     */
    explicit DatasetGroup(util::BitReader& reader, FileHeader& fhd, size_t start_pos, size_t length);

    /**
     * @brief
     * @param _references
     */
    void addReferences(std::vector<Reference>&& _references);

    /**
     * @brief
     * @return
     */
    const std::vector<Reference>& getReferences() const;

    /**
     * @brief
     * @param _ref_metadata
     */
    void addReferenceMetadata(std::vector<ReferenceMetadata>&& _ref_metadata);

    /**
     * @brief Get an array of reference metadata
     * @return
     */
    const std::vector<ReferenceMetadata>& getReferenceMetadata() const;

    /**
     * @brief
     * @param _labels
     */
    void addLabels(std::vector<Label>&& _labels);

    /**
     * @brief
     * @param _label_list
     */
    void addLabelList(std::unique_ptr<LabelList> _label_list);

    /**
     * @brief
     * @return
     */
    const LabelList& getLabelList() const;

    /**
     * @brief Add DG_metadata
     * @param _dg_metadata
     */
    void addDGMetadata(std::unique_ptr<DGMetadata> _dg_metadata);

    /**
     * @brief Get DG_metadata
     * @return
     */
    const DGMetadata& getDgMetadata() const;

    /**
     * @brief Add DG_protection
     * @param _dg_protection
     */
    void addDGProtection(std::unique_ptr<DGProtection> _dg_protection);

    /**
     * @brief Get DG_protection
     * @return
     */
    const DGProtection& getDgProtection() const;

    /**
     * @brief
     * @return
     */
    const std::vector<Dataset>& getDatasets() const;

    /**
     * @brief
     * @param ID
     */
    void setID(uint8_t ID);

    /**
     * @brief Get length of Dataset in byte
     * @return
     */
    uint64_t getLength() const;

    /**
     * @brief
     * @param writer
     */
    void write(util::BitWriter& writer) const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MPEGG_P1_DATASET_GROUP_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
